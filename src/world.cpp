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

#include "world.hpp"

void Rectangle::setPosition (Sint16 newPosX, Sint16 newPosY)
{
    rectangle.x = newPosX;
    rectangle.y = newPosY;

}

Sint16 Rectangle::getPositionX (void)
{
    return rectangle.x;
}

Sint16 Rectangle::getPositionY (void)
{
    return rectangle.y;
}

void Rectangle::setSize (Uint16 newWidth, Uint16 newHeight)
{
    rectangle.w = newWidth;
    rectangle.h = newHeight;
}

Sint16 Rectangle::getSizeX (void)
{
    return rectangle.w;
}

Sint16 Rectangle::getSizeY (void)
{
    return rectangle.h;
}

void Rectangle::setVisible (int visibility)
{
    visible = visibility;
}

int Rectangle::isVisible ()
{
    return visible;
}


SDL_Rect* Rectangle::sdlRectangle ()
{
    return &rectangle;
}
