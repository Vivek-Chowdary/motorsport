
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

GuiEngine::GuiEngine ()
{
    // first of all start the logger (automatically logs the start of itself)
    GuiData *data = new GuiData;
    data->gui = this;
    data->showStatistics = 1;
    processXmlFile ("guiConfig.xml", &GuiEngine::processGuiConfigFile, (void *) data);

    log = new LogEngine (data->localLogLevel, data->localLogName);
    log->put (LOG_INFO, "Temporary parsing data already loaded into memory...");

    // get the direction of the graphics data
    log->put (LOG_INFO, "Setting up data pointers...");
    worldData = WorldData::getWorldDataPointer ();
    systemData = SystemData::getSystemDataPointer ();

    log->put (LOG_INFO, "Setting showStatistics ");
    showStatistics = data->showStatistics;

    log->put (LOG_INFO, "Unloading temporary parsing data from memory...");
    delete[](data->localLogName);
    delete data;
}

int GuiEngine::computeStep (void)
//makes the graphics engine draw one frame
{
    log->put (LOG_VERBOSE, "Doing an step...");
    static Uint32 lastRefreshTime = 0;
    if (SDL_GetTicks () - lastRefreshTime >= 1000)
    {
        updateStatistics ();
        lastRefreshTime += 1000;
    }

    Overlay *overlay = (Overlay *) OverlayManager::getSingleton ().getByName ("gui");
    if (!overlay)
    {
        Except (Exception::ERR_ITEM_NOT_FOUND, "Could not find gui overlay", "statusPanel");
    }

    if (systemData->getInvertShowStatistics ())
    {
        log->put (LOG_INFO, "Showing/hiding statistics on screen.");
        showStatistics = !showStatistics;
    }
    if (showStatistics)
        overlay->show ();
    else
        overlay->hide ();

    return (0);
}


void GuiEngine::updateStatistics (void)
{
    // update stats when necessary
    GuiElement *guiAvg = GuiManager::getSingleton ().getGuiElement ("gui/AverageFps");
    GuiElement *guiCurr = GuiManager::getSingleton ().getGuiElement ("gui/CurrFps");
    GuiElement *guiBest = GuiManager::getSingleton ().getGuiElement ("gui/BestFps");
    GuiElement *guiPhysics = GuiManager::getSingleton ().getGuiElement ("gui/PhysicsRate");
    const RenderTarget::FrameStats & stats = systemData->ogreWindow->getStatistics ();
    guiAvg->setCaption ("Average FPS: " + StringConverter::toString (stats.avgFPS));
    guiCurr->setCaption ("Current FPS: " + StringConverter::toString (systemData->graphicsFrequency));
    guiBest->setCaption ("Best FPS: " + StringConverter::toString (stats.bestFPS) + "FPS " + StringConverter::toString (stats.bestFrameTime) + " ms");
    guiPhysics->setCaption ("Physics Rate: " + StringConverter::toString (systemData->physicsFrequency) + "Hz " + StringConverter::toString (systemData->physicsTimeStep) + " ms");
    GuiElement *guiTris = GuiManager::getSingleton ().getGuiElement ("gui/NumTris");
    guiTris->setCaption ("Triangle Count: " + StringConverter::toString (stats.triangleCount));
    GuiElement *guiDbg = GuiManager::getSingleton ().getGuiElement ("gui/DebugText");
    guiDbg->setCaption (systemData->ogreWindow->getDebugText ());
}

GuiEngine::~GuiEngine (void)
{
    // finally stop the log engine
    delete log;
}
