
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

#include "dataEngine.hpp"

DataEngine::DataEngine ()
{
    XmlFile * xmlFile = new XmlFile ("dataConfig.xml");
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;
    
    log->put (LOG_INFO, "Setting up data pointers...");
    // we tell the dataEngine where to find/store all the data in memory.
    worldData = WorldData::getWorldDataPointer ();  // world data is for the simulated world data (cars,
    // track, weather, etc...)
    systemData = SystemData::getSystemDataPointer ();   // system data is for the rest of things (screen
    // resolution, 
}

int DataEngine::loadWorldData (void)
{
    // Create the infinite plane
    log->put (LOG_INFO, "Creating the ode plane");
    dCreatePlane (WorldData::getWorldDataPointer ()->spaceID, 0, 0, 1, 0);

    // create the camera and initialize it
    int numberOfCameras = 4;
    log->format (LOG_INFO, "Creating %i cameras", numberOfCameras);
    int i;
	for (i = 0; i < numberOfCameras; i++)
    {
        Camera *cameraPointer = new Camera (i, -20, -20, 5, 0, 0, 0);
        WorldData::getWorldDataPointer()->cameraList.push_back (cameraPointer);
    }
    log->put (LOG_INFO, "Setting camera viewport");
    Ogre::Viewport * vp = systemData->ogreWindow->addViewport (WorldData::getWorldDataPointer()->cameraList[0]->ogreCamera);
    log->put (LOG_INFO, "Setting bg color");
    vp->setBackgroundColour (Ogre::ColourValue (0, 0, 0));

    // Create the skybox
    Ogre::Quaternion rotationToZAxis;
    rotationToZAxis.FromRotationMatrix (Ogre::Matrix3 (1, 0, 0, 0, 0, -1, 0, 1, 0));
    systemData->ogreSceneManager->setSkyBox (true, "skyboxMaterial", 5000, true, rotationToZAxis);

    // Create the bodies
    int numberOfBodies = 2;
    log->format (LOG_INFO, "Creating an array of %i bodies", numberOfBodies);
    for (int i = 0; i < numberOfBodies; i++)
    {
        log->format (LOG_VERBOSE, "Adding body number %i", i);
        const int separation = 5;
        Body *bodyPointer;
        bodyPointer = new Body ("../data/body.xml");
        bodyPointer->setPosition (- separation - i / 10 % 10 * separation, - separation - i % 10 * separation, i / 100 % 10 * separation + (separation * ((int (i / 1000)) +1)));
        WorldData::getWorldDataPointer()->bodyList.push_back (bodyPointer);
    }

    // Create the cubes
    int numberOfCubes = 20;
    log->format (LOG_INFO, "Creating an array of %i cubes", numberOfCubes);
    for (i = 0; i < numberOfCubes; i++)
    {
        log->format (LOG_VERBOSE, "Adding cube number %i", i);
        const int separation = 4;
        Cube *cubePointer;
        cubePointer = new Cube ("../data/cube.xml");
        cubePointer->setPosition (i / 10 % 10 * separation, i / 100 % 10 * separation + (separation * ((int (i / 1000)) +1)), separation + i % 10 * separation);
        WorldData::getWorldDataPointer()->cubeList.push_back (cubePointer);
    }

    return (0);
}

int DataEngine::unloadWorldData (void)
{
    // unload the bodies from memory
    log->put (LOG_INFO, "Unloading bodies from memory...");
    int size = worldData->bodyList.size ();
    for (int i = 0; i < size; i++)
    {
        delete worldData->bodyList[i];
    }
    WorldData::getWorldDataPointer()->bodyList.clear ();
    
    // unload the cubes from memory
    log->put (LOG_INFO, "Unloading cubes from memory...");
    size = worldData->cubeList.size ();
    for (int i = 0; i < size; i++)
    {
        delete worldData->cubeList[i];
    }
    WorldData::getWorldDataPointer()->cubeList.clear ();
    
    // unload the cameras from memory
    log->put (LOG_INFO, "Unloading cameras from memory...");
    size = worldData->cameraList.size ();
    for (int i = 0; i < size; i++)
    {
        delete worldData->cameraList[i];
    }
    WorldData::getWorldDataPointer()->cameraList.clear ();

    return (0);
}

DataEngine::~DataEngine (void)
{
    // finally stop the log engine
    delete log;
}

void DataEngine::processXmlRootNode (DOMNode * n)
{
    DataData * data = new DataData;
    LogEngine * tmpLog = new LogEngine (LOG_TRACE, "XML");
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            char *name = XMLString::transcode (n->getNodeName ());
            tmpLog->format (LOG_INFO, "Name: %s", name);

            if (!strncmp (name, "dataConfig", 11))
            {
                tmpLog->put (LOG_INFO, "Found the data engine config element.");
                if (n->hasAttributes ())
                {
                    // get all the attributes of the node
                    DOMNamedNodeMap *pAttributes = n->getAttributes ();
                    int nSize = pAttributes->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *pAttributeNode = (DOMAttr *) pAttributes->item (i);
                        char *name = XMLString::transcode (pAttributeNode->getName ());
                        if (!strncmp (name, "localLogLevel", 14))
                        {
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            tmpLog->format (LOG_INFO, "\tFound the local log level: %s", name);

                            if (!strncmp (name, "LOG_ERROR", 10))
                                (*(DataData *) data).localLogLevel = LOG_ERROR;
                            if (!strncmp (name, "LOG_WARNING", 13))
                                (*(DataData *) data).localLogLevel = LOG_WARNING;
                            if (!strncmp (name, "LOG_INFO", 9))
                                (*(DataData *) data).localLogLevel = LOG_INFO;
                            if (!strncmp (name, "LOG_VERBOSE", 12))
                                (*(DataData *) data).localLogLevel = LOG_VERBOSE;
                            if (!strncmp (name, "LOG_TRACE", 9))
                                (*(DataData *) data).localLogLevel = LOG_TRACE;
                        }

                        if (!strncmp (name, "localLogName", 13))
                        {
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            tmpLog->format (LOG_INFO, "\tFound the log name: %s", name);

                            (*(DataData *) data).localLogName = new char[strlen (name) + 1];
                            strncpy ((*(DataData *) data).localLogName, name, strlen (name) + 1);
                        }
                        XMLString::release (&name);
                    }
                }
            }
        }
    }
    delete tmpLog;
    
    log = new LogEngine (data->localLogLevel, data->localLogName);
    log->put (LOG_INFO, "Temporary parsing data already loaded into memory...");

    log->put (LOG_INFO, "Unloading temporary parsing data from memory");
    delete[](data->localLogName);
    delete data;
}
