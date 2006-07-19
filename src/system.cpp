/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "system.hpp"
#include "SDL/begin_code.h"
#include <iostream>

pSystem System::system;

pSystem System::get()
{
    if (!system)
    {
        std::cout << "Auto-creating a default system!" << std::endl;
        new System();
    }
    return (system);
}
void System::destroy()
{
    if (!system)
    {
        std::cout << "WARNING: There's no world to be destroyed!" << std::endl;
    }
    else
    {
        system.reset();
    }
}
System::System ()
/*/
  : cfmValue(-1),
    erpValue(-1),
    currentPhysicsFrequency(0),
    physicsSteps(0),
    graphicsSteps(0),
    graphicsFrequency(0),
    timeScale(1),
    pauseStep(0),
    height(1),
    width(1),
    videoRecordTimestep(0),
    cameraDirector(false)
//*/
{
    if (system)
    {
        std::cout << "ERROR: Tried to create another system. This shouldn't have happened!" << std::endl;
    } else {
        pSystem tmp(this);
        system = tmp;
        cfmValue = -1;
        erpValue = -1;
        currentPhysicsFrequency = 0;
        physicsSteps = 0;
        graphicsSteps = 0;
        graphicsFrequency = 0;
        timeScale = 1;
        pauseStep = 0;
        height = 1;
        width = 1;
        videoRecordTimestep = 0;
    }
}

System::~System ()
{
    //empty
}

bool System::isMainLoopEnabled (void) { return mainLoopEnabled; }
void System::enableMainLoop (void)    { mainLoopEnabled = true; }
void System::disableMainLoop (void)   { mainLoopEnabled = false;}

double System::getCfmValue()                   { return cfmValue; }
double System::getErpValue()                   { return erpValue; }
void System::setCfmValue(const double & value) { cfmValue = value;}
void System::setErpValue(const double & value) { erpValue = value;}

double System::getCurrentPhysicsFrequency()               { return currentPhysicsFrequency;        }
double System::getCurrentPhysicsTimestep()                { return 1.0/currentPhysicsFrequency;    }                            
void System::setCurrentPhysicsFrequency(double frequency) { currentPhysicsFrequency = frequency;   }
void System::setCurrentPhysicsTimestep(double timestep)   { currentPhysicsFrequency = 1.0/timestep;}

double System::getDesiredPhysicsFrequency()               { return desiredPhysicsFrequency;        }
double System::getDesiredPhysicsTimestep()                { return 1.0/desiredPhysicsFrequency;    }
void System::setDesiredPhysicsFrequency(double frequency) { desiredPhysicsFrequency = frequency;   }
void System::setDesiredPhysicsTimestep(double timestep)   { desiredPhysicsFrequency = 1.0/timestep;}

void System::increaseDesiredPhysicsRate()
{
    if      (desiredPhysicsFrequency < 1)   setDesiredPhysicsFrequency (1);
    else if (desiredPhysicsFrequency < 10)  setDesiredPhysicsFrequency (desiredPhysicsFrequency + 1);
    else if (desiredPhysicsFrequency < 30)  setDesiredPhysicsFrequency (desiredPhysicsFrequency + 2);
    else if (desiredPhysicsFrequency < 60)  setDesiredPhysicsFrequency (desiredPhysicsFrequency + 5);
    else if (desiredPhysicsFrequency < 150) setDesiredPhysicsFrequency (desiredPhysicsFrequency + 10);
    else if (desiredPhysicsFrequency < 350) setDesiredPhysicsFrequency (desiredPhysicsFrequency + 20);
    else if (desiredPhysicsFrequency < 1000)setDesiredPhysicsFrequency (desiredPhysicsFrequency + 50);
    else                                    setDesiredPhysicsFrequency (desiredPhysicsFrequency + 100);
    
}
void System::decreaseDesiredPhysicsRate()
{
    if      (desiredPhysicsFrequency <= 1)   setDesiredPhysicsFrequency (1);
    else if (desiredPhysicsFrequency <= 10)  setDesiredPhysicsFrequency (desiredPhysicsFrequency - 1);
    else if (desiredPhysicsFrequency <= 30)  setDesiredPhysicsFrequency (desiredPhysicsFrequency - 2);
    else if (desiredPhysicsFrequency <= 60)  setDesiredPhysicsFrequency (desiredPhysicsFrequency - 5);
    else if (desiredPhysicsFrequency <= 150) setDesiredPhysicsFrequency (desiredPhysicsFrequency - 10);
    else if (desiredPhysicsFrequency <= 350) setDesiredPhysicsFrequency (desiredPhysicsFrequency - 20);
    else if (desiredPhysicsFrequency <= 1000)setDesiredPhysicsFrequency (desiredPhysicsFrequency - 50);
    else                                     setDesiredPhysicsFrequency (desiredPhysicsFrequency - 100);
    
}

std::string System::getCurrentPath()
{
    return currentPath;
}

void System::setCurrentPath(std::string path)
{
    currentPath = path;
}
