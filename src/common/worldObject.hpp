/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef WORLDOBJECT_HPP
#   define WORLDOBJECT_HPP
#   include "tools/xercesc_fwd.hpp"
#include "log/logEngine.hpp"
#include "ogreObject.hpp"
#include "odeObject.hpp"

//forward declatations
class OgreObject;
typedef std::map <std::string, OgreObject *> OgreObjects;
typedef std::map <std::string, OgreObject *>::iterator OgreObjectsIt;
class OdeObject;
typedef std::map <std::string, OdeObject *> OdeObjects;
typedef std::map <std::string, OdeObject *>::iterator OdeObjectsIt;

class WorldObject
{
  private:
    WorldObject ();
    void construct (std::string identifier);
  protected:
    LogEngine * log;
    WorldObject * base;
    std::string identifier;
    OgreObjects ogreObjects;
    OdeObjects odeObjects;
  public:
    WorldObject (std::string identifier);
    ~WorldObject ();
    std::string getIdentifier();
    LogEngine * getLog();
    void WorldObject::stepGraphics ();
};
#endif
