
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
#include "physicsEngine.hpp"
#include "test_ode.cpp"
#include <math.h>

#ifdef dDOUBLE
#    pragma message ( "[BUILDMESG] ODE double precision library loaded")
#    pragma comment( lib, "ode_double.lib" )
#else
#    ifdef dSINGLE
#        pragma message ( "[BUILDMESG] ODE single precision library loaded")
#        pragma comment( lib, "ode_single.lib" )
#    else
#        pragma message ( "[BUILDMESG] No ODE-mode specified, you _will_ run into problems ")
#    endif
#endif

PhysicsEngine::PhysicsEngine ( )
{
    //first of all start the logger (automatically logs the start of itself)
    log.start ( LOG_INFO, "logPhysics.txt" );

    //get the direction of the graphics data
    log.put ( LOG_INFO, "Setting up data pointers..." );
    worldData = WorldData::getWorldDataPointer();
    systemData = SystemData::getSystemDataPointer();
    physicsData = &(systemData->physicsData );
    log.append ( LOG_INFO, "Ok" );

/*    //get the direction of the graphics data
    log.put(LOG_INFO, "Testing ODE library...");
    testOde();
    log.append(LOG_INFO, "Ok");
*/

    log.put ( LOG_INFO, "Creating ODE world");
    worldData->worldID = dWorldCreate();
    log.put ( LOG_INFO, "Setting ODE world gravity");
    dWorldSetGravity (worldData->worldID, 0,0,-0.0001);
    log.put ( LOG_INFO, "Creating sample mass");
    dMass mass;
    log.put ( LOG_INFO, "Assigning box mass values");
    dMassSetBoxTotal (&mass, 1, 1, 1, 1);
    log.put ( LOG_INFO, "Creating cubes in ODE world");
    
    for ( int currentCube = 0;
          currentCube < worldData->numberOfCubes; currentCube++ )
    {
    
        worldData->cubeList[currentCube].cubeID = dBodyCreate (worldData->worldID);
        dBodySetPosition (worldData->cubeList[currentCube].cubeID, currentCube % 10 * 300, currentCube / 10 % 10 * 300, currentCube / 100 % 10 * 300 * ((int(currentCube/1000))+1));

        dBodySetMass (worldData->cubeList[currentCube].cubeID, &mass);
        dBodyAddForce (worldData->cubeList[0/*currentCube*/].cubeID,0.01,0,0);
    }
}

int PhysicsEngine::step ( void )
//makes the graphics engine draw one frame
{
    //mega-verbosity
    log.put ( LOG_TRACE, "Doing an step: calculating a physics step" );

    dWorldStep (worldData->worldID, systemData->physicsData.timeStep);
    
    float x = 0,
        z = 0;

    //translation of the camera1, advancing or strafing
    x += ( worldData->camera1->goRight ) ? physicsData->timeStep : 0;
    x -= ( worldData->camera1->goLeft ) ? physicsData->timeStep : 0;
    z -= ( worldData->camera1->goForward ) ? physicsData->timeStep : 0;
    z += ( worldData->camera1->goBack ) ? physicsData->timeStep : 0;
    worldData->camera1->ogreCamera->moveRelative ( Ogre::Vector3 ( x, 0, z ) );
    x = z = 0;
    //rotation of the camera1, to the sides or up/down
    //+x -- rotateRight, +z -- rotateUp
    x -= ( worldData->camera1->rotateRight ) ? float ( physicsData->timeStep ) /
        10.0f : 0;
    x += ( worldData->camera1->rotateLeft ) ? float ( physicsData->timeStep ) /
        10.0f : 0;
    z += ( worldData->camera1->rotateUp ) ? float ( physicsData->timeStep ) /
        10.0f : 0;
    z -= ( worldData->camera1->rotateDown ) ? float ( physicsData->timeStep ) /
        10.0f : 0;
    //FIXME this for the new x,y,z coords. system!
    worldData->camera1->ogreCamera->pitch ( z );
    worldData->camera1->ogreCamera->yaw ( x );

    //rotate the cubes... this is still not handled by ODE
    for ( int currentCube = 0;
          currentCube < worldData->numberOfCubes; currentCube++ )
    {
/*        worldData->cubeList[currentCube].cubeNode->
            pitch ( float ( physicsData->timeStep ) /
                    ( ( currentCube % 2 ) ? 20.0f : -15.0f ) );
  */  }

    return ( 0 );
}

PhysicsEngine::~PhysicsEngine ( void )
{
    log.put ( LOG_INFO, "Removing ODE world data from memory");
    dCloseODE();
    //finally stop the log engine
    log.stop (  );
}
