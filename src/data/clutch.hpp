/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef CLUTCH_HPP
#   define CLUTCH_HPP
#   include "driveJoint.hpp"
#   include "data/xercesc_fwd.hpp"

class Clutch : public DriveJoint
{
  private:
    // data
    static int instancesCount;

    // physics
    double coeffStaticFriction;
    double coeffDynamicFriction;
    double maxTorqueTransfer;
    double lockedParam;
    bool locked;
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stopPhysics ();
  public:
    // data
    Clutch (XERCES_CPP_NAMESPACE::DOMNode * n);
    Clutch ();
    ~Clutch ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

    // physics
    void stepPhysics ();
};
#endif
