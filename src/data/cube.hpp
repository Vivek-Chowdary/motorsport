/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef CUBE_HPP
#   define CUBE_HPP
#   include "ode/objects.h"
#   include "worldObject.hpp"

namespace Ogre {
  class Entity;
  class SceneNode;
}

class Cube : public WorldObject
{
  private:
    static int instancesCount;

  public:
    // data
    Cube (char * xmlFilename);
    ~Cube ();
    void updateOgrePosition ();
    void updateOgreOrientation ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepPhysics ();
    void stopPhysics ();
    dBodyID cubeID;
    dGeomID cubeGeomID;
    void setPosition (float posX, float posY, float posZ);

    // graphics
    void startGraphics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepGraphics ();
    void stopGraphics ();

    Ogre::Entity * cubeEntity;
    Ogre::SceneNode * cubeNode;

    // input
    void startInput ();
    void stepInput ();
    void stopInput ();
};
#endif
