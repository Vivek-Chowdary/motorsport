/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef WHEEL_HPP
#   define WHEEL_HPP
#   include "ode/objects.h"
#   include "driveMass.hpp"
#   include "worldObject.hpp"
#   include "data/xercesc_fwd.hpp"
#   include <string>

//forward declarations
class Vector3d;
class Quaternion;
namespace Ogre {
  class Entity;
  class SceneNode;
}

class Wheel : public DriveMass
{
  private:
    // data
    static int instancesCount;
    bool userDriver;
    std::string index;
    double powered;
    dBodyID bodyID;
    dJointID suspJointID;
    void updateOgrePosition ();
    void updateOgreOrientation ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    
    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();
    
    // graphics
    void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopGraphics ();
  public:
    // data
    Wheel (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Wheel ();
    std::string getIndex();
    void setRefBody(dBodyID inputID) { bodyID = inputID; };
    void setSuspJoint(dJointID injointID) { suspJointID = injointID; };
    void setUserDriver ();
    
    // physics
    void stepPhysics ();
    dBodyID wheelID;
    dGeomID wheelGeomID;
    void setPosition (Vector3d position);
    Vector3d getPosition ();
    void applyRotation (Quaternion rotation);
    Quaternion getRotation ();
    double getInputAngularVel();
    
    // graphics
    void stepGraphics ();
    class Ogre::Entity;
    class Ogre::SceneNode;
    Ogre::Entity * wheelEntity;
    Ogre::SceneNode * wheelNode;
};
#endif
