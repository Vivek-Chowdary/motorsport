/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef PART_HPP
#   define PART_HPP
#   include <string>
#   include "ode/objects.h"
#   include "worldObject.hpp"

class Vector3d;
class Quaternion;
namespace Ogre {
  class Entity;
  class SceneNode;
}

class Part : public WorldObject
{
  private:
    // data
    static int instancesCount;
    void updateOgrePosition ();
    void updateOgreOrientation ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    std::string partType;

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();

    // graphics
    void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopGraphics ();

    // input
    void startInput ();
    void stopInput ();
  public:
    // data
    Part (const std::string & partName);
    ~Part ();

    // physics
    void stepPhysics ();
    dBodyID partID;
    dGeomID partGeomID;
    void setPosition (Vector3d position);
    void setRotation (Quaternion rotation);

    // graphics
    void stepGraphics ();
    Ogre::Entity * partEntity;
    Ogre::SceneNode * partNode;

    // input
    void stepInput ();
};
#endif
