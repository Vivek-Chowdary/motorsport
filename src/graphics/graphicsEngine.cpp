
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
#include "world.hpp"
#include "graphicsEngine.hpp"
#include <stdlib.h>

int GraphicsEngine::start ( WorldData * wrlData, SystemData * sysData )
{
    //first of all start the logger (automatically logs the start of itself)
    log.start ( LOG_VERBOSE, "logGraphics.txt" );

    //get the direction of the graphics data
    log.put ( LOG_INFO, "Setting up data pointers..." );
    graphicsData = &( sysData->graphicsData );
    worldData = wrlData;
    systemData = sysData;
    log.append ( LOG_INFO, "Ok" );

    printf ( "\nhaciendo new simulation" );
    sim = new Simulation;

    printf ( "\ntrying the simulation" );
    try
    {
        sim->go ( graphicsData->width, graphicsData->height, worldData,
                  systemData );
    } catch ( Exception & e )
    {
        fprintf ( stderr, "An exception has occured: %s\n",
                  e.getFullDescription (  ).c_str (  ) );
    }
    return ( 0 );
}

int GraphicsEngine::step ( void )
//makes the graphics engine draw one frame
{

    sim->mRoot->_fireFrameStarted (  );
    showStatistics ( systemData->graphicsData.getStatisticsEnabled() );
    systemData->graphicsData.ogreWindow->update (  );
    sim->mRoot->_fireFrameEnded (  );
    updateStatistics (  );

    return ( 0 );
}

void GraphicsEngine::showStatistics ( bool show )
{
    Overlay *o = ( Overlay * ) OverlayManager::getSingleton (  ).
        getByName ( "Core/DebugOverlay" );
    if ( !o )
        Except ( Exception::ERR_ITEM_NOT_FOUND,
                 "Could not find overlay Core/DebugOverlay",
                 "showDebugOverlay" );
    if ( show )
    {
        o->show (  );
    }
    else
    {
        o->hide (  );
    }
}
void GraphicsEngine::updateStatistics (  )
{
    static String currFps = "Current FPS: ";
    static String avgFps = "Average FPS: ";
    static String bestFps = "Best FPS: ";
    static String worstFps = "Worst FPS: ";
    static String tris = "Triangle Count: ";

// update stats when necessary
    GuiElement *guiAvg =
        GuiManager::getSingleton (  ).getGuiElement ( "Core/AverageFps" );
    GuiElement *guiCurr =
        GuiManager::getSingleton (  ).getGuiElement ( "Core/CurrFps" );
    GuiElement *guiBest =
        GuiManager::getSingleton (  ).getGuiElement ( "Core/BestFps" );
    GuiElement *guiWorst =
        GuiManager::getSingleton (  ).getGuiElement ( "Core/WorstFps" );
    const RenderTarget::FrameStats & stats =
        systemData->graphicsData.ogreWindow->getStatistics (  );
    guiAvg->setCaption ( avgFps + StringConverter::toString ( stats.avgFPS ) );
    guiCurr->setCaption ( currFps +
                          StringConverter::toString ( stats.lastFPS ) );
    guiBest->setCaption ( bestFps +
                          StringConverter::toString ( stats.bestFPS ) + " " +
                          StringConverter::toString ( stats.bestFrameTime ) +
                          " ms" );
    guiWorst->setCaption ( worstFps +
                           StringConverter::toString ( stats.worstFPS ) +
                           " " +
                           StringConverter::toString ( stats.
                                                       worstFrameTime ) +
                           " ms" );
    GuiElement *guiTris =
        GuiManager::getSingleton (  ).getGuiElement ( "Core/NumTris" );
    guiTris->setCaption ( tris +
                          StringConverter::toString ( stats.triangleCount ) );
    GuiElement *guiDbg =
        GuiManager::getSingleton (  ).getGuiElement ( "Core/DebugText" );
    guiDbg->setCaption ( systemData->graphicsData.ogreWindow->
                         getDebugText (  ) );

}

int GraphicsEngine::stop ( void )
{
    delete sim;

    //sim = NULL;

    //finally stop the log engine
    log.stop (  );

    return ( 0 );
}
