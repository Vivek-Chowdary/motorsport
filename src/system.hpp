
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

class GraphicsData
{
  private:
    bool statisticsEnabled;
  public:
    SDL_Surface * screen;
    SDL_PixelFormat *fmt;

    int width;
    int height;
    int bpp;
    Uint32 flags;

    char *title;
    char *icon;
    int titleLength;
    int iconLength;

    bool getStatisticsEnabled (  );
    void invertStatisticsEnabled (  );
    void enableStatistics (  );

    int anisotropic;
    Ogre::TextureFilterOptions filtering;
    Ogre::RenderWindow * ogreWindow;

    //note that the camera data (origin, dest., angle, focus, etc...) should not
    // be here. that's part of the worldData. there should exist real TV
    // cameras in the track, and the virtual-driver's eyes are the in-car
    // camera.
    // Of course, we can add more cameras than TV+driver+spectators. We can make
    // use of some of the advantages of being a simulation in a computer :)
};

class InputData
{
    public:
    //here will go the definition of controllers, be it keyboard, mouse, wheel...
    // and probably some other things, such as number of controllers, number of
    // axis available, axis-calibration data...
};

class PhysicsData
{
    public:
    Uint32 timeStep;
    Uint32 desiredStepsPerSecond;
    // this struct is used by the physics engine. it will store things like
    // timestep to use... .... mmm well, i can't think of other example of
    // system data for the physicsEngine, but i suppose you get the idea.
};

class GuiData
{
    public:
    int nextMenuIndex;          //main menu = 1;  options menu = 2;
    int lastMenuIndex;
};

class SystemData
{                               //this contains all the data not related with the simulated/virtual world

    bool mainLoopEnabled;
    bool simLoopEnabled;
    bool guiLoopEnabled;
  public:
    //independent data
    GraphicsData graphicsData;
    InputData inputData;
    PhysicsData physicsData;
    GuiData guiData;

    //main loop time control data
    Uint32 currentSimLoopTime;
    Uint32 currentPhysicsTime;
    Uint32 lastStatTime;
    int physicsSteps;
    Uint32 physicsStepsPerSecond;
    int graphicsSteps;
    Uint32 graphicsStepsPerSecond;

    bool canMainLoopRun ( void );   //does the program have to keep running?
    bool canSimLoopRun ( void );    //does the simulation have to keep running?
    bool canGuiLoopRun ( void );    //does the gui have to keep running?

    void enableMainLoop ( void );   //allows the program to start running
    void enableSimLoop ( void );    //allows the simulation to start running
    void enableGuiLoop ( void );    //allows the gui to start running

    void disableMainLoop ( void );  //allows the program to stop running
    void disableSimLoop ( void );   //allows the simulation to stop running
    void disableGuiLoop ( void );   //allows the gui to stop running

};

#endif
