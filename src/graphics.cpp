/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifdef WIN32
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif
#include <OgreNoMemoryMacros.h>
#include "graphics.hpp"
#include "system.hpp"
#include <Ogre.h>
#include <OgreConfigFile.h>
#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include <SDL/SDL_keysym.h>
#include "world.hpp"
#include "area.hpp"
#include "camera.hpp"
#include "part.hpp"
#include "vehicle.hpp"
#include "logEngine.hpp"
#include "paths.hpp"

void Graphics::manualInitialize (const std::string & renderer)
{
    Ogre::RenderSystem * renderSystem;
    bool ok = false;

    log->__format (LOG_DEVELOPER, "Loading Ogre renderers list into memory.");
    Ogre::RenderSystemList * renderers = Ogre::Root::getSingleton ().getAvailableRenderers ();
    if (renderers->empty ())
    {
        log->__format (LOG_ERROR, "No Ogre renderers available!");
    }
    for (Ogre::RenderSystemList::iterator it = renderers->begin (); it != renderers->end (); ++it)
    {
        renderSystem = (*it);
        log->__format (LOG_DEVELOPER, "Locating desired renderer (%s).", renderer.c_str());
        if (renderSystem->getName() == renderer)
        {
            log->__format (LOG_DEVELOPER, "Ok, desired renderer found.");
            ok = true;
            break;
        }
    }
    if (!ok)
    {
        log->__format (LOG_WARNING, "Desired renderer not found. Using first available one.");
        renderSystem = (*renderers->begin ());
    }
    Ogre::Root::getSingleton ().setRenderSystem (renderSystem);

    char resolution[32];
    snprintf (resolution, 32, "%i x %i", width, height);
    log->__format (LOG_ENDUSER, "Setting screen resolution and bpp: %s", resolution);
    renderSystem->setConfigOption ("Video Mode", resolution);
}

