
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

#ifndef WORLD_HPP
#    define WORLD_HPP

#    include "SDL.h"
#    include "Ogre.h"
#include "ode.h"

class Cube
{
    private:
        int moveToX;
        int moveToY;
    public:
        //should be private
        Ogre::Entity * cubeEntity;
        Ogre::SceneNode * cubeNode;
    
        dBodyID cubeID;
        dGeomID cubeGeomID;
    
        //should be public or similar
        void updateOgrePosition();
        void updateOgreOrientation();
        void setMoveToX ( int multiplier );
        void setMoveToY ( int multiplier );
        int getMoveToX ( );
        int getMoveToY ( );
};

class MospCamera
{
    public:
        //should be private
        Ogre::Camera * ogreCamera;

        //should be public
        bool goBack;
        bool goForward;
        bool goLeft;
        bool goRight;
        bool rotateUp;
        bool rotateDown;
        bool rotateLeft;
        bool rotateRight;
};


class WorldData
{                               //this will contain everything related to the simulated/virtual world
    private:
        static WorldData * worldDataPointer;
    public:
        static WorldData * getWorldDataPointer ( );
        WorldData  ( );
        ~WorldData ( );
        int numberOfCubes;
        Cube *cubeList;             //for our example,the worlddata has a set of cubes.
        MospCamera *camera1;

        dWorldID worldID;
        dSpaceID spaceID;
        dJointGroupID jointGroupID;
};

#endif
