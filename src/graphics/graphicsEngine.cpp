
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

#include "graphicsEngine.hpp"

GraphicsEngine::GraphicsEngine (  )
{
    //first of all start the logger (automatically logs the start of itself)
    GraphicsData * data = new GraphicsData;
    data->graphics = this;
    processXmlFile ("graphicsConfig.xml", &GraphicsEngine::processGraphicsConfigFile, (void*)data);

    log = new LogEngine ( data->localLogLevel, data->localLogName );
    log->put( LOG_INFO, "Temporary parsing data already loaded into memory...");

    //get the direction of the graphics data
    log->put ( LOG_INFO, "Setting up data pointers..." );
    worldData = WorldData::getWorldDataPointer (  );
    systemData = SystemData::getSystemDataPointer (  );

    log->put ( LOG_INFO, "Loading screenshot filename into memory...");
    screenshotFilename = new char[strlen(data->screenshotFile)+1];
    strncpy (screenshotFilename, data->screenshotFile, strlen(data->screenshotFile)+1);

    log->put ( LOG_INFO, "Setting screen properties..." );
    width = data->width;
    height = data->height;
    bpp = data->bpp;
    fullScreen = data->fullScreen;
    log->format ( LOG_INFO, "Graphics data initialized for %ix%i@%ibpp", width,
                  height, bpp );

    ogreRoot = new Ogre::Root (  );
    setupResources ( data );
    // Initialise the system
    if ( !manualInitialize ( data ) )
    {
//        return false;
    }
    // Here we choose to let the system create a default rendering window
    // by passing 'true'
    ogreRoot->getRenderSystem (  )->setConfigOption ( "Full Screen",
                                                      fullScreen ? "Yes" :
                                                      "No" );
    systemData->ogreWindow = ogreRoot->initialise ( true );
    systemData->ogreSceneManager =
        ogreRoot->getSceneManager ( data->sceneManager );

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton (  ).setDefaultNumMipMaps ( data->defaultNumMipMaps );

    // Create the skybox
    Ogre::Quaternion rotationToZAxis;
    rotationToZAxis.
        FromRotationMatrix ( Ogre::Matrix3 ( 1, 0, 0, 0, 0, -1, 0, 1, 0 ) );
    systemData->ogreSceneManager->setSkyBox ( true, "skybox", 5000, true,
                                              rotationToZAxis );

    //Set some graphics settings
    Ogre::MaterialManager::getSingleton (  ).setDefaultAnisotropy ( data->anisotropy );
    Ogre::MaterialManager::getSingleton (  ).
        setDefaultTextureFiltering ( data->filtering );

    log->put( LOG_INFO, "Unloading temporary parsing data from memory...");
    delete [](data->localLogName);
    delete [](data->screenshotFile);
    delete [](data->ogreConfigFile);
    delete [](data->renderer);
    delete data;
}

bool GraphicsEngine::manualInitialize ( GraphicsData * data )
{
    Ogre::RenderSystem * renderSystem;
    bool ok = false;

    Ogre::RenderSystemList * renderers =
        Ogre::Root::getSingleton (  ).getAvailableRenderers (  );
    // See if the list is empty (no renderers available)
    if ( renderers->empty (  ) )
        return false;
    for ( Ogre::RenderSystemList::iterator it = renderers->begin (  );
          it != renderers->end (  ); it++ )
    {
        renderSystem = ( *it );
        log->put ( LOG_INFO, "Loading ogre renderer name into memory...");
        if ( strstr ( &( *renderSystem->getName (  ) ), data->renderer ) )
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

    Ogre::Root::getSingleton (  ).setRenderSystem ( renderSystem );
    char resolution[32];

    sprintf ( resolution, "%i x %i", width, height );

    // Manually set configuration options. These are optional.
    renderSystem->setConfigOption ( "Video Mode", resolution );

    return true;
}

void GraphicsEngine::setupResources ( GraphicsData * data )
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    log->put ( LOG_INFO, "Loading ogre config filename into memory...");
    cf.load ( data->ogreConfigFile );

    // Go through all settings in the file
    Ogre::ConfigFile::SettingsIterator i = cf.getSettingsIterator (  );
    Ogre::String typeName, archName;
    while ( i.hasMoreElements (  ) )
    {
        typeName = i.peekNextKey (  );
        archName = i.getNext (  );
        Ogre::ResourceManager::addCommonArchiveEx ( archName, typeName );
    }
}

int GraphicsEngine::step ( void )
{
    //take a screenshot if needed
    if ( systemData->getTakeScreenshot (  ) )
    {
        log->format ( LOG_INFO, "Taking a screenshot in %s.", screenshotFilename );
        systemData->ogreWindow->writeContentsToFile ( screenshotFilename );
    }
    
    //Update Ogre's cubes positions with Ode's positions.
    int numberOfCubes = Cube::cubeList.size (  );

    for ( int currentCube = 0; currentCube < numberOfCubes; currentCube++ )
    {
        Cube::cubeList[currentCube]->stepGraphics (  );
    }
    //Let the listener frames be started and ended: they are needed for particle systems.
    ogreRoot->_fireFrameStarted (  );
    systemData->ogreWindow->update (  );
    ogreRoot->_fireFrameEnded (  );

    return ( 0 );
}

GraphicsEngine::~GraphicsEngine ( void )
{
    log->put ( LOG_INFO, "Unloading ogre window data from memory..." );
    delete ( systemData->ogreWindow );
    
    log->put ( LOG_INFO, "Unloading screenshot filename from memory...");
    delete screenshotFilename;

    //finally stop the log engine
    delete log;
}
