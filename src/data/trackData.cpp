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

Track::Track (const std::string & xmlFilename)
{
    log = new LogEngine (LOG_TRACE, "TRK");
    log->put (LOG_INFO, "Starting to parse the track xml file");
    std::string file = SystemData::getSystemDataPointer()->dataDir;
    file.append("/tracks/");
    file.append(xmlFilename);
    file.append("/track.xml");
    XmlFile * xmlFile = new XmlFile (file.c_str());
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;
}

Track::~Track ()
{
    // unload the cubes from memory
    log->put (LOG_INFO, "Unloading cubes from memory...");
    int size = cubeList.size ();
    for (int i = 0; i < size; i++)
    {
        delete cubeList[i];
    }
    cubeList.clear ();

    // unload the cameras from memory
    log->put (LOG_INFO, "Unloading cameras from memory...");
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
    double groundHeight = 0.0;
    std::string groundMaterialName = "groundMaterial";
    std::string skyMaterialName = "skyboxMaterial";
    double skyDistance = 5000.0;
    bool skyDrawFirst = true;
    int numberOfCubes = 20;
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string nodeName;
            assignXmlString (nodeName, n->getNodeName());
            if (nodeName == "track")
            {
                log->put (LOG_TRACE, "Found a track.");
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
                            name.clear();
                            assignXmlString (name, attNode->getValue());
                            log->loadscreen (LOG_TRACE, "Found the name: %s", name.c_str());
                        }
                        if (attribute == "revision")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            log->format (LOG_TRACE, "Found the revision number: %s", attribute.c_str());
                            revision = stoi (attribute);
                        }
                        if (attribute == "description")
                        {
                            description.clear();
                            assignXmlString (description, attNode->getValue());
                            log->loadscreen (LOG_TRACE, "Found the description: %s", description.c_str());
                        }
                        if (attribute == "author")
                        {
                            author.clear();
                            assignXmlString (author, attNode->getValue());
                            log->loadscreen (LOG_TRACE, "Found the author: %s", author.c_str());
                        }
                        if (attribute == "contact")
                        {
                            contact.clear();
                            assignXmlString (contact, attNode->getValue());
                            log->format (LOG_TRACE, "Found the contact information: %s", contact.c_str());
                        }
                        if (attribute == "license")
                        {
                            license.clear();
                            assignXmlString (license, attNode->getValue());
                            log->format (LOG_TRACE, "Found the license: %s", license.c_str());
                        }
                        attribute.clear();
                    }
                }
                for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
                {
                    if (n)
                    {
                        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
                        {
                            nodeName.clear();
                            assignXmlString (nodeName, n->getNodeName());
                            if (nodeName == "ground")
                            {
                                log->put (LOG_TRACE, "Found a track ground.");
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
                                            attribute.clear();
                                            assignXmlString (attribute, attNode->getValue());
                                            log->format (LOG_TRACE, "Found the ground height: %s", attribute.c_str());
                                            groundHeight = stod (attribute);
                                        }
                                        if (attribute == "materialName")
                                        {
                                            groundMaterialName.clear();
                                            assignXmlString (groundMaterialName, attNode->getValue());
                                            log->format (LOG_TRACE, "Found the ground material name: %s", groundMaterialName.c_str());
                                        }
                                        attribute.clear();
                                    }
                                }
                            }
                            if (nodeName == "sky")
                            {
                                log->put (LOG_TRACE, "Found a track sky.");
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
                                            skyMaterialName.clear();
                                            assignXmlString (skyMaterialName, attNode->getValue());
                                            log->format (LOG_TRACE, "Found the sky material name: %s", skyMaterialName.c_str());
                                        }
                                        if (attribute == "distance")
                                        {
                                            attribute.clear();
                                            assignXmlString (attribute, attNode->getValue());
                                            log->format (LOG_TRACE, "Found the sky distance: %s", attribute.c_str());
                                            skyDistance = stod (attribute);
                                        }
                                        if (attribute == "drawFirst")
                                        {
                                            attribute.clear();
                                            assignXmlString (attribute, attNode->getValue());
                                            log->format (LOG_TRACE, "Found whether to draw the sky first: %s", attribute.c_str());
                                            skyDrawFirst = stob (attribute);
                                        }
                                        attribute.clear();
                                    }
                                }
                            }
                            if (nodeName == "parts")
                            {
                                log->put (LOG_TRACE, "Found a list of track parts.");
                                if (n->hasAttributes ())
                                {
                                    DOMNamedNodeMap *attList = n->getAttributes ();
                                    int nSize = attList->getLength ();
                                    for (int i = 0; i < nSize; ++i)
                                    {
                                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                                        std::string attribute;
                                        assignXmlString (attribute, attNode->getName());
                                        if (attribute == "numberOfCubes")
                                        {
                                            attribute.clear();
                                            assignXmlString (attribute, attNode->getValue());
                                            log->format (LOG_TRACE, "Found the number of cubes: %s", attribute.c_str());
                                            numberOfCubes = stoi (attribute);
                                        }
                                        attribute.clear();
                                    }
                                }
                            }
                            if (nodeName == "vehiclePositionList")
                            {
                                log->put (LOG_TRACE, "Found a list of vehicle positions.");
                                DOMNode * n2;
                                for (n2 = n->getFirstChild (); n2 != 0; n2 = n2->getNextSibling ())
                                {
                                    if (n2->getNodeType () == DOMNode::ELEMENT_NODE)
                                    {
                                        std::string node2Name;
                                        assignXmlString (node2Name, n2->getNodeName());
                                        if (node2Name == "vehicle")
                                        {
                                            log->put (LOG_TRACE, "Found a vehicle position.");
                                            processXmlVehiclePositionNode (n2);
                                        }
                                        node2Name.clear();
                                    }
                                }
                            }
                            if (nodeName == "cameraList")
                            {
                                log->put (LOG_TRACE, "Found a list of cameras.");
                                DOMNode* n2;
                                for (n2 = n->getFirstChild (); n2 != 0; n2 = n2->getNextSibling ())
                                {
                                    if (n2->getNodeType () == DOMNode::ELEMENT_NODE)
                                    {
                                        std::string node2Name;
                                        assignXmlString (node2Name, n2->getNodeName());
                                        if (node2Name == "camera")
                                        {
                                            log->put (LOG_TRACE, "Found a camera.");
                                            Camera * tmpCam = new Camera (n2);
                                            cameraList.push_back (tmpCam);
                                        }
                                        node2Name.clear();
                                    }
                                }
                            }
                        }
                    }
                }
            }
            nodeName.clear();
        }
    }

    //load cubes
    log->loadscreen (LOG_INFO, "Creating an array of %i cubes", numberOfCubes);
    for (int i = 0; i < numberOfCubes; i++)
    {
        log->format (LOG_VERBOSE, "Adding cube number %i", i);
        const int separation = 4;
        Cube *cubePointer;
        cubePointer = new Cube ();
        cubePointer->setPosition (Vector3d (i / 10 % 10 * separation, i / 100 % 10 * separation + (separation * ((int (i / 1000)) +1)), separation + i % 10 * separation));
        cubeList.push_back (cubePointer);
        if ( i%4 && false) //snake mode. will slow down things. i%4 sets the number of cubes per snake.
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
    }

    log->loadscreen (LOG_INFO, "Creating the track ground");
    log->put (LOG_INFO, "Creating the ode plane");
    dCreatePlane (World::getWorldPointer()->spaceID, 0, 0, 1, groundHeight);
    Ogre::Plane plane;
    plane.normal = Ogre::Vector3(0,0,1);
    plane.d = -groundHeight;
    Ogre::SceneManager* pOgreSceneManager = SystemData::getSystemDataPointer()->ogreSceneManager;
    Ogre::MeshManager::getSingleton().createPlane("Ground plane",plane, 1000,1000,1,1,true,1,20,20,Ogre::Vector3::UNIT_Y);
    Ogre::Entity* pPlaneEnt = pOgreSceneManager->createEntity("plane", "Ground plane");
    pPlaneEnt->setMaterialName(groundMaterialName.c_str());
    pPlaneEnt->setCastShadows(true);
    pOgreSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(pPlaneEnt);
    trackBodyID = dBodyCreate (World::getWorldPointer ()->ghostWorldID);

    // FIXME should be part of the world, not the track
    log->loadscreen (LOG_INFO, "Creating the track sky");
    Ogre::Quaternion rotationToZAxis;
    rotationToZAxis.FromRotationMatrix (Ogre::Matrix3 (1, 0, 0, 0, 0, -1, 0, 1, 0));
    SystemData::getSystemDataPointer()->ogreSceneManager->setSkyBox (true, skyMaterialName.c_str(), skyDistance, skyDrawFirst, rotationToZAxis);
}


