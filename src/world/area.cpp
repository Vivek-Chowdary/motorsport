/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#include "area.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "log/logEngine.hpp"
#include "system.hpp"
#include "camera.hpp"
#include "part.hpp"
#include "vehicle.hpp"
#include "ode/ode.h"
#include "location.hpp"
#include "world.hpp"
#include "SDL.h"

void getMeshInformation (Ogre::MeshPtr mesh, size_t & vertex_count, dVector3 * &vertices, size_t & index_count, unsigned *&indices, const Ogre::Vector3 & position = Ogre::Vector3::ZERO, const Ogre::Quaternion & orient = Ogre::Quaternion::IDENTITY, const Ogre::Vector3 & scale = Ogre::Vector3::UNIT_SCALE);

pArea Area::create (std::string areaName)
{
    pArea area(new Area(areaName));
    return area;
}
Area::Area (std::string areaName)
    :WorldObject("area")
{
    setPath(Paths::area(areaName));
    setXmlPath(Paths::areaXml(areaName));
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPath(), "FileSystem", "areas." + areaName);
    //Ogre::ResourceGroupManager::getSingleton().addResourceLocation(getPath() + "skybox.zip", "Zip", "areas."+areaName);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("areas." + areaName);
    XmlTag * tag = new XmlTag (getXmlPath());
    construct (tag);
    delete tag;
}

Area::~Area ()
{
}

void Area::readCustomDataTag(XmlTag * tag)
{
    Vector3d checkpointPosition (0, 0, 0);
    double checkpointRadius = 5;
    std::string mesh = "none";

    setName (     tag->getAttribute("name"));
    description = tag->getAttribute("description");
    author =      tag->getAttribute("author");
    contact =     tag->getAttribute("contact");
    license =     tag->getAttribute("license");
    checkpointPosition = Vector3d (tag->getAttribute("checkpointPosition"));
    checkpointRadius = stod(tag->getAttribute("checkpointRadius"));
    XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
    {
        if (t->getName() == "ground")
        {
            pOgreObjectData ogreData(new OgreObjectData);
            ogreData->meshPath = "";
            //create main mesh
            ogreData->meshPath = getPath() + ogreData->meshPath;
            pOgreObject ogreObject (new OgreObject(this, ogreData, getId(), false));
            ogreObjects[ogreObject->getId()] = ogreObject;
            //create child meshes
            XmlTag * u = t->getTag(0); for (int i = 0; i < t->nTags(); u = t->getTag(++i))
            {
                if (u->getName() == "mesh")
                {
                    log->__format (LOG_CCREATOR, "Creating an ogre mesh for the ground");
                    pOgreObjectData childData (new OgreObjectData);
                    childData->meshPath = getPath() + u->getAttribute("file");
                    Vector3d posDiff (u->getAttribute("position"));
                    Vector3d scale (u->getAttribute("scale"));
                    Quaternion rotDiff (u->getAttribute("rotation"));
                    pOgreObject ogreChild (new OgreObject(this, childData, getId()));
                    ogreObjects[ogreChild->getId()] = ogreChild;
                    ogreChild->setOgreReference(ogreObjects[ogreObject->getId()], rotDiff, posDiff, scale);
                    // declare ode mesh
                    dTriMeshDataID ground = dGeomTriMeshDataCreate ();
                    dGeomSetBody (dCreateTriMesh (World::get()->spaceID, ground, 0, 0, 0), 0);

                    size_t vertex_count, index_count;
                    dVector3 * vertices;
                    unsigned int *indices;
                    getMeshInformation (ogreChild->getEntity()->getMesh (), vertex_count, vertices, index_count, indices, ogreChild->getNode()->getPosition(), ogreChild->getNode()->getOrientation(), ogreChild->getNode()->getScale());
                    log->__format (LOG_CCREATOR, "Building the ode mesh for the ground");
                    //FIXME: keep the vertex data somewhere; ODE doesn't keep a copy of the trimesh data!!
#if defined(dSINGLE)
                    dGeomTriMeshDataBuildSingle (ground, vertices, sizeof (dVector3), vertex_count, indices, index_count, 3 * sizeof (unsigned int));
#else
                    dGeomTriMeshDataBuildDouble (ground, vertices, sizeof (dVector3), vertex_count, indices, index_count, 3 * sizeof (unsigned int));
#endif
                    System::get()->ogreWindow->update ();
                }
            }
        }
        if (t->getName() == "parts")
        {
            Vector3d position(0,0,0);
            Quaternion rotation(1,0,0,0);
            XmlTag * u = t->getTag(0); for (int j = 0; j < t->nTags(); u = t->getTag(++j))
            {
                position = Vector3d (u->getAttribute("position"));
                rotation = Quaternion (u->getAttribute("rotation"));
                pPart tmp = Part::create (u->getName());
                tmp->setPosition (position);
                tmp->setRotation (rotation);
                objects[tmp->getName()] = tmp;
                tmp->stepGraphics();
                System::get()->ogreWindow->update ();
            }
        }
        if (t->getName() == "vehicleLocation")
        {
            pLocation tmp = Location::create (t);
            locations[tmp->getName()] = tmp;
        }
        if (t->getName() == "camera")
        {
            pCamera tmp = Camera::create (t);
            objects[tmp->getName()] = tmp;
        }
    }
    log->loadscreen (LOG_CCREATOR, "Creating the area ground");

    areaBodyID = dBodyCreate (World::get()->ghostWorldID);
    positionCameras(areaBodyID);
    System::get()->ogreWindow->update ();

    // create the checkpoint sphere
    dGeomID checkpointID = dCreateSphere (World::get()->spaceID, checkpointRadius);
    dGeomSetBody (checkpointID, 0);
    dGeomSetPosition (checkpointID, checkpointPosition.x, checkpointPosition.y, checkpointPosition.z); 
}
void Area::construct (XmlTag * tag)
{
    constructFromTag(tag);
}

