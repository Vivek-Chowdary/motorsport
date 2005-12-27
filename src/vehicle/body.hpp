/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef BODY_HPP
#define BODY_HPP
#include "ode/objects.h"
#include "worldObject.hpp"
#include "tools/xercesc_fwd.hpp"
#include "vector3d.hpp"
#include "paths.hpp"

//forward declarations
namespace Ogre {
  class Entity;
  class SceneNode;
}
class Quaternion;
class Vehicle;

class Body : public WorldObject
{
  private:
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    double dragCoefficient;
    double frontalArea;
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();
    void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopGraphics ();
  public:
    Body (WorldObject * container, std::string name, XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Body ();
    void stepPhysics ();
    void applyRotation (Quaternion rotation);
    dBodyID getBodyID ();
};
#endif
