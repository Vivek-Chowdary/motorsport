/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

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
