/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "physicsEngine.hpp"

#include "math.h"
#include "system.hpp"
#include "world.hpp"
#include "cube.hpp"
#include "vehicle.hpp"
#include "camera.hpp"
#include "xmlParser.hpp"
#include "track.hpp"
#include "system.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "ode/ode.h"

PhysicsEngine::PhysicsEngine ()
{
    XmlFile * xmlFile = new XmlFile ("physicsConfig.xml");
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;
}


void PhysicsEngine::nearCallback (void *data, dGeomID o1, dGeomID o2)
{
    int i, n;
    dBodyID b1 = dGeomGetBody (o1);
    dBodyID b2 = dGeomGetBody (o2);
    if (b1 && b2 && dAreConnected (b1, b2))
        return;

    const int N = 10;
    dContact contact[N];
    n = dCollide (o1, o2, N, &contact[0].geom, sizeof (dContact));
    if (n > 0)
    {
        for (i = 0; i < n; i++)
        {
            contact[i].surface.mode = dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;
            if (dGeomGetClass (o1) == dSphereClass || dGeomGetClass (o2) == dSphereClass)
                contact[i].surface.mu = 20;
            else
                contact[i].surface.mu = 0.5;
            contact[i].surface.slip1 = 0.0;
            contact[i].surface.slip2 = 0.0;
            contact[i].surface.soft_erp = 0.8;
            contact[i].surface.soft_cfm = 0.01;
            dJointID c = dJointCreateContact (World::getWorldPointer ()->worldID, World::getWorldPointer ()->jointGroupID, contact + i);
            dJointAttach (c, dGeomGetBody (o1), dGeomGetBody (o2));
        }
    }
}

int PhysicsEngine::computeStep (void)
//makes the graphics engine draw one frame
{
    // mega-verbosity
    log->put (LOG_TRACE, "Doing an step: calculating a physics step");

    int size = World::getWorldPointer ()->trackList[0]->cubeList.size ();
    for (int i = 0; i < size; i++)
    {
        World::getWorldPointer()->trackList[0]->cubeList[i]->stepPhysics ();
    }

    size = World::getWorldPointer ()->vehicleList.size ();
    for (int i = 0; i < size; i++)
    {
        World::getWorldPointer ()->vehicleList[i]->stepPhysics ();
    }

    int numberOfCameras = World::getWorldPointer ()->trackList[0]->cameraList.size ();
    for (int currentCamera = 0; currentCamera < numberOfCameras; currentCamera++)
    {
        World::getWorldPointer ()->trackList[0]->cameraList[currentCamera]->stepPhysics ();
    }
    numberOfCameras = World::getWorldPointer ()->vehicleList[0]->cameraList.size ();
    for (int currentCamera = 0; currentCamera < numberOfCameras; currentCamera++)
    {
        World::getWorldPointer ()->vehicleList[0]->cameraList[currentCamera]->stepPhysics ();
    }
                        

    dSpaceCollide (World::getWorldPointer ()->spaceID, 0, &nearCallback);
    switch (stepType)
    {
    default:
    case 1:
        // traditional (x^y), theorycally slowest, and most accurate physics calculations:
        dWorldStep (World::getWorldPointer ()->worldID, systemData->physicsTimeStep);
        break;
    case 2:
        // alternative (x*y), fastest and less accurate physics calculations:
        dWorldStepFast1 (World::getWorldPointer ()->worldID, systemData->physicsTimeStep, dWorldStepFast1MaxIterations);
    }
    dJointGroupEmpty (World::getWorldPointer ()->jointGroupID);

    return (0);
}

PhysicsEngine::~PhysicsEngine (void)
{
    dCloseODE ();

    // finally stop the log engine
    delete log;
}

void PhysicsEngine::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    LOG_LEVEL localLogLevel = LOG_TRACE;
    std::string localLogName = "FSX" ;
    int frequency = 250;
    int timeScale = 1;
    int pauseStep = 0;
    SystemData::getSystemDataPointer()->setCfmValue (-1);
    SystemData::getSystemDataPointer()->setErpValue (-1);
    int stepType = 1;
    int dWorldStepFast1MaxIterations = 100;

    LogEngine * tmpLog = new LogEngine (LOG_TRACE, "XML");
    if (n)
    {
        if (n->getNodeType () == XERCES_CPP_NAMESPACE::DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            tmpLog->format (LOG_INFO, "Name: %s", name.c_str());
            if (name == "physicsConfig")
            {
                tmpLog->put (LOG_INFO, "Found the physics engine config element.");
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
                        if (attribute == "frequency")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            frequency = stoi (attribute);
                            tmpLog->format (LOG_INFO, "Found the frecuency: %s", attribute.c_str());
                        }
                        if (attribute == "timeScale")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            timeScale = stoi (attribute);
                            tmpLog->format (LOG_INFO, "Found the time scale: %s", attribute.c_str());
                        }
                        if (attribute == "pauseStep")
                        {
                            attribute.clear();
                            assignXmlString (attribute, attNode->getValue());
                            pauseStep = stoi (attribute);
                            tmpLog->format (LOG_INFO, "Found the pause step: %s", attribute.c_str());
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
                            tmpLog->format (LOG_INFO, "Name: %s", name.c_str());
                            if (name == "ode")
                            {
                                tmpLog->put (LOG_INFO, "Found the ode config element.");
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
                                        if (attribute == "cfmValue")
                                        {
                                            attribute.clear();
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_INFO, "Found the constraint force mixing factor (CFM): %s", attribute.c_str());
                                            if (attribute != "default")
                                            {
                                                SystemData::getSystemDataPointer()->setCfmValue (stod (attribute));
                                            }
                                        }
                                        if (attribute == "erpValue")
                                        {
                                            attribute.clear();
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_INFO, "Found the error reduction parameter (ERP): %s", attribute.c_str());
                                            if (attribute != "default")
                                            {
                                                SystemData::getSystemDataPointer()->setErpValue (stod (attribute));
                                            }
                                        }
                                        if (attribute == "stepType")
                                        {
                                            attribute.clear();
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_INFO, "Found the type of stepping to be used in ODE: %s", attribute.c_str());
                                            if (attribute == "dWorldStep")
                                                stepType = 1;
                                            if (attribute == "dWorldStepFast1")
                                                stepType = 2;
                                        }
                                        if (attribute == "dWorldStepFast1MaxIterations")
                                        {
                                            attribute.clear();
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_INFO, "Found the max. number of iterations to be calculated with dWorldStepFast1: %s", attribute.c_str());

                                            dWorldStepFast1MaxIterations = stoi (attribute);
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
    delete tmpLog;

    // first of all start the logger (automatically logs the start of itself)
    log = new LogEngine (localLogLevel, localLogName.c_str());
    log->put (LOG_INFO, "Temporary parsing data already loaded into memory...");

    // get the direction of the graphics data
    log->put (LOG_INFO, "Setting up data pointers...");
    systemData = SystemData::getSystemDataPointer ();

    log->put (LOG_INFO, "Setting physics data");
    systemData->physicsDesiredFrequency = frequency;
    systemData->physicsTimeStep = 1000 / systemData->physicsDesiredFrequency;
    systemData->timeScale = timeScale;
    systemData->pauseStep = pauseStep;
    log->format (LOG_INFO, "Physics rate set @ %i Hz (%i ms)", systemData->physicsDesiredFrequency, systemData->physicsTimeStep);

    localLogName.clear();
}
