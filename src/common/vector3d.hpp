/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#ifndef VECTOR3D_HPP
#define VECTOR3D_HPP

#include <vector>
#include <cmath>
#include <ode/ode.h>

class Vector3d
{
  public:
    dReal x;
    dReal y;
    dReal z;
    Vector3d ();
    Vector3d (dReal x, dReal y, dReal z);                // from separated components
    Vector3d(dReal qw, dReal qx, dReal qy, dReal qz);   // from quaternion to euler radians
    Vector3d (const dReal * odeArray);                      // from ode array
//    Vector3d (const Vector3d & cpy); default copy constructor used.
    Vector3d (const std::string &srcString);

    const Vector3d & operator= (const Vector3d & cpy);
    const Vector3d & operator= (const dReal * & odeArray);  // from ode array
    bool operator== (const Vector3d & cpy) const;
    bool operator!= (const Vector3d & cpy) const;
    Vector3d operator+ (const Vector3d & k) const;
    Vector3d operator- (const Vector3d & k) const;
    Vector3d operator/ (const Vector3d & k) const;
    Vector3d operator* (const Vector3d & k) const;
    const Vector3d & operator+= (const Vector3d & k);
    const Vector3d & operator-= (const Vector3d & k);
    dReal distance () const;
    dReal distance (const Vector3d & k) const;
    Vector3d & degreesToRadians();
    Vector3d & scalarAdd(const dReal value);
    Vector3d & scalarMultiply(const dReal value);
    Vector3d & scalarDivide(const dReal value);
    static const Vector3d GetCrossProduct (const std::vector < Vector3d > &vec);
};

#endif
