/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "system.hpp"
#include "SDL/begin_code.h"

SystemData SystemData::systemDataObject = SystemData();

SystemData *SystemData::getSystemDataPointer ()
{
    return (&systemDataObject);
}

SystemData::SystemData ()
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
    dataDir("../data"),
    tmpPath("")
{
  //empty
}

SystemData::~SystemData ()
{
  //empty
}

bool SystemData::isMainLoopEnabled (void) { return mainLoopEnabled; }
void SystemData::enableMainLoop (void)    { mainLoopEnabled = true; }
void SystemData::disableMainLoop (void)   { mainLoopEnabled = false;}

double SystemData::getCfmValue()                   { return cfmValue; }
double SystemData::getErpValue()                   { return erpValue; }
void SystemData::setCfmValue(const double & value) { cfmValue = value;}
void SystemData::setErpValue(const double & value) { erpValue = value;}

double SystemData::getCurrentPhysicsFrequency()               { return currentPhysicsFrequency;        }
double SystemData::getCurrentPhysicsTimestep()                { return 1.0/currentPhysicsFrequency;    }                            
void SystemData::setCurrentPhysicsFrequency(double frequency) { currentPhysicsFrequency = frequency;   }
void SystemData::setCurrentPhysicsTimestep(double timestep)   { currentPhysicsFrequency = 1.0/timestep;}

double SystemData::getDesiredPhysicsFrequency()               { return desiredPhysicsFrequency;        }
double SystemData::getDesiredPhysicsTimestep()                { return 1.0/desiredPhysicsFrequency;    }
void SystemData::setDesiredPhysicsFrequency(double frequency) { desiredPhysicsFrequency = frequency;   }
void SystemData::setDesiredPhysicsTimestep(double timestep)   { desiredPhysicsFrequency = 1.0/timestep;}

void SystemData::increaseDesiredPhysicsRate()
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
void SystemData::decreaseDesiredPhysicsRate()
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

