/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef ENGINE_HPP
#   define ENGINE_HPP
#   include "drive.hpp"
#   include "driveJoint.hpp"
#   include "data/xercesc_fwd.hpp"

class Engine : public Drive
{
  private:
   double torqueLinearMultiplier;

    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();
  public:
    // data
    Engine (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Engine ();

    // physics
    void stepPhysics ();
};
#endif
