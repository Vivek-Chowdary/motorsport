/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#ifndef VECTOR3D_HPP
#   define VECTOR3D_HPP
#   include <vector>
#   include <cmath>
#   include "ode/ode.h"

class Vector3d
{
  public:
    double x;
    double y;
    double z;
    Vector3d ();
    Vector3d (double x, double y, double z);                // from separated components
    Vector3d(double qw, double qx, double qy, double qz);   // from quaternion to euler radians
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
    double distance () const;
    double distance (const Vector3d & k) const;
    Vector3d & degreesToRadians();
    Vector3d & scalarAdd(const double value);
    Vector3d & scalarMultiply(const double value);
    Vector3d & scalarDivide(const double value);
};

#endif
