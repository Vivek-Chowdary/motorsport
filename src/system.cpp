
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

SystemData * SystemData::systemDataPointer = NULL;

SystemData * SystemData::getSystemDataPointer ()
{
    if (!systemDataPointer)
    {
        new SystemData ();
    }
    return ( systemDataPointer );
}

SystemData::SystemData ( )
{
    if (systemDataPointer)
    {
        delete this;
    }else{
        systemDataPointer = this;
        
        //initialize main loop data
        physicsSteps = 0;
        physicsStepsPerSecond = 0;
        graphicsSteps = 0;
        graphicsStepsPerSecond = 0;
        takeScreenshot = false;
        invertShowStatistics = false;
    }
}

SystemData::~SystemData ( )
{
    delete systemDataPointer;
    systemDataPointer = NULL;
}

bool mainLoopEnabled;

bool SystemData::canMainLoopRun ( void )    //does the program have to keep running?
{
    return mainLoopEnabled;
}

void SystemData::enableMainLoop ( void )    //allows the program to start running
{
    mainLoopEnabled = true;
}
void SystemData::disableMainLoop ( void )   //allows the program to stop running
{
    mainLoopEnabled = false;
}

/*
bool SystemData::getStatisticsEnabled (  )
{
    return statisticsEnabled;
}

void SystemData::invertStatisticsEnabled (  )
{
    statisticsEnabled = !statisticsEnabled;
}

void SystemData::enableStatistics (  )
{
    statisticsEnabled = true;
}
*/
void SystemData::setInvertShowStatistics (  )
{
    invertShowStatistics = true;
}

bool SystemData::getInvertShowStatistics (  )
{
    if (!invertShowStatistics) return false;
    invertShowStatistics = false;
    return true;
}

void SystemData::setTakeScreenshot (  )
{
    takeScreenshot = true;
}

bool SystemData::getTakeScreenshot (  )
{
    if (!takeScreenshot) return false;
    takeScreenshot = false;
    return true;
}
