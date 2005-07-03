/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "data/track.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "data/xmlParser.hpp"
#include "log/logEngine.hpp"
#include "system.hpp"
#include "camera.hpp"
#include "cube.hpp"
#include "ode/ode.h"
#include "vehiclePosition.hpp"
#include "world.hpp"
#include "SDL.h"

void getMeshInformation (Ogre::MeshPtr mesh, size_t & vertex_count, dVector3 * &vertices, size_t & index_count, unsigned *&indices, const Ogre::Vector3 & position = Ogre::Vector3::ZERO, const Ogre::Quaternion & orient = Ogre::Quaternion::IDENTITY, const Ogre::Vector3 & scale = Ogre::Vector3::UNIT_SCALE);

Track::Track (const std::string & xmlFilename)
{
    log = new LogEngine (LOG_DEVELOPER, "TRK");
    path = SystemData::getSystemDataPointer()->dataDir + "/tracks/" + xmlFilename;
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path, "FileSystem", path);
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(path + "/skybox.zip", "Zip", path);
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup(path);
    log->loadscreen (LOG_ENDUSER, "Starting to load a track (%s)", path.c_str());
    double time = SDL_GetTicks()/1000.0;
    std::string filename = path + "/track.xml";
    XmlFile * xmlFile = new XmlFile (filename.c_str());
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;
    log->loadscreen (LOG_ENDUSER, "Finished loading a track (%s). %f seconds.", filename.c_str(), (SDL_GetTicks()/1000.0 - time));
}

Track::~Track ()
{
    // unload the cubes from memory
    log->put (LOG_DEVELOPER, "Unloading cubes from memory...");
    int size = cubeList.size ();
    for (int i = 0; i < size; i++)
    {
        delete cubeList[i];
    }
    cubeList.clear ();

    // unload the cameras from memory
    log->put (LOG_DEVELOPER, "Unloading cameras from memory...");
    size = cameraList.size ();
    for (int i = 0; i < size; i++)
    {
        delete cameraList[i];
    }
    cameraList.clear ();

/*    stopPhysics ();
    stopGraphics ();
    stopInput ();*/

    delete log;
}

