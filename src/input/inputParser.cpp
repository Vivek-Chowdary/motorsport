
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

#include "inputEngine.hpp"

void InputEngine::processInputConfigFile (DOMNode * n, void *data)
{
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            char *name = XMLString::transcode (n->getNodeName ());
            XERCES_STD_QUALIFIER cout << "Name:" << name << XERCES_STD_QUALIFIER endl;

            if (!strncmp (name, "inputConfig", 12))
            {
                XERCES_STD_QUALIFIER cout << "Found the input engine config element." << XERCES_STD_QUALIFIER endl;
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
                            XERCES_STD_QUALIFIER cout << "\tFound the local log level:";
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                            if (!strncmp (name, "LOG_ERROR", 10))
                                (*(InputData *) data).localLogLevel = LOG_ERROR;
                            if (!strncmp (name, "LOG_WARNING", 13))
                                (*(InputData *) data).localLogLevel = LOG_WARNING;
                            if (!strncmp (name, "LOG_INFO", 9))
                                (*(InputData *) data).localLogLevel = LOG_INFO;
                            if (!strncmp (name, "LOG_VERBOSE", 12))
                                (*(InputData *) data).localLogLevel = LOG_VERBOSE;
                            if (!strncmp (name, "LOG_TRACE", 9))
                                (*(InputData *) data).localLogLevel = LOG_TRACE;
                        }

                        if (!strncmp (name, "localLogName", 13))
                        {
                            XMLString::release (&name);
                            XERCES_STD_QUALIFIER cout << "\tFound the log name:";
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;

                            (*(InputData *) data).localLogName = new char[strlen (name) + 1];
                            strncpy ((*(InputData *) data).localLogName, name, strlen (name) + 1);
                        }
                        XMLString::release (&name);
                    }
                }
            }
        }
    }
}
