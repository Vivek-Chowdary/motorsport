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


void Cube::startInput ()
{
    moveToXPositive = moveToXNegative = moveToYPositive = moveToYNegative = 0;
}

void Cube::stopInput ()
{

}
void Cube::stepInput ()
{

}

void Cube::setMoveToXPositive (float multiplier)
{
    moveToXPositive = multiplier;
}
void Cube::setMoveToXNegative (float multiplier)
{
    moveToXNegative = multiplier;
}

void Cube::setMoveToYPositive (float multiplier)
{
    moveToYPositive = multiplier;
}
void Cube::setMoveToYNegative (float multiplier)
{
    moveToYNegative = multiplier;
}
