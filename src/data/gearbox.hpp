/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef GEARBOX_HPP
#   define GEARBOX_HPP
#   include "drive.hpp"
#   include "wheel.hpp"
#   include "data/xercesc_fwd.hpp"

class Gearbox : public Drive
{
  private:
    // data
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();

    // physics
    double gearRatio;
  public:
    // data
    Gearbox (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Gearbox ();

    // physics
    void stepPhysics ();
};
#endif
