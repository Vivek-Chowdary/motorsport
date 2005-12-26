/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef PART_HPP
#define PART_HPP
#include <string>
#include "worldObject.hpp"
#include "paths.hpp"
#include "ogreObject.hpp"
#include "odeObject.hpp"

class Vector3d;
class Quaternion;
namespace Ogre {
  class Entity;
  class SceneNode;
}

class Part : public WorldObject
{
  private:
    std::string relativePartDir;
    OgreObjects ogreObjects;
    OdeObjects odeObjects;
    // data
    static int instancesCount;
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();

    // graphics
    void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopGraphics ();
  public:
    Part (const std::string & partName);
    ~Part ();

    void setPosition (Vector3d position);
    void setRotation (Quaternion rotation);

    void stepPhysics ();
    void stepGraphics ();
};
#endif
