/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifndef WHEEL_HPP
#define WHEEL_HPP

#include <ode/objects.h>
#include "driveMass.hpp"
#include "worldObject.hpp"
#include "paths.hpp"

// Forward declarations
class Vector3d;
class Quaternion;
namespace Ogre {
  class Entity;
  class SceneNode;
}
SHARED_PTR(Suspension, pSuspension, wSuspension);
SHARED_PTR(Wheel, pWheel, wWheel);

class Wheel : public DriveMass
{
  private:
    double powered;
    pSuspension baseSuspension;
    pPedal brakePedal;
    Wheel (XmlTag * tag);
    void readCustomDataTag(XmlTag * tag);
  public:
    static pWheel create(XmlTag * tag);
    ~Wheel ();
    void setSusp(pSuspension suspension) { baseSuspension = suspension; };
    void setUserDriver ();
    void setBrakePedal (pPedal pedal);
    void stepPhysics ();
    double getInputAngularVel();
};
#endif