void Graphics::setupResources ()
{
    //load some default constant resources
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation("./", "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(Paths::data(), "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(Paths::gui(), "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(Paths::vehicles(), "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(Paths::areas(), "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(Paths::parts(), "FileSystem", "General");
    Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("General");
}

int Graphics::computeStep (void)
{
    Vector3d rotAngles (0, 0, 0);
    // Sun controller. FIXME the sun should be a class, physically behaved. */
    if (System::get()->axisMap[getIDKeyboardKey(SDLK_o)]->getValue() == 1)
    {
        // Increase degree by degree, in Y axis.
        rotAngles.y++;
    }
    if (System::get()->axisMap[getIDKeyboardKey(SDLK_p)]->getValue() == 1)
    {
        // Increase degree by degree, in Y axis.
        rotAngles.y--;
    }
    if (rotAngles.y != 0)
    {
        Quaternion rotation (rotAngles.x, rotAngles.y, rotAngles.z);

        // Calculate and set the new direction of the sun light rays.
        Ogre::Light * light = System::get()->ogreSceneManager->getLight("Sun");
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
            System::get()->ogreSceneManager->setAmbientLight(Ogre::ColourValue(r.x, r.y, r.z));

            r = minl - ( ( ( z        - min ) / ( med - min ) ) * (minl - medl) );
            light->setDiffuseColour (r.x, r.y, r.z);
            light->setSpecularColour(r.x, r.y, r.z);
        }
        // Check if the sun is high (midday)
        if (zm>=zd && zm<zx)
        {
            // Linearly interpolate light values.
            r = meda - ( ( ( z        - med ) / ( max - med ) ) * (meda - maxa) );
            System::get()->ogreSceneManager->setAmbientLight(Ogre::ColourValue(r.x, r.y, r.z));

            r = medl - ( ( ( z        - med ) / ( max - med ) ) * (medl - maxl) );
            light->setDiffuseColour (r.x, r.y, r.z);
            light->setSpecularColour(r.x, r.y, r.z);
        }
    }

    // take a screenshot if needed
    if (System::get()->axisMap[getIDKeyboardKey(SDLK_PRINT)]->getValue() == 1)
    {
        static unsigned int count = initialFrame;
        static char tmpName[64];
        snprintf (tmpName, 64, screenshotFilename.c_str(), count++);
        log->__format (LOG_ENDUSER, "Taking a screenshot in %s.", tmpName);
        System::get()->ogreWindow->writeContentsToFile (tmpName);
    }

    // change camera if needed
    if (System::get()->axisMap[getIDKeyboardKey(SDLK_c)]->getValue() == 1)
    {
        World::get()->switchNextAreaCamera();
    }
    if (System::get()->axisMap[getIDKeyboardKey(SDLK_v)]->getValue() == 1)
    {
        World::get()->switchNextVehicleCamera();
    }
    if (System::get()->axisMap[getIDKeyboardKey(SDLK_b)]->getValue() == 1)
    {
        World::get()->switchCameraDirector();
        System::get()->axisMap[getIDKeyboardKey(SDLK_b)]->setNewRawValue(0);
    }

    World::get()->stepGraphics();

    // Update area shadows state and area render mode
    World::get()->setCastShadows(castAreaShadows);
    World::get()->setRenderDetail(areaRenderMode);

    // Let the listener frames be started and ended: they are needed for particle System::get()s.
    ogreRoot->_fireFrameStarted ();
    System::get()->ogreWindow->update ();
    ogreRoot->_fireFrameEnded ();

    return (0);
}

Graphics::~Graphics (void)
{
    log->__format (LOG_DEVELOPER, "Unloading ogre window data from memory...");
    delete (System::get()->ogreWindow);
#ifdef LINUX
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
#endif
}

Graphics::Graphics ()
{
#ifdef MACOSX
    XmlTag * tag = new XmlTag ("motorsport.app/Contents/Resources/graphicsConfig.xml");
#else
    XmlTag * tag = new XmlTag ("../cfg/graphicsConfig.xml");
#endif
    LOG_LEVEL localLogLevel = LOG_DEVELOPER;
    std::string localLogName = "GFX";
    screenshotFilename.assign ("frame%i.jpg");
    initialFrame = 0;
    castAreaShadows = true;
    vehicleRenderMode = Ogre::PM_SOLID;
    areaRenderMode = Ogre::PM_SOLID;
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

    log = LogEngine::create (LOG_DEVELOPER, "Graphics");

    if (tag->getName() == "graphicsConfig")
    {
        screenshotFilename = tag->getAttribute("screenshotFile");
        initialFrame = stoi(tag->getAttribute("initialScreenshotFileNumber"));
        castAreaShadows = stob(tag->getAttribute("castAreaShadows"));
        if ( tag->getAttribute("vehicleRenderMode") == "points") vehicleRenderMode = 1;
        if ( tag->getAttribute("vehicleRenderMode") == "wireframe") vehicleRenderMode = 2;
        if ( tag->getAttribute("vehicleRenderMode") == "solid") vehicleRenderMode = 3;
        if ( tag->getAttribute("areaRenderMode") == "points") areaRenderMode = 1;
        if ( tag->getAttribute("areaRenderMode") == "wireframe") areaRenderMode = 2;
        if ( tag->getAttribute("areaRenderMode") == "solid") areaRenderMode = 3;
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        {
            if (t->getName() == "ogre")
            {
                #ifdef WIN32
                ogrePluginsDir = t->getAttribute("windowsPluginsDir");
                #else
                ogrePluginsDir = t->getAttribute("linuxPluginsDir");
                #endif
                anisotropy = stoi(t->getAttribute("anisotropy"));
                width = stoi(t->getAttribute("width"));
                height = stoi(t->getAttribute("height"));
                bpp = stoi(t->getAttribute("bpp"));
                renderer = t->getAttribute("renderer");
                defaultNumMipMaps = stoi(t->getAttribute("defaultNumMipmaps"));
                fullScreen = stob(t->getAttribute("fullScreen"));
                if (t->getAttribute("sceneManager") == "ST_GENERIC") sceneManager = Ogre::ST_GENERIC;
                if (t->getAttribute("sceneManager") == "ST_EXTERIOR_CLOSE") sceneManager = Ogre::ST_EXTERIOR_CLOSE;
                if (t->getAttribute("sceneManager") == "ST_EXTERIOR_FAR") sceneManager = Ogre::ST_EXTERIOR_FAR;
                if (t->getAttribute("sceneManager") == "ST_EXTERIOR_REAL_FAR") sceneManager = Ogre::ST_EXTERIOR_REAL_FAR;
                if (t->getAttribute("sceneManager") == "ST_INTERIOR") sceneManager = Ogre::ST_INTERIOR;
                if (t->getAttribute("filtering") == "TFO_NONE") filtering = Ogre::TFO_NONE;
                if (t->getAttribute("filtering") == "TFO_BILINEAR") filtering = Ogre::TFO_BILINEAR;
                if (t->getAttribute("filtering") == "TFO_TRILINEAR") filtering = Ogre::TFO_TRILINEAR;
                if (t->getAttribute("filtering") == "TFO_ANISOTROPIC") filtering = Ogre::TFO_ANISOTROPIC;
                if (t->getAttribute("shadowTechnique") == "SHADOWTYPE_NONE") shadowTechnique = Ogre::SHADOWTYPE_NONE;
                if (t->getAttribute("shadowTechnique") == "SHADOWTYPE_STENCIL_MODULATIVE") shadowTechnique = Ogre::SHADOWTYPE_STENCIL_MODULATIVE;
                if (t->getAttribute("shadowTechnique") == "SHADOWTYPE_STENCIL_ADDITIVE") shadowTechnique = Ogre::SHADOWTYPE_STENCIL_ADDITIVE;
                if (t->getAttribute("shadowTechnique") == "SHADOWTYPE_TEXTURE_MODULATIVE") shadowTechnique = Ogre::SHADOWTYPE_TEXTURE_MODULATIVE;
            }
        }
    }
    delete tag;

    
    log = LogEngine::create (localLogLevel, localLogName.c_str());
    log->__format (LOG_DEVELOPER, "Temporary parsing data already loaded into memory...");

    // get the direction of the graphics data
    log->__format (LOG_DEVELOPER, "Setting up data pointers...");
    System::get() = System::get();

    System::get()->height = height;
    System::get()->width = width;
    log->__format (LOG_DEVELOPER, "Graphics data initialized to %ix%i@%ibpp", width, height, bpp);

    log->__format (LOG_DEVELOPER, "Creating temporary ogre plugins config file (plugins.cfg)");
    FILE *ogrePluginsConfig = fopen("plugins.cfg", "w");
    if (ogrePluginsConfig == NULL)
    {
        log->__format(LOG_ERROR, "Error opening ogre plugins config file!!");
    }
    log->__format (LOG_DEVELOPER, "Writing configuration to plugins.cfg");
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
    log->__format (LOG_DEVELOPER, "Closing temporary ogre plugins config file (plugins.cfg)");
    fclose(ogrePluginsConfig);

    log->__format (LOG_DEVELOPER, "Creating Ogre root element");
    ogreRoot = new Ogre::Root ("plugins.cfg", "removeme.cfg", "motorsport-ogre.log");

    // select renderer and set resolution and bpp
    manualInitialize (renderer);

    log->__format (LOG_DEVELOPER, "Setting up fullscreen/windowed mode");
    ogreRoot->getRenderSystem ()->setConfigOption ("Full Screen", fullScreen ? "Yes" : "No");
    log->__format (LOG_DEVELOPER, "Saving config file to removeme.cfg");
    ogreRoot->saveConfig();
#ifdef LINUX
    // Make SDL input work with GLX platform
    // Code borrowed from the Ogre wiki at http://www.ogre3d.org/wiki/index.php/Using_SDL_Input#Linux
    if(SDL_WasInit(SDL_INIT_VIDEO)==0) {
        ///SDL hasn't been initilized, we thus know that we're using the GLX platform, and need to initialize SDL ourselves
        
        /// initialise root, without creating a window
        ogreRoot->initialise(false);
        
        SDL_Init(SDL_INIT_VIDEO);
        ///set the window size
        SDL_SetVideoMode(width, height,0,0); // create an SDL window
        
        SDL_WM_SetCaption("Motorsport","motorsport");
        
        SDL_SysWMinfo info;
        SDL_VERSION(&info.version);
        
        SDL_GetWMInfo(&info);
        
        std::string dsp(&(DisplayString(info.info.x11.display)[1]));
        std::vector<Ogre::String> tokens = Ogre::StringUtil::split(dsp, ".");
        
        Ogre::NameValuePairList misc;
        std::string s = Ogre::StringConverter::toString((long)info.info.x11.display);
	//Ivan: the following doesn't work with OGRE 1.4.0:
        //s += ":" + tokens[1] +":";
        s += ":"; 
        s += Ogre::StringConverter::toString((long)info.info.x11.window);
        misc["parentWindowHandle"] = s;
        System::get()->ogreWindow = ogreRoot->createRenderWindow("ogre", width, height, fullScreen, &misc);
        
        ///we need to set the window to be active by ourselves, since GLX by default sets it to false, but then activates it upon recieving some X event (which it will never recieve since we'll use SDL).
        ///see OgreGLXWindow.cpp
        System::get()->ogreWindow->setActive(true);
        System::get()->ogreWindow->setAutoUpdated(true);
    } else {
#endif
    // Here we choose to let the System::get() create a default rendering window
    log->__format (LOG_DEVELOPER, "Initializing ogre root element");
    System::get()->ogreWindow = ogreRoot->initialise (true);
#ifdef LINUX
}
#endif

    setupResources ();
#ifdef WIN32

	char tmp[64];
	// New method: As proposed by Sinbad.
	//  This method always works.
	HWND hWnd;
	System::get()->ogreWindow->getCustomAttribute("HWND", &hWnd);
	snprintf(tmp, 64, "SDL_WINDOWID=%d", hWnd);
	_putenv(tmp);

#endif
    log->__format (LOG_DEVELOPER, "Getting ogre scene manager");
    // Set shadowing System::get()
    System::get()->ogreSceneManager = ogreRoot->createSceneManager (sceneManager);
    //Ivan: createSceneManager doesn't exist in 1.0.6, but this one exists:
    //System::get()->ogreSceneManager = ogreRoot->getSceneManager (sceneManager);
    System::get()->ogreSceneManager->setShadowTechnique(shadowTechnique);
    System::get()->ogreSceneManager->setAmbientLight(Ogre::ColourValue(0.67, 0.94, 1.00));
    System::get()->ogreSceneManager->setShadowColour(Ogre::ColourValue(0.5, 0.5, 0.5));
    // Set default mipmap level (NB some APIs ignore this)
    log->__format (LOG_ENDUSER, "Setting up default number of mipmap levels");
    Ogre::TextureManager::getSingleton ().setDefaultNumMipmaps (defaultNumMipMaps);

    // Set some graphics settings
    log->__format (LOG_ENDUSER, "Setting up anisotropy and filtering parameters");
    Ogre::MaterialManager::getSingleton ().setDefaultAnisotropy (anisotropy);
    Ogre::MaterialManager::getSingleton ().setDefaultTextureFiltering (filtering);

    Ogre::Light * light = System::get()->ogreSceneManager->createLight("Sun");
    //light->setType(Ogre::Light::LT_SPOTLIGHT);
    //light->setType(Ogre::Light::LT_POINT);
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    light->setDirection(0.16,0.2,-1);
    //light->setPosition(50, 50, 1);
    light->setDiffuseColour(1, 1, 1);
    light->setSpecularColour(1, 1, 1);
    System::get()->ogreSceneManager->getRootSceneNode()->attachObject (light);

    log->__format (LOG_DEVELOPER, "Removing temporary ogre plugins config file (plugins.cfg)");
    remove("plugins.cfg");
    log->__format (LOG_DEVELOPER, "Removing temporary ogre file");
    remove("removeme.cfg");
}
