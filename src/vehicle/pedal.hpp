/*****************************************************************************\
|* Copyright (C) 2003, 2005 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef PEDAL_HPP
#define PEDAL_HPP
#include "tools/xercesc_fwd.hpp"
#include "worldObject.hpp"

class Pedal : public WorldObject
{
  private:
    bool userDriver;
    double angleRange;
    double currentAngle;
  public:
    Pedal (WorldObject * container, XmlTag * tag);
    ~Pedal ();
    void setUserDriver ();
    void stepPhysics ();
    double getNormalizedAngle();
};
#endif
