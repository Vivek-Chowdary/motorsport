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

/******************************************************************************
*
* Includes
*
******************************************************************************/

#include "system.hpp"

/******************************************************************************
*
* Functions
*
******************************************************************************/

bool mainLoopEnabled;
bool simLoopEnabled;
bool guiLoopEnabled;

bool SystemData::canMainLoopRun(void)   //does the program have to keep running?
{
    return mainLoopEnabled;
}

bool SystemData::canSimLoopRun(void)    //does the simulation have to keep running?
{
    return simLoopEnabled;
}
bool SystemData::canGuiLoopRun(void)    //does the gui have to keep running?
{
    return guiLoopEnabled;
}

void SystemData::enableMainLoop (void)  //allows the program to start running
{
    mainLoopEnabled = true;
}
void SystemData::enableSimLoop (void)   //allows the simulation to start running
{
    simLoopEnabled = true;
}
void SystemData::enableGuiLoop (void)   //allows the gui to start running
{
    guiLoopEnabled = true;
}

void SystemData::disableMainLoop (void) //allows the program to stop running
{
    mainLoopEnabled = false;
}
void SystemData::disableSimLoop (void)  //allows the simulation to stop running
{
    simLoopEnabled = false;
}
void SystemData::disableGuiLoop (void)  //allows the gui to stop running{
{
    guiLoopEnabled = false;
}
