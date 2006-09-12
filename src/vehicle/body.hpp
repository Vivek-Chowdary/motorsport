/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifndef BODY_HPP
#define BODY_HPP
#include "worldObject.hpp"
#include "vector3d.hpp"
#include "paths.hpp"

// Forward declarations
namespace Ogre { class Entity; class SceneNode; }
class Quaternion;
class Vehicle;

SHARED_PTR(Body, pBody, wBody);

class Body : public WorldObject
{
  private:
    double dragCoefficient;
    double frontalArea;
    void stopPhysics ();
    void stopGraphics ();
    Body (XmlTag * tag);
    void readCustomDataTag(XmlTag * tag);
  public:
    static pBody create (XmlTag * tag);
    ~Body ();
    void stepPhysics ();
    void setUserDriver ();
};
#endif
