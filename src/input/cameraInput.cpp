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

#include "camera.hpp"

void Camera::startInput ()
{
    rotateLeft = rotateRight = rotateUp = rotateDown = 0;
    goBack = goForward = goLeft = goRight = false;
}

void Camera::stepInput ()
{

}

void Camera::stopInput ()
{

}

void Camera::setRotateLeft (int multiplier)
{
    rotateLeft = multiplier;
}
void Camera::setRotateRight (int multiplier)
{
    rotateRight = multiplier;
}
void Camera::setRotateUp (int multiplier)
{
    rotateUp = multiplier;
}
void Camera::setRotateDown (int multiplier)
{
    rotateDown = multiplier;
}
