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

#include "guiEngine.hpp"

/******************************************************************************
*
* Functions
*
******************************************************************************/

PARAGUI_CALLBACK(optionsMenu_handler)
{
    ((GuiEngine*)clientdata)->getSystemData()->guiData.nextMenuIndex = 2;
    ((GuiEngine*)clientdata)->getCurrentMenu()->Quit();

    return true;
}

PARAGUI_CALLBACK(goBack_handler)
{
    ((GuiEngine*)clientdata)->getSystemData()->guiData.nextMenuIndex = 1;
    ((GuiEngine*)clientdata)->getCurrentMenu()->Quit();

    return true;
}

PARAGUI_CALLBACK(exit_handler)
{
    ((GuiEngine*)clientdata)->getSystemData()->disableGuiLoop();
    ((GuiEngine*)clientdata)->getSystemData()->disableSimLoop();
    ((GuiEngine*)clientdata)->getSystemData()->disableMainLoop();
    ((GuiEngine*)clientdata)->getCurrentMenu()->Quit();

    return true;
}

PARAGUI_CALLBACK(startSim_handler)
{
    ((GuiEngine*)clientdata)->getLog()->put(LOG_INFO,"Starting simulation.");
    ((GuiEngine*)clientdata)->getSystemData()->enableSimLoop();
    ((GuiEngine*)clientdata)->getSystemData()->disableGuiLoop();
    ((GuiEngine*)clientdata)->getCurrentMenu()->Quit();

    return true;
}

PARAGUI_CALLBACK(setRes800_handler)
{
    ((GuiEngine*)clientdata)->getSystemData()->graphicsData.width=800;
    ((GuiEngine*)clientdata)->getSystemData()->graphicsData.height=600;

    return true;
}

PARAGUI_CALLBACK(setRes640_handler)
{
    ((GuiEngine*)clientdata)->getSystemData()->graphicsData.width=640;
    ((GuiEngine*)clientdata)->getSystemData()->graphicsData.height=480;

    return true;
}

int GuiEngine::optionsMenu(void) {
    PG_Application tmpMenu;
    currentMenu = &tmpMenu;
    currentMenu->LoadTheme("default");

    currentMenu->DisableDirtyUpdates (true);

    if (!currentMenu->InitScreen (640,480,0,SDL_SWSURFACE))
    {
        log.format (LOG_ERROR,"Resolution %dx%d not supported\n",640,480);
        exit (-1);
    }

    PG_Button buttonSetRes800(NULL,1,PG_Rect(230, 50, 180, 50),"800x600");
    buttonSetRes800.SetTransparency(128,255,64);
    buttonSetRes800.SetEventCallback(MSG_BUTTONCLICK,setRes800_handler, this);
    buttonSetRes800.Show();

    PG_Button buttonSetRes640(NULL,1,PG_Rect(230, 100, 180, 50),"640x480");
    buttonSetRes640.SetTransparency(128,255,64);
    buttonSetRes640.SetEventCallback(MSG_BUTTONCLICK,setRes640_handler, this);
    buttonSetRes640.Show();

    PG_Button buttonExit(NULL,1,PG_Rect(230, 200, 180, 50),"Go back");
    buttonExit.SetTransparency(128,255,64);
    buttonExit.SetEventCallback(MSG_BUTTONCLICK, goBack_handler, this);
    buttonExit.Show();

    currentMenu->Run(false);

    return 0;
}

int GuiEngine::mainMenu(void) {
    PG_Application tmpMenu;
    currentMenu = &tmpMenu;
    currentMenu->LoadTheme("default");

    currentMenu->DisableDirtyUpdates (true);

    if (!currentMenu->InitScreen (640,480,0,SDL_SWSURFACE))
    {
        log.format (LOG_ERROR,"Resolution %dx%d not supported\n",640,480);
        exit (-1);
    }

    PG_Button buttonStartSim(NULL,1,PG_Rect(230, 50, 180, 50),"Start simulation");
    buttonStartSim.SetTransparency(128,255,64);
    buttonStartSim.SetEventCallback(MSG_BUTTONCLICK,startSim_handler, this);
    buttonStartSim.Show();

    PG_Button buttonOptionsMenu(NULL,1,PG_Rect(230, 100, 180, 50),"Options Menu");
    buttonOptionsMenu.SetTransparency(128,255,64);
    buttonOptionsMenu.SetEventCallback(MSG_BUTTONCLICK, optionsMenu_handler, this);
    buttonOptionsMenu.Show();

    PG_Button buttonExit(NULL,1,PG_Rect(230, 200, 180, 50),"Exit");
    buttonExit.SetTransparency(128,255,64);
    buttonExit.SetEventCallback(MSG_BUTTONCLICK, exit_handler, this);
    buttonExit.Show();

    currentMenu->Run(false);

    return 0;
}


PG_Application* GuiEngine::getCurrentMenu(void)
{
    return currentMenu;
}
WorldData *GuiEngine::getWorldData(void)
{
    return worldData;
}
SystemData *GuiEngine::getSystemData(void)
{
    return systemData;
}
LogEngine* GuiEngine::getLog(void)
{
    return &log;
}

int GuiEngine::start (WorldData *wrlData, SystemData *sysData)
{
    //first of all start the logger (automatically logs the start of itself)
    log.start(LOG_INFO, "logGui.txt");

    //get the direction of the graphics data
    log.put(LOG_INFO, "Setting up data pointers...");
    worldData = wrlData;
    systemData = sysData;
    log.append (LOG_INFO, "Ok");

    return (0);
}

int GuiEngine::step (void)
//makes the graphics engine draw one frame
{
    log.put(LOG_VERBOSE, "Doing an step...");
    switch (systemData->guiData.nextMenuIndex)
    {
        case 1:
            mainMenu ();
            break;
        case 2:
            optionsMenu ();
            break;
    }
    log.append(LOG_VERBOSE, "Ok");

    return (0);
}

int GuiEngine::stop (void)
{
    //finally stop the log engine
    log.stop ();
    
    return (0);
}
