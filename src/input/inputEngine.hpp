
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
#include "system.hpp"
#include "world.hpp"
#include "cube.hpp"
#include "camera.hpp"

/// Manages everything related to user input.
/** Manages everything related to user input, such as keyboard keypresses, mouse movement, steering wheel movement, joystick, etc... Essentially, it's the interface between the end-user and the simulated world, allowing the user to transparently make changes in this simulated world. Input devices calibration issues and many other things are handled by this engine.
*/
class InputEngine
{
  private:
    // / Log engine used by the input engine.
    /** This log engine allows the input engine to write data in the log file, allowing to be differenciated from other engines.
    */
    LogEngine * log;
    // / Pointer to the system data, used by different engines in order to store common data.
    SystemData *systemData;

    // / Keyboard keys state array.
    /** This is a pointer to SDL's internal keyboard keys state array. It allows to check whether a key is pressed or not, instead of having to go through the whole events queue, storing data by ourselves.
    */
    Uint8 *keyState;

    // / Stores the latest horizontal mouse movement difference, in pixels.
    /** Stores the latest horizontal mouse movement difference, in pixels. A positive value equals a movement to the right.
    */
    int mouseMovementX;
    // / Stores the latest horizontal mouse movement difference, in pixels.
    /** Stores the latest horizontal mouse movement difference, in pixels. A positive value equals a movement to the bottom.
    */
    int mouseMovementY;

    // / Finds out keyboard keys state, and modifies the world data according to it.
    void processKeyboard ();
    // / Finds out mouse movements, and modifies the world data according to them.
    void processMouseMovement ();
  public:
    // / Creates a new input engine.
    /** Creates a new input engine, with its associated own log engine. It initializes all necessary related data.
    */
      InputEngine ();
    // / Deletes the input engine.
    /** Deletes the input engine, as well as its associated log engine.
    */
     ~InputEngine (void);
    // / Processes all user input events since last call.
    /** Processes all user input events since last call, transforming them into modifications to the world data.
    */
    int computeStep (void);
    // / Called by the generic XML parser; it loads configuration data from a file.
    void processXmlRootNode (DOMNode * n);

};
