
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
    LOG_LEVEL localLogLevel = LOG_TRACE;
    std::string localLogName = "DAT";
    LogEngine * tmpLog = new LogEngine (LOG_TRACE, "XML");
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            tmpLog->format (LOG_INFO, "Name: %s", name.c_str());
            if (name == "dataConfig")
            {
                tmpLog->put (LOG_INFO, "Found the data engine config element.");
                if (n->hasAttributes ())
                {
                    // get all the attributes of the node
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "localLogLevel")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            localLogLevel = stologlevel (attribute);
                            tmpLog->format (LOG_INFO, "Found the local log level: %s", attribute.c_str());
                        }
                        if (attribute == "localLogName")
                        {
                            localLogName.clear();
                            assignXmlString (localLogName, attNode->getValue());
                            tmpLog->format (LOG_INFO, "Found the log name: %s", localLogName.c_str());
                        }
                        attribute.clear();
                    }
                }
            }
            name.clear();
        }
    }
    delete tmpLog;
    
    log = new LogEngine (localLogLevel, localLogName.c_str());
    localLogName.clear();
    log->put (LOG_INFO, "All config has been parsed");
}
