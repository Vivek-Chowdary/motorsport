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

//forward declatations

class WorldObject
{
  private:
    WorldObject ();
    void construct (std::string identifier);
  protected:
    LogEngine * log;
    std::string identifier;
  public:
    WorldObject (std::string identifier);
    ~WorldObject ();
    std::string getIdentifier();
    LogEngine * getLog();
};
#endif
