
/******************************************************************************
*
* Copyright (C) 2004 Bruno Gonz�lez Campo (stenyak@users.sourceforge.net)
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

#include "logEngine.hpp"
void LogEngine::processLogConfigFile (DOMNode * n, void *data)
{
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            char *name = XMLString::transcode (n->getNodeName ());
            XERCES_STD_QUALIFIER cout << "Name:" << name << XERCES_STD_QUALIFIER endl;

            if (!strncmp (name, "logConfig", 10))
            {
                XERCES_STD_QUALIFIER cout << "Found the log engine config element." << XERCES_STD_QUALIFIER endl;
                if (n->hasAttributes ())
                {
                    // get all the attributes of the node
                    DOMNamedNodeMap *pAttributes = n->getAttributes ();
                    int nSize = pAttributes->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *pAttributeNode = (DOMAttr *) pAttributes->item (i);
                        char *name = XMLString::transcode (pAttributeNode->getName ());
                        if (!strncmp (name, "globalLevel", 12))
                        {
                            XMLString::release (&name);
                            XERCES_STD_QUALIFIER cout << "\tFound the global log level:";
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;

                            if (!strncmp (name, "LOG_ERROR", 10))
                                (*(LogData *) data).globalLevel = LOG_ERROR;
                            if (!strncmp (name, "LOG_WARNING", 13))
                                (*(LogData *) data).globalLevel = LOG_WARNING;
                            if (!strncmp (name, "LOG_INFO", 9))
                                (*(LogData *) data).globalLevel = LOG_INFO;
                            if (!strncmp (name, "LOG_VERBOSE", 12))
                                (*(LogData *) data).globalLevel = LOG_VERBOSE;
                            if (!strncmp (name, "LOG_TRACE", 9))
                                (*(LogData *) data).globalLevel = LOG_TRACE;
                        }
                        if (!strncmp (name, "fileName", 9))
                        {
                            XERCES_STD_QUALIFIER cout << "\tFound the log filename:";
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;

                            (*(LogData *) data).fileName = new char[strlen (name) + 1];
                            strncpy ((*(LogData *) data).fileName, name, strlen (name) + 1);
                        }
                        if (!strncmp (name, "textBuffer", 11))
                        {
                            XERCES_STD_QUALIFIER cout << "\tFound the buffer length:";
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;

                            (*(LogData *) data).textBuffer = atoi (name);
                        }
                        XMLString::release (&name);
                    }
                }
            }
        }
    }
}
