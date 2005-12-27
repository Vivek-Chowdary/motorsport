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
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n);
  public:
    Part (WorldObject * container, const std::string & name);
    ~Part ();
    void stepPhysics ();
};
#endif
