
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

#include "graphicsEngine.hpp"

int GraphicsEngine::processGraphicsConfigFile ( DOMNode * n, void * data)
{
    if ( n )
    {
        if ( n->getNodeType (  ) == DOMNode::ELEMENT_NODE )
        {
            char *name = XMLString::transcode ( n->getNodeName (  ) );
            XERCES_STD_QUALIFIER cout << "Name:" << name << XERCES_STD_QUALIFIER endl;

            if ( !strncmp ( name, "graphicsConfig", 15 ) )
            {
                XERCES_STD_QUALIFIER cout << "Found the graphics engine config element." << XERCES_STD_QUALIFIER endl;
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
                                (*(GraphicsData*)data).localLogLevel = LOG_ERROR;
                            if ( !strncmp ( name, "LOG_WARNING", 13 ) )
                                (*(GraphicsData*)data).localLogLevel = LOG_WARNING;
                            if ( !strncmp ( name, "LOG_INFO", 9 ) )
                                (*(GraphicsData*)data).localLogLevel = LOG_INFO;
                            if ( !strncmp ( name, "LOG_VERBOSE", 12 ) )
                                (*(GraphicsData*)data).localLogLevel = LOG_VERBOSE;
                            if ( !strncmp ( name, "LOG_TRACE", 9 ) )
                                (*(GraphicsData*)data).localLogLevel = LOG_TRACE;
                        }

                        if ( !strncmp ( name, "localLogName", 13 ) )
                        {
                            XMLString::release ( &name );
                            XERCES_STD_QUALIFIER cout <<
                                "\tFound the log name:";
                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                            
                            (*(GraphicsData*)data).localLogName = new char[strlen(name)+1];
                            strncpy ((*(GraphicsData*)data).localLogName, name, strlen(name)+1);
                        }
                        if ( !strncmp ( name, "screenshotFile", 15 ) )
                        {
                            XERCES_STD_QUALIFIER cout <<
                                "\tFound the screenshot filename:";
                            name =
                                XMLString::transcode ( pAttributeNode->
                                                       getValue (  ) );
                            XERCES_STD_QUALIFIER cout << name <<
                                XERCES_STD_QUALIFIER endl;
                            
                            (*(GraphicsData*)data).screenshotFile = new char[strlen(name)+1];
                            strncpy ((*(GraphicsData*)data).screenshotFile, name, strlen(name)+1);
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
                            if ( !strncmp ( name, "ogre", 5 ) )
                            {
                                XERCES_STD_QUALIFIER cout << "Found the ogre config element." << XERCES_STD_QUALIFIER endl;
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
                                        if ( !strncmp ( name, "configFile", 11 ) )
                                        {
                                            XMLString::release ( &name );
                                            XERCES_STD_QUALIFIER cout << "\tFound the ogre config filename:";
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                
                                            (*(GraphicsData*)data).ogreConfigFile = new char[strlen(name)+1];
                                            strncpy ((*(GraphicsData*)data).ogreConfigFile, name, strlen(name)+1);
                                        }
                                        if ( !strncmp ( name, "sceneManager", 13 ) )
                                        {
                                            XERCES_STD_QUALIFIER cout << "\tFound the scene manager type:";
                                            XMLString::release ( &name );
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;

                                            if ( !strncmp ( name, "ST_GENERIC", 11 ) )
                                                (*(GraphicsData*)data).sceneManager = Ogre::ST_GENERIC;
                                            if ( !strncmp ( name, "ST_EXTERIOR_CLOSE", 18 ) )
                                                (*(GraphicsData*)data).sceneManager = Ogre::ST_EXTERIOR_CLOSE;
                                            if ( !strncmp ( name, "ST_EXTERIOR_FAR", 16 ) )
                                                (*(GraphicsData*)data).sceneManager = Ogre::ST_EXTERIOR_FAR;
                                            if ( !strncmp ( name, "ST_EXTERIOR_REAL_FAR", 21 ) )
                                                (*(GraphicsData*)data).sceneManager = Ogre::ST_EXTERIOR_REAL_FAR;
                                            if ( !strncmp ( name, "ST_INTERIOR", 12 ) )
                                                (*(GraphicsData*)data).sceneManager = Ogre::ST_INTERIOR;
                                        }
                                        if ( !strncmp ( name, "anisotropy", 11 ) )
                                        {
                                            XERCES_STD_QUALIFIER cout << "\tFound the anisotropy level:";
                                            XMLString::release ( &name );
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                                            
                                            (*(GraphicsData*)data).anisotropy = atoi (name);
                                        }
                                        if ( !strncmp ( name, "filtering", 10 ) )
                                        {
                                            XERCES_STD_QUALIFIER cout << "\tFound the texture filtering level:";
                                            XMLString::release ( &name );
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;

                                            if ( !strncmp ( name, "TFO_NONE", 9 ) )
                                                (*(GraphicsData*)data).filtering = Ogre::TFO_NONE;
                                            if ( !strncmp ( name, "TFO_BILINEAR", 13 ) )
                                                (*(GraphicsData*)data).filtering = Ogre::TFO_BILINEAR;
                                            if ( !strncmp ( name, "TFO_TRILINEAR", 14 ) )
                                                (*(GraphicsData*)data).filtering = Ogre::TFO_TRILINEAR;
                                            if ( !strncmp ( name, "TFO_ANISOTROPIC", 16 ) )
                                                (*(GraphicsData*)data).filtering = Ogre::TFO_ANISOTROPIC;
                                        }
                                        if ( !strncmp ( name, "width", 6 ) )
                                        {
                                            XERCES_STD_QUALIFIER cout << "\tFound the resolution width value:";
                                            XMLString::release ( &name );
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                                            
                                            (*(GraphicsData*)data).width = atoi (name);
                                        }
                                        if ( !strncmp ( name, "height", 7 ) )
                                        {
                                            XERCES_STD_QUALIFIER cout << "\tFound the resolution height value:";
                                            XMLString::release ( &name );
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                                            
                                            (*(GraphicsData*)data).height = atoi (name);
                                        }
                                        if ( !strncmp ( name, "bpp", 4 ) )
                                        {
                                            XERCES_STD_QUALIFIER cout << "\tFound the resolution bpp value:";
                                            XMLString::release ( &name );
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                                            
                                            (*(GraphicsData*)data).bpp = atoi (name);
                                        }
                                        if ( !strncmp ( name, "renderer", 9 ) )
                                        {
                                            XERCES_STD_QUALIFIER cout << "\tFound the renderer type:";
                                            XMLString::release ( &name );
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                
                                            (*(GraphicsData*)data).renderer = new char[strlen(name)+1];
                                            strncpy ((*(GraphicsData*)data).renderer, name, strlen(name)+1);
                                        }
                                        if ( !strncmp ( name, "defaultNumMipmaps", 18 ) )
                                        {
                                            XERCES_STD_QUALIFIER cout << "\tFound the default number of mip maps:";
                                            XMLString::release ( &name );
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                                            
                                            (*(GraphicsData*)data).defaultNumMipMaps = atoi (name);
                                        }
                                        if ( !strncmp ( name, "fullScreen", 11 ) )
                                        {
                                            XERCES_STD_QUALIFIER cout << "\tFound the fullscreen option:";
                                            XMLString::release ( &name );
                                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                                            
                                            int tmpBool;
                                            tmpBool = atoi (name);
                                            (*(GraphicsData*)data).fullScreen = tmpBool ? true : false;
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
