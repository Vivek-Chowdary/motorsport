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
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    bool userDriver;
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    double angleRange;
    double currentAngle;
  public:
    Pedal (WorldObject * container, std::string name, XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Pedal ();
    void setUserDriver ();
    void stepPhysics ();
    double getNormalizedAngle();
};
#endif
