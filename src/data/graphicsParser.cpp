
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

void GraphicsEngine::processGraphicsConfigFile (DOMNode * n, void *data)
{
    LogEngine * log = new LogEngine (LOG_TRACE, "XML");
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            char *name = XMLString::transcode (n->getNodeName ());
            log->format (LOG_INFO, "Name: %s", name);

            if (!strncmp (name, "graphicsConfig", 15))
            {
                log->put (LOG_INFO, "Found the graphics engine config element.");
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
                            log->format (LOG_INFO, "Found the local log level: %s", name);
                            if (!strncmp (name, "LOG_ERROR", 10))
                                (*(GraphicsData *) data).localLogLevel = LOG_ERROR;
                            if (!strncmp (name, "LOG_WARNING", 13))
                                (*(GraphicsData *) data).localLogLevel = LOG_WARNING;
                            if (!strncmp (name, "LOG_INFO", 9))
                                (*(GraphicsData *) data).localLogLevel = LOG_INFO;
                            if (!strncmp (name, "LOG_VERBOSE", 12))
                                (*(GraphicsData *) data).localLogLevel = LOG_VERBOSE;
                            if (!strncmp (name, "LOG_TRACE", 9))
                                (*(GraphicsData *) data).localLogLevel = LOG_TRACE;
                        }

                        if (!strncmp (name, "localLogName", 13))
                        {
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            log->format (LOG_INFO, "Found the log name: %s", name);

                            (*(GraphicsData *) data).localLogName = new char[strlen (name) + 1];
                            strncpy ((*(GraphicsData *) data).localLogName, name, strlen (name) + 1);
                        }
                        if (!strncmp (name, "screenshotFile", 15))
                        {
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            log->format (LOG_INFO, "Found the screenshot filename: %s", name);

                            (*(GraphicsData *) data).screenshotFile = new char[strlen (name) + 1];
                            strncpy ((*(GraphicsData *) data).screenshotFile, name, strlen (name) + 1);
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
                            if (!strncmp (name, "ogre", 5))
                            {
                                log->put (LOG_INFO, "Found the ogre config element.");
                                if (n->hasAttributes ())
                                {
                                    // get all the attributes of the node
                                    DOMNamedNodeMap *pAttributes = n->getAttributes ();
                                    int nSize = pAttributes->getLength ();

                                    for (int i = 0; i < nSize; ++i)
                                    {
                                        DOMAttr *pAttributeNode = (DOMAttr *) pAttributes->item (i);
                                        char *name = XMLString::transcode (pAttributeNode->getName ());
                                        if (!strncmp (name, "configFile", 11))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "Found the ogre config filename: %s", name);

                                            (*(GraphicsData *) data).ogreConfigFile = new char[strlen (name) + 1];
                                            strncpy ((*(GraphicsData *) data).ogreConfigFile, name, strlen (name) + 1);
                                        }
                                        if (!strncmp (name, "sceneManager", 13))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "Found the scene manager type: %s", name);

                                            if (!strncmp (name, "ST_GENERIC", 11))
                                                (*(GraphicsData *) data).sceneManager = Ogre::ST_GENERIC;
                                            if (!strncmp (name, "ST_EXTERIOR_CLOSE", 18))
                                                (*(GraphicsData *) data).sceneManager = Ogre::ST_EXTERIOR_CLOSE;
                                            if (!strncmp (name, "ST_EXTERIOR_FAR", 16))
                                                (*(GraphicsData *) data).sceneManager = Ogre::ST_EXTERIOR_FAR;
                                            if (!strncmp (name, "ST_EXTERIOR_REAL_FAR", 21))
                                                (*(GraphicsData *) data).sceneManager = Ogre::ST_EXTERIOR_REAL_FAR;
                                            if (!strncmp (name, "ST_INTERIOR", 12))
                                                (*(GraphicsData *) data).sceneManager = Ogre::ST_INTERIOR;
                                        }
                                        if (!strncmp (name, "anisotropy", 11))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "Found the anisotropy level: %s", name);

                                            (*(GraphicsData *) data).anisotropy = atoi (name);
                                        }
                                        if (!strncmp (name, "filtering", 10))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "Found the texture filtering level: %s", name);

                                            if (!strncmp (name, "TFO_NONE", 9))
                                                (*(GraphicsData *) data).filtering = Ogre::TFO_NONE;
                                            if (!strncmp (name, "TFO_BILINEAR", 13))
                                                (*(GraphicsData *) data).filtering = Ogre::TFO_BILINEAR;
                                            if (!strncmp (name, "TFO_TRILINEAR", 14))
                                                (*(GraphicsData *) data).filtering = Ogre::TFO_TRILINEAR;
                                            if (!strncmp (name, "TFO_ANISOTROPIC", 16))
                                                (*(GraphicsData *) data).filtering = Ogre::TFO_ANISOTROPIC;
                                        }
                                        if (!strncmp (name, "width", 6))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "Found the resolution width value: %s", name);

                                            (*(GraphicsData *) data).width = atoi (name);
                                        }
                                        if (!strncmp (name, "height", 7))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "Found the resolution height value: %s", name);

                                            (*(GraphicsData *) data).height = atoi (name);
                                        }
                                        if (!strncmp (name, "bpp", 4))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "Found the resolution bpp value: %s", name);

                                            (*(GraphicsData *) data).bpp = atoi (name);
                                        }
                                        if (!strncmp (name, "renderer", 9))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "Found the renderer type: %s", name);

                                            (*(GraphicsData *) data).renderer = new char[strlen (name) + 1];
                                            strncpy ((*(GraphicsData *) data).renderer, name, strlen (name) + 1);
                                        }
                                        if (!strncmp (name, "defaultNumMipmaps", 18))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "Found the default number of mipmaps: %s", name);

                                            (*(GraphicsData *) data).defaultNumMipMaps = atoi (name);
                                        }
                                        if (!strncmp (name, "fullScreen", 11))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            log->format (LOG_INFO, "Found the fullscreen option: %s", name);

                                            int tmpBool;
                                            tmpBool = atoi (name);
                                            (*(GraphicsData *) data).fullScreen = tmpBool ? true : false;
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
