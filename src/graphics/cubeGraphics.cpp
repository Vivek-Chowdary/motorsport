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

void Cube::startGraphics ( int cubeNumber, CubeGraphicsData * graphics )
{
    char name[20];
    sprintf ( name, graphics->ogreName, cubeNumber );
    cubeEntity = SystemData::getSystemDataPointer()->ogreSceneManager->createEntity ( name, graphics->mesh );
    cubeEntity->setMaterialName ( graphics->material );
    cubeNode = static_cast < Ogre::SceneNode* > (SystemData::getSystemDataPointer()->ogreSceneManager->getRootSceneNode()->createChild() );
    cubeNode->attachObject (cubeEntity);
}

void Cube::stepGraphics()
{
    updateOgrePosition();
    updateOgreOrientation();
}

void Cube::stopGraphics()
{
    //empty
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
