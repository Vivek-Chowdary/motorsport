
/******************************************************************************
*
* Copyright (C) 2004 Bruno González Campo (stenyak@users.sourceforge.net)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
******************************************************************************/

#include "world.hpp"

World *World::worldPointer = NULL;

World *World::getWorldPointer ()
{
    if (worldPointer == 0)
    {
        std::cout << "WARNING: Auto-creating a default world!" << std::endl;
        worldPointer = new World("../data/worlds/default.xml");
    }
    return (worldPointer);
}

World::World (char * xmlFilename)
{
    log = new LogEngine(LOG_TRACE, "WRL");
    worldPointer = this;

    XmlFile* xmlFile = new XmlFile (xmlFilename);
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;
}

World::~World ()
{
    // unload the bodies from memory
    log->put (LOG_INFO, "Unloading bodies from memory...");
    int size = bodyList.size ();
    for (int i = 0; i < size; i++)
    {
        delete bodyList[i];
    }
    bodyList.clear ();
    
    // unload the cubes from memory
    log->put (LOG_INFO, "Unloading cubes from memory...");
    size = cubeList.size ();
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

    log->put (LOG_INFO, "Destroying ODE world");
    dSpaceDestroy (spaceID);
    log->put (LOG_INFO, "Destroying ODE main collision space");
    dWorldDestroy (worldID);
    log->put (LOG_INFO, "Destroying ODE joints group");
    dJointGroupDestroy (jointGroupID);

    worldPointer = NULL;
    delete log;
}


void World::processXmlRootNode (DOMNode * n)
{
    name.assign ("None");
    description.assign ("None");
    double gravityX = 0.0;
    double gravityY = 0.0;
    double gravityZ = 0.0;
    bool useTrackCamera = true;    //if false, use car camera
    std::string carDirectory = "testCar";
    int carStartPosition = 0;   //first 'grid' position
    std::string driver = "user"; //still no other option, but in the future: ai, net, user, replay, ghostReplay, none, etc...
    std::string trackDirectory = "testingGround";

    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            log->format (LOG_INFO, "Name: %s", name.c_str());
            if (name == "world")
            {
                log->put (LOG_INFO, "Found the world element.");
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
                            log->format (LOG_INFO, "Found the world name: %s", name.c_str());
                        }
                        if (attribute == "description")
                        {
                            description.clear();
                            assignXmlString (description, attNode->getValue());
                            log->format (LOG_INFO, "Found the world description: %s", description.c_str());
                        }
                        if (attribute == "camera")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            useTrackCamera = stob (attribute);
                            log->format (LOG_INFO, "Found the selected world camera: %s camera", attribute.c_str());
                        }
                        if (attribute == "gravityX")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            gravityX = stod (attribute);
                            log->format (LOG_INFO, "Found the world gravity X module: %s m/s^2", attribute.c_str());
                        }
                        if (attribute == "gravityY")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            gravityY = stod (attribute);
                            log->format (LOG_INFO, "Found the world gravity Y module: %s m/s^2", attribute.c_str());
                        }
                        if (attribute == "gravityZ")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            gravityZ = stod (attribute);
                            log->format (LOG_INFO, "Found the world gravity Z module: %s m/s^2", attribute.c_str());
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
                            name.clear();
                            assignXmlString (name, n->getNodeName());
                            log->format (LOG_INFO, "Name: %s", name.c_str());
                            if (name == "vehicle")
                            {
                                log->put (LOG_INFO, "Found the vehicle");
                                if (n->hasAttributes ())
                                {
                                    DOMNamedNodeMap *attList = n->getAttributes ();
                                    int nSize = attList->getLength ();
                                    for (int i = 0; i < nSize; ++i)
                                    {
                                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                                        std::string attribute;
                                        assignXmlString (attribute, attNode->getName());
                                        if (attribute == "directory")
                                        {
                                            carDirectory.clear();
                                            assignXmlString (carDirectory, attNode->getValue());
                                            log->format (LOG_INFO, "Found the car directory: %s", carDirectory.c_str());
                                        }
                                        if (attribute == "driver")
                                        {
                                            driver.clear();
                                            assignXmlString (driver, attNode->getValue());
                                            log->format (LOG_INFO, "Found the car driver: %s", driver.c_str());
                                        }
                                        if (attribute == "startPosition")
                                        {
                                            attribute.clear();
                                            assignXmlString (attribute, attNode->getValue());
                                            log->format (LOG_INFO, "Found the car start position index: %s", attribute.c_str());
                                            carStartPosition = stoi (attribute);
                                        }
                                        attribute.clear();
                                    }
                                }
                            }
                            if (name == "track")
                            {
                                log->put (LOG_INFO, "Found the track");
                                if (n->hasAttributes ())
                                {
                                    DOMNamedNodeMap *attList = n->getAttributes ();
                                    int nSize = attList->getLength ();
                                    for (int i = 0; i < nSize; ++i)
                                    {
                                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                                        std::string attribute;
                                        assignXmlString (attribute, attNode->getName());
                                        if (attribute == "directory")
                                        {
                                            trackDirectory.clear();
                                            assignXmlString (trackDirectory, attNode->getValue());
                                            log->format (LOG_INFO, "Found the track directory: %s", trackDirectory.c_str());
                                        }
                                        attribute.clear();
                                    }
                                }
                            }
                        }
                    }
                }
            }
            name.clear();
        }
    }
    log->put (LOG_INFO, "Temporary parsing data already loaded into memory...");

