
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
    if (worldPointer != 0)
    {
        delete this;
    } else {
        log = new LogEngine(LOG_TRACE, "WRL");
        worldPointer = this;

        log->format(LOG_INFO,"Reading %s world file", xmlFilename);
        XmlFile* xmlFile = new XmlFile (xmlFilename);
        processXmlRootNode (xmlFile->getRootNode());
        delete xmlFile;
    }
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
    
    size = trackList.size ();
    for (int i = 0; i < size; i++)
    {
        delete trackList[i];
    }
    trackList.clear ();
    
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
    std::string carStartPosition = "0";   //first 'grid' position
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
                        if (attribute == "useTrackCamera")
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
                                            carStartPosition.clear();
                                            assignXmlString (carStartPosition, attNode->getValue());
                                            log->format (LOG_INFO, "Found the car start position index: %s", carStartPosition.c_str());
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

    std::string tmpPath = ("../data/tracks/");
    tmpPath.append (trackDirectory);
    tmpPath.append ("/track.xml");
    Track * tmpTrack = new Track (tmpPath);
    //tmpTrack->setPosition (0.0, 0.0, 0.0); //evo2 maybe... ;)
    trackList.push_back (tmpTrack);

    tmpPath = ("../data/vehicles/");
    tmpPath.append (carDirectory);
    tmpPath.append ("/body.xml");
    Body * tmpBody = new Body (tmpPath);
    Vector3d tmpPos = tmpTrack->vehiclePositionMap[carStartPosition]->getPosition();
    tmpBody->setPosition (tmpPos.x, tmpPos.y, tmpPos.z);
    Vector3d tmpRot = tmpTrack->vehiclePositionMap[carStartPosition]->getRotation();
    tmpBody->setRotation (tmpPos.x, tmpPos.y, tmpPos.z);
    bodyList.push_back (tmpBody);

    Ogre::Viewport * vp;
    log->put (LOG_INFO, "Setting camera viewport");
    if (useTrackCamera)
    {
        vp = SystemData::getSystemDataPointer()->ogreWindow->addViewport (trackList[0]->cameraList[0]->ogreCamera);
    } else {
        //don't use track camera; use vehicle camera. until there's vehicles, let's use 2nd track camera instead.
        vp = SystemData::getSystemDataPointer()->ogreWindow->addViewport (trackList[0]->cameraList[1]->ogreCamera);
    }
    log->put (LOG_INFO, "Setting bg color");
    vp->setBackgroundColour (Ogre::ColourValue (0, 0, 0));

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

    // Clean up things, leave others in memory (world properties).
    log->put (LOG_INFO, "Unloading temporary parsing data from memory...");
    carDirectory.clear();
    driver.clear();
    trackDirectory.clear();
}
