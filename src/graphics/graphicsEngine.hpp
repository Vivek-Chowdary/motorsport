
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

#include "logEngine.hpp"
#include "SDL.h"
#include "Ogre.h"
#include "OgreConfigFile.h"
#include "OgreNoMemoryMacros.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "system.hpp"
#include "world.hpp"
#include "cube.hpp"
#include "body.hpp"
#include "camera.hpp"
#include "xmlParser.hpp"
#include "dataEngine.hpp"

struct GraphicsData;

/// Manages everything related to the rendering process.
/** Manages everything related to the process of world data graphics rendering on screen. The menues and similar gadgets are not rendered by this engine; only world data is handled here (cars, track, etc...).
*/
class GraphicsEngine
{
  private:
    // / Log engine used by the graphics engine.
    /** This log engine allows the graphics engine to write data in the log file, allowing to be differenciated from other engines.
    */
    LogEngine * log;
    // / Pointer to the world data, used by different engines in order to store the simulated world data.  
    WorldData *worldData;
    // / Pointer to the system data, used by different engines in order to store common data.
    SystemData *systemData;

    // / Name of the file were screenshots will be saved.
    char *screenshotFilename;
  public:
    int width;
    int height;
    int bpp;
    bool fullScreen;

    Ogre::Root * ogreRoot;

    // / Creates a new graphics engine.
    /** Creates a new graphics engine, with its associated own log engine. It initializes all necessary related data, such as initial graphics representation of world data (meshes, textures...), and sets up the underlying rendering libray (Ogre).
    */
    GraphicsEngine ();
    // / Deletes the graphics engine.
    /** Deletes de graphics engine, as well as its associated log engine. It also removes all the related data from memory, and shuts down Ogre if necessary.
    */
    ~GraphicsEngine (void);
    // / Renders all the world data on screen.
    /** Renders on screen all the world data for this frame.
    */
    int computeStep (void);

    // / Selects a renderer and switches to the chosen resolution.
    /** Selects a renderer and switches to the chosen resolution. If the desired render (OpenGL) is not found, the first available one is then chosen.
    */
    bool manualInitialize (GraphicsData * data);
    // / Tells Ogre where to find resources.
    /** Tells Ogre where to find resources via the resources.cfg file. This allows Ogre to know where all the data (meshes, materials, textures...) can be found, not needing to explicitly indicate it.
    */
    void setupResources (GraphicsData * data);

    // / Called by the generic XML parser; it loads configuration data from a file.
    void processXmlRootNode (DOMNode * n);
};

struct GraphicsData
{
    GraphicsEngine *graphics;
    LOG_LEVEL localLogLevel;
    char *localLogName;
    char *screenshotFile;
    char *ogreConfigFile;
    char *ogrePluginsDir;
    /* ST_GENERIC ST_EXTERIOR_CLOSE ST_EXTERIOR_FAR ST_EXTERIOR_REAL_FAR ST_INTERIOR */
      Ogre::SceneType sceneManager;
    int anisotropy;
    /* TFO_NONE TFO_BILINEAR TFO_TRILINEAR TFO_ANISOTROPIC */
      Ogre::TextureFilterOptions filtering;
    int width;
    int height;
    int bpp;
    char *renderer;
    int defaultNumMipMaps;
    bool fullScreen;            // 0, 1
};