void Track::processXmlRootNode (DOMNode * n)
{
    name = "None";
    description = "None";
    author = "Anonymous";
    contact = "None";
    license = "Creative Commons Attribution-NonCommercial-ShareAlike License";
    Vector3d checkpointPosition (0, 0, 0);
    double checkpointRadius = 5;
    double groundHeight = 0.0;
    std::string mesh = "none";
    std::string groundMaterialName = "groundMaterial";
    std::string skyMaterialName = "skyboxMaterial";
    double skyDistance = 5000.0;
    bool skyDrawFirst = true;
    DOMNode * partListNode = 0;
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string nodeName;
            assignXmlString (nodeName, n->getNodeName());
            if (nodeName == "track")
            {
                log->put (LOG_CCREATOR, "Found a track.");
                if (n->hasAttributes ())
                {
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "name")
                        {
                            assignXmlString (name, attNode->getValue());
                            log->loadscreen (LOG_CCREATOR, "Found the name: %s", name.c_str());
                        }
                        if (attribute == "description")
                        {
                            assignXmlString (description, attNode->getValue());
                            log->loadscreen (LOG_CCREATOR, "Found the description: %s", description.c_str());
                        }
                        if (attribute == "author")
                        {
                            assignXmlString (author, attNode->getValue());
                            log->loadscreen (LOG_CCREATOR, "Found the author: %s", author.c_str());
                        }
                        if (attribute == "contact")
                        {
                            assignXmlString (contact, attNode->getValue());
                            log->format (LOG_CCREATOR, "Found the contact information: %s", contact.c_str());
                        }
                        if (attribute == "license")
                        {
                            assignXmlString (license, attNode->getValue());
                            log->format (LOG_CCREATOR, "Found the license: %s", license.c_str());
                        }
                        if (attribute == "checkpointPosition")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_CCREATOR, "Found the checkpoint position: %s", attribute.c_str());
                            checkpointPosition = Vector3d (attribute);
                        }
                        if (attribute == "checkpointRadius")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_CCREATOR, "Found the checkpoint radius: %s", attribute.c_str());
                            checkpointRadius = stod (attribute);
                        }
                    }
                }
                for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
                {
                    if (n)
                    {
                        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
                        {
                            assignXmlString (nodeName, n->getNodeName());
                            if (nodeName == "ground")
                            {
                                log->put (LOG_CCREATOR, "Found a track ground.");
                                if (n->hasAttributes ())
                                {
                                    DOMNamedNodeMap *attList = n->getAttributes ();
                                    int nSize = attList->getLength ();
                                    for (int i = 0; i < nSize; ++i)
                                    {
                                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                                        std::string attribute;
                                        assignXmlString (attribute, attNode->getName());
                                        if (attribute == "height")
                                        {
                                            assignXmlString (attribute, attNode->getValue());
                                            log->format (LOG_CCREATOR, "Found the ground height: %s", attribute.c_str());
                                            groundHeight = stod (attribute);
                                        }
                                        if (attribute == "materialName")
                                        {
                                            assignXmlString (groundMaterialName, attNode->getValue());
                                            log->format (LOG_CCREATOR, "Found the ground material name: %s", groundMaterialName.c_str());
                                        }
                                        if (attribute == "mesh")
                                        {
                                            assignXmlString (mesh, attNode->getValue());
                                            log->format (LOG_CCREATOR, "Found the ground mesh file name: %s", mesh.c_str());
                                        }
                                    }
                                }
                            }
                            if (nodeName == "sky")
                            {
                                log->put (LOG_CCREATOR, "Found a track sky.");
                                if (n->hasAttributes ())
                                {
                                    DOMNamedNodeMap *attList = n->getAttributes ();
                                    int nSize = attList->getLength ();
                                    for (int i = 0; i < nSize; ++i)
                                    {
                                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                                        std::string attribute;
                                        assignXmlString (attribute, attNode->getName());
                                        if (attribute == "materialName")
                                        {
                                            assignXmlString (skyMaterialName, attNode->getValue());
                                            log->format (LOG_CCREATOR, "Found the sky material name: %s", skyMaterialName.c_str());
                                        }
                                        if (attribute == "distance")
                                        {
                                            assignXmlString (attribute, attNode->getValue());
                                            log->format (LOG_CCREATOR, "Found the sky distance: %s", attribute.c_str());
                                            skyDistance = stod (attribute);
                                        }
                                        if (attribute == "drawFirst")
                                        {
                                            assignXmlString (attribute, attNode->getValue());
                                            log->format (LOG_CCREATOR, "Found whether to draw the sky first: %s", attribute.c_str());
                                            skyDrawFirst = stob (attribute);
                                        }
                                    }
                                }
                            }
                            if (nodeName == "parts")
                            {
                                log->put (LOG_CCREATOR, "Found a list of track parts.");
                                partListNode = n;
                            }
                            if (nodeName == "vehiclePositionList")
                            {
                                log->put (LOG_CCREATOR, "Found a list of vehicle positions.");
                                DOMNode * n2;
                                for (n2 = n->getFirstChild (); n2 != 0; n2 = n2->getNextSibling ())
                                {
                                    if (n2->getNodeType () == DOMNode::ELEMENT_NODE)
                                    {
                                        std::string node2Name;
                                        assignXmlString (node2Name, n2->getNodeName());
                                        if (node2Name == "vehicle")
                                        {
                                            log->put (LOG_CCREATOR, "Found a vehicle position.");
                                            VehiclePosition * tmpVehicle = new VehiclePosition (n2);
                                            vehiclePositionMap[tmpVehicle->getIndex()] = tmpVehicle;
                                        }
                                    }
                                }
                            }
                            if (nodeName == "cameraList")
                            {
                                log->put (LOG_CCREATOR, "Found a list of cameras.");
                                DOMNode* n2;
                                for (n2 = n->getFirstChild (); n2 != 0; n2 = n2->getNextSibling ())
                                {
                                    if (n2->getNodeType () == DOMNode::ELEMENT_NODE)
                                    {
                                        std::string node2Name;
                                        assignXmlString (node2Name, n2->getNodeName());
                                        if (node2Name == "camera")
                                        {
                                            log->put (LOG_CCREATOR, "Found a camera.");
                                            Camera * tmpCam = new Camera (n2);
                                            cameraList.push_back (tmpCam);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    processXmlPartListNode(partListNode);

    log->loadscreen (LOG_CCREATOR, "Creating the track ground");

    log->put (LOG_DEVELOPER, "Creating the ode plane");
    dCreatePlane (World::getWorldPointer()->spaceID, 0, 0, 1, groundHeight);

    log->put (LOG_DEVELOPER, "Creating the ogre plane");
    Ogre::Plane plane;
    plane.normal = Ogre::Vector3(0,0,1);
    plane.d = -groundHeight;
    Ogre::SceneManager* pOgreSceneManager = SystemData::getSystemDataPointer()->ogreSceneManager;
    Ogre::MeshManager::getSingleton().createPlane("Ground plane", "general", plane, 1000,1000,1,1,true,1,20,20,Ogre::Vector3::UNIT_Y);
    planeEntity = pOgreSceneManager->createEntity("plane", "Ground plane");
    planeEntity->setMaterialName(groundMaterialName.c_str());
    planeNode = pOgreSceneManager->getRootSceneNode()->createChildSceneNode();
    planeNode->attachObject(planeEntity);
    trackBodyID = dBodyCreate (World::getWorldPointer ()->ghostWorldID);

    // FIXME should be part of the world, not the track
    log->loadscreen (LOG_CCREATOR, "Creating the track sky");
    Ogre::Quaternion rotationToZAxis;
    rotationToZAxis.FromRotationMatrix (Ogre::Matrix3 (1, 0, 0, 0, 0, -1, 0, 1, 0));
    SystemData::getSystemDataPointer()->ogreSceneManager->setSkyBox (true, skyMaterialName.c_str(), skyDistance, skyDrawFirst, rotationToZAxis);
    
    log->loadscreen (LOG_CCREATOR, "Creating the track ground");
    // declare ode mesh
    dTriMeshDataID ground = dGeomTriMeshDataCreate ();
    dGeomSetBody (dCreateTriMesh (World::getWorldPointer ()->spaceID, ground, 0, 0, 0), 0);

    if (mesh != "none")
    {
        log->loadscreen (LOG_CCREATOR, "Creating loading the ogre track mesh");
        trackEntity = SystemData::getSystemDataPointer ()->ogreSceneManager->createEntity ("ground", path + "/" + mesh);
        Ogre::SceneNode * trackNode = static_cast < Ogre::SceneNode * >(SystemData::getSystemDataPointer ()->ogreSceneManager->getRootSceneNode ()->createChild ());
        trackNode->attachObject (trackEntity);
        trackNode->setScale (1, 1, 1);
        trackNode->setPosition (0, 0, 0);
        Quaternion quat (0, 0, 0);
        trackNode->setOrientation (quat.w, quat.x, quat.y, quat.z);
        size_t vertex_count, index_count;
        dVector3 * vertices;
        unsigned int *indices;
        getMeshInformation (trackEntity->getMesh (), vertex_count, vertices, index_count, indices, trackNode->getPosition(), trackNode->getOrientation(), trackNode->getScale());
        log->loadscreen (LOG_CCREATOR, "Building the ode track mesh");
        dGeomTriMeshDataBuildDouble (ground, vertices, sizeof (dVector3), vertex_count, indices, index_count, 3 * sizeof (unsigned int));
        // dGeomTriMeshDataDestroy (ground);
    } else {
        trackEntity = 0;
    }

    // create the checkpoint sphere
    dGeomID checkpointID = dCreateSphere (World::getWorldPointer()->spaceID, checkpointRadius);
    dGeomSetBody (checkpointID, 0);
    dGeomSetPosition (checkpointID, checkpointPosition.x, checkpointPosition.y, checkpointPosition.z); 
    
}

void Track::processXmlPartListNode(DOMNode * partListNode)
{
    int gridCubes = 20;
    // parse attributes
    if (partListNode->hasAttributes ())
    {
        DOMNamedNodeMap *attList = partListNode->getAttributes ();
        int nSize = attList->getLength ();
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "gridCubes")
            {
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_CCREATOR, "Found the number of cubes aligned in a grid: %s", attribute.c_str());
                gridCubes = stoi (attribute);
            }
        }
    }
    // parse individual parts
    DOMNode * partNode;
    for (partNode = partListNode->getFirstChild (); partNode != 0; partNode = partNode->getNextSibling ())
    {
        if (partNode->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string nodeName;
            assignXmlString (nodeName, partNode->getNodeName());
            if (nodeName == "cube")
            {
                log->put (LOG_CCREATOR, "Found a cube.");
                Vector3d position(0, 0, 0);
                Quaternion rotation(0, 0, 0);
                if (partNode->hasAttributes ())
                {
                    DOMNamedNodeMap *attList = partNode->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "position")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_CCREATOR, "Found the cube position: %s", attribute.c_str());
                            position = Vector3d (attribute);
                        }
                        if (attribute == "rotation")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_CCREATOR, "Found the cube rotation: %s", attribute.c_str());
                            rotation = Quaternion (attribute);
                        }
                    }
                }
                Cube * cubePointer = new Cube ();
                cubePointer->setPosition (position);
                cubePointer->setRotation (rotation);
                cubeList.push_back (cubePointer);
                cubePointer->stepGraphics();
            }
        }
    }
    //load cubes
    log->loadscreen (LOG_CCREATOR, "Creating an array of %i cubes", gridCubes);
    for (int i = 0; i < gridCubes; i++)
    {
        log->format (LOG_DEVELOPER, "Adding cube number %i", i);
        const int separation = 4;
        Cube *cubePointer;
        cubePointer = new Cube ();
        cubePointer->setPosition (Vector3d (i / 10 % 10 * separation, i / 100 % 10 * separation + (separation * ((int (i / 1000)) +1)), separation + i % 10 * separation));
        cubeList.push_back (cubePointer);
        cubePointer->stepGraphics();
        /* snake mode. will slow down things. i%4 sets the number of cubes per snake.
        if ( i%4 && false)
        {
            if (0) //type of snake: universal joint vs. ball joint.
            {
                dJointID jointID = dJointCreateUniversal (World::getWorldPointer()->worldID, 0);
                dJointAttach (jointID, cubeList[i-1]->cubeID, cubeList[i]->cubeID);
                dJointSetUniversalAnchor (jointID, i / 10 % 10 * separation, i / 100 % 10 * separation + (separation * ((int (i / 1000)) +1)), separation + i % 10 * separation);
                dJointSetUniversalAxis1 (jointID, 1,0,0);
                dJointSetUniversalAxis2 (jointID, 0,1,0);
            }else{
                dJointID jointID = dJointCreateBall (World::getWorldPointer()->worldID, 0);
                dJointAttach (jointID, cubeList[i-1]->cubeID, cubeList[i]->cubeID);
                dJointSetBallAnchor (jointID, i / 10 % 10 * separation, i / 100 % 10 * separation + (separation * ((int (i / 1000)) +1)), separation + i % 10 * separation);
            } 
        }
        */
    }
}
void Track::setCastShadows(bool castShadows)
{
    planeEntity->setCastShadows(castShadows);
    if (trackEntity != 0) trackEntity->setCastShadows(castShadows);
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
