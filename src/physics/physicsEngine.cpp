
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

#include "physicsEngine.hpp"

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
        World::getWorldPointer ()->trackList[0]->cubeList[i]->stepPhysics ();
    }

    size = World::getWorldPointer ()->vehicleList.size ();
    for (int i = 0; i < size; i++)
    {
        World::getWorldPointer ()->vehicleList[i]->stepPhysics ();
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

    // camera should be a physics object?
    {
        float x = 0, z = 0;
        // translation of the camera, advancing or strafing
        x += (World::getWorldPointer ()->getActiveCamera()->goRight) ? systemData->physicsTimeStep : 0;
        x -= (World::getWorldPointer ()->getActiveCamera()->goLeft) ? systemData->physicsTimeStep : 0;
        z -= (World::getWorldPointer ()->getActiveCamera()->goForward) ? systemData->physicsTimeStep : 0;
        z += (World::getWorldPointer ()->getActiveCamera()->goBack) ? systemData->physicsTimeStep : 0;
        x /= 100;
        z /= 100;
        World::getWorldPointer ()->getActiveCamera()->ogreCamera->moveRelative (Ogre::Vector3 (x, 0, z));
    }
    {
        float x = 0, z = 0;
        x -= World::getWorldPointer ()->getActiveCamera()->getRotateRight ();
        x += World::getWorldPointer ()->getActiveCamera()->getRotateLeft ();
        z += World::getWorldPointer ()->getActiveCamera()->getRotateUp ();
        z -= World::getWorldPointer ()->getActiveCamera()->getRotateDown ();
        x /= 100;
        z /= 100;
        World::getWorldPointer ()->getActiveCamera()->ogreCamera->yaw (x * (systemData->physicsTimeStep));
        World::getWorldPointer ()->getActiveCamera()->ogreCamera->pitch (z * (systemData->physicsTimeStep));
    }

    return (0);
}

PhysicsEngine::~PhysicsEngine (void)
{
    dCloseODE ();

    // finally stop the log engine
    delete log;
}

void PhysicsEngine::processXmlRootNode (DOMNode * n)
{
    LOG_LEVEL localLogLevel = LOG_TRACE;
    std::string localLogName = "FSX" ;
    int frequency = 250;
    SystemData::getSystemDataPointer()->setCfmValue (-1);
    SystemData::getSystemDataPointer()->setErpValue (-1);
    int stepType = 1;
    int dWorldStepFast1MaxIterations = 100;

    LogEngine * tmpLog = new LogEngine (LOG_TRACE, "XML");
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
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
                            tmpLog->format (LOG_INFO, "\tFound the local log level: %s", attribute.c_str());
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
                            tmpLog->format (LOG_INFO, "\tFound the frecuency: %s", attribute.c_str());
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
                                            tmpLog->format (LOG_INFO, "\tFound the constraint force mixing factor (CFM): %s", attribute.c_str());
                                            if (attribute != "default")
                                            {
                                                SystemData::getSystemDataPointer()->setCfmValue (stod (attribute));
                                            }
                                        }
                                        if (attribute == "erpValue")
                                        {
                                            attribute.clear();
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_INFO, "\tFound the error reduction parameter (ERP): %s", attribute.c_str());
                                            if (attribute != "default")
                                            {
                                                SystemData::getSystemDataPointer()->setErpValue (stod (attribute));
                                            }
                                        }
                                        if (attribute == "stepType")
                                        {
                                            attribute.clear();
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_INFO, "\tFound the type of stepping to be used in ODE: %s", attribute.c_str());
                                            if (attribute == "dWorldStep")
                                                stepType = 1;
                                            if (attribute == "dWorldStepFast1")
                                                stepType = 2;
                                        }
                                        if (attribute == "dWorldStepFast1MaxIterations")
                                        {
                                            attribute.clear();
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_INFO, "\tFound the max. number of iterations to be calculated with dWorldStepFast1: %s", attribute.c_str());

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
    log->format (LOG_INFO, "Physics rate set @ %i Hz (%i ms)", systemData->physicsDesiredFrequency, systemData->physicsTimeStep);

    localLogName.clear();
}
