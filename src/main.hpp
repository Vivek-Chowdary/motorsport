/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef MAIN_HPP
#define MAIN_HPP

//forward declarations
class LogEngine;

/** 
 * @brief Stops all SDL subsystems.
 */
void stopSdl (void);
/** 
 * @brief Starts all SDL subsystems.
 * 
 * @param log A correctly initialized log engine.
 */
void startSdl (LogEngine * log);
/** 
 * @brief Process all input events related to game logic (such as petition to exit, or to modify physic engine rates, etc...
 * 
 * @param log A correctly initialized log engine.
 */
void computeLogic (LogEngine * log);
/** 
 * @brief If video recording is enabled, it checks whether the screen should be recorded to hard disk and does it if necessary.
 */
void recordVideoFrames ();
#endif
