
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

#ifdef WIN32
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
#endif

PhysicsEngine::PhysicsEngine ( )
{
    //first of all start the logger (automatically logs the start of itself)
    log = new LogEngine ( LOG_INFO, "FSX" );

    //get the direction of the graphics data
    log->put ( LOG_INFO, "Setting up data pointers..." );
    worldData = WorldData::getWorldDataPointer();
    systemData = SystemData::getSystemDataPointer();

/*    //get the direction of the graphics data
    log->put(LOG_INFO, "Testing ODE library...");
    testOde();
*/
    log->put ( LOG_INFO, "Setting physics data" );
    systemData->physicsDesiredStepsPerSecond = 30;
    systemData->physicsTimeStep = 1000 / systemData->physicsDesiredStepsPerSecond;
    log->format ( LOG_INFO, "Physics rate set @ %i Hz (%i ms)",systemData->physicsDesiredStepsPerSecond, systemData->physicsTimeStep );

    log->put ( LOG_INFO, "Creating ODE world");
    worldData->worldID = dWorldCreate();
    worldData->spaceID = dHashSpaceCreate(0);
    worldData->jointGroupID = dJointGroupCreate (0);
    log->put ( LOG_INFO, "Setting ODE world gravity");
//    dWorldSetGravity (worldData->worldID, 0,0,-0.000098);
    //dWorldSetCFM (worldData->worldID, 1e-5);
    //dWorldSetERP (worldData->worldID, 0.8);
    //dWorldSetCFM (worldData->worldID, 1e-5);
    //dWorldSetERP (worldData->worldID, 0.8);
    log->put ( LOG_INFO, "Creating sample mass");
    dMass mass;
    log->put ( LOG_INFO, "Assigning box mass values");
    dMassSetBoxTotal (&mass, 10, 1, 1, 1);
    log->put ( LOG_INFO, "Creating cubes in ODE world");
    
    const int separation = 150;
    int size = Cube::cubeList.size();
    for ( int currentCube = 0; currentCube < size; currentCube++ )
    {
        Cube::cubeList[currentCube]->cubeID = dBodyCreate (worldData->worldID);

        if (currentCube+1 == size)
        {
            const float width = 1000000.0;
            dMassSetBox (&mass, 1, width, width, width);
            dBodySetMass(Cube::cubeList[currentCube]->cubeID,&mass);
            dBodySetPosition (Cube::cubeList[currentCube]->cubeID, 0, 0, (-width/2)-1000); 
            Cube::cubeList[currentCube]->cubeGeomID = dCreateBox (worldData->spaceID, width,width,width);
        }else{
        dBodySetMass (Cube::cubeList[currentCube]->cubeID, &mass);
            dBodySetPosition (Cube::cubeList[currentCube]->cubeID, currentCube % 10 * separation, currentCube / 10 % 10 * separation, currentCube / 100 % 10 * separation * ((int(currentCube/1000))+1));
            dBodySetAngularVel (Cube::cubeList[currentCube]->cubeID, float(random()%10)/10000.0, float(random()%10)/10000.0, float(random()%10)/10000.0);
            Cube::cubeList[currentCube]->cubeGeomID = dCreateBox (worldData->spaceID, 100,100,100);
        }
        dGeomSetBody (Cube::cubeList[currentCube]->cubeGeomID, Cube::cubeList[currentCube]->cubeID);
    }
}


void PhysicsEngine::nearCallback (void *data, dGeomID o1, dGeomID o2)
{
    int i,n;
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    if (b1 && b2 && dAreConnected(b1, b2))
        return;

    const int N = 10;
    dContact contact[N];
    n = dCollide (o1,o2,N,&contact[0].geom,sizeof(dContact));
    if (n > 0) {
        for (i=0; i<n; i++) {
            contact[i].surface.mode = dContactSlip1 | dContactSlip2 | dContactSoftERP | dContactSoftCFM | dContactApprox1;
            if (dGeomGetClass(o1) == dSphereClass || dGeomGetClass(o2) == dSphereClass)
                contact[i].surface.mu = 20;
            else
                contact[i].surface.mu = 0.5;
            contact[i].surface.slip1 = 0.0;
            contact[i].surface.slip2 = 0.0;
            contact[i].surface.soft_erp = 0.8;
            contact[i].surface.soft_cfm = 0.01;
            dJointID c = dJointCreateContact (WorldData::getWorldDataPointer()->worldID,WorldData::getWorldDataPointer()->jointGroupID,contact+i);
            dJointAttach (c,dGeomGetBody(o1),dGeomGetBody(o2));
        }
    }
}

