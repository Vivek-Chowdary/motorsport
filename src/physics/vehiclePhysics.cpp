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
#include "vehicle.hpp"
#include "data/xercesc_fwd.hpp"
#include "body.hpp"
#include "engine.hpp"
#include "wheel.hpp"

void Vehicle::startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
}

void Vehicle::setPosition (double posX, double posY, double posZ)
{
    body->setPosition (posX, posY, posZ);
    for (int i=0; i<4; i++)
    {
        double offX = 1.50;
        double offY = 1;
        double offZ = 0.4;
        wheelMap[0]->setRotation(90,0,0);
        wheelMap[1]->setRotation(-90,0,0);
        wheelMap[2]->setRotation(90,0,0);
        wheelMap[3]->setRotation(-90,0,0);
        wheelMap[0]->setPosition(posX+offX,posY+offY,posZ-offZ);
        wheelMap[1]->setPosition(posX+offX,posY-offY,posZ-offZ);
        wheelMap[2]->setPosition(posX-offX,posY+offY,posZ-offZ);
        wheelMap[3]->setPosition(posX-offX,posY-offY,posZ-offZ);
        int i=0;
        dJointID jointID = dJointCreateHinge (World::getWorldPointer()->worldID, 0);
        dJointAttach (jointID, wheelMap[i]->wheelID, body->bodyID);
        dJointSetHingeAnchor (jointID, posX+offX, posY+offY, posZ-offZ);
        dJointSetHingeAxis (jointID, 0,1,0);
        i=1;
        jointID = dJointCreateHinge (World::getWorldPointer()->worldID, 0);
        dJointAttach (jointID, wheelMap[i]->wheelID, body->bodyID);
        dJointSetHingeAnchor (jointID, posX+offX, posY-offY, posZ-offZ);
        dJointSetHingeAxis (jointID, 0,1,0);
        i=2;
        jointID = dJointCreateHinge (World::getWorldPointer()->worldID, 0);
        dJointAttach (jointID, wheelMap[i]->wheelID, body->bodyID);
        dJointSetHingeAnchor (jointID, posX-offX, posY+offY, posZ-offZ);
        dJointSetHingeAxis (jointID, 0,1,0);
        i=3;
        jointID = dJointCreateHinge (World::getWorldPointer()->worldID, 0);
        dJointAttach (jointID, wheelMap[i]->wheelID, body->bodyID);
        dJointSetHingeAnchor (jointID, posX-offX, posY-offY, posZ-offZ);
        dJointSetHingeAxis (jointID, 0,1,0);
    }
}

void Vehicle::setRotation (double rotX, double rotY, double rotZ)
{
    body->setRotation (rotX, rotY, rotZ);
}

void Vehicle::stopPhysics ()
{
    body->stopPhysics();
    engine->stopPhysics();
    for (unsigned int i=0; i<wheelMap.size(); i++)
    {
        wheelMap[i]->stopPhysics();
    }
}

void Vehicle::stepPhysics ()
{
    body->stepPhysics();
    engine->stepPhysics();
    for (unsigned int i=0; i<wheelMap.size(); i++)
    {
        wheelMap[i]->stepPhysics();
    }
    wheelMap[2]->addTorque (engine->getTorque());
    wheelMap[3]->addTorque (-engine->getTorque());
}
