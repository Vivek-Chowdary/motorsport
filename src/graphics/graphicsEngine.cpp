/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifdef WIN32
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include "OgreWin32Window.h"
#endif
#include "graphicsEngine.hpp"
#include "track.hpp"
#include "camera.hpp"
#include "cube.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "Ogre.h"
#include "OgreConfigFile.h"
#include "OgreNoMemoryMacros.h"
#include "log/logEngine.hpp"
#include "world.hpp"
#include "SDL/SDL_keysym.h"
#include "vehicle.hpp"

GraphicsEngine::GraphicsEngine ()
{
    XmlFile* xmlFile = new XmlFile ("graphicsConfig.xml");
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;
}

void GraphicsEngine::manualInitialize (const std::string & renderer)
{
    Ogre::RenderSystem * renderSystem;
    bool ok = false;

    log->put (LOG_DEVELOPER, "Loading Ogre renderers list into memory.");
    Ogre::RenderSystemList * renderers = Ogre::Root::getSingleton ().getAvailableRenderers ();
    if (renderers->empty ())
    {
        log->put (LOG_ERROR, "No Ogre renderers available!");
    }
    for (Ogre::RenderSystemList::iterator it = renderers->begin (); it != renderers->end (); it++)
    {
        renderSystem = (*it);
        log->format (LOG_DEVELOPER, "Locating desired renderer (%s).", renderer.c_str());
        if (renderSystem->getName() == renderer)
        {
            log->put (LOG_DEVELOPER, "Ok, desired renderer found.");
            ok = true;
            break;
        }
    }
    if (!ok)
    {
        log->put (LOG_WARNING, "Desired renderer not found. Using first available one.");
        renderSystem = (*renderers->begin ());
    }
    Ogre::Root::getSingleton ().setRenderSystem (renderSystem);

    char resolution[32];
    sprintf (resolution, "%i x %i", width, height);
    log->format (LOG_ENDUSER, "Setting screen resolution and bpp: %s", resolution);
    renderSystem->setConfigOption ("Video Mode", resolution);
}

void GraphicsEngine::setupResources (const std::string & ogreConfigFile)
{
    //load some default constant resources
    std::string dataDir = SystemData::getSystemDataPointer()->dataDir;
//    Ogre::MaterialManager::getSingleton().parseAllSources();

    // Load some the user resources
    Ogre::ConfigFile cf;
    log->put (LOG_CCREATOR, "Loading user resources into memory...");
    cf.load (ogreConfigFile.c_str());
    Ogre::ConfigFile::SettingsIterator i = cf.getSettingsIterator ();
    Ogre::String typeName, archName;
    while (i.hasMoreElements ())
    {
        typeName = i.peekNextKey ();
        archName = i.getNext ();
        std::string file = dataDir;
        file.append("/");
        file.append(archName);
        Ogre::ResourceManager::addCommonArchiveEx (file, typeName);
    }
}

int GraphicsEngine::computeStep (void)
{
    // take a screenshot if neededa
    if (systemData->axisMap[getIDKeyboardKey(SDLK_PRINT)]->getValue() == 1)
    {
        static unsigned int count = initialFrame;
        static char tmpName[64];
        sprintf (tmpName, screenshotFilename.c_str(), count++);
        log->format (LOG_ENDUSER, "Taking a screenshot in %s.", tmpName);
        systemData->ogreWindow->writeContentsToFile (tmpName);
    }

    // change camera if needed
    if (systemData->axisMap[getIDKeyboardKey(SDLK_c)]->getValue() == 1)
    {
        int nextCam;
        nextCam = World::getWorldPointer()->getActiveTrackCameraIndex()+1;
        int maxCams = World::getWorldPointer()->trackList[0]->cameraList.size();
        if (nextCam >= maxCams )
        {
            nextCam = 0;
        }
        World::getWorldPointer()->setActiveCamera(World::getWorldPointer ()->trackList[0]->cameraList[nextCam]);
        World::getWorldPointer()->activeTrackCamera = World::getWorldPointer()->trackList[0]->cameraList[nextCam];
    }
    if (systemData->axisMap[getIDKeyboardKey(SDLK_v)]->getValue() == 1)
    {
        int nextCam = World::getWorldPointer()->getActiveVehicleCameraIndex()+1;
        int maxCams = World::getWorldPointer()->vehicleList[0]->cameraList.size();
        if (nextCam >= maxCams )
        {
            nextCam = 0;
        }
        World::getWorldPointer()->setActiveCamera(World::getWorldPointer ()->vehicleList[0]->cameraList[nextCam]);
        World::getWorldPointer()->activeVehicleCamera = World::getWorldPointer()->vehicleList[0]->cameraList[nextCam];
    }
    // Update Ogre's bodies positions with Ode's positions.
    int numberOfVehicles = World::getWorldPointer ()->vehicleList.size ();
    for (int currentVehicle = 0; currentVehicle < numberOfVehicles; currentVehicle++)
    {
        World::getWorldPointer ()->vehicleList[currentVehicle]->stepGraphics ();
    }

    // Update Ogre's cubes positions with Ode's positions.
    int numberOfCubes = World::getWorldPointer ()->trackList[0]->cubeList.size ();
    for (int currentCube = 0; currentCube < numberOfCubes; currentCube++)
    {
        World::getWorldPointer ()->trackList[0]->cubeList[currentCube]->stepGraphics ();
    }

    // Update cameras positions (should be done in the fsx engine FIXME.
    int numberOfCameras = World::getWorldPointer ()->trackList[0]->cameraList.size ();
    for (int currentCamera = 0; currentCamera < numberOfCameras; currentCamera++)
    {
        World::getWorldPointer ()->trackList[0]->cameraList[currentCamera]->stepGraphics ();
    }
    numberOfCameras = World::getWorldPointer ()->vehicleList[0]->cameraList.size ();
    for (int currentCamera = 0; currentCamera < numberOfCameras; currentCamera++)
    {
        World::getWorldPointer ()->vehicleList[0]->cameraList[currentCamera]->stepGraphics ();
    }

    // Let the listener frames be started and ended: they are needed for particle systems.
    ogreRoot->_fireFrameStarted ();
    systemData->ogreWindow->update ();
    ogreRoot->_fireFrameEnded ();

    return (0);
}

