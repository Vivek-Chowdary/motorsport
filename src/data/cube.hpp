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

#ifndef CUBE_HPP
#   define CUBE_HPP
#   include "ode.h"
#   include "Ogre.h"
#   include <vector>

class Cube
{
    private:
        float moveToXPositive;
        float moveToXNegative;
        float moveToYPositive;
        float moveToYNegative;

    public:
        static std::vector <Cube*> cubeList;             //for our example,the worlddata has a set of cubes.
        //should be private
        Ogre::Entity * cubeEntity;
        Ogre::SceneNode * cubeNode;

        dBodyID cubeID;
        dGeomID cubeGeomID;

        //should be public or similar
        void updateOgrePosition();
        void updateOgreOrientation();
        void setMoveToXPositive ( float multiplier );
        void setMoveToXNegative ( float multiplier );
        void setMoveToYPositive ( float multiplier );
        void setMoveToYNegative ( float multiplier );
        float getMoveToXPositive ( );
        float getMoveToXNegative ( );
        float getMoveToYPositive ( );
        float getMoveToYNegative ( );
};

#endif
