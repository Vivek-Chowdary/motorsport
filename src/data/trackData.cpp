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
    log = new LogEngine (LOG_DEVELOPER, "TRK");
    log->put (LOG_CCREATOR, "Starting to parse the track xml file");
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
                                            assignXmlString (attribute, attNode->getValue());
                                            log->format (LOG_CCREATOR, "Found the number of cubes: %s", attribute.c_str());
                                            numberOfCubes = stoi (attribute);
                                        }
                                    }
                                }
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

    //load cubes
    log->loadscreen (LOG_CCREATOR, "Creating an array of %i cubes", numberOfCubes);
    for (int i = 0; i < numberOfCubes; i++)
    {
        log->format (LOG_DEVELOPER, "Adding cube number %i", i);
        const int separation = 4;
        Cube *cubePointer;
        cubePointer = new Cube ();
        cubePointer->setPosition (Vector3d (i / 10 % 10 * separation, i / 100 % 10 * separation + (separation * ((int (i / 1000)) +1)), separation + i % 10 * separation));
        cubeList.push_back (cubePointer);
        cubePointer->stepGraphics();
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

    log->loadscreen (LOG_CCREATOR, "Creating the track ground");
    log->put (LOG_DEVELOPER, "Creating the ode plane");
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
    log->loadscreen (LOG_CCREATOR, "Creating the track sky");
    Ogre::Quaternion rotationToZAxis;
    rotationToZAxis.FromRotationMatrix (Ogre::Matrix3 (1, 0, 0, 0, 0, -1, 0, 1, 0));
    SystemData::getSystemDataPointer()->ogreSceneManager->setSkyBox (true, skyMaterialName.c_str(), skyDistance, skyDrawFirst, rotationToZAxis);
}
