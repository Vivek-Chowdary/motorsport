/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "vector3d.hpp"

Vector3d::Vector3d ()
  : x(0), y(0),z(0)
{
  //empty
}

Vector3d::Vector3d (double _x, double _y, double _z)
  : x(_x), y(_y),z(_z)
{
  //empty
}
Vector3d::Vector3d(double qw, double qx, double qy, double qz)
{
    //from quaternion to euler radians
    double sqw = qw*qw;
    double sqx = qx*qx;
    double sqy = qy*qy;
    double sqz = qz*qz;

    y = atan2(2.0 * (qx * qy + qz * qw), (sqx - sqy - sqz + sqw));
    x = -atan2(2.0 * (qy * qz + qx * qw), (-sqx - sqy + sqz + sqw));
    z = -asin(-2.0 * (qx * qz - qy * qw));
}

const Vector3d & Vector3d::operator= (const Vector3d & cpy)
{
    //TODO: Check for this=this if the class becomes complicated.
    x = cpy.x;
    y = cpy.y;
    z = cpy.z;
    return *this;
}
bool Vector3d::operator== (const Vector3d & cmp) const
{
    return (x == cmp.x && y == cmp.y && z == cmp.z);
}

bool Vector3d::operator!= (const Vector3d & cmp) const
{
    return (!operator== (cmp));
}

Vector3d Vector3d::operator- (const Vector3d & k) const
{
    Vector3d tmp (*this);
    tmp -= k;
    return tmp;
}
Vector3d Vector3d::operator+ (const Vector3d & k) const
{
    Vector3d tmp (*this);
    tmp += k;
    return tmp;
}
const Vector3d & Vector3d::operator+= (const Vector3d & k)
{
    x += k.x;
    y += k.y;
    z += k.z;
    return *this;
}
const Vector3d & Vector3d::operator-= (const Vector3d & k)
{
    x -= k.x;
    y -= k.y;
    z -= k.z;
    return *this;
}
double Vector3d::distance () const
{
    return sqrt (x * x + y * y + z * z);
}
double Vector3d::distance (const Vector3d & k) const
{
    return (*this - k).distance ();
}




// cross product (normalized to 1 unit, is needed for OpenGL normal-vectors)
const Vector3d GetCrossProduct (const std::vector < Vector3d > &vec)
{
    // Normalx = (vtx1y - vtx2y) * (vtx2z - vtx3z) - (vtx1z - vtx2z) * (vtx2y - vtx3y) 
    // Normaly = (vtx1z - vtx2z) * (vtx2x - vtx3x) - (vtx1x - vtx2x) * (vtx2z - vtx3z) 
    // Normalz = (vtx1x - vtx2x) * (vtx2y - vtx3y) - (vtx1y - vtx2y) * (vtx2x - vtx3x) 

    Vector3d cross;
    
    if(vec.size() < 3 ) {
      return cross;        //maybe throw exception?
    }

    // calc cross product 
    cross.x = (vec[0].y - vec[1].y) * (vec[1].z - vec[2].z) - (vec[0].z - vec[1].z) * (vec[1].y - vec[2].y);
    cross.y = (vec[0].z - vec[1].z) * (vec[1].x - vec[2].x) - (vec[0].x - vec[1].x) * (vec[1].z - vec[2].z);
    cross.z = (vec[0].x - vec[1].x) * (vec[1].y - vec[2].y) - (vec[0].y - vec[1].y) * (vec[1].x - vec[2].x);

    // normalize vector 
    cross.scalarDivide(cross.distance());

    return cross;
}

// cross product (normalized to 1 unit, is needed for OpenGL normal-vectors)
const Vector3d GetCrossProduct_old (const std::vector < Vector3d > &vec)
{
  // Normalx = (vtx1y - vtx2y) * (vtx2z - vtx3z) - (vtx1z - vtx2z) * (vtx2y - vtx3y) 
  // Normaly = (vtx1z - vtx2z) * (vtx2x - vtx3x) - (vtx1x - vtx2x) * (vtx2z - vtx3z) 
  // Normalz = (vtx1x - vtx2x) * (vtx2y - vtx3y) - (vtx1y - vtx2y) * (vtx2x - vtx3x) 

    Vector3d cross, ret;

    /* * calc cross product */
    cross.x = (vec[0].y - vec[1].y) * (vec[1].z - vec[2].z) - (vec[0].z - vec[1]
.z) * (vec[1].y - vec[2].y);
    cross.y = (vec[0].z - vec[1].z) * (vec[1].x - vec[2].x) - (vec[0].x - vec[1]
.x) * (vec[1].z - vec[2].z);
    cross.z = (vec[0].x - vec[1].x) * (vec[1].y - vec[2].y) - (vec[0].y - vec[1]
.y) * (vec[1].x - vec[2].x);

    /* * normalize vector */
    double x, y, z, sum, sqr;
    x = y = z = sum = 0;

    // square coordinates
    x = cross.x * cross.x;
    y = cross.y * cross.y;
    z = cross.z * cross.z;

    // add
    sum = x + y + z;

    // square root
    sqr = pow (sum, (double) 0.5);
    if (!sqr)
    {
        sqr = 1;                // wg. Division durch 0
    }
    // vector ist normalized
    ret.x = cross.x / sqr;
    ret.y = cross.y / sqr;
    ret.z = cross.z / sqr;
    return ret;
}

const Vector3d GetAvgVector_old (const std::vector < Vector3d > & vec)
{
    Vector3d ret;
    size_t i, size;
    size = vec.size();
    double x, y, z;
    x = y = z = 0;
    for (i = 0; i < size; i++)
    {
        x += vec[i].x;
        y += vec[i].y;
        z += vec[i].z;
    }
    ret.x = x / size;
    ret.y = y / size;
    ret.z = z / size;
    return ret;
}

const Vector3d GetAvgVector (const std::vector < Vector3d > & vec)
{
    Vector3d ret;
    size_t size = vec.size();
    for (size_t i = 0; i < size; i++) {
      ret += vec[i];
    }
    ret.scalarDivide(size); //scalarDivide checks for div0
    return ret;
}

Vector3d& Vector3d::degreesToRadians ()
{
    const double piK = 3.14159265358979323846264338327950288419716939937510 / 180;
    scalarMultiply(piK);
    return *this;
}

// convenient functions to clean up the code.
// maybe these should overload operator+=(double)
// im also uncertain if this->x is better then x.
// but I have a vague memory about polymorph inheritance and problems.

Vector3d&  Vector3d::scalarAdd(const double value)
{
  this->x += value;
  this->y += value;
  this->z += value;
  return *this;
}
Vector3d&  Vector3d::scalarMultiply(const double value)
{
  this->x *= value;
  this->y *= value;
  this->z *= value;
  return *this;
}

Vector3d& Vector3d::scalarDivide(const double value)
{
  if (value != 0) {
    this->x /= value;
    this->y /= value;
    this->z /= value;
  }
  return *this;
}
