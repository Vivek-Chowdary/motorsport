
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

#ifndef MAIN_HPP
#    define MAIN_HPP
#    include "SDL.h"
#    include "world.hpp"        //contains the IDF for the simulated/virtual world data
#    include "system.hpp"       //contains the IDF for the system data
#    include "logEngine.hpp"    //allows to easily log actions
#    include "dataEngine.hpp"   //loads/saves data from memory into the virtual world
#    include "inputEngine.hpp"  //process the queue of input events
#    include "graphicsEngine.hpp"   //displays the virtual and system data (sim+gui)
#    include "physicsEngine.hpp"    //calculates the physics of the world data
#    include "guiEngine.hpp"    //displays all the user interface on screen

struct MainData;

void sdl_stop ( void );
void sdl_start ( LogEngine * log );
int processMainConfigFile ( DOMNode * n, void * data);

struct MainData
{
    LOG_LEVEL localLogLevel;
    char * localLogName;
};

#endif
