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

class Vector3d
{
  public:
    double x;
    double y;
    double z;
    Vector3d ();
    Vector3d (double x, double y, double z);
    Vector3d(double qw, double qx, double qy, double qz); //quat to euler radians
//    Vector3d (const Vector3d & cpy); default copy constructor used.
    const Vector3d & operator= (const Vector3d & cpy);
    bool operator== (const Vector3d & cpy) const;
    bool operator!= (const Vector3d & cpy) const;
    Vector3d operator+ (const Vector3d & k) const;
    Vector3d operator- (const Vector3d & k) const;
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
