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
    physicsSteps(0),
    physicsFrequency(0),
    graphicsSteps(0),
    graphicsFrequency(0),
    timeScale(1),
    pauseStep(0),
    height(1),
    width(1)
{
  //empty
}

SystemData::~SystemData ()
{
  //empty
}

bool SystemData::isMainLoopEnabled (void)
{
    return mainLoopEnabled;
}

void SystemData::enableMainLoop (void)
{
    mainLoopEnabled = true;
}
void SystemData::disableMainLoop (void)
{
    mainLoopEnabled = false;
}
double SystemData::getCfmValue()
{
    return cfmValue;
}
double SystemData::getErpValue()
{
    return erpValue;
}
void SystemData::setCfmValue(const double & value)
{
    cfmValue = value;
}
void SystemData::setErpValue(const double & value)
{
    erpValue = value;
}
