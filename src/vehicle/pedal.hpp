/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef PEDAL_HPP
#define PEDAL_HPP
#include "tools/xercesc_fwd.hpp"
#include "worldObject.hpp"
#include "vehicleComponent.hpp"

class Pedal : public WorldObject, public VehicleComponent
{
  private:
    // data
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    bool userDriver;
    std::string id;

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();
    double angleRange;
    double currentAngle;
  public:
    // data
    Pedal (XERCES_CPP_NAMESPACE::DOMNode * n, Vehicle * vehicle);
    ~Pedal ();
    void setUserDriver ();
    std::string getId();

    // physics
    void stepPhysics ();
    double getNormalizedAngle();
};
#endif
