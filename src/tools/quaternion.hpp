/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef QUATERNION_HPP
#   define QUATERNION_HPP
#   include "ode.h"

class Vector3d;

class Quaternion
{
  public:
    double w;
    double x;
    double y;
    double z;
    Quaternion ();                                      // defaults to (1, 0, 0, 0)
    Quaternion (double w, double x, double y, double z);// from quaternion values
    Quaternion (double phi, double theta, double psi);  // from Euler angles
    Quaternion (dQuaternion odeQuaternion);             // from ode quaternion
    Quaternion (const dReal * odeQuaternion);           // from ode array
    dReal * getOdeQuaternion ();                        // to ode array
    void getOdeMatrix (dMatrix3 & dstRotation);           // to ode matrix

    const Quaternion & operator= (const Quaternion & cpy);
    bool operator== (const Quaternion & cpy) const;
    bool operator!= (const Quaternion & cpy) const;

    Quaternion operator* (const Quaternion & k) const;
    Quaternion operator- (const Quaternion & k) const;
    const Quaternion & operator-= (const Quaternion & k);
    Quaternion operator+ (const Quaternion & k) const;
    const Quaternion & operator+= (const Quaternion & k);

    Vector3d Quaternion::rotateObject (Vector3d objectPosition);
};

#endif
