
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
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "OgreWin32Window.h"
#endif
#include "graphicsEngine.hpp"

GraphicsEngine::GraphicsEngine ()
{
    XmlFile* xmlFile = new XmlFile ("graphicsConfig.xml");
    processXmlRootNode (xmlFile->getRootNode());
    delete xmlFile;
}

bool GraphicsEngine::manualInitialize (GraphicsData * data)
{
    Ogre::RenderSystem * renderSystem;
    bool ok = false;

    Ogre::RenderSystemList * renderers = Ogre::Root::getSingleton ().getAvailableRenderers ();
    // See if the list is empty (no renderers available)
    if (renderers->empty ())
        return false;
    for (Ogre::RenderSystemList::iterator it = renderers->begin (); it != renderers->end (); it++)
    {
        renderSystem = (*it);
        log->put (LOG_INFO, "Loading ogre renderer name into memory...");
        if (strstr (&(*renderSystem->getName ()), data->renderer))
        {
            ok = true;
            break;
        }
    }
    if (!ok)
    {
        // We still don't have a renderer; pick up the first one from the list
        renderSystem = (*renderers->begin ());
    }

    Ogre::Root::getSingleton ().setRenderSystem (renderSystem);
    char resolution[32];

    sprintf (resolution, "%i x %i", width, height);

    // Manually set configuration options. These are optional.
    renderSystem->setConfigOption ("Video Mode", resolution);

    return true;
}

void GraphicsEngine::setupResources (GraphicsData * data)
{
    // Load resource paths from config file
    Ogre::ConfigFile cf;
    log->put (LOG_INFO, "Loading ogre config filename into memory...");
    cf.load (data->ogreConfigFile);

    // Go through all settings in the file
    Ogre::ConfigFile::SettingsIterator i = cf.getSettingsIterator ();
    Ogre::String typeName, archName;
    while (i.hasMoreElements ())
    {
        typeName = i.peekNextKey ();
        archName = i.getNext ();
        Ogre::ResourceManager::addCommonArchiveEx (archName, typeName);
    }
}

int GraphicsEngine::computeStep (void)
{
    // take a screenshot if needed
    if (systemData->getTakeScreenshot ())
    {
        log->format (LOG_INFO, "Taking a screenshot in %s.", screenshotFilename);
        systemData->ogreWindow->writeContentsToFile (screenshotFilename);
    }
    // Update Ogre's bodies positions with Ode's positions.
    int numberOfBodies = worldData->bodyList.size ();
    for (int currentBody = 0; currentBody < numberOfBodies; currentBody++)
    {
        worldData->bodyList[currentBody]->stepGraphics ();
    }

    // Update Ogre's cubes positions with Ode's positions.
    int numberOfCubes = worldData->cubeList.size ();
    for (int currentCube = 0; currentCube < numberOfCubes; currentCube++)
    {
        worldData->cubeList[currentCube]->stepGraphics ();
    }

    // Let the listener frames be started and ended: they are needed for particle systems.
    ogreRoot->_fireFrameStarted ();
    systemData->ogreWindow->update ();
    ogreRoot->_fireFrameEnded ();

    return (0);
}

GraphicsEngine::~GraphicsEngine (void)
{
    log->put (LOG_INFO, "Unloading ogre window data from memory...");
    delete (systemData->ogreWindow);

    log->put (LOG_INFO, "Unloading screenshot filename from memory...");
    delete screenshotFilename;

    // finally stop the log engine
    delete log;
}


