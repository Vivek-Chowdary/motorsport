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

#ifndef CLUTCH_HPP
#   define CLUTCH_HPP
#   include "worldObject.hpp"
#   include "engine.hpp"
#   include "data/xercesc_fwd.hpp"
//forward declarations
class Engine;
class Gearbox;

class Clutch : public WorldObject
{
  private:
    static int instancesCount;
    double coeffStaticFriction;
    double coeffDynamicFriction;
    double rotationalVelocity;
    double rotationalAcceleration;
    double maxTorqueTransfer;
    double torqueTransfer;
    double revTorque;
    bool locked;
    Engine *inputClass;
    Gearbox *pOutTorque;
    
  public:

    // data
    Clutch (XERCES_CPP_NAMESPACE::DOMNode * n, Engine *input);
    ~Clutch ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepPhysics ();
    void stopPhysics ();
    double getTorque ();
    double getRevTorque ();
    double getAngularVel ();
    void setOutputPointer (Gearbox *output);
};
#endif
