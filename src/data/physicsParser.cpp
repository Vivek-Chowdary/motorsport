
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

void PhysicsEngine::processPhysicsConfigFile (DOMNode * n, void *data)
{
    LogEngine * log = new LogEngine (LOG_TRACE, "XML");
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            char *name = XMLString::transcode (n->getNodeName ());
            log->format (LOG_INFO, "Name: %s", name);

            if (!strncmp (name, "physicsConfig", 14))
            {
                log->put (LOG_INFO, "Found the physics engine config element.");
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
                            log->format (LOG_INFO, "\tFound the local log level: %s", name);

                            if (!strncmp (name, "LOG_ERROR", 10))
                                (*(PhysicsData *) data).localLogLevel = LOG_ERROR;
                            if (!strncmp (name, "LOG_WARNING", 13))
                                (*(PhysicsData *) data).localLogLevel = LOG_WARNING;
                            if (!strncmp (name, "LOG_INFO", 9))
                                (*(PhysicsData *) data).localLogLevel = LOG_INFO;
                            if (!strncmp (name, "LOG_VERBOSE", 12))
                                (*(PhysicsData *) data).localLogLevel = LOG_VERBOSE;
                            if (!strncmp (name, "LOG_TRACE", 9))
                                (*(PhysicsData *) data).localLogLevel = LOG_TRACE;
                        }

                        if (!strncmp (name, "localLogName", 13))
                        {
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            log->format (LOG_INFO, "\tFound the log name: %s", name);

                            (*(PhysicsData *) data).localLogName = new char[strlen (name) + 1];
                            strncpy ((*(PhysicsData *) data).localLogName, name, strlen (name) + 1);
                        }
                        if (!strncmp (name, "frequency", 10))
                        {
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            log->format (LOG_INFO, "\tFound the frecuency: %s", name);

                            (*(PhysicsData *) data).frequency = atoi (name);
                        }
                        XMLString::release (&name);
                    }
                }
                for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
                {
                    if (n)
                    {
                        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
                        {
                            char *name = XMLString::transcode (n->getNodeName ());
                            log->format (LOG_INFO, "Name: %s", name);
                            if (!strncmp (name, "ode", 4))
                            {
                                log->put (LOG_INFO, "Found the ode config element.");
                                if (n->hasAttributes ())
                                {
                                    // get all the attributes of the node
                                    DOMNamedNodeMap *pAttributes = n->getAttributes ();
                                    int nSize = pAttributes->getLength ();

                                    for (int i = 0; i < nSize; ++i)
                                    {
                                        DOMAttr *pAttributeNode = (DOMAttr *) pAttributes->item (i);
                                        char *name = XMLString::transcode (pAttributeNode->getName ());
                                        if (!strncmp (name, "cfmValue", 9))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "\tFound the constraint force mixing factor (CFM): %s", name);

                                            if (strncmp (name, "default", 8))
                                            {
                                                (*(PhysicsData *) data).cfmValue = atof (name);
                                            }
                                        }
                                        if (!strncmp (name, "erpValue", 9))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "\tFound the error reduction parameter (ERP): %s", name);

                                            if (strncmp (name, "default", 8))
                                            {
                                                (*(PhysicsData *) data).erpValue = atof (name);
                                            }
                                        }
                                        if (!strncmp (name, "stepType", 8))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "\tFound the type of stepping to be used in ODE: %s", name);

                                            if (!strncmp (name, "dWorldStep", 11))
                                                (*(PhysicsData *) data).stepType = 1;
                                            if (!strncmp (name, "dWorldStepFast1", 16))
                                                (*(PhysicsData *) data).stepType = 2;
                                        }
                                        if (!strncmp (name, "dWorldStepFast1MaxIterations", 29))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "\tFound the max. number of iterations to be calculated with dWorldStepFast1: %s", name);

                                            (*(PhysicsData *) data).dWorldStepFast1MaxIterations = atoi (name);
                                        }
                                        XMLString::release (&name);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    delete log;
}
