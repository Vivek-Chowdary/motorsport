/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "driveMass.hpp"

DriveMass::DriveMass (WorldObject * container, std::string name)
    :WorldObject(container, name)
{
    inertia = 0.0;
    friction = 0.0;
    inputAngularVel = 0.0;
    prevAngularVel = 0.0;
    angularAcc = 0.0;
    outputAngularVel = 0.0;
    outputTorqueTransfer = 0.0;
    inputTorqueTransfer = 0.0;
}
DriveMass::~DriveMass ()
{
}
