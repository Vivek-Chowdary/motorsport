/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef FINALDRIVE_HPP
#define FINALDRIVE_HPP
#include "driveMass.hpp"

SHARED_PTR(FinalDrive, pFinalDrive);
class FinalDrive : public DriveMass
{
  private:
    double finalDriveRatio;
    DriveJoint *outputJoint2; 
    FinalDrive (WorldObject * container, XmlTag * tag);
  public:
    static pFinalDrive create (WorldObject * container, XmlTag * tag);
    ~FinalDrive ();
    void stepPhysics ();
};
#endif
