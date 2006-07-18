/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef PEDAL_HPP
#define PEDAL_HPP
#include "worldObject.hpp"

SHARED_PTR(Pedal, pPedal, wPedal);
class Pedal : public WorldObject
{
  private:
    bool userDriver;
    double angleRange;
    double currentAngle;
    Pedal (XmlTag * tag);
  public:
    static pPedal create(XmlTag * tag);
    ~Pedal ();
    void setUserDriver ();
    void stepPhysics ();
    double getNormalizedAngle();
};
#endif