GraphicsEngine::~GraphicsEngine (void)
{
    log->put (LOG_DEVELOPER, "Unloading ogre window data from memory...");
    delete (systemData->ogreWindow);

    // finally stop the log engine
    delete log;
}

void GraphicsEngine::processXmlRootNode (DOMNode * n)
{
    LOG_LEVEL localLogLevel = LOG_DEVELOPER;
    std::string localLogName = "GFX";
    screenshotFilename.assign ("frame%i.jpg");
    initialFrame = 0;
    std::string ogreConfigFile = "resources.cfg";
    #ifdef WIN32
    std::string ogrePluginsDir = "plugins";
    #else
    std::string ogrePluginsDir = "/usr/lib/OGRE";
    #endif
    Ogre::SceneType sceneManager = Ogre::ST_GENERIC;
    int anisotropy = 1;
    Ogre::TextureFilterOptions filtering = Ogre::TFO_NONE;
    width = 800;
    height = 600;
    bpp = 0;
    std::string renderer = "OpenGL";
    int defaultNumMipMaps = 5;
    fullScreen = false;

    LogEngine * tmpLog = new LogEngine (LOG_DEVELOPER, "XML");
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            std::string name;
            assignXmlString (name, n->getNodeName());
            tmpLog->format (LOG_DEVELOPER, "Name: %s", name.c_str());
            if (name == "graphicsConfig")
            {
                tmpLog->put (LOG_DEVELOPER, "Found the graphics engine config element.");
                if (n->hasAttributes ())
                {
                    // get all the attributes of the node
                    DOMNamedNodeMap *attList = n->getAttributes ();
                    int nSize = attList->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                        std::string attribute;
                        assignXmlString (attribute, attNode->getName());
                        if (attribute == "localLogLevel")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            localLogLevel = stologlevel (attribute);
                            tmpLog->format (LOG_ENDUSER, "Found the local log level: %s", attribute.c_str());
                        }
                        if (attribute == "localLogName")
                        {
                            assignXmlString (localLogName, attNode->getValue());
                            tmpLog->format (LOG_ENDUSER, "Found the log name: %s", localLogName.c_str());
                        }
                        if (attribute == "screenshotFile")
                        {
                            assignXmlString (screenshotFilename, attNode->getValue());
                            tmpLog->format (LOG_ENDUSER, "Found the screenshot filename: %s", screenshotFilename.c_str());
                        }
                        if (attribute == "initialScreenshotFileNumber")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            tmpLog->format (LOG_ENDUSER, "Found the initial screenshot number: %s", attribute.c_str());
                            initialFrame = stoi (attribute);
                        }
                    }
                }
                for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
                {
                    if (n)
                    {
                        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
                        {
                            assignXmlString (name, n->getNodeName());
                            tmpLog->format (LOG_DEVELOPER, "Name: %s", name.c_str());
                            if (name == "ogre")
                            {
                                tmpLog->put (LOG_DEVELOPER, "Found the ogre config element.");
                                if (n->hasAttributes ())
                                {
                                    DOMNamedNodeMap *attList = n->getAttributes ();
                                    int nSize = attList->getLength ();
                                    for (int i = 0; i < nSize; ++i)
                                    {
                                        DOMAttr *attNode = (DOMAttr *) attList->item (i);
                                        std::string attribute;
                                        assignXmlString (attribute, attNode->getName());
                                        if (attribute == "configFile")
                                        {
                                            assignXmlString (ogreConfigFile, attNode->getValue());
                                            tmpLog->format (LOG_ENDUSER, "Found the ogre config filename: %s", ogreConfigFile.c_str());
                                        }
                                        #ifdef WIN32
                                        if (attribute == "windowsPluginsDir")
                                        #else
                                        if (attribute == "linuxPluginsDir")
                                        #endif
                                        {
                                            assignXmlString (ogrePluginsDir, attNode->getValue());
                                            tmpLog->format (LOG_ENDUSER, "Found the ogre plugins directory: %s", ogrePluginsDir.c_str());
                                        }
                                        if (attribute == "sceneManager")
                                        {
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_ENDUSER, "Found the scene manager type: %s", attribute.c_str());

                                            if (attribute == "ST_GENERIC")
                                                sceneManager = Ogre::ST_GENERIC;
                                            if (attribute == "ST_EXTERIOR_CLOSE")
                                                sceneManager = Ogre::ST_EXTERIOR_CLOSE;
                                            if (attribute == "ST_EXTERIOR_FAR")
                                                sceneManager = Ogre::ST_EXTERIOR_FAR;
                                            if (attribute == "ST_EXTERIOR_REAL_FAR")
                                                sceneManager = Ogre::ST_EXTERIOR_REAL_FAR;
                                            if (attribute == "ST_INTERIOR")
                                                sceneManager = Ogre::ST_INTERIOR;
                                        }
                                        if (attribute == "anisotropy")
                                        {
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_ENDUSER, "Found the anisotropy level: %s", attribute.c_str());
                                            anisotropy = stoi (attribute);
                                        }
                                        if (attribute == "filtering")
                                        {
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_ENDUSER, "Found the texture filtering level: %s", attribute.c_str());
                                            if (attribute == "TFO_NONE")
                                                filtering = Ogre::TFO_NONE;
                                            if (attribute == "TFO_BILINEAR")
                                                filtering = Ogre::TFO_BILINEAR;
                                            if (attribute == "TFO_TRILINEAR")
                                                filtering = Ogre::TFO_TRILINEAR;
                                            if (attribute == "TFO_ANISOTROPIC")
                                                filtering = Ogre::TFO_ANISOTROPIC;
                                        }
                                        if (attribute == "width")
                                        {
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_ENDUSER, "Found the resolution width value: %s", attribute.c_str());
                                            width = stoi (attribute);
                                        }
                                        if (attribute == "height")
                                        {
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_ENDUSER, "Found the resolution height value: %s", attribute.c_str());
                                            height = stoi (attribute);
                                        }
                                        if (attribute == "bpp")
                                        {
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_ENDUSER, "Found the resolution bpp value: %s", attribute.c_str());
                                            bpp = stoi (attribute);
                                        }
                                        if (attribute == "renderer")
                                        {
                                            assignXmlString (renderer, attNode->getValue());
                                            tmpLog->format (LOG_ENDUSER, "Found the renderer type: %s", renderer.c_str());
                                        }
                                        if (attribute == "defaultNumMipmaps")
                                        {
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_ENDUSER, "Found the default number of mipmaps: %s", attribute.c_str());
                                            defaultNumMipMaps = stoi (attribute);
                                        }
                                        if (attribute == "fullScreen")
                                        {
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_ENDUSER, "Found the fullscreen option: %s", attribute.c_str());
                                            fullScreen = stob (attribute);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    delete tmpLog;

    log = new LogEngine (localLogLevel, localLogName.c_str());
    log->put (LOG_DEVELOPER, "Temporary parsing data already loaded into memory...");

    // get the direction of the graphics data
    log->put (LOG_DEVELOPER, "Setting up data pointers...");
    systemData = SystemData::getSystemDataPointer ();

    systemData->height = height;
    systemData->width = width;
    log->format (LOG_DEVELOPER, "Graphics data initialized to %ix%i@%ibpp", width, height, bpp);

    log->put (LOG_DEVELOPER, "Creating temporary ogre plugins config file (plugins.cfg)");
    FILE *ogrePluginsConfig = fopen("plugins.cfg", "w");
    if (ogrePluginsConfig == NULL)
    {
        log->put(LOG_ERROR, "Error opening ogre plugins config file!!");
    }
    log->put (LOG_DEVELOPER, "Writing configuration to plugins.cfg");
    fprintf(ogrePluginsConfig, "# IMPORTANT NOTE #"
        "\n#Everything you write in this file will be ignored and overwriten next time you run motorsport. oou can therefore safely delete this file.\n\n"
        "\n# Define plugins folder"
        "\nPluginFolder=%s"
        "\n# Define plugins"
        "\nPlugin=RenderSystem_GL"
#ifdef WIN32
//        "\nPlugin=RenderSystem_Direct3D9"
#endif
        "\nPlugin=Plugin_FileSystem"
//        "\nPlugin=Plugin_ParticleFX"
//        "\nPlugin=Plugin_BSPSceneManager"
//        "\nPlugin=Plugin_OctreeSceneManager"
        "\nPlugin=Plugin_GuiElements"
//        "\nPlugin=Plugin_NatureSceneManager"
//        "\nPlugin=Plugin_CgProgramManager"
        ,ogrePluginsDir.c_str());
    log->put (LOG_DEVELOPER, "Closing temporary ogre plugins config file (plugins.cfg)");
    fclose(ogrePluginsConfig);

    log->put (LOG_DEVELOPER, "Creating Ogre root element");
    ogreRoot = new Ogre::Root ("plugins.cfg", "removeme.cfg", "motorsport-ogre.log");

    std::string file = SystemData::getSystemDataPointer()->dataDir;
    file.append("/");
    file.append(ogreConfigFile);
    setupResources (file);
    // select renderer and set resolution and bpp
    manualInitialize (renderer);

    log->put (LOG_DEVELOPER, "Setting up fullscreen/windowed mode");
    ogreRoot->getRenderSystem ()->setConfigOption ("Full Screen", fullScreen ? "Yes" : "No");
    log->put (LOG_DEVELOPER, "Saving config file to removeme.cfg");
    ogreRoot->saveConfig();

    // Here we choose to let the system create a default rendering window
    log->put (LOG_DEVELOPER, "Initializing ogre root element");
    systemData->ogreWindow = ogreRoot->initialise (true);
