/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#include "odeObject.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "system.hpp"
#include "vector3d.hpp"
#include "quaternion.hpp"
#include "world.hpp"

unsigned int OdeObject::instancesCount = 0;

void getMeshInformation (Ogre::MeshPtr mesh, size_t & vertex_count, dVector3 * &vertices, size_t & index_count, unsigned *&indices, const Ogre::Vector3 & position = Ogre::Vector3::ZERO, const Ogre::Quaternion & orient = Ogre::Quaternion::IDENTITY, const Ogre::Vector3 & scale = Ogre::Vector3::UNIT_SCALE);

std::string OdeObject::getId()
{
    return this->id;
}

void OdeObject::updateId()
{
    const int numberStringSize = 64;
    char numberString[numberStringSize];
    snprintf (numberString, numberStringSize, "%i", instancesCount);
    this->id = std::string(numberString);
    instancesCount++;
}
OdeObject::OdeObject (WorldObject * worldObject, pMeshOdeData data, std::string name)
{
    updateId();
    this->worldObject = worldObject;
    this->name = name;
    bodyID = 0; //dBodyCreate (worldID);

    dTriMeshDataID ground = dGeomTriMeshDataCreate ();
    dGeomID geom = dCreateTriMesh (World::get()->spaceID, ground, 0, 0, 0);
    dGeomSetBody (geom, 0);
    geomIDs[name+id] = geom;
    size_t vertex_count, index_count;
    dVector3 * vertices;
    unsigned int *indices;
    getMeshInformation (data->ogreChild->getEntity()->getMesh (), vertex_count, vertices, index_count, indices, data->ogreChild->getNode()->getPosition(), data->ogreChild->getNode()->getOrientation(), data->ogreChild->getNode()->getScale());
    Vector3d posi; Ogre::Vector3 oposi = data->ogreChild->getNode()->getPosition(); posi.x = oposi.x; posi.y = oposi.y; posi.z = oposi.z;
    worldObject->getLog()->__format (LOG_DEVELOPER, "Building mesh; Adding at pos(%f,%f,%f)",posi.x,posi.y,posi.z);
    //FIXME: keep the vertex data (ground pointer) somewhere; ODE doesn't keep a copy of the trimesh data!!
#if defined(dSINGLE)
    dGeomTriMeshDataBuildSingle (ground, vertices, sizeof (dVector3), vertex_count, indices, index_count, 3 * sizeof (unsigned int));
#else
    dGeomTriMeshDataBuildDouble (ground, vertices, sizeof (dVector3), vertex_count, indices, index_count, 3 * sizeof (unsigned int));
#endif
}
OdeObject::OdeObject (WorldObject * worldObject, pPartOdeData data, std::string name)
{
    updateId();
    this->worldObject = worldObject;
    this->name = name;
    bodyID = NULL;
    bodyID = dBodyCreate (World::get()->worldID);
 
    //TODO make this optional via xml or something similar   
    //set autodisable
    dBodySetAutoDisableFlag (bodyID, 1);
    dBodySetAutoDisableLinearThreshold (bodyID, 0.15);
    dBodySetAutoDisableAngularThreshold (bodyID, 0.20);
    dBodySetAutoDisableSteps (bodyID, 1);
    //*/
    
    dBodySetData (bodyID, static_cast<void*>(worldObject->getContainer().get()));
    dMass dmass;
    if (data->shape == "box")
    {
        geomIDs[name] = dCreateBox (World::get()->spaceID, data->size.x, data->size.y, data->size.z);
        dMassSetBoxTotal (&dmass, data->mass, data->size.x, data->size.y, data->size.z);
    }
    if (data->shape == "sphere")
    {
        geomIDs[name] = dCreateSphere (World::get()->spaceID, data->radius);
        dMassSetSphereTotal (&dmass, data->mass, data->radius);
    }
    if (data->shape == "capsule")
    {
        geomIDs[name] = dCreateCCylinder (World::get()->spaceID, data->radius, data->length);
        dMassSetCappedCylinderTotal (&dmass, data->mass, data->directionAxis, data->radius, data->length);
    }
    dGeomSetBody (geomIDs[name], bodyID);
    dBodySetMass (bodyID, &dmass);
}
OdeObject::OdeObject (WorldObject * worldObject, std::string name)
{
    updateId();
    this->worldObject = worldObject;
    this->name = name;
    bodyID = NULL;
    bodyID = dBodyCreate (World::get()->ghostWorldID);
}
OdeObject::OdeObject (WorldObject * worldObject, pBodyOdeData data, std::string name)
{
    updateId();
    this->worldObject = worldObject;
    this->name = name;
    bodyID = NULL;
    // create dBody
    bodyID = dBodyCreate (World::get()->worldID);
    dBodySetData (bodyID, static_cast<void*>(worldObject->getContainer().get()));
    
    // create transform spaces (in order to be able to 'offset' the collision geoms
    geomIDs["GeomSpace(A)"] = dCreateGeomTransform (World::get()->spaceID);
    geomIDs["GeomSpace(B)"] = dCreateGeomTransform (World::get()->spaceID);

    // avoid ODE removing objects itself. transform geoms now must be manually removed
    dGeomTransformSetCleanup (geomIDs["GeomSpace(A)"], 0);
    dGeomTransformSetCleanup (geomIDs["GeomSpace(B)"], 0);
    
    // create collision geoms
    geomIDs["Geom(A)"] = dCreateBox (0, data->length, data->width, data->height / 2.0f);
    geomIDs["Geom(B)"] = dCreateBox (0, data->length / 2.0f, data->width / 2.0f, data->height / 2.0f);

    // insert collision geoms into transformation spaces
    dGeomTransformSetGeom (geomIDs["GeomSpace(A)"], geomIDs["Geom(A)"]);
    dGeomTransformSetGeom (geomIDs["GeomSpace(B)"], geomIDs["Geom(B)"]);

    // apply offsets to the collision geoms
    dGeomSetPosition (geomIDs["Geom(A)"], 0.f, 0.f, - data->height / 4.0f);
    dGeomSetPosition (geomIDs["Geom(B)"], -data->width / 6.f, 0.f, (data->height / 4.0f) + 0.1f);

    // associate the dBody with the 2 collision geoms via the transformation spaces
    dGeomSetBody (geomIDs["GeomSpace(A)"], bodyID);
    dGeomSetBody (geomIDs["GeomSpace(B)"], bodyID);

    // set dBody mass
    dMass tmpMass;
    dMassSetBoxTotal (&tmpMass, data->mass, data->length, data->width, data->height);
    dBodySetMass (bodyID, &tmpMass);

    // make sure it's initialized with correct values.
    Quaternion finalRotation = Quaternion(0,0,0,0);
    dMatrix3 rot;
    finalRotation.getOdeMatrix (rot);
    dBodySetRotation (bodyID, rot);
    dBodySetPosition (bodyID, 0, 0 ,0);
    dBodySetLinearVel  (bodyID, 0, 0, 0);
    dBodySetAngularVel (bodyID, 0, 0, 0);
}
OdeObject::OdeObject (WorldObject * worldObject, pBoneOdeData data, std::string name)
{
    updateId();
    this->worldObject = worldObject;
    this->name = name;
    bodyID = NULL;

    bodyID = dBodyCreate (World::get()->worldID);
    dBodySetData (bodyID, static_cast<void*>(worldObject->getContainer().get()));
    dMass dmass;
    if (data->useMass)
    {
        dMassSetBoxTotal(&dmass, data->mass, data->radius, data->radius, data->length);
    }
    else
    {
        dMassSetBox(&dmass, data->density, data->radius, data->radius, data->length);
    }
    dBodySetMass (bodyID, &dmass);

    //geomIDs[name] = dCreateBox (World::get()->spaceID, data->radius, data->radius, data->length);
    dBodySetLinearVel  (bodyID, 0, 0, 0);
    dBodySetAngularVel (bodyID, 0, 0, 0);
    //dGeomSetBody (geomIDs[name], bodyID);
}
OdeObject::~OdeObject ()
{
    //TODO: check if there's anything left to do here with ODE...
    worldObject->getLog()->__format (LOG_DEVELOPER, "Removing geoms");
    GeomIDsIt i = geomIDs.begin();
    for(;i != geomIDs.end(); i++)
    {
        worldObject->getLog()->__format (LOG_DEVELOPER, "Removing geom id=%s", i->first.c_str());
        dGeomDestroy (i->second);
        i->second = NULL;
    }
    geomIDs.clear();
    if (bodyID) dBodyDestroy (bodyID);
    bodyID = NULL;
    this->worldObject = NULL;
}
void OdeObject::addRotation (Vector3d origin, Quaternion diff)
{
    Vector3d rotMovement = diff.rotateObject(this->getPosition() - origin);
    this->setPosition (origin + rotMovement);

    Quaternion current = this->getRotation();
    Quaternion newrot = diff * current;
    setRotation (newrot);
}
void OdeObject::addPosition (Vector3d diff)
{
    Vector3d current = this->getPosition();
    Vector3d newpos = current + diff;
    this->setPosition (newpos);
}
void OdeObject::setPosition (Vector3d position)
{
    if (bodyID == 0)
    {
        if (geomIDs.begin() == geomIDs.end())
        {
            worldObject->getLog()->__format (LOG_ERROR, "No geoms!! OdeObj name=%s", this->getId().c_str());
        }
        GeomIDsIt i = geomIDs.begin();
        for(;i != geomIDs.end(); i++)
        {
            dGeomSetPosition (i->second, position.x, position.y, position.z);
        }
    } else {
        dBodySetPosition (bodyID, position.x, position.y, position.z);
    }
}

