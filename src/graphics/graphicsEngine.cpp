/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
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
#include "OgreNoMemoryMacros.h" // <- After
#include "graphicsEngine.hpp"
#include "track.hpp"
#include "camera.hpp"
#include "part.hpp"
#include "system.hpp"
#include "xmlParser.hpp"
#include "Ogre.h"
#include "OgreConfigFile.h"
//#include "OgreNoMemoryMacros.h" // <- Before
#include "log/logEngine.hpp"
#include "world.hpp"
#include "SDL/SDL_keysym.h"
#include "vehicle.hpp"

GraphicsEngine::GraphicsEngine ()
{
#ifdef MACOSX
    XmlFile *xmlFile = new XmlFile ("motorsport.app/Contents/Resources/graphicsConfig.xml");
#else
    XmlFile *xmlFile = new XmlFile ("graphicsConfig.xml");
#endif
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

void GraphicsEngine::setupResources ()
{
    //load some default constant resources
    std::string dataDir = SystemData::getSystemDataPointer()->dataDir;
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./", "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(dataDir, "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(dataDir + "/gui", "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(dataDir + "/vehicles", "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(dataDir + "/tracks", "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(dataDir + "/parts", "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
}

int GraphicsEngine::computeStep (void)
{
    Vector3d rotAngles (0, 0, 0);
    // Sun controller. FIXME the sun should be a class, physically behaved. */
    if (systemData->axisMap[getIDKeyboardKey(SDLK_o)]->getValue() == 1)
    {
        // Increase degree by degree, in Y axis.
        rotAngles.y++;
    }
    if (systemData->axisMap[getIDKeyboardKey(SDLK_p)]->getValue() == 1)
    {
        // Increase degree by degree, in Y axis.
        rotAngles.y--;
    }
    if (rotAngles.y != 0)
    {
        Quaternion rotation (rotAngles.x, rotAngles.y, rotAngles.z);

        // Calculate and set the new direction of the sun light rays.
        Ogre::Light * light = SystemData::getSystemDataPointer ()->ogreSceneManager->getLight("Sun");
        Vector3d direction (light->getDirection().x, light->getDirection().y, light->getDirection().z);
        direction = rotation.rotateObject(direction);
        light->setDirection(direction.x, direction.y, direction.z);
        
        // Predefined values for ambient light, depending on height of sun (max, med, min height)
        Vector3d maxa (0.87, 0.97, 1.00);
        Vector3d meda (0.53, 0.27, 0.61);
        Vector3d mina (0.20, 0.24, 0.38);
        
        // Predefined values for sun light, depending on height of sun (max, med, min height)
        Vector3d maxl (0.98, 1.00, 0.76);
        Vector3d medl (0.88, 0.58, 0.40);
        Vector3d minl (0.20, 0.24, 0.38);

        // Cache some values
        double zm = -direction.z;
        Vector3d z (zm, zm, zm);
        const double zx = 1, zd = 0.00001, zn = 0;
        const Vector3d max(zx,zx,zx), med(zd,zd,zd), min(zn,zn,zn);
        Vector3d r;

        // Check if the sun is low (sunset)
        if (zm>=zn && zm<zd)
        {
            // Linearly interpolate light values.
            r = mina - ( ( ( z        - min ) / ( med - min ) ) * (mina - meda) );
            systemData->ogreSceneManager->setAmbientLight(Ogre::ColourValue(r.x, r.y, r.z));

            r = minl - ( ( ( z        - min ) / ( med - min ) ) * (minl - medl) );
            light->setDiffuseColour (r.x, r.y, r.z);
            light->setSpecularColour(r.x, r.y, r.z);
        }
        // Check if the sun is high (midday)
        if (zm>=zd && zm<zx)
        {
            // Linearly interpolate light values.
            r = meda - ( ( ( z        - med ) / ( max - med ) ) * (meda - maxa) );
            systemData->ogreSceneManager->setAmbientLight(Ogre::ColourValue(r.x, r.y, r.z));

            r = medl - ( ( ( z        - med ) / ( max - med ) ) * (medl - maxl) );
            light->setDiffuseColour (r.x, r.y, r.z);
            light->setSpecularColour(r.x, r.y, r.z);
        }
    }

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
        World::getWorldPointer ()->vehicleList[currentVehicle]->setRenderDetail(vehicleRenderMode);
        World::getWorldPointer ()->vehicleList[currentVehicle]->stepGraphics ();
    }

    // Update Ogre's parts positions with Ode's positions.
    int numberOfCubes = World::getWorldPointer ()->trackList[0]->partList.size ();
    for (int currentCube = 0; currentCube < numberOfCubes; currentCube++)
    {
        World::getWorldPointer ()->trackList[0]->partList[currentCube]->stepGraphics ();
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

    // Update infinite plane position according to vehicle position
    Ogre::Vector3 trackPos (World::getWorldPointer()->trackList[0]->planeNode->getPosition());
    Vector3d vehiclePos (World::getWorldPointer()->vehicleList[0]->getPosition());
    Vector3d diff (trackPos.x - vehiclePos.x, trackPos.y - vehiclePos.y, trackPos.z - vehiclePos.z);
    const double tile = 1000.0 / 20.0;
    if (diff.x > tile || diff.x < -tile) trackPos.x -= int ((diff.x) / (tile)) * (tile);
    if (diff.y > tile || diff.y < -tile) trackPos.y -= int ((diff.y) / (tile)) * (tile);
    World::getWorldPointer()->trackList[0]->planeNode->setPosition(trackPos);
    
    // Update track shadows state
    World::getWorldPointer()->trackList[0]->setCastShadows(castTrackShadows);
    
    // Update track render mode
    World::getWorldPointer ()->trackList[0]->setRenderDetail(trackRenderMode);
    
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
    castTrackShadows = true;
    vehicleRenderMode = Ogre::SDL_SOLID;
    trackRenderMode = Ogre::SDL_SOLID;
    #ifdef WIN32
    std::string ogrePluginsDir = "plugins";
    #else
    std::string ogrePluginsDir = "/usr/lib/OGRE";
    #endif
    Ogre::SceneType sceneManager = Ogre::ST_GENERIC;
    int anisotropy = 1;
    Ogre::TextureFilterOptions filtering = Ogre::TFO_NONE;
    Ogre::ShadowTechnique shadowTechnique = Ogre::SHADOWTYPE_NONE;
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
                        if (attribute == "castTrackShadows")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            tmpLog->format (LOG_ENDUSER, "Found whether to cast track shadows or not: %s", attribute.c_str());
                            castTrackShadows = stob (attribute);
                        }
                        if (attribute == "vehicleRenderMode")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            tmpLog->format (LOG_CCREATOR, "Found the vehicles rendering mode: %s", attribute.c_str());
                            
                            if(attribute == "points")
                                vehicleRenderMode=1;
                            if(attribute == "wireframe")
                                vehicleRenderMode=2;
                            if(attribute == "solid")
                                vehicleRenderMode=3;
                        }
                        if (attribute == "trackRenderMode")
                        {
                            assignXmlString (attribute, attNode->getValue());
                            tmpLog->format (LOG_CCREATOR, "Found the tracks rendering mode: %s", attribute.c_str());
                            if(attribute == "points")
                                trackRenderMode=1;
                            if(attribute == "wireframe")
                                trackRenderMode=2;
                            if(attribute == "solid")
                                trackRenderMode=3;
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
                                        if (attribute == "shadowTechnique")
                                        {
                                            assignXmlString (attribute, attNode->getValue());
                                            tmpLog->format (LOG_ENDUSER, "Found the texture filtering level: %s", attribute.c_str());
                                            if (attribute == "SHADOWTYPE_NONE")
                                                shadowTechnique = Ogre::SHADOWTYPE_NONE;    
                                            if (attribute == "SHADOWTYPE_STENCIL_MODULATIVE") 
                                                shadowTechnique = Ogre::SHADOWTYPE_STENCIL_MODULATIVE;
                                            if (attribute == "SHADOWTYPE_STENCIL_ADDITIVE")
                                                shadowTechnique = Ogre::SHADOWTYPE_STENCIL_ADDITIVE;
                                            if (attribute == "SHADOWTYPE_TEXTURE_MODULATIVE")
                                                shadowTechnique = Ogre::SHADOWTYPE_TEXTURE_MODULATIVE;
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
        "\n#Everything you write in this file will be ignored and overwriten next time you run motorsport. You can therefore safely delete this file.\n\n"
        "\n# Define plugins folder"
#ifdef MACOSX
        "\n#PluginFolder=%s"
#else
        "\nPluginFolder=%s"
#endif
        "\n# Define plugins"
        "\nPlugin=RenderSystem_GL"
#ifdef WIN32
        "\n#Plugin=RenderSystem_Direct3D9"
#endif
        "\n#Plugin=Plugin_FileSystem"
        "\n#Plugin=Plugin_ParticleFX"
        "\n#Plugin=Plugin_BSPSceneManager"
        "\n#Plugin=Plugin_OctreeSceneManager"
        "\n#Plugin=Plugin_GuiElements"
        "\n#Plugin=Plugin_NatureSceneManager"
        "\n#Plugin=Plugin_CgProgramManager"
        ,ogrePluginsDir.c_str());
    log->put (LOG_DEVELOPER, "Closing temporary ogre plugins config file (plugins.cfg)");
    fclose(ogrePluginsConfig);

    log->put (LOG_DEVELOPER, "Creating Ogre root element");
    ogreRoot = new Ogre::Root ("plugins.cfg", "removeme.cfg", "motorsport-ogre.log");

    // select renderer and set resolution and bpp
    manualInitialize (renderer);

    log->put (LOG_DEVELOPER, "Setting up fullscreen/windowed mode");
    ogreRoot->getRenderSystem ()->setConfigOption ("Full Screen", fullScreen ? "Yes" : "No");
    log->put (LOG_DEVELOPER, "Saving config file to removeme.cfg");
    ogreRoot->saveConfig();

    // Here we choose to let the system create a default rendering window
    log->put (LOG_DEVELOPER, "Initializing ogre root element");
    systemData->ogreWindow = ogreRoot->initialise (true);  
    setupResources ();
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
    systemData->ogreSceneManager->setShadowTechnique(shadowTechnique);
    systemData->ogreSceneManager->setAmbientLight(Ogre::ColourValue(0.67, 0.94, 1.00));
    systemData->ogreSceneManager->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5));
    // Set default mipmap level (NB some APIs ignore this)
    log->put (LOG_ENDUSER, "Setting up default number of mipmap levels");
    Ogre::TextureManager::getSingleton ().setDefaultNumMipmaps (defaultNumMipMaps);

    // Set some graphics settings
    log->put (LOG_ENDUSER, "Setting up anisotropy and filtering parameters");
    Ogre::MaterialManager::getSingleton ().setDefaultAnisotropy (anisotropy);
    Ogre::MaterialManager::getSingleton ().setDefaultTextureFiltering (filtering);

    Ogre::Light * light = SystemData::getSystemDataPointer ()->ogreSceneManager->createLight("Sun");
    //light->setType(Ogre::Light::LT_SPOTLIGHT);
    //light->setType(Ogre::Light::LT_POINT);
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(0.16,0.2,-1);
    //light->setPosition(50, 50, 1);
    light->setDiffuseColour(1, 1, 1);
    light->setSpecularColour(1, 1, 1);
    systemData->ogreSceneManager->getRootSceneNode()->attachObject (light);

    log->put (LOG_DEVELOPER, "Removing temporary ogre plugins config file (plugins.cfg)");
    remove("plugins.cfg");
    log->put (LOG_DEVELOPER, "Removing temporary ogre file");
    remove("removeme.cfg");
}

