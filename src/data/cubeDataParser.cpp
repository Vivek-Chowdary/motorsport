
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

#include "cube.hpp"

int Cube::processCubeDataFile ( DOMNode * n, void * data)
{
    if ( n )
    {
        if ( n->getNodeType (  ) == DOMNode::ELEMENT_NODE )
        {
            char *name = XMLString::transcode ( n->getNodeName (  ) );
            XERCES_STD_QUALIFIER cout << "Name:" << name << XERCES_STD_QUALIFIER endl;

            if ( !strncmp ( name, "cube", 5 ) )
            {
                XERCES_STD_QUALIFIER cout << "Found the cube main data config element." << XERCES_STD_QUALIFIER endl;
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
                        if ( !strncmp ( name, "name", 5 ) )
                        {
                            XMLString::release ( &name );
                            XERCES_STD_QUALIFIER cout <<
                                "\tFound the name:";
                            name =
                                XMLString::transcode ( pAttributeNode->
                                                       getValue (  ) );
                            XERCES_STD_QUALIFIER cout << name <<
                                XERCES_STD_QUALIFIER endl;
                        }

                        if ( !strncmp ( name, "description", 13 ) )
                        {
                            XMLString::release ( &name );
                            XERCES_STD_QUALIFIER cout <<
                                "\tFound the description:";
                            name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                            XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
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
                            if ( !strncmp ( name, "graphics", 9 ) )
                            {
                                XERCES_STD_QUALIFIER cout << "Found the cube graphics data element." << XERCES_STD_QUALIFIER endl;
                                (*(CubeData*)data).cube->processCubeGraphicsDataNode ( n, (*(CubeData*)data).graphics );
                            }
                            if ( !strncmp ( name, "physics", 8 ) )
                            {
                                XERCES_STD_QUALIFIER cout << "Found the cube physics data element." << XERCES_STD_QUALIFIER endl;
                                (*(CubeData*)data).cube->processCubePhysicsDataNode ( n, (*(CubeData*)data).physics );
                            }
                        } 
                    }
                }
            }
        }
    }
    return 1;
}

int Cube::processCubeGraphicsDataNode ( DOMNode * n, CubeGraphicsData * graphics )
{
    XERCES_STD_QUALIFIER cout << "\t\t\tParsing cube graphics:";
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
            if ( !strncmp ( name, "material", 9 ) )
            {
                XMLString::release ( &name );
                XERCES_STD_QUALIFIER cout <<
                    "\tFound the cube graphics material:";
                name =
                    XMLString::transcode ( pAttributeNode->
                                           getValue (  ) );
                XERCES_STD_QUALIFIER cout << name <<
                    XERCES_STD_QUALIFIER endl;

                graphics->material = new char[strlen(name)+1];
                strncpy (graphics->material, name, strlen(name)+1);
            }

            if ( !strncmp ( name, "mesh", 5 ) )
            {
                XMLString::release ( &name );
                XERCES_STD_QUALIFIER cout <<
                    "\tFound the cube graphics mesh filename:";
                name = XMLString::transcode ( pAttributeNode->getValue (  ) );
                XERCES_STD_QUALIFIER cout << name << XERCES_STD_QUALIFIER endl;
                
                graphics->mesh = new char[strlen(name)+1];
                strncpy (graphics->mesh, name, strlen(name)+1);
            }
            if ( !strncmp ( name, "ogreName", 9 ) )
            {
                XERCES_STD_QUALIFIER cout <<
                    "\tFound the cube graphics ogre-identifier format:";
                name =
                    XMLString::transcode ( pAttributeNode->
                                           getValue (  ) );
                XERCES_STD_QUALIFIER cout << name <<
                    XERCES_STD_QUALIFIER endl;

                graphics->ogreName = new char[strlen(name)+1];
                strncpy (graphics->ogreName, name, strlen(name)+1);
            }
            XMLString::release ( &name );
        }
    }
    XERCES_STD_QUALIFIER cout << "\t\t\tFinished cube graphics:";
    return 1;
}

int Cube::processCubePhysicsDataNode ( DOMNode * n,
                                        CubePhysicsData * physics )
{
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
                XMLString::transcode ( pAttributeNode->getName (  ) );
            if ( !strncmp ( name, "size", 5 ) )
            {
                XMLString::release ( &name );
                XERCES_STD_QUALIFIER cout <<
                    "\tFound the cube physics size:";
                name =
                    XMLString::transcode ( pAttributeNode->
                                           getValue (  ) );
                XERCES_STD_QUALIFIER cout << name <<
                    XERCES_STD_QUALIFIER endl;

                physics->size = atoi ( name );
            }
/*            if ( !strncmp ( name, "screenshotFile", 15 ) )
            {
                XERCES_STD_QUALIFIER cout <<
                    "\tFound the screenshot filename:";
                name =
                    XMLString::transcode ( pAttributeNode->
                                           getValue (  ) );
                XERCES_STD_QUALIFIER cout << name <<
                    XERCES_STD_QUALIFIER endl;

                //physics.screenshotFile = new char[strlen(name)+1];
                //strncpy (physics->screenshotFile, name, strlen(name)+1);
            }*/
            XMLString::release ( &name );
        }
    }
    return 1;
}
