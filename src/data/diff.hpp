/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef DIFF_HPP
#   define DIFF_HPP
#   include "drive.hpp"
#   include "data/xercesc_fwd.hpp"

class Diff : public Drive
{
  private:
    double finalDriveRatio;
    DriveJoint *outputJoint2; 

  public:

    // data
    Diff (XERCES_CPP_NAMESPACE::DOMNode * n);
    ~Diff ();
    void processXmlRootNode (XERCES_CPP_NAMESPACE::DOMNode * n);

    // physics
    void startPhysics (XERCES_CPP_NAMESPACE::DOMNode * n);
    void stepPhysics ();
    void stopPhysics ();

    void setOutputPointer2(DriveJoint *output2) { outputJoint2 = output2; } ;
};
#endif
