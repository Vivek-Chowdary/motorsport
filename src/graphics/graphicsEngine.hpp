/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include <string>
#include "data/xercesc_fwd.hpp"

//forward declarations
class SystemData;
class LogEngine;
namespace Ogre {
  class Root;
}

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
    // / Pointer to the system data, used by different engines in order to store common data.
    SystemData *systemData;

    // / Name of the file were screenshots will be saved.
    std::string screenshotFilename;
    int initialFrame;
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
    void manualInitialize (const std::string & renderer);
    // / Tells Ogre where to find resources.
    /** Tells Ogre where to find resources via the resources.cfg file. This allows Ogre to know where all the data (meshes, materials, textures...) can be found, not needing to explicitly indicate it.
    */
    void setupResources ();
    // / Called by the generic XML parser; it loads configuration data from a file.
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
};
