
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
#include "logEngine.hpp"
#include "SDL.h"
#include "Ogre.h"
#include "domParser.hpp"

struct GuiData;

using namespace Ogre;

/// Manages everything related to the gui rendering process.
/** Manages everything related to the process of system data (graphical user interface) graphics rendering on screen (menues and similar gadgets)
*/
class GuiEngine
{
  private:
    // / Log engine used by the gui engine.
    /** This log engine allows the gui engine to write data in the log file, allowing to be differenciated from other engines.
    */
    LogEngine * log;
    // / Pointer to the world data, used by different engines in order to store the simulated world data.
    WorldData *worldData;
    // / Pointer to the system data, used by different engines in order to store common data.
    SystemData *systemData;
    // / Allows or disallows to show statistics on screen
    bool showStatistics;
  public:
    // / Creates a new gui engine.
    /** Creates a new gui engine, with its associated own log engine. It initializes all necessary related data, such as initial graphics representation of system data (backgrounds, fonts,...), and sets up the underlying rendering libray (Ogre).
    */
      GuiEngine ();
    // / Deletes the gui engine.
    /** Deletes de gui engine, as well as its associated log engine. It also removes all the related data from memory.
    */
     ~GuiEngine ();
    // / Updates some statistics.
    /** Updates some statistics (framerates, etc...) in order to show them on screen if needed..
    */
    void updateStatistics (void);
    // / Renders all the gui on screen.
    /** Renders on screen all the system data (gui) for this frame.
    */
    int computeStep (void);
    // / Called by the generic XML parser; it loads configuration data from a file.
    static void processGuiConfigFile (DOMNode * n, void *data);
};

struct GuiData
{
    GuiEngine *gui;
    LOG_LEVEL localLogLevel;
    char *localLogName;
    bool showStatistics;        // 1->yes.  0->no.
};
