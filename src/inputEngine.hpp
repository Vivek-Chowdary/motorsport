/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "SDL/SDL_types.h"
#include "xercesc_fwd.hpp"

//forward declatation
class SystemData;
class LogEngine;

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
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void clearLogicEventAxis ();
    void clearGraphicsEventAxis ();
};