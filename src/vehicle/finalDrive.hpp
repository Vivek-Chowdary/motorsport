/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifndef FINALDRIVE_HPP
#define FINALDRIVE_HPP
#include "driveMass.hpp"

SHARED_PTR(FinalDrive, pFinalDrive, wFinalDrive);
class FinalDrive : public DriveMass
{
  private:
    double finalDriveRatio;
    DriveJoint *outputJoint2; 
    FinalDrive (XmlTag * tag);
    void readCustomDataTag(XmlTag * tag);
  public:
    static pFinalDrive create (XmlTag * tag);
    ~FinalDrive ();
    void stepPhysics ();
};
#endif
