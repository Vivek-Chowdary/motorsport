
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

#include "system.hpp"
#include "SDL/begin_code.h"

SystemData *SystemData::systemDataPointer = NULL;

SystemData *SystemData::getSystemDataPointer ()
{
    if (!systemDataPointer)
    {
        new SystemData ();
    }
    return (systemDataPointer);
}

SystemData::SystemData ()
{
    if (systemDataPointer)
    {
        delete this;
    } else
    {
        systemDataPointer = this;

        // initialize main loop data
        physicsSteps = 0;
        physicsFrequency = 0;
        graphicsSteps = 0;
        graphicsFrequency = 0;
        height = 1;
        width = 1;
    }
}

SystemData::~SystemData ()
{
    delete systemDataPointer;
    systemDataPointer = NULL;
}

bool SystemData::isMainLoopEnabled (void)
{
    return mainLoopEnabled;
}

void SystemData::enableMainLoop (void)
{
    mainLoopEnabled = true;
}
void SystemData::disableMainLoop (void)
{
    mainLoopEnabled = false;
}
double SystemData::getCfmValue()
{
    return cfmValue;
}
double SystemData::getErpValue()
{
    return erpValue;
}
void SystemData::setCfmValue(const double & value)
{
    cfmValue = value;
}
void SystemData::setErpValue(const double & value)
{
    erpValue = value;
}
