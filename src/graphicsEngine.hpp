/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include <common/shared.hpp>

//forward declarations
class LogEngine;
namespace Ogre {
  class Root;
}

SHARED_PTR_MAP(LogEngine, pLogEngine, LogEngines, LogEnginesIt);
SHARED_PTR(System,pSystem);

/** Manages everything related to the process of world data graphics rendering on screen. The menues and similar gadgets are not rendered by this engine; only world data is handled here (cars, area, etc...).
 * @brief Manages everything related to the rendering process.
 */
class GraphicsEngine
{
  private:
    /** This log engine allows the graphics engine to write data in the log file, allowing to be differenciated from other engines.
     * @brief Log engine used by the graphics engine.
     */
    pLogEngine log;
    /** 
     * @brief Pointer to the system data, used by different engines in order to store common data.
     */
    pSystem system;

    /** 
     * @brief Name of the file were screenshots will be saved.
     */
    std::string screenshotFilename;
    /** 
     * @brief Specifies the number where the screenshot filename numbering starts.
     */
    int initialFrame;
  public:
    /** 
     * @brief Stores the horizontal resolution of the screen.
     */
    int width;
    /** 
     * @brief Stores the vertical resolution of the screen.
     */
    int height;
    /** 
     * @brief Stores the bits per pixel of depth.
     */
    int bpp;
    /** 
     * @brief Specifies whether to use fullScreen mode, or windowed mode.
     */
    bool fullScreen;
    /** 
     * @brief Specifies whether the area will cast shadows or not.
     */
    bool castAreaShadows;
    /** 
    * @brief Specifies how the vehicle will be rendered: 1=points, 2=wireframe, 3=solid.
     */
    int vehicleRenderMode;
    /** 
     * @brief Specifies how the area will be rendered: 1=points, 2=wireframe, 3=solid.
     */
    int areaRenderMode;
    /** 
     * @brief The OGRE root element.
     */
    Ogre::Root * ogreRoot;
    /** Creates a new graphics engine, with its associated own log engine. It initializes all necessary related data, such as initial graphics representation of world data (meshes, textures...), and sets up the underlying rendering libray (Ogre).
     * @brief Creates a new graphics engine.
     */
    GraphicsEngine ();
    /** Deletes de graphics engine, as well as its associated log engine. It also removes all the related data from memory, and shuts down Ogre if necessary.
     * @brief Deletes the graphics engine.
     */
    ~GraphicsEngine (void);
    /** Renders on screen all the world data for this frame.
     * @brief Renders all the world data on screen.
     * 
     * @return 0 on success.
     */
    int computeStep (void);
    /** Selects a renderer and switches to the chosen resolution. If the desired render (OpenGL) is not found, the first available one is then chosen.
     * @brief Selects a renderer and switches to the chosen resolution.
     * 
     * @param renderer What renderer to use. For example "OpenGL". Case sensitive.
     */
    void manualInitialize (const std::string & renderer);
    /** Tells Ogre where to find resources via the resources.cfg file. This allows Ogre to know where all the data (meshes, materials, textures...) can be found, not needing to explicitly indicate it.
     * @brief Tells Ogre where to find resources.
     */
    void setupResources ();
};
