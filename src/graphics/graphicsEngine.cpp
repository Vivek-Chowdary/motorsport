
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
    worldData->camera1->ogreCamera->setFixedYawAxis(true,Vector3(0,0,1));
    worldData->camera1->ogreCamera->setPosition ( Vector3 ( 0, 0, 0 ) );
    worldData->camera1->ogreCamera->lookAt ( Vector3 ( 10, 10, 0 ) );
    worldData->camera1->ogreCamera->setNearClipDistance ( 5 );
    
    // Create one viewport, entire window
    Viewport *vp =
        systemData->graphicsData.ogreWindow->addViewport ( worldData->camera1->
                                                           ogreCamera );
    vp->setBackgroundColour ( ColourValue ( 0, 0, 0 ) );
    
    // Set default mipmap level (NB some APIs ignore this)
    TextureManager::getSingleton (  ).setDefaultNumMipMaps ( 5 );

    // Create the skybox
    Quaternion rotationToZAxis;
    rotationToZAxis.FromRotationMatrix(Matrix3(1,0,0,0,0,-1,0,1,0));
    systemData->graphicsData.ogreSceneManager->setSkyBox ( true,
                                                           "MotorsportSkyBox", 5000, true, rotationToZAxis );
                                                           
    //Create cubes
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
    }

    //Set some graphics settings
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
    // Initialise the system
    if ( manualInitialize (  ) )
    {
        // If returned true, everything's ok, we have a valid config.
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
{
    //Update Ogre's cubes positions with Ode's positions.
    for ( int currentCube = 0;
          currentCube < worldData->numberOfCubes; currentCube++ )
    {
        worldData->cubeList[currentCube].updateOgrePosition();
    }

    //Update status of statistics overlays.
    showStatistics ( systemData->graphicsData.getStatisticsEnabled (  ) );
    
    //Let the listener frames be started and ended: they are needed for particle systems.
    systemData->graphicsData.ogreRoot->_fireFrameStarted (  );
    systemData->graphicsData.ogreWindow->update (  );
    systemData->graphicsData.ogreRoot->_fireFrameEnded (  );
    
    //Update statistics.... this should be done inside the main loop.
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
    o->hide (  );
    if ( show )
    {
        o->show (  );
    }
}

void GraphicsEngine::updateStatistics (  )
{
    // update stats when necessary
    GuiElement *guiAvg = GuiManager::getSingleton (  ).getGuiElement ( "Core/AverageFps" );
    GuiElement *guiCurr = GuiManager::getSingleton (  ).getGuiElement ( "Core/CurrFps" );
    GuiElement *guiBest = GuiManager::getSingleton (  ).getGuiElement ( "Core/BestFps" );
    GuiElement *guiWorst = GuiManager::getSingleton (  ).getGuiElement ( "Core/WorstFps" );
    const RenderTarget::FrameStats & stats = systemData->graphicsData.ogreWindow->getStatistics (  );
    guiAvg->setCaption ( "Average FPS: " + StringConverter::toString ( stats.avgFPS ) );
    guiCurr->setCaption ( "Current FPS: " + StringConverter::toString ( systemData->graphicsStepsPerSecond ) );
    guiBest->setCaption ( "Best FPS: " + StringConverter::toString ( stats.bestFPS ) + "FPS " + StringConverter::toString ( stats.bestFrameTime ) + " ms" );
    guiWorst->setCaption ( "Physics Rate: " + StringConverter::toString ( systemData->physicsStepsPerSecond ) + "Hz " + StringConverter::toString ( systemData->physicsData.timeStep ) + " ms" );
    GuiElement *guiTris = GuiManager::getSingleton (  ).getGuiElement ( "Core/NumTris" );
    guiTris->setCaption ( "Triangle Count: " + StringConverter::toString ( stats.triangleCount ) );
    GuiElement *guiDbg = GuiManager::getSingleton (  ).getGuiElement ( "Core/DebugText" );
    guiDbg->setCaption ( systemData->graphicsData.ogreWindow->getDebugText (  ) );
}

int GraphicsEngine::stop ( void )
{
    //finally stop the log engine
    log.stop (  );

    return ( 0 );
}
