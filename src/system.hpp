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
#define SYSTEM_HPP
#ifdef WIN32
	#include "sdl.h"
#else										
	#include <SDL/SDL.h>
#endif

/******************************************************************************
*
* Classes
*
******************************************************************************/

struct GraphicsData
{
    SDL_Surface *screen;
    SDL_PixelFormat *fmt;
    int width;
    int height;
    int bpp;
    Uint32 flags;
    char *title;
    char *icon;
};

struct InputData
{
    //here will go the definition of controllers, be it keyboard, mouse, wheel...
    // and probably some other things, such as number of controllers, number of
    // axis available, axis-calibration data...
};

class SystemData
{ //this contains all the data not related with the simulated/virtual world

    int mainLoopDone;    //allows to stop the mainloop (exit)
    public:
        //main program
        void startMainLoop (void); //sets mainLoopDone to 1
        void stopMainLoop (void); //set mainLoopDone to 0
        int isLoopDone (void);

        struct GraphicsData graphicsData;
        struct InputData inputData;

};


#endif