int PhysicsEngine::step ( void )
//makes the graphics engine draw one frame
{
    //mega-verbosity
    log->put ( LOG_TRACE, "Doing an step: calculating a physics step" );
    
    int size = Cube::cubeList.size();
    for ( int i = 0; i < size; i++ )
    {
        Cube * currentCube = Cube::cubeList[i];
        dBodyID cubeID = currentCube->cubeID;
        ////////////////simplified air friction (test) 
        dBodySetAngularVel (Cube::cubeList[i]->cubeID,
            *(dReal*)(dBodyGetAngularVel(cubeID)+0),
            *(dReal*)(dBodyGetAngularVel(cubeID)+1),
            *(dReal*)(dBodyGetAngularVel(cubeID)+2)*(dReal)(0.01)*systemData->physicsTimeStep
        );
        //////////////////////////////////////simplified air friction
        //applying user input [forces]
        float moveToX = 0, moveToY = 0;
        moveToX += currentCube->getMoveToXPositive();
        moveToX -= currentCube->getMoveToXNegative();
        moveToY += currentCube->getMoveToYPositive();
        moveToY -= currentCube->getMoveToYNegative();
        moveToX /= 500;
        moveToY /= 500;
        dBodyAddForce (cubeID, moveToX * (systemData->physicsTimeStep), moveToY * (systemData->physicsTimeStep),0.0f );
    }
    
    dSpaceCollide (worldData->spaceID,0,&nearCallback);
    //alternative (x*y), fastest and less accurate physics calculations:
//    dWorldStepFast1(worldData->worldID, systemData->physicsTimeStep, 100/*int maxiterations*/);
    //traditional (x^y), theorycally slowest, and most accurate physics calculations:
    dWorldStep (worldData->worldID, systemData->physicsTimeStep);
    dJointGroupEmpty (worldData->jointGroupID);

    //camera should be a physics object?
    {
    float x = 0, z = 0;
    //translation of the camera, advancing or strafing
    x += ( Camera::cameraList[0]->goRight ) ? systemData->physicsTimeStep : 0;
    x -= ( Camera::cameraList[0]->goLeft ) ? systemData->physicsTimeStep : 0;
    z -= ( Camera::cameraList[0]->goForward ) ? systemData->physicsTimeStep : 0;
    z += ( Camera::cameraList[0]->goBack ) ? systemData->physicsTimeStep : 0;
    Camera::cameraList[0]->ogreCamera->moveRelative ( Ogre::Vector3 ( x, 0, z ) );
    }
    {
    float x = 0, z = 0;
    x -= Camera::cameraList[0]->getRotateRight ();
    x += Camera::cameraList[0]->getRotateLeft ();
    z += Camera::cameraList[0]->getRotateUp ();
    z -= Camera::cameraList[0]->getRotateDown ();
    x /= 100;
    z /= 100;
    Camera::cameraList[0]->ogreCamera->yaw (x * (systemData->physicsTimeStep));
    Camera::cameraList[0]->ogreCamera->pitch (z * (systemData->physicsTimeStep));
    }

    return ( 0 );
}

PhysicsEngine::~PhysicsEngine ( void )
{
    log->put ( LOG_INFO, "Destroying ODE world");
    dSpaceDestroy(worldData->spaceID); 
    log->put ( LOG_INFO, "Destroying ODE main collision space");
    dWorldDestroy(worldData->worldID); 
    log->put ( LOG_INFO, "Destroying ODE joints group");
    dJointGroupDestroy(worldData->jointGroupID); 
    log->put ( LOG_INFO, "Shutting down ODE");
    dCloseODE(); 
    
    //finally stop the log engine
    delete log;
}
