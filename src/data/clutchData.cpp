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

#include "clutch.hpp"
#include "xmlParser.hpp"
#include "log/logEngine.hpp"

int Clutch::instancesCount = 0;

Clutch::Clutch (XERCES_CPP_NAMESPACE::DOMNode * n, Engine *input)
{
    log = new LogEngine (LOG_TRACE, "CLU");
    log->put (LOG_INFO, "Starting to parse a clutch node");
    processXmlRootNode (n);
    instancesCount++;
    inputClass = input;
}

Clutch::~Clutch ()
{
    instancesCount--;
    stopPhysics ();
    delete log;
}


void Clutch::processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n)
{
    startPhysics (n);
}

void Clutch::setOutputPointer(Gearbox *output)
{
    pOutTorque = output;
}
