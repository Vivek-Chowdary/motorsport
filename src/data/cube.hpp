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
    // data
    static int instancesCount;
    void updateOgrePosition ();
    void updateOgreOrientation ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

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
    Cube (char * xmlFilename);
    ~Cube ();

    // physics
    void stepPhysics ();
    dBodyID cubeID;
    dGeomID cubeGeomID;
    void setPosition (float posX, float posY, float posZ);

    // graphics
    void stepGraphics ();
    Ogre::Entity * cubeEntity;
    Ogre::SceneNode * cubeNode;

    // input
    void stepInput ();
};
#endif
