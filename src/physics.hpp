/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include <common/shared.hpp>
#include "ode/objects.h"

/// Manages everything related to the simulated world data.
/** Manages everything related to the simulated world data. This is not limited to real life physics simulation, but is also related to scripted movement, virtual world events, and many other concepts.
*/

// Forward declarations
SHARED_PTR_MAP(LogEngine, pLogEngine, LogEngines, LogEnginesIt);
SHARED_PTR(System,pSystem);
SHARED_PTR(Physics,pPhysics);

class Physics
{
  private:
    // / Log engine used by the physics engine.
    /** This log engine allows the physics engine to write data in the log file, allowing to be differenciated from other engines.
    */
    pLogEngine log;
    // / Pointer to the system data, used by different engines in order to store common data.
    pSystem system;
    // / Callback function for collisions detection.
    /** Callback function used with ODE. It processes the physic entities in the simulated world, generating the necessary forces according to the detected collisions.
    */
    static void nearCallback (void *data, dGeomID o1, dGeomID o2);
    static bool checkpointPassed;
    // / Type of ODE step function: 1->dWorldStep. 2->dWorldStepFast1. 
    int stepType;               // "dWorldStep"=1 || "dWorldStepFast1"=2
    // / Max. number of iterations to be calculated with dWorldStepFast1.
    int dWorldStepFast1MaxIterations;
  public:
    // / Creates a new physics engine.
    /** Creates a new physics engine, with its associated own log engine. It initializes all necessary related data, including the simulated world's physic entities.
    */
      Physics ();
    // / Deletes the physics engine.
    /** Deletes the physics engine, as well as its associated log engine. It also removes all the simulated world's physic entities from memory.
    */
     ~Physics (void);
    // / Updates all the simulated world state.
    /** Updates all the simulated world state, including physic movements (collision detection and correction, physic forces...), world events, and other issues.
    */
    int computeStep (void);
};
