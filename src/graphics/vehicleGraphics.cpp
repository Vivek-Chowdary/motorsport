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

#include "vehicle.hpp"
#include "data/xercesc_fwd.hpp"
#include "body.hpp"
#include "wheel.hpp"
#include "suspension.hpp"

void Vehicle::startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n)
{
}

void Vehicle::stepGraphics ()
{
    body->stepGraphics();
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        suspIter->second->stepGraphics();
    }
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        wheelIter->second->stepGraphics();
    }
}

void Vehicle::stopGraphics ()
{
    body->stopGraphics();
    std::map < std::string, Suspension * >::const_iterator suspIter;
    for (suspIter=suspensionMap.begin(); suspIter != suspensionMap.end(); suspIter++)
    {
        suspIter->second->stopGraphics();
    }
    std::map < std::string, Wheel * >::const_iterator wheelIter;
    for (wheelIter=wheelMap.begin(); wheelIter != wheelMap.end(); wheelIter++)
    {
        wheelIter->second->stopGraphics();
    }
}
