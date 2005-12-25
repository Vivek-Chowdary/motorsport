/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef VEHICLECOMPONENT_HPP
#   define VEHICLECOMPONENT_HPP
#   include "tools/xercesc_fwd.hpp"
#include "log/logEngine.hpp"

//forward declatations
class Vehicle;

class VehicleComponent
{
  protected:
    Vehicle * vehicle;
  public:
    VehicleComponent (Vehicle * vehicle);
    ~VehicleComponent ();
};
#endif
