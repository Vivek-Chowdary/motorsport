
/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright © 2000-2002 The OGRE Team
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
Filename:    ExampleApplication.h
Description: Base class for all the OGRE examples
-----------------------------------------------------------------------------
*/

#include "simulation.hpp"

Simulation::Simulation (  )
{
    mRoot = 0;
}

    /// Start the example
void Simulation::go ( int resX, int resY, WorldData * wrlData,
                      SystemData * sysData )
{
    this->worldData = wrlData;
    this->systemData = sysData;
    mRoot = new Root (  );
    setupResources (  );
    if ( !configure ( resX, resY ) )
        return /*false */ ;

    mSceneMgr = mRoot->getSceneManager ( ST_GENERIC );
    createCamera (  );
    createViewports (  );
    // Set default mipmap level (NB some APIs ignore this)
    TextureManager::getSingleton (  ).setDefaultNumMipMaps ( 5 );
    // Create the scene
    createScene (  );
    MaterialManager::getSingleton (  ).setDefaultAnisotropy ( systemData->
                                                              graphicsData.
                                                              anisotropic );
    MaterialManager::getSingleton (  ).setDefaultTextureFiltering ( systemData->
                                                                    graphicsData.
                                                                    filtering );

    return /*true */ ;
}

    /// Standard destructor
Simulation::~Simulation (  )
{
  //  if ( mRoot )
    //    delete mRoot;
 //   if (mSceneMgr)
//       delete mSceneMgr;
}

    // These internal methods package up the stages in the startup process

    /** Sets up the application - returns false if the user chooses to abandon configuration. */

    /** Configures the application - returns false if the user chooses to abandon configuration. */
bool Simulation::configure ( int resX, int resY )
{
    // Show the configuration dialog and initialise the system
    // You can skip this and use root.restoreConfig() to load configuration
    // settings if you were sure there are valid ones saved in ogre.cfg
//        if(mRoot->showConfigDialog())
    if ( manualInitialize ( "OpenGL", resX, resY ) )
    {
        // If returned true, user clicked OK so initialise
        // Here we choose to let the system create a default rendering window by passing 'true'
        systemData->graphicsData.ogreWindow = mRoot->initialise ( true );
        return true;
    }
    return false;
}

void Simulation::createCamera ( void )
{
    // Create the camera
    worldData->camera1->ogreCamera = mSceneMgr->createCamera ( "Camera1" );

    // Position it at 500 in Z direction
    worldData->camera1->ogreCamera->setPosition ( Vector3 ( 0, 0, 500 ) );
    // Look back along -Z
    worldData->camera1->ogreCamera->lookAt ( Vector3 ( 0, 0, -300 ) );
    worldData->camera1->ogreCamera->setNearClipDistance ( 5 );

}
bool Simulation::manualInitialize ( const String & desiredRenderer, int resX,
                                    int resY )
{
    RenderSystem *renderSystem;
    bool ok = false;

    RenderSystemList *renderers =
        Root::getSingleton (  ).getAvailableRenderers (  );

    // See if the list is empty (no renderers available)
    if ( renderers->empty (  ) )
        return false;

    printf ( "\n\nDesired renderer:[%s]", &( *desiredRenderer ) );
    printf ( "\n\nAvailable renderers(until we find the desired one:" );
    for ( RenderSystemList::iterator it = renderers->begin (  );
          it != renderers->end (  ); it++ )
    {
        renderSystem = ( *it );
        printf ( "\n\t[%s]", &( *renderSystem->getName (  ) ) );
        if ( strstr
             ( &( *renderSystem->getName (  ) ), &( *desiredRenderer ) ) )
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

    sprintf ( resolution, "%i x %i", resX, resY );

    // Manually set configuration options. These are optional.
    renderSystem->setConfigOption ( "Video Mode", resolution );

    return true;
}

void Simulation::createScene ( void )   // pure virtual - this has to be overridden
{
    mSceneMgr->setSkyBox ( true, "MotorsportSkyBox" );
}

void Simulation::createViewports ( void )
{
    // Create one viewport, entire window
    Viewport *vp =
        systemData->graphicsData.ogreWindow->addViewport ( worldData->camera1->
                                                           ogreCamera );

    vp->setBackgroundColour ( ColourValue ( 0, 0, 0 ) );
}

    /// Method which will define the source of resources (other than current folder)
void Simulation::setupResources ( void )
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