void GraphicsEngine::processXmlRootNode (DOMNode * n)
{
    GraphicsData *data = new GraphicsData;

    LogEngine * tmpLog = new LogEngine (LOG_TRACE, "XML");
    if (n)
    {
        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
        {
            char *name = XMLString::transcode (n->getNodeName ());
            tmpLog->format (LOG_INFO, "Name: %s", name);

            if (!strncmp (name, "graphicsConfig", 15))
            {
                tmpLog->put (LOG_INFO, "Found the graphics engine config element.");
                if (n->hasAttributes ())
                {
                    // get all the attributes of the node
                    DOMNamedNodeMap *pAttributes = n->getAttributes ();
                    int nSize = pAttributes->getLength ();
                    for (int i = 0; i < nSize; ++i)
                    {
                        DOMAttr *pAttributeNode = (DOMAttr *) pAttributes->item (i);
                        char *name = XMLString::transcode (pAttributeNode->getName ());
                        if (!strncmp (name, "localLogLevel", 14))
                        {
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            tmpLog->format (LOG_INFO, "Found the local log level: %s", name);
                            if (!strncmp (name, "LOG_ERROR", 10))
                                (*(GraphicsData *) data).localLogLevel = LOG_ERROR;
                            if (!strncmp (name, "LOG_WARNING", 13))
                                (*(GraphicsData *) data).localLogLevel = LOG_WARNING;
                            if (!strncmp (name, "LOG_INFO", 9))
                                (*(GraphicsData *) data).localLogLevel = LOG_INFO;
                            if (!strncmp (name, "LOG_VERBOSE", 12))
                                (*(GraphicsData *) data).localLogLevel = LOG_VERBOSE;
                            if (!strncmp (name, "LOG_TRACE", 9))
                                (*(GraphicsData *) data).localLogLevel = LOG_TRACE;
                        }

                        if (!strncmp (name, "localLogName", 13))
                        {
                            XMLString::release (&name);
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            tmpLog->format (LOG_INFO, "Found the log name: %s", name);

                            (*(GraphicsData *) data).localLogName = new char[strlen (name) + 1];
                            strncpy ((*(GraphicsData *) data).localLogName, name, strlen (name) + 1);
                        }
                        if (!strncmp (name, "screenshotFile", 15))
                        {
                            name = XMLString::transcode (pAttributeNode->getValue ());
                            tmpLog->format (LOG_INFO, "Found the screenshot filename: %s", name);

                            (*(GraphicsData *) data).screenshotFile = new char[strlen (name) + 1];
                            strncpy ((*(GraphicsData *) data).screenshotFile, name, strlen (name) + 1);
                        }
                        XMLString::release (&name);
                    }
                }
                for (n = n->getFirstChild (); n != 0; n = n->getNextSibling ())
                {
                    if (n)
                    {
                        if (n->getNodeType () == DOMNode::ELEMENT_NODE)
                        {
                            char *name = XMLString::transcode (n->getNodeName ());
                            tmpLog->format (LOG_INFO, "Name: %s", name);
                            if (!strncmp (name, "ogre", 5))
                            {
                                tmpLog->put (LOG_INFO, "Found the ogre config element.");
                                if (n->hasAttributes ())
                                {
                                    // get all the attributes of the node
                                    DOMNamedNodeMap *pAttributes = n->getAttributes ();
                                    int nSize = pAttributes->getLength ();

                                    for (int i = 0; i < nSize; ++i)
                                    {
                                        DOMAttr *pAttributeNode = (DOMAttr *) pAttributes->item (i);
                                        char *name = XMLString::transcode (pAttributeNode->getName ());
                                        if (!strncmp (name, "configFile", 11))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            tmpLog->format (LOG_INFO, "Found the ogre config filename: %s", name);

                                            (*(GraphicsData *) data).ogreConfigFile = new char[strlen (name) + 1];
                                            strncpy ((*(GraphicsData *) data).ogreConfigFile, name, strlen (name) + 1);
                                        }
                                        #ifdef WIN32
                                        if (!strncmp (name, "windowsPluginsDir", 18))
                                        #else
                                        if (!strncmp (name, "linuxPluginsDir", 16))
                                        #endif
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            tmpLog->format (LOG_INFO, "Found the ogre plugins directory: %s", name);

                                            (*(GraphicsData *) data).ogrePluginsDir = new char[strlen (name) + 1];
                                            strncpy ((*(GraphicsData *) data).ogrePluginsDir, name, strlen (name) + 1);
                                        }
                                        if (!strncmp (name, "sceneManager", 13))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            tmpLog->format (LOG_INFO, "Found the scene manager type: %s", name);

                                            if (!strncmp (name, "ST_GENERIC", 11))
                                                (*(GraphicsData *) data).sceneManager = Ogre::ST_GENERIC;
                                            if (!strncmp (name, "ST_EXTERIOR_CLOSE", 18))
                                                (*(GraphicsData *) data).sceneManager = Ogre::ST_EXTERIOR_CLOSE;
                                            if (!strncmp (name, "ST_EXTERIOR_FAR", 16))
                                                (*(GraphicsData *) data).sceneManager = Ogre::ST_EXTERIOR_FAR;
                                            if (!strncmp (name, "ST_EXTERIOR_REAL_FAR", 21))
                                                (*(GraphicsData *) data).sceneManager = Ogre::ST_EXTERIOR_REAL_FAR;
                                            if (!strncmp (name, "ST_INTERIOR", 12))
                                                (*(GraphicsData *) data).sceneManager = Ogre::ST_INTERIOR;
                                        }
                                        if (!strncmp (name, "anisotropy", 11))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            tmpLog->format (LOG_INFO, "Found the anisotropy level: %s", name);

                                            (*(GraphicsData *) data).anisotropy = atoi (name);
                                        }
                                        if (!strncmp (name, "filtering", 10))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            tmpLog->format (LOG_INFO, "Found the texture filtering level: %s", name);

                                            if (!strncmp (name, "TFO_NONE", 9))
                                                (*(GraphicsData *) data).filtering = Ogre::TFO_NONE;
                                            if (!strncmp (name, "TFO_BILINEAR", 13))
                                                (*(GraphicsData *) data).filtering = Ogre::TFO_BILINEAR;
                                            if (!strncmp (name, "TFO_TRILINEAR", 14))
                                                (*(GraphicsData *) data).filtering = Ogre::TFO_TRILINEAR;
                                            if (!strncmp (name, "TFO_ANISOTROPIC", 16))
                                                (*(GraphicsData *) data).filtering = Ogre::TFO_ANISOTROPIC;
                                        }
                                        if (!strncmp (name, "width", 6))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            tmpLog->format (LOG_INFO, "Found the resolution width value: %s", name);

                                            (*(GraphicsData *) data).width = atoi (name);
                                        }
                                        if (!strncmp (name, "height", 7))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            tmpLog->format (LOG_INFO, "Found the resolution height value: %s", name);

                                            (*(GraphicsData *) data).height = atoi (name);
                                        }
                                        if (!strncmp (name, "bpp", 4))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            tmpLog->format (LOG_INFO, "Found the resolution bpp value: %s", name);

                                            (*(GraphicsData *) data).bpp = atoi (name);
                                        }
                                        if (!strncmp (name, "renderer", 9))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            tmpLog->format (LOG_INFO, "Found the renderer type: %s", name);

                                            (*(GraphicsData *) data).renderer = new char[strlen (name) + 1];
                                            strncpy ((*(GraphicsData *) data).renderer, name, strlen (name) + 1);
                                        }
                                        if (!strncmp (name, "defaultNumMipmaps", 18))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            tmpLog->format (LOG_INFO, "Found the default number of mipmaps: %s", name);

                                            (*(GraphicsData *) data).defaultNumMipMaps = atoi (name);
                                        }
                                        if (!strncmp (name, "fullScreen", 11))
                                        {
                                            XMLString::release (&name);
                                            name = XMLString::transcode (pAttributeNode->getValue ());
                                            tmpLog->format (LOG_INFO, "Found the fullscreen option: %s", name);

                                            int tmpBool;
                                            tmpBool = atoi (name);
                                            (*(GraphicsData *) data).fullScreen = tmpBool ? true : false;
                                        }
                                        XMLString::release (&name);
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

    log = new LogEngine (data->localLogLevel, data->localLogName);
    log->put (LOG_INFO, "Temporary parsing data already loaded into memory...");

    // get the direction of the graphics data
    log->put (LOG_INFO, "Setting up data pointers...");
    worldData = WorldData::getWorldDataPointer ();
    systemData = SystemData::getSystemDataPointer ();

    log->put (LOG_INFO, "Loading screenshot filename into memory...");
    screenshotFilename = new char[strlen (data->screenshotFile) + 1];
    strncpy (screenshotFilename, data->screenshotFile, strlen (data->screenshotFile) + 1);

    log->put (LOG_INFO, "Setting screen properties...");
    width = data->width;
    height = data->height;
    bpp = data->bpp;
    fullScreen = data->fullScreen;
    log->format (LOG_INFO, "Graphics data initialized for %ix%i@%ibpp", width, height, bpp);

    log->put (LOG_INFO, "Creating temporary ogre plugins config file (plugins.cfg)");
    FILE *ogrePluginsConfig = fopen("plugins.cfg", "w");
    log->put (LOG_INFO, "Writing configuration to plugins.cfg");
    fprintf(ogrePluginsConfig, "# IMPORTANT NOTE #"
        "\n#Everything you write in this file will be ignored and overwriten next time you run motorsport. You can therefore safely delete this file.\n\n"
        "\n# Define plugins folder"
        "\nPluginFolder=%s"
        "\n# Define plugins"
        "\nPlugin=RenderSystem_GL"
#ifdef WIN32
//        "\nPlugin=RenderSystem_Direct3D9"
#endif
        "\nPlugin=Plugin_FileSystem"
        "\nPlugin=Plugin_ParticleFX"
        "\nPlugin=Plugin_BSPSceneManager"
        "\nPlugin=Plugin_OctreeSceneManager"
        "\nPlugin=Plugin_GuiElements"
        "\nPlugin=Plugin_NatureSceneManager"
        "\nPlugin=Plugin_CgProgramManager",
        data->ogrePluginsDir);
    log->put (LOG_INFO, "Closing temporary ogre plugins config file (plugins.cfg)");
    fclose(ogrePluginsConfig);

    log->put (LOG_INFO, "Creating Ogre root element");
    ogreRoot = new Ogre::Root ("plugins.cfg", "nothingToSeeHere.cfg", "motorsport-ogre.log");

    setupResources (data);
    // Initialise the system
    if (!manualInitialize (data))
    {
//        return false;
    }
    // Here we choose to let the system create a default rendering window
    // by passing 'true'
    ogreRoot->getRenderSystem ()->setConfigOption ("Full Screen", fullScreen ? "Yes" : "No");
