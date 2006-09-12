/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#include "physics.hpp"
#include "logEngine.hpp"
#include "SDL.h"
#include "math.h"
#include "system.hpp"
#include "world.hpp"
#include "gui.hpp"
#include "part.hpp"
#include "vehicle.hpp"
#include "camera.hpp"
#include "xmlTag.hpp"
#include "area.hpp"
#include "system.hpp"
#include "Ogre.h"
#include "OgreNoMemoryMacros.h"
#include "ode/ode.h"

bool Physics::checkpointPassed = false;

Physics::Physics ()
  : stepType(0), dWorldStepFast1MaxIterations(0) 
{
#ifdef MACOSX
    XmlTag * tag = new XmlTag ("motorsport.app/Contents/Resources/physicsConfig.xml");
#else
    XmlTag * tag = new XmlTag ("../cfg/physicsConfig.xml");
#endif
    double frequency = 250.0;
    int timeScale = 1;
    int pauseStep = 0;
    System::get()->setCfmValue (-1);
    System::get()->setErpValue (-1);
    int stepType = 1;
    int dWorldStepFast1MaxIterations = 100;

    log = LogEngine::create (LOG_DEVELOPER, "Physics");
    if (tag->getName() == "physicsConfig")
    {
        frequency = stod(tag->getAttribute("frequency"));
        timeScale = stoi(tag->getAttribute("timeScale"));
        pauseStep = stoi(tag->getAttribute("pauseStep"));
        XmlTag * t = tag->getTag(0); for (int i = 0; i < tag->nTags(); t = tag->getTag(++i))
        {
            if (t->getName() == "ode")
            {
                if (t->getAttribute("cfmValue") != "default") System::get()->setCfmValue(stod(t->getAttribute("cfmValue")));
                if (t->getAttribute("erpValue") != "default") System::get()->setErpValue(stod(t->getAttribute("erpValue")));
                if (t->getAttribute("stepType") == "dWorldStep") stepType = 1;
                if (t->getAttribute("stepType") == "dWorldStepFast1") stepType = 2;
                dWorldStepFast1MaxIterations = stoi (t->getAttribute("dWorldStepFast1MaxIterations"));
            }
        }
    }
    // get the direction of the graphics data
    log->__format (LOG_DEVELOPER, "Setting up data pointers...");
    system = System::get();

    log->__format (LOG_DEVELOPER, "Setting physics data");
    system->setDesiredPhysicsFrequency(frequency);
    system->timeScale = timeScale;
    system->pauseStep = pauseStep;
    log->__format (LOG_ENDUSER, "Physics rate set @ %f Hz (%f ms)", system->getDesiredPhysicsFrequency(), system->getDesiredPhysicsTimestep() * 1000);
    delete tag;
}


void Physics::nearCallback (void *data, dGeomID o1, dGeomID o2)
{
    int i, n;

    // TODO: allow jointed bodies to collide by default (truck and trailer), and use attached data to identify bodies that should not collide (wheels and vehicle body). At least until creation of geoms via boolean operations is possible.
    dBodyID b1 = dGeomGetBody (o1);
    dBodyID b2 = dGeomGetBody (o2);
    if (b1 == 0 && b2 == 0) return;
    if (b1 && b2 && dAreConnected (b1, b2)) return;
    if (b1 && b2 && dBodyGetData (b1) == dBodyGetData (b2)) return;

    const int N = 10;
    dContact contact[N];
    n = dCollide (o1, o2, N, &contact[0].geom, sizeof (dContact));
    if (n > 0)
    {
        for (i = 0; i < n; i++)
        {
            contact[i].surface.mode = dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;
            if ( (dGeomGetClass (o1) == dSphereClass && dGeomGetClass (o2) == dCCylinderClass) ||
                 (dGeomGetClass (o2) == dSphereClass && dGeomGetClass (o1) == dCCylinderClass))
            {
                checkpointPassed = true;
                return;
            }
            // Collision with the checkpoint object. This assumes all spheres are a checkpoint, so FIXME.
            if (dGeomGetClass (o1) == dSphereClass || dGeomGetClass (o2) == dSphereClass)
            {
                return;
            }
            contact[i].surface.mu = 0.6;
            contact[i].surface.slip1 = 0.0;
            contact[i].surface.slip2 = 0.0;
            // FIXME!!! check what kind of surfaces are colliding, setting the appropriate cfm and erp values!
            contact[i].surface.soft_erp = 0.8;
            contact[i].surface.soft_cfm = 0.00000001;

            dJointID c = dJointCreateContact (World::get()->worldID, World::get()->jointGroupID, contact + i);
            dJointAttach (c, dGeomGetBody (o1), dGeomGetBody (o2));
        }
    }
}

int Physics::computeStep (void)
{
    World::get()->stepPhysics();

    checkpointPassed = false;
    dSpaceCollide (World::get()->spaceID, 0, &nearCallback);
    switch (stepType)
    {
    default:
    case 1:
        // traditional (x^y), theorycally slowest, and most accurate physics calculations:
        dWorldStep (World::get()->worldID, system->getDesiredPhysicsTimestep());
        break;
    case 2:
        // alternative (x*y), fastest and less accurate physics calculations:
        dWorldStepFast1 (World::get()->worldID, system->getDesiredPhysicsTimestep(), dWorldStepFast1MaxIterations);
        break;
    }
    dJointGroupEmpty (World::get()->jointGroupID);
    
    // check if a car has passed the checkpoint
    static bool checkpointWasPassed = false;
    static double time;
    time = SDL_GetTicks()/1000.0;
    static double lapTime = time;
    if (checkpointWasPassed && (!checkpointPassed))
    {
        Gui::get()->updateLapTime (time - lapTime);
        log->__format(LOG_ENDUSER, "Checkpoint passed! Last lap time is: %f seconds.", time-lapTime);
        lapTime = time;
    }
    checkpointWasPassed = checkpointPassed;
    Gui::get()->updateTime (time - lapTime);
    return (0);
}

Physics::~Physics (void)
{
    dCloseODE ();
}