void Area::setCastShadows(bool castShadows)
{
    //UNCOMMENT ME
    //if (areaEntity != 0) areaEntity->setCastShadows(castShadows);
}

void Area::setRenderDetail(int renderMode)
{
    Ogre::PolygonMode mode;
    switch (renderMode)
    {
    case 1:
        mode = Ogre::PM_POINTS;
        break;
    case 2:
        mode = Ogre::PM_WIREFRAME;
        break;
    case 3:
        mode = Ogre::PM_SOLID;
        break;
    }
    WorldObjectsIt i = objects.begin();
    for(;i != objects.end(); i++)
    {
        if (pCamera tmp = boost::dynamic_pointer_cast<Camera>(i->second))
        {
            tmp->ogreCamera->setPolygonMode(mode);
        }
    }
}

void getMeshInformation (Ogre::MeshPtr mesh, size_t & vertex_count, dVector3 * &vertices, size_t & index_count, unsigned *&indices, const Ogre::Vector3 & position, const Ogre::Quaternion & orient, const Ogre::Vector3 & scale)
{
    vertex_count = index_count = 0;
    bool added_shared = false;
    size_t current_offset = vertex_count;
    size_t shared_offset = vertex_count;
    size_t next_offset = vertex_count;
    size_t index_offset = index_count;

    // Calculate how many vertices and indices we're going to need
    for (int i = 0; i < mesh->getNumSubMeshes (); i++)
    {
        Ogre::SubMesh * submesh = mesh->getSubMesh (i);

        // We only need to add the shared vertices once
        if (submesh->useSharedVertices)
        {
            if (!added_shared)
            {
                Ogre::VertexData * vertex_data = mesh->sharedVertexData;
                vertex_count += vertex_data->vertexCount;
                added_shared = true;
            }
        } else
        {
            Ogre::VertexData * vertex_data = submesh->vertexData;
            vertex_count += vertex_data->vertexCount;
        }

        // Add the indices
        Ogre::IndexData * index_data = submesh->indexData;
        index_count += index_data->indexCount;
    }

    // Allocate space for the vertices and indices
    vertices = new dVector3[vertex_count];
    indices = new unsigned[index_count];

    added_shared = false;

    // Run through the submeshes again, adding the data into the arrays
    for (int i = 0; i < mesh->getNumSubMeshes (); i++)
    {
        Ogre::SubMesh * submesh = mesh->getSubMesh (i);

        Ogre::VertexData * vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;
        if ((!submesh->useSharedVertices) || (submesh->useSharedVertices && !added_shared))
        {
            if (submesh->useSharedVertices)
            {
                added_shared = true;
                shared_offset = current_offset;
            }

            const Ogre::VertexElement * posElem = vertex_data->vertexDeclaration->findElementBySemantic (Ogre::VES_POSITION);
            Ogre::HardwareVertexBufferSharedPtr vbuf = vertex_data->vertexBufferBinding->getBuffer (posElem->getSource ());
            unsigned char *vertex = static_cast < unsigned char *>(vbuf->lock (Ogre::HardwareBuffer::HBL_READ_ONLY));
            Ogre::Real * pReal;

            for (size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize ())
            {
                posElem->baseVertexPointerToElement (vertex, &pReal);

                Ogre::Vector3 pt;

                pt.x = (*pReal++);
                pt.y = (*pReal++);
                pt.z = (*pReal++);

                pt = (orient * (pt * scale)) + position;

                vertices[current_offset + j][0] = pt.x;
                vertices[current_offset + j][1] = pt.y;
                vertices[current_offset + j][2] = pt.z;
            }
            vbuf->unlock ();
            next_offset += vertex_data->vertexCount;
        }

        Ogre::IndexData * index_data = submesh->indexData;

        size_t numTris = index_data->indexCount / 3;
        unsigned short *pShort;
        unsigned int *pInt;
        Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;
        bool use32bitindexes = (ibuf->getType () == Ogre::HardwareIndexBuffer::IT_32BIT);
        if (use32bitindexes)
            pInt = static_cast < unsigned int *>(ibuf->lock (Ogre::HardwareBuffer::HBL_READ_ONLY));
        else
        pShort = static_cast < unsigned short *>(ibuf->lock (Ogre::HardwareBuffer::HBL_READ_ONLY));

        for (size_t k = 0; k < numTris; ++k)
        {
            size_t offset = (submesh->useSharedVertices) ? shared_offset : current_offset;

            unsigned int vindex = use32bitindexes ? *pInt++ : *pShort++;
            indices[index_offset + 0] = vindex + offset;
            vindex = use32bitindexes ? *pInt++ : *pShort++;
            indices[index_offset + 1] = vindex + offset;
            vindex = use32bitindexes ? *pInt++ : *pShort++;
            indices[index_offset + 2] = vindex + offset;

            index_offset += 3;
        }
        ibuf->unlock ();
        current_offset = next_offset;
    }
}

pCamera Area::getClosestCamera(Vector3d location)
{
    log->__format(LOG_DEVELOPER, "Finding closest camera");
    double closestDistance = 99999999999.0;
    WorldObjectsIt i = objects.begin();
    pCamera closestCam;
    bool first = false;
    for (;i != objects.end(); i++)
    {
        if (pCamera tmp = boost::dynamic_pointer_cast<Camera>(i->second))
        {
            if (first == false)
            {
                closestCam = tmp;
                first = true;
            }
            log->__format(LOG_DEVELOPER, "Checking cam id: %s", tmp->getName().c_str());
            Ogre::Vector3 p = tmp->ogreCamera->getPosition();
            Vector3d cPos (p.x, p.y, p.z);
            double distance = cPos.distance(location);
            if (distance < closestDistance)
            {
                closestDistance = distance;
                closestCam = tmp;
            }
        }
    }
    activeCamera = closestCam;
    return activeCamera;
}
