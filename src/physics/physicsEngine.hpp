
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
#include "Ogre.h"
#include "ode.h"
#include "math.h"
#include "system.hpp"
#include "world.hpp"
#include "cube.hpp"
#include "body.hpp"
#include "camera.hpp"
#include "domParser.hpp"

struct PhysicsData;

/// Manages everything related to the simulated world data.
/** Manages everything related to the simulated world data. This is not limited to real life physics simulation, but is also related to scripted movement, virtual world events, and many other concepts.
*/
class PhysicsEngine
{
  private:
    // / Log engine used by the physics engine.
    /** This log engine allows the physics engine to write data in the log file, allowing to be differenciated from other engines.
    */
    LogEngine * log;
    // / Pointer to the system data, used by different engines in order to store common data.
    SystemData *systemData;
    // / Pointer to the world data, used by different engines in order to store the simulated world data.
    WorldData *worldData;
    // / Callback function for collisions detection.
    /** Callback function used with ODE. It processes the physic entities in the simulated world, generating the necessary forces according to the detected collisions.
    */
    static void nearCallback (void *data, dGeomID o1, dGeomID o2);
    // / Type of ODE step function: 1->dWorldStep. 2->dWorldStepFast1. 
    int stepType;               // "dWorldStep"=1 || "dWorldStepFast1"=2
    // / Max. number of iterations to be calculated with dWorldStepFast1.
    int dWorldStepFast1MaxIterations;
  public:
    // / Creates a new physics engine.
    /** Creates a new physics engine, with its associated own log engine. It initializes all necessary related data, including the simulated world's physic entities.
    */
      PhysicsEngine ();
    // / Deletes the physics engine.
    /** Deletes the physics engine, as well as its associated log engine. It also removes all the simulated world's physic entities from memory.
    */
     ~PhysicsEngine (void);
    // / Updates all the simulated world state.
    /** Updates all the simulated world state, including physic movements (collision detection and correction, physic forces...), world events, and other issues.
    */
    int computeStep (void);

    // / Called by the generic XML parser; it loads configuration data from a file.
    static void processPhysicsConfigFile (DOMNode * n, void *data);
};

struct PhysicsData
{
    PhysicsEngine *physics;
    LOG_LEVEL localLogLevel;
    char *localLogName;
    int frequency;
    double cfmValue;            // a double number, or "default"
    double erpValue;            // a double number, or "default"
    int stepType;               // "dWorldStep"=1 || "dWorldStepFast1"=2 || "default"=...
    int dWorldStepFast1MaxIterations;
};
