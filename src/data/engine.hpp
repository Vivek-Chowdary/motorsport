/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

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
