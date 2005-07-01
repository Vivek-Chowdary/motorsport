/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef BODY_HPP
#   define BODY_HPP
#   include "ode/objects.h"
#   include "worldObject.hpp"
#   include "data/xercesc_fwd.hpp"
#   include "vector3d.hpp"

//forward declarations
namespace Ogre {
  class Entity;
  class SceneNode;
}

class Quaternion;

class Body : public WorldObject
{
  private:
    // data
    static int instancesCount;
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void updateOgrePosition ();
    void updateOgreOrientation ();
    double dragCoefficient;
    double frontalArea;

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();

    // graphics
    void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopGraphics ();
  public:
    // data
    Body (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Body ();

    // physics
    void stepPhysics ();
    dBodyID bodyID;
    dGeomID bodyGeomID;
    dGeomID bodyGeom2ID;
    dGeomID geomSpace;
    dGeomID geomSpace2;
    void setPosition (Vector3d position);
    void applyRotation (Quaternion rotation);
    Vector3d getPosition ();
    Quaternion getRotation ();

    // graphics
    void stepGraphics ();
    class Ogre::Entity;
    class Ogre::SceneNode;
    Ogre::Entity * bodyEntity;
    Ogre::SceneNode * bodyNode;
};
#endif