/* /////////////////////////////////// STILL THOSE 2 VARS TO USE TODO
    bool useTrackCamera = true;    //if false, use car camera
    std::string driver = "user"; //still no other option, but in the future: ai, net, user, replay, ghostReplay, none, etc...
/////////////////////////////////// */

    log->put (LOG_INFO, "Creating ODE world");
    worldID = dWorldCreate ();
    spaceID = dHashSpaceCreate (0);
    jointGroupID = dJointGroupCreate (0);

    if (double cfmValue = SystemData::getSystemDataPointer()->getCfmValue() != -1)
    {
        log->put (LOG_INFO, "Setting ODE cfm value");
        dWorldSetCFM (worldID, cfmValue);
    }
    if (double erpValue = SystemData::getSystemDataPointer()->getErpValue() != -1)
    {
        log->put (LOG_INFO, "Setting ODE erp value");
        dWorldSetERP (worldID, erpValue);
    }

    log->put ( LOG_INFO, "Setting ODE world gravity");
    dWorldSetGravity (worldID, gravityX, gravityY, gravityZ);

    //track = new Track ("../data/tracks" + trackDirectory + "/track.xml");
    //car = new Car ("../data/vehicles/" + carDirectory + "/vehicle.xml"); // TODO
    std::string tmpPath = ("../data/vehicles/");
    tmpPath.append (carDirectory);
    tmpPath.append ("/body.xml");
    body = new Body ( tmpPath );
    //car->setPosition (track->getPositionX(carStartPosition), track->getPositionY(carStartPosition), track->getPositionZ(carStartPosition));  //TODO
    //body->setPosition (track->getPositionX(carStartPosition), track->getPositionY(carStartPosition), track->getPositionZ(carStartPosition));
    body->setPosition (-5.0, -5.0, 3.0);

    // small hack to continue using the vectors:
    bodyList.push_back (body);

//////// OLD CODE, WILL BE REUSED FOR SEVERAL CARS
//    for (int i = 0; i < 10; i++)
//    {
//        const int separation = 5;
//        Body *bodyPointer;
//        bodyPointer = new Body ("../data/vehicles/testCar/body.xml");
//        bodyPointer->setPosition (- separation - i / 10 % 10 * separation, - separation - i % 10 * separation, i / 100 % 10 * separation + (separation * ((int (i / 1000)) +1)));
//        bodyList.push_back (bodyPointer);
//    }
//////////////////////////////////////////////////

    // Create the cubes. Must be moved to track constructor.
    int numberOfCubes = 20;
    log->format (LOG_INFO, "Creating an array of %i cubes", numberOfCubes);
    for (int i = 0; i < numberOfCubes; i++)
    {
        log->format (LOG_VERBOSE, "Adding cube number %i", i);
        const int separation = 4;
        Cube *cubePointer;
        cubePointer = new Cube ("../data/parts/cube/cube.xml");
        cubePointer->setPosition (i / 10 % 10 * separation, i / 100 % 10 * separation + (separation * ((int (i / 1000)) +1)), separation + i % 10 * separation);
        cubeList.push_back (cubePointer);
    }
    
    // Must be moved to track and vehicle constructors.
    int numberOfCameras = 4;
    log->format (LOG_INFO, "Creating %i cameras", numberOfCameras);
    for (int i = 0; i < numberOfCameras; i++)
    {
        Camera *cameraPointer = new Camera (i, -20, -20, 5, 0, 0, 0);
        cameraList.push_back (cameraPointer);
    }
    
    log->put (LOG_INFO, "Setting camera viewport");
    Ogre::Viewport * vp = SystemData::getSystemDataPointer()->ogreWindow->addViewport (cameraList[0]->ogreCamera);
    log->put (LOG_INFO, "Setting bg color");
    vp->setBackgroundColour (Ogre::ColourValue (0, 0, 0));
    Ogre::Quaternion rotationToZAxis;
    rotationToZAxis.FromRotationMatrix (Ogre::Matrix3 (1, 0, 0, 0, 0, -1, 0, 1, 0));
    SystemData::getSystemDataPointer()->ogreSceneManager->setSkyBox (true, "skyboxMaterial", 5000, true, rotationToZAxis);
    log->put (LOG_INFO, "Creating the ode plane");
    dCreatePlane (spaceID, 0, 0, 1, 0);
    Ogre::Plane plane; 
    plane.normal = Ogre::Vector3::UNIT_Z; 
    plane.d = 0; 
    Ogre::SceneManager* pOgreSceneManager = SystemData::getSystemDataPointer()->ogreSceneManager; 
    Ogre::MeshManager::getSingleton().createPlane("Myplane",plane, 1000,1000,1,1,true,1,20,20,Ogre::Vector3::UNIT_Y); 
    Ogre::Entity* pPlaneEnt = pOgreSceneManager->createEntity("plane", "Myplane"); //"MyPlane" name,generated on the fly with instancesNumber
    pPlaneEnt->setMaterialName("groundMaterial"); 
    pPlaneEnt->setCastShadows(true); 
    pOgreSceneManager->getRootSceneNode()->createChildSceneNode()->attachObject(pPlaneEnt);

    // Clean up things, leave others in memory (world properties).
    log->put (LOG_INFO, "Unloading temporary parsing data from memory...");
    carDirectory.clear();
    driver.clear();
    trackDirectory.clear();
}
