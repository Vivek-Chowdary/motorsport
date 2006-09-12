/*****************************************************************************\
|* Copyright (C) 2003, 2006 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifndef QUATERNION_HPP
#   define QUATERNION_HPP
#   include "ode/ode.h"
#   include <string>

class Vector3d;

class Quaternion
{
  public:
    dReal w;
    dReal x;
    dReal y;
    dReal z;
    Quaternion ();                                      // defaults to (1, 0, 0, 0)
    Quaternion (dReal w, dReal x, dReal y, dReal z);// from quaternion values
    Quaternion (dReal phi, dReal theta, dReal psi);  // from Euler angles
    Quaternion (dQuaternion odeQuaternion);             // from ode quaternion
    Quaternion (const dReal * odeQuaternion);           // from ode array
    Quaternion (const std::string &srcString);          // from std::string
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

    Vector3d rotateObject (Vector3d objectPosition);

  private:
  void SetFromEuler(dReal phi, dReal theta, dReal psi); // bad name
};

#endif