void OdeObject::setRotation (Quaternion rotation)
{
    dMatrix3 rot;
    rotation.getOdeMatrix (rot);
    if (bodyID == 0)
    {
        GeomIDsIt i = geomIDs.begin();
        for(;i != geomIDs.end(); i++)
        {
            dGeomSetRotation (i->second, rot);
        }
    } else {
        dBodySetRotation (bodyID, rot);
    }
}
Vector3d OdeObject::getPosition()
{
    Vector3d result(0,0,0);
    if (bodyID == 0)
    {
        if (geomIDs.begin() == geomIDs.end()) 
        {
            //empty
        } else {
            const dReal * tmp = dGeomGetPosition (geomIDs.begin()->second);
            result = Vector3d(tmp[0], tmp[1], tmp[2]);
        }
    } else {
        const dReal * tmp = dBodyGetPosition (bodyID);
        result = Vector3d(tmp[0], tmp[1], tmp[2]);
    }
    //const dReal * tmp = dBodyGetPosition (bodyID);
    return result;
}
Quaternion OdeObject::getRotation()
{
    dReal * tmp = NULL;
    if (bodyID == 0)
    {
        if (geomIDs.begin() == geomIDs.end()) 
        {
            //empty
        } else {
            dQuaternion q;
            dGeomGetQuaternion (geomIDs.begin()->second, q);
            tmp = &(q[0]);
        }
    } else {
        tmp = (dReal*)dBodyGetQuaternion (bodyID);
    }
    if (tmp == NULL) return Quaternion(0,0,0);
    //const dReal * tmp = dBodyGetQuaternion (bodyID);
    return Quaternion(tmp);
}
dBodyID OdeObject::getBodyID()
{
    return bodyID;
}
dGeomID OdeObject::getGeomID(std::string name)
{
    return geomIDs[name];
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

