
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

#ifndef WORLD_HPP
#    define WORLD_HPP

#    include "SDL.h"

class Rectangle
{
    SDL_Rect rectangle;
    int visible;
  public:
    void setPosition ( Sint16 newPosX, Sint16 newPosY );
    Sint16 getPositionX ( void );
    Sint16 getPositionY ( void );
    void setSize ( Uint16 newWidth, Uint16 newHeight );
    Sint16 getSizeX ( void );
    Sint16 getSizeY ( void );
    void setVisible ( int visibility );
    int isVisible (  );
    Uint8 red,
          green,
          blue;
    SDL_Rect *sdlRectangle (  );
    bool colorSpeed;            //0 -> increasing; 1 -> decreasing
};

class WorldData
{                               //this will contain everything related to the simulated/virtual world
  public:
    int numberOfRectangles;
    Rectangle *rectangleList;   //for our example,the worlddata has only 2 rect.
};

#endif
