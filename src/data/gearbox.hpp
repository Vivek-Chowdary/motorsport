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

#ifndef GEARBOX_HPP
#   define GEARBOX_HPP
#   include "worldObject.hpp"
#   include "clutch.hpp"
#   include "wheel.hpp"
#   include "data/xercesc_fwd.hpp"
//forward declarations
class Clutch;
class Wheel;

class Gearbox : public WorldObject
{
  private:
    static int instancesCount;
    double gearboxFriction;
    double gearboxInertia;
    double angularVel;
    double angularAcc;
    double gearRatio;
    double torqueTransfer;
    double revTorque;
    bool locked;
    Clutch *inputClass;
    Wheel *pOutWheel1;
    Wheel *pOutWheel2;
  public:

    // data
    Gearbox (XERCES_CPP_NAMESPACE::DOMNode * n, Clutch *input);
    ~Gearbox ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepPhysics ();
    void stopPhysics ();
    double getTorque ();
    double getRevTorque ();
    double getAngularVel ();

    // links
    void setOutputPointer(Wheel * wheel1, Wheel * wheel2);
};
#endif
