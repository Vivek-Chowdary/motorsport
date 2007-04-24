/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
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

SHARED_PTR_MAP(Part, pPart, Parts, PartsIt, wPart);

class Part : public WorldObject
{
  private:
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    Part (const std::string & name);
    void readCustomDataTag(XmlTag * tag);
  public:
    static pPart create (const std::string & name);
    ~Part ();
    void stepPhysics ();
};
#endif
