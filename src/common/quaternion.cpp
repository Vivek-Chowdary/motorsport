/*****************************************************************************\
|* Copyright (C) 2003, 2007 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*    [ https://svn.sourceforge.net/svnroot/motorsport/trunk/doc/LICENSE ]   *|
\*****************************************************************************/

#include "quaternion.hpp"
#include "vector3d.hpp"
#include <sstream>

Quaternion::Quaternion ()
  : w(1), x(0), y(0),z(0)
{
    // empty
}

Quaternion::Quaternion (dReal _w, dReal _x, dReal _y, dReal _z)
  : w(_w), x(_x), y(_y),z(_z)
{
    // empty
}

void Quaternion::SetFromEuler(dReal phi, dReal theta, dReal psi)
{
    // euler degrees to euler radians
#if defined(dSINGLE)
    const dReal piK = (dReal)(3.14159265358979323846 / 180.);
#else
    const dReal piK = 3.14159265358979323846264338327950288419716939937510 / 180.;
#endif
	phi *= piK;
    theta *= piK;
    psi *= piK;
    // euler angles to ode matrix
    dMatrix3 tmpMatrix;
    dRFromEulerAngles (tmpMatrix, phi, theta, psi);
    // ode matrix to ode quaternion
    dQuaternion tmpQuaternion;
    dRtoQ (tmpMatrix, tmpQuaternion);
    // ode quaternion to mosp quaternion
    w = tmpQuaternion[0];
    x = tmpQuaternion[1];
    y = tmpQuaternion[2];
    z = tmpQuaternion[3];
}

Quaternion::Quaternion (dReal phi, dReal theta, dReal psi)
{
  SetFromEuler(phi, theta, psi);
}

Quaternion::Quaternion (dQuaternion odeQuaternion)
{
    w = odeQuaternion[0];
    x = odeQuaternion[1];
    y = odeQuaternion[2];
    z = odeQuaternion[3];
}
Quaternion::Quaternion (const dReal * odeQuaternion)
{
    w = odeQuaternion[0];
    x = odeQuaternion[1];
    y = odeQuaternion[2];
    z = odeQuaternion[3];
}

Quaternion::Quaternion(const std::string &srcString)
{
    std::stringstream tmpString (srcString);
    const dReal magicValue = 0.123456789f;
    dReal tmpW, tmpX, tmpY, tmpZ = magicValue;
    tmpString >> tmpW >> tmpX >> tmpY >> tmpZ;
    if ( tmpZ == magicValue ) {
      SetFromEuler (tmpW, tmpX, tmpY);   
    } else {
      w = tmpW;
      x = tmpX; 
      y = tmpY;
      z = tmpZ;
    }
}

dReal * Quaternion::getOdeQuaternion ()
{
    // removing 'static' shows a warning: address of local variable `quat' returned
    static dReal quat[4];
    quat[0] = w;
    quat[1] = x;
    quat[2] = y;
    quat[3] = z;
    return quat;
}
void Quaternion::getOdeMatrix (dMatrix3 & dstRotation)
{
    dQtoR (getOdeQuaternion(), dstRotation);
}

const Quaternion & Quaternion::operator= (const Quaternion & cpy)
{
    //TODO: Check for this=this if the class becomes complicated.
    w = cpy.w;
    x = cpy.x;
    y = cpy.y;
    z = cpy.z;
    return *this;
}
bool Quaternion::operator== (const Quaternion & cmp) const
{
    return (w == cmp.w && x == cmp.x && y == cmp.y && z == cmp.z);
}

bool Quaternion::operator!= (const Quaternion & cmp) const
{
    return (!operator== (cmp));
}
Quaternion Quaternion::operator* (const Quaternion & k) const
{
    return Quaternion
    (
        w * k.w - x * k.x - y * k.y - z * k.z,
        w * k.x + x * k.w + y * k.z - z * k.y,
        w * k.y + y * k.w + z * k.x - x * k.z,
        w * k.z + z * k.w + x * k.y - y * k.x
    );
}
Quaternion Quaternion::operator- (const Quaternion & k) const
{
    return Quaternion
    (
        w - k.w,
        x - k.x,
        y - k.y,
        z - k.z
    );
}
const Quaternion & Quaternion::operator-= (const Quaternion & k)
{
    return (*this) = (*this) - k;
}
Quaternion Quaternion::operator+ (const Quaternion & k) const
{
    return Quaternion
    (
        w + k.w,
        x + k.x,
        y + k.y,
        z + k.z
    );
}
const Quaternion & Quaternion::operator+= (const Quaternion & k)
{
    return (*this) = (*this) + k;
}

Vector3d Quaternion::rotateObject (Vector3d objectPosition)
{
    // convert this to ode rotation quaternion
    dQuaternion odeQuaternion; // 4x1
    odeQuaternion[0] = w;
    odeQuaternion[1] = x;
    odeQuaternion[2] = y;
    odeQuaternion[3] = z;

    // convert ode rotation quaternion to ode rotation matrix
    dMatrix3 odeMatrix; // 3x3        B p*q  p=3  q=3
    dQtoR (odeQuaternion, odeMatrix);

    // create odeOldPosition
    dReal odeOldPosition[3]; // 3x1   C q*r  q=3  r=1
    odeOldPosition[0] = objectPosition.x;
    odeOldPosition[1] = objectPosition.y;
    odeOldPosition[2] = objectPosition.z;
    
    // create odeNewPosition holder
    dReal odeNewPosition[3]; // 3x1   A p*r  p=3  r=1

    // multiply matrix * oldPos into newPos
    //    void dMultiply0 (dReal *A, const dReal *B, const dReal *C, int p,int q,int r);
    //    A = B  * C   (sizes: A:p*r B:p*q C:q*r)
    dMultiply0 (odeNewPosition, odeMatrix, odeOldPosition, 3, 3, 1);

    // convert odeNewPosition into a Vector3d
    Vector3d newPosition;
    newPosition.x = odeNewPosition[0];
    newPosition.y = odeNewPosition[1];
    newPosition.z = odeNewPosition[2];

    // return the new position
    return newPosition;
}


