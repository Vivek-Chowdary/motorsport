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

#ifndef ENGINE_HPP
#   define ENGINE_HPP
#   include "worldObject.hpp"
#   include "data/xercesc_fwd.hpp"
//forward declaration
class Clutch;

class Engine : public WorldObject
{
  private:
    static int instancesCount;
    double torqueLinearMultiplier;
    double rotationalVelocity;
    double prevRotationalVelocity;
    double rotationalAcceleration;
    double torque;
    double engineTorque;
    double engineInertia;
    double engineFriction;
    Clutch *pOutTorque;
  
  public:

    // data
    Engine (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Engine ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepPhysics ();
    void stopPhysics ();
    double getTorque ();
    double getRotationalVelocity ();
    void setOutputPointer (Clutch *output);
};
#endif