#ifdef WIN32
    // Let the user changes some parameters.
	ogreRoot->showConfigDialog();
#endif
    systemData->ogreWindow = ogreRoot->initialise (true);

#ifdef WIN32
    //
    // This is a bit of a hack way to get the HWND from Ogre.
    // Currently only works for the OpenGL renderer.
    //
	char tmp[64];
    Ogre::Win32Window* ow32_win = static_cast<Ogre::Win32Window*>(systemData->ogreWindow);
    if (ow32_win != NULL){
	    sprintf(tmp, "SDL_WINDOWID=%d", ow32_win->getWindowHandle());
	    _putenv(tmp);
    }
#endif

    systemData->ogreSceneManager = ogreRoot->getSceneManager (data->sceneManager);

    // Set default mipmap level (NB some APIs ignore this)
    Ogre::TextureManager::getSingleton ().setDefaultNumMipMaps (data->defaultNumMipMaps);

    // Set some graphics settings
    Ogre::MaterialManager::getSingleton ().setDefaultAnisotropy (data->anisotropy);
    Ogre::MaterialManager::getSingleton ().setDefaultTextureFiltering (data->filtering);

    log->put (LOG_INFO, "Removing temporary ogre plugins config file (plugins.cfg)");
    remove("plugins.cfg");
    log->put (LOG_INFO, "Removing temporary ogre file");
    remove("nothingToSeeHere.cfg");
    
    log->put (LOG_INFO, "Unloading temporary parsing data from memory...");
    delete[](data->localLogName);
    delete[](data->screenshotFile);
    delete[](data->ogreConfigFile);
    delete[](data->ogrePluginsDir);
    delete[](data->renderer);
    delete data;
}
