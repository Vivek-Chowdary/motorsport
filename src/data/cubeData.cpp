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

std::vector < Cube * >Cube::cubeList;

Cube::Cube (int cubeNumber, float posX, float posY, float posZ)
{
    CubeData *cubeData = new CubeData;
    cubeData->cube = this;
    cubeData->physics = new CubePhysicsData;
    cubeData->physics->size = 100;
    cubeData->graphics = new CubeGraphicsData;
    processXmlFile ("../data/cube.xml", &Cube::processCubeDataFile, (void *) cubeData);

    startPhysics (posX, posY, posZ, cubeData->physics);
    startInput ();
    startGraphics (cubeNumber, cubeData->graphics);

    // delete [](cubeData->name);
    // delete [](cubeData->description);
    // delete [](cubeData->physics->author);
    // delete [](cubeData->physics->license);
    delete cubeData->physics;
    // delete [](cubeData->graphics->author);
    // delete [](cubeData->graphics->license);
    delete[](cubeData->graphics->material);
    delete[](cubeData->graphics->mesh);
    delete[](cubeData->graphics->ogreName);
    delete cubeData->graphics;
    delete cubeData;
}

Cube::~Cube ()
{
    stopPhysics ();
    stopGraphics ();
}


void Cube::updateOgrePosition ()
{
    const dReal *temp = dBodyGetPosition (cubeID);  // need to allocate memory first??
    cubeNode->setPosition (*(temp + 0), *(temp + 1), *(temp + 2));
}
float Cube::getMoveToXPositive ()
{
    return moveToXPositive;
}
float Cube::getMoveToXNegative ()
{
    return moveToXNegative;
}

float Cube::getMoveToYPositive ()
{
    return moveToYPositive;
}
float Cube::getMoveToYNegative ()
{
    return moveToYNegative;
}
void Cube::updateOgreOrientation ()
{
    const dReal *temp = dBodyGetQuaternion (cubeID);    // need to allocate memory first??
    cubeNode->setOrientation (*(temp + 0), *(temp + 1), *(temp + 2), *(temp + 3));
}
