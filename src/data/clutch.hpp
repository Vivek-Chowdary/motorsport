/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

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
    // data
    static int instancesCount;

    // physics
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
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();
  public:
    // data
    Clutch (XERCES_CPP_NAMESPACE::DOMNode * n, Engine *input);
    ~Clutch ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

    // physics
    void stepPhysics ();
    double getTorque ();
    double getRevTorque ();
    double getAngularVel ();
    void setOutputPointer (Gearbox *output);
};
#endif
