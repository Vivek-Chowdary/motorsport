
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

#ifndef SYSTEM_HPP
#    define SYSTEM_HPP

#    include "SDL.h"
#    include "Ogre.h"


class SystemData
{                               //this contains all the data not related with the simulated/virtual world
    bool statisticsEnabled;
    static SystemData * systemDataPointer;
    bool mainLoopEnabled;
  public:
    static SystemData * getSystemDataPointer ( );
    SystemData ( );
    ~SystemData ( );
    
    Uint32 physicsTimeStep;
    Uint32 physicsDesiredStepsPerSecond;

    //main loop time control data
    Uint32 currentMainLoopTime;
    Uint32 currentPhysicsTime;
    Uint32 lastStatTime;
    int physicsSteps;
    Uint32 physicsStepsPerSecond;
    int graphicsSteps;
    Uint32 graphicsStepsPerSecond;

    bool canMainLoopRun ( void );   //does the program have to keep running?
    void enableMainLoop ( void );   //allows the program to start running
    void disableMainLoop ( void );  //allows the program to stop running

    bool getStatisticsEnabled (  );
    void invertStatisticsEnabled (  );
    void enableStatistics (  );

    Ogre::RenderWindow * ogreWindow;
};

#endif