#ifdef WIN32
    // This is a bit of a hack way to get the HWND from Ogre.
    // Currently only works for the OpenGL renderer.
    log->put (LOG_WARNING, "Windows version: temporary hackish workaround in order to get SDL input working");
    char tmp[64];
    Ogre::Win32Window* ow32_win = static_cast<Ogre::Win32Window*>(systemData->ogreWindow);
    if (ow32_win != NULL)
    {
        sprintf(tmp, "SDL_WINDOWID=%d", ow32_win->getWindowHandle());
        _putenv(tmp);
    }
#endif
    log->put (LOG_DEVELOPER, "Getting ogre scene manager");
    // Set shadowing system
    systemData->ogreSceneManager = ogreRoot->getSceneManager (sceneManager);
    //systemData->ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_NONE);    
    systemData->ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_MODULATIVE);
    //systemData->ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
    //systemData->ogreSceneManager->setShadowTechnique(Ogre::SHADOWTYPE_TEXTURE_MODULATIVE);
    systemData->ogreSceneManager->setAmbientLight(Ogre::ColourValue(5.0, 5.0, 5.0));

    // Set default mipmap level (NB some APIs ignore this)
    log->put (LOG_ENDUSER, "Setting up default number of mipmap levels");
    Ogre::TextureManager::getSingleton ().setDefaultNumMipMaps (defaultNumMipMaps);

    // Set some graphics settings
    log->put (LOG_ENDUSER, "Setting up anisotropy and filtering parameters");
    Ogre::MaterialManager::getSingleton ().setDefaultAnisotropy (anisotropy);
    Ogre::MaterialManager::getSingleton ().setDefaultTextureFiltering (filtering);

    Ogre::Light * light = SystemData::getSystemDataPointer ()->ogreSceneManager->createLight("Sun");
    //light->setType(Ogre::Light::LT_SPOTLIGHT);
    //light->setType(Ogre::Light::LT_POINT);
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(1,1,-1);
    //light->setPosition(50, 50, 1);
    light->setDiffuseColour(1, 1, 1);
    light->setSpecularColour(1, 1, 1);
    systemData->ogreSceneManager->getRootSceneNode()->attachObject (light);

    log->put (LOG_DEVELOPER, "Removing temporary ogre plugins config file (plugins.cfg)");
    remove("plugins.cfg");
    log->put (LOG_DEVELOPER, "Removing temporary ogre file");
    remove("removeme.cfg");
}

