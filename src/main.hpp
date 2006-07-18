/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef MAIN_HPP
#define MAIN_HPP
#include <shared.hpp>

//forward declarations
SHARED_PTR_MAP(LogEngine, pLogEngine, LogEngines, LogEnginesIt, wLogEngine);
SHARED_PTR(Graphics,pGraphics, wGraphics);

/** 
 * @brief Stops all SDL subsystems.
 */
void stopSdl (void);
/** 
 * @brief Starts all SDL subsystems.
 * 
 * @param log A correctly initialized log engine.
 */
void startSdl (pLogEngine log);
/** 
 * @brief Process all input events related to game logic (such as petition to exit, or to modify physic engine rates, etc...
 * 
 * @param log A correctly initialized log engine.
 */
void computeLogic (pLogEngine log);
/** 
 * @brief If video recording is enabled, it checks whether the screen should be recorded to hard disk and does it if necessary.
 */
void recordVideoFrames ();
#endif
