
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

#include "guiEngine.hpp"

int GuiEngine::start ( WorldData * wrlData, SystemData * sysData )
{
    //first of all start the logger (automatically logs the start of itself)
    log.start ( LOG_INFO, "logGui.txt" );

    //get the direction of the graphics data
    log.put ( LOG_INFO, "Setting up data pointers..." );
    worldData = wrlData;
    systemData = sysData;
    log.append ( LOG_INFO, "Ok" );

    return ( 0 );
}

int GuiEngine::step ( void )
//makes the graphics engine draw one frame
{
    log.put ( LOG_VERBOSE, "Doing an step..." );
    log.append ( LOG_VERBOSE, "Ok" );

    return ( 0 );
}

int GuiEngine::stop ( void )
{
    //finally stop the log engine
    log.stop (  );

    return ( 0 );
}
