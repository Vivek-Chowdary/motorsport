
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

    systemData->graphicsData.ogreRoot = new Root (  );
    setupResources (  );
    if ( !configure (  ) )
        return false;
    systemData->graphicsData.ogreSceneManager =
        systemData->graphicsData.ogreRoot->getSceneManager ( ST_GENERIC );
    // Create the camera
    worldData->camera1->ogreCamera =
        systemData->graphicsData.ogreSceneManager->createCamera ( "Camera1" );
    // Position it at 500 in Z direction
    worldData->camera1->ogreCamera->setPosition ( Vector3 ( 0, 0, 500 ) );
    // Look back along -Z
    worldData->camera1->ogreCamera->lookAt ( Vector3 ( 0, 0, -300 ) );
    worldData->camera1->ogreCamera->setNearClipDistance ( 5 );
    // Create one viewport, entire window
    Viewport *vp =
        systemData->graphicsData.ogreWindow->addViewport ( worldData->camera1->
                                                           ogreCamera );
    vp->setBackgroundColour ( ColourValue ( 0, 0, 0 ) );
    // Set default mipmap level (NB some APIs ignore this)
    TextureManager::getSingleton (  ).setDefaultNumMipMaps ( 5 );

    // Create the scene
    systemData->graphicsData.ogreSceneManager->setSkyBox ( true,
                                                           "MotorsportSkyBox" );

    for ( int i = 0; i < worldData->numberOfCubes; i++ )
    {
        char nombre[20];

        sprintf ( nombre, "Cubo%i", i );
        worldData->cubeList[i].cubeEntity =
            systemData->graphicsData.ogreSceneManager->createEntity ( nombre,
                                                                      "../data/cube.mesh" );
        worldData->cubeList[i].cubeNode =
            static_cast <
            SceneNode *
            >( systemData->graphicsData.ogreSceneManager->
               getRootSceneNode (  )->createChild (  ) );
        worldData->cubeList[i].cubeNode->attachObject ( worldData->cubeList[i].
                                                        cubeEntity );
        worldData->cubeList[i].cubeNode->translate ( i % 10 * 300,
                                                     i / 10 % 10 * 300,
                                                     i / 100 % 10 * 300 );
    }

    MaterialManager::getSingleton (  ).setDefaultAnisotropy ( systemData->
                                                              graphicsData.
                                                              anisotropic );
    MaterialManager::getSingleton (  ).setDefaultTextureFiltering ( systemData->
                                                                    graphicsData.
                                                                    filtering );
    return ( 0 );

}

bool GraphicsEngine::configure (  )
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
    //        if(mRoot->showConfigDialog())
    if ( manualInitialize (  ) )
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window
        // by passing 'true'
        systemData->graphicsData.ogreWindow =
            systemData->graphicsData.ogreRoot->initialise ( true );
        return true;
    }
    return false;
}

bool GraphicsEngine::manualInitialize (  )
{
    RenderSystem *renderSystem;
    bool ok = false;
    RenderSystemList *renderers =
        Root::getSingleton (  ).getAvailableRenderers (  );
    // See if the list is empty (no renderers available)
    if ( renderers->empty (  ) )
        return false;
    for ( RenderSystemList::iterator it = renderers->begin (  );
          it != renderers->end (  ); it++ )
    {
        renderSystem = ( *it );
        if ( strstr ( &( *renderSystem->getName (  ) ), "OpenGL" ) )
        {
            ok = true;
            break;
        }
    }
    if ( !ok )
    {
        // We still don't have a renderer; pick up the first one from the list
        renderSystem = ( *renderers->begin (  ) );
    }

    Root::getSingleton (  ).setRenderSystem ( renderSystem );
    char resolution[32];

    sprintf ( resolution, "%i x %i", graphicsData->width,
              graphicsData->height );

    // Manually set configuration options. These are optional.
    renderSystem->setConfigOption ( "Video Mode", resolution );

    return true;
}

void GraphicsEngine::setupResources ( void )
{
    // Load resource paths from config file
    ConfigFile cf;

    cf.load ( "resources.cfg" );

    // Go through all settings in the file
    ConfigFile::SettingsIterator i = cf.getSettingsIterator (  );

    String typeName,
        archName;

    while ( i.hasMoreElements (  ) )
    {
        typeName = i.peekNextKey (  );
        archName = i.getNext (  );
        ResourceManager::addCommonArchiveEx ( archName, typeName );
    }
}

int GraphicsEngine::step ( void )
    //makes the graphics engine draw one frame
{
    systemData->graphicsData.ogreRoot->_fireFrameStarted (  );
    showStatistics ( systemData->graphicsData.getStatisticsEnabled (  ) );
    systemData->graphicsData.ogreWindow->update (  );
    systemData->graphicsData.ogreRoot->_fireFrameEnded (  );
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

    //static String worstFps = "Worst FPS: ";
    static String worstFps = "Physics Rate: ";
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
                          StringConverter::toString ( stats.bestFPS ) + "FPS " +
                          StringConverter::toString ( stats.bestFrameTime ) +
                          " ms" );
    guiWorst->setCaption ( worstFps +
                           StringConverter::toString ( 1000.0f /
                                                       float ( systemData->
                                                               physicsData.
                                                               timeStep ) ) +
                           //StringConverter::toString ( stats.worstFPS ) +
                           "Hz " +
                           //StringConverter::toString ( stats.
                           //                            worstFrameTime ) +
                           StringConverter::toString ( systemData->physicsData.
                                                       timeStep ) + " ms" );
    //" ms" );
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
    //finally stop the log engine
    log.stop (  );

    return ( 0 );
}