void Track::processXmlVehiclePositionNode (DOMNode * n)
{
    if (n->hasAttributes ())
    {
        DOMNamedNodeMap *attList = n->getAttributes ();
        int nSize = attList->getLength ();
        std::string index = "0";
        Vector3d position (0, 0, 0);
        Vector3d rotation (0, 0, 0);
        for (int i = 0; i < nSize; ++i)
        {
            DOMAttr *attNode = (DOMAttr *) attList->item (i);
            std::string attribute;
            assignXmlString (attribute, attNode->getName());
            if (attribute == "index")
            {
                index.clear();
                assignXmlString (index, attNode->getValue());
                log->format (LOG_TRACE, "Found the position index: %s", index.c_str());
            }
            if (attribute == "position")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the position: %s", attribute.c_str());
                position = stov3d(attribute);
            }
            if (attribute == "rotation")
            {
                attribute.clear();
                assignXmlString (attribute, attNode->getValue());
                log->format (LOG_TRACE, "Found the rotation: %s", attribute.c_str());
                rotation = stov3d(attribute);
            }
            attribute.clear();
        }
        rotation.degreesToRadians();
        VehiclePosition * tmpVehicle = new VehiclePosition (position, rotation);
        vehiclePositionMap[index] = tmpVehicle;
    }
}
