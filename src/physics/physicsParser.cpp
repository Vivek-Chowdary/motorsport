
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

int PhysicsEngine::processPhysicsConfigFile ( DOMNode * n, void * data)
{
    if ( n )
    {
        if ( n->getNodeType (  ) == DOMNode::ELEMENT_NODE )
        {
            char *name = XMLString::transcode ( n->getNodeName (  ) );
            XERCES_STD_QUALIFIER cout << "Name:" << name << XERCES_STD_QUALIFIER endl;

            if ( !strncmp ( name, "physicsConfig", 14 ) )
            {
                XERCES_STD_QUALIFIER cout << "Found the physics engine config element." << XERCES_STD_QUALIFIER endl;
                if ( n->hasAttributes (  ) )
                {
                    // get all the attributes of the node
                    DOMNamedNodeMap *pAttributes = n->getAttributes (  );
                    int nSize = pAttributes->getLength (  );
                    for ( int i = 0; i < nSize; ++i )
                    {
                        DOMAttr *pAttributeNode =
                            ( DOMAttr * ) pAttributes->item ( i );
                        char *name =
                            XMLString::transcode ( pAttributeNode->
                                                   getName (  ) );
                        if ( !strncmp ( name, "localLogLevel", 14 ) )
                        {
                            XMLString::release ( &name );
                            XERCES_STD_QUALIFIER cout <<
                                "\tFound the local log level:";
                            name =
                                XMLString::transcode ( pAttributeNode->
                                                       getValue (  ) );
                            XERCES_STD_QUALIFIER cout << name <<
                                XERCES_STD_QUALIFIER endl;
                            if ( !strncmp ( name, "LOG_ERROR", 10 ) )
                                (*(PhysicsData*)data).localLogLevel = LOG_ERROR;
                            if ( !strncmp ( name, "LOG_WARNING", 13 ) )
                                (*(PhysicsData*)data).localLogLevel = LOG_WARNING;
                            if ( !strncmp ( name, "LOG_INFO", 9 ) )
                                (*(PhysicsData*)data).localLogLevel = LOG_INFO;
                            if ( !strncmp ( name, "LOG_VERBOSE", 12 ) )
                                (*(PhysicsData*)data).localLogLevel = LOG_VERBOSE;
                            if ( !strncmp ( name, "LOG_TRACE", 9 ) )
                                (*(PhysicsData*)data).localLogLevel = LOG_TRACE;
                        }

                        if ( !strncmp ( name, "localLogName", 13 ) )
                        {
                            XMLString::release ( &name );
                            XERCES_STD_QUALIFIER cout <<
                                "\tFound the log name:";
                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                            
                            (*(PhysicsData*)data).localLogName = new char[strlen(name)+1];
                            strncpy ((*(PhysicsData*)data).localLogName, name, strlen(name)+1);
                        }
                        if ( !strncmp ( name, "frequency", 10 ) )
                        {
                            XERCES_STD_QUALIFIER cout << "\tFound the frecuency:";
                            XMLString::release ( &name );
                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                            
                            (*(PhysicsData*)data).frequency = atoi (name);
                        }
                        XMLString::release ( &name );
                    }
                }
                for ( n = n->getFirstChild (  ); n != 0; n = n->getNextSibling (  ) )
                {
                    if ( n )
                    {
                        if ( n->getNodeType (  ) == DOMNode::ELEMENT_NODE )
                        {
                            char *name = XMLString::transcode ( n->getNodeName (  ) );
                            XERCES_STD_QUALIFIER cout << "Name:" << name << XERCES_STD_QUALIFIER endl;
                            if ( !strncmp ( name, "ode", 4 ) )
                            {
                                XERCES_STD_QUALIFIER cout << "Found the ode config element." << XERCES_STD_QUALIFIER endl;
                                if ( n->hasAttributes (  ) )
                                {
                                    // get all the attributes of the node
                                    DOMNamedNodeMap *pAttributes = n->getAttributes (  );
                                    int nSize = pAttributes->getLength (  );
                
                                    for ( int i = 0; i < nSize; ++i )
                                    {
                                        DOMAttr *pAttributeNode =
                                            ( DOMAttr * ) pAttributes->item ( i );
                                        char *name =
                                            XMLString::transcode ( pAttributeNode->
                                                                   getName (  ) );
                                        if ( !strncmp ( name, "cfmValue", 9 ) )
                                        {
                                            XERCES_STD_QUALIFIER cout << "\tFound the constraint force mixing factor (CFM):";
                                            XMLString::release ( &name );
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                                            
                                            if ( strncmp ( name, "default", 8 ) )
                                            {
                                                (*(PhysicsData*)data).cfmValue = atof (name);
                                            }
                                        }
                                        if ( !strncmp ( name, "erpValue", 9 ) )
                                        {
                                            XERCES_STD_QUALIFIER cout << "\tFound the error reduction parameter (ERP):";
                                            XMLString::release ( &name );
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                                            
                                            if ( strncmp ( name, "default", 8 ) )
                                            {
                                                (*(PhysicsData*)data).erpValue = atof (name);
                                            }
                                        }
                                        if ( !strncmp ( name, "stepType", 8 ) )
                                        {
                                            XERCES_STD_QUALIFIER cout << "\tFound the type of stepping to be used in ODE:";
                                            XMLString::release ( &name );
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;

                                            if ( !strncmp ( name, "dWorldStep", 11 ) )
                                                (*(PhysicsData*)data).stepType = 1;
                                            if ( !strncmp ( name, "dWorldStepFast1", 16 ) )
                                                (*(PhysicsData*)data).stepType = 2;
                                        }
                                        if ( !strncmp ( name, "dWorldStepFast1MaxIterations", 29 ) )
                                        {
                                            XERCES_STD_QUALIFIER cout << "\tFound the max. number of iterations to be calculated with dWorldStepFast1:";
                                            XMLString::release ( &name );
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                            
                                            (*(PhysicsData*)data).dWorldStepFast1MaxIterations = atoi (name);
                                        }
                                        XMLString::release ( &name );
                                    }
                                }
                            }
                        } 
                    }
                }
            }
        }
    }
    return 1;
}
