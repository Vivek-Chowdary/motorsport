/******************************************************************************
*
* Copyright (C) 2004 Benjamin Beacock (bbeacock@uoguelph.ca)
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

#include "gearbox.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"

int Gearbox::instancesCount = 0;

Gearbox::Gearbox (XERCES_CPP_NAMESPACE::DOMNode * n, Clutch *input)
{
    log = new LogEngine (LOG_TRACE, "GBX");
    log->put (LOG_INFO, "Starting to parse a gearbox node");
    processXmlRootNode (n);
    instancesCount++;
    inputClass = input;
}

Gearbox::~Gearbox ()
{
    instancesCount--;
    stopPhysics ();
    delete log;
}


void Gearbox::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}

void Gearbox::setOutputPointer(Wheel * wheel1, Wheel * wheel2)
{
    pOutWheel1 = wheel1;
    pOutWheel2 = wheel2;
}
