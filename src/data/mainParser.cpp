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

#include "main.hpp"

void processMainConfigFile (DOMNode * node, void *data)
{
    if (node)
    {
        if (node->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            char *nodeName = XMLString::transcode (node->getNodeName ());
            XERCES_STD_QUALIFIER cout << "Name:" << nodeName << XERCES_STD_QUALIFIER endl;

            if (!strncmp (nodeName, "mainConfig", 11))
            {
                XERCES_STD_QUALIFIER cout << "Found the main config element." << XERCES_STD_QUALIFIER endl;
                if (node->hasAttributes ())
                {
                    // get all the attributes of the node
                    DOMNamedNodeMap *pAttributes = node->getAttributes ();
                    int nodeSize = pAttributes->getLength ();

                    for (int i = 0; i < nodeSize; ++i)
                    {
                        DOMAttr *pAttributeNode = (DOMAttr *) pAttributes->item (i);
                        char *subnodeName = XMLString::transcode (pAttributeNode->getName ());
                        if (!strncmp (subnodeName, "localLogLevel", 14))
                        {
                            XMLString::release (&subnodeName);
                            XERCES_STD_QUALIFIER cout << "\tFound the local log level:";
                            subnodeName = XMLString::transcode (pAttributeNode->getValue ());
                            XERCES_STD_QUALIFIER cout << subnodeName << XERCES_STD_QUALIFIER endl;
                            if (!strncmp (subnodeName, "LOG_ERROR", 10))
                                (*(ParsingMainData *) data).localLogLevel = LOG_ERROR;
                            if (!strncmp (subnodeName, "LOG_WARNING", 13))
                                (*(ParsingMainData *) data).localLogLevel = LOG_WARNING;
                            if (!strncmp (subnodeName, "LOG_INFO", 9))
                                (*(ParsingMainData *) data).localLogLevel = LOG_INFO;
                            if (!strncmp (subnodeName, "LOG_VERBOSE", 12))
                                (*(ParsingMainData *) data).localLogLevel = LOG_VERBOSE;
                            if (!strncmp (subnodeName, "LOG_TRACE", 9))
                                (*(ParsingMainData *) data).localLogLevel = LOG_TRACE;
                        }
                        if (!strncmp (subnodeName, "localLogName", 13))
                        {
                            XMLString::release (&subnodeName);
                            XERCES_STD_QUALIFIER cout << "\tFound the log subnodeName:";
                            subnodeName = XMLString::transcode (pAttributeNode->getValue ());
                            XERCES_STD_QUALIFIER cout << subnodeName << XERCES_STD_QUALIFIER endl;

                            (*(ParsingMainData *) data).localLogName = new char[strlen (subnodeName) + 1];
                            strncpy ((*(ParsingMainData *) data).localLogName, subnodeName, strlen (subnodeName) + 1);
                        }
                        XMLString::release (&subnodeName);
                    }
                }
            }
            XMLString::release (&nodeName);
        }
    }
}
