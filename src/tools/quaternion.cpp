/*****************************************************************************\
|* Copyright (C) 2003, 2004 "Motorsport" developers (*)                      *|
|* Part of the "Motorsport" project (http://motorsport.sourceforge.net)      *|
|* Licensed under the GNU General Public License (*)                         *|
|*                                                                           *|
|* (*) Complete information is provided at the following URL:                *|
|*           [ http://motorsport-sim.org/svn/trunk/doc/LICENSE ]             *|
\*****************************************************************************/

#include "quaternion.hpp"
#include "vector3d.hpp"

Quaternion::Quaternion ()
  : w(1), x(0), y(0),z(0)
{
    // empty
}

Quaternion::Quaternion (double _w, double _x, double _y, double _z)
  : w(_w), x(_x), y(_y),z(_z)
{
    // empty
}

Quaternion::Quaternion (double phi, double theta, double psi)
{
    // euler degrees to euler radians
    const double piK = 3.14159265358979323846264338327950288419716939937510 / 180;
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
    /*
    dQuaternion odeQuaternion;

    odeQuaternion[0] = w;
    odeQuaternion[1] = x;
    odeQuaternion[2] = y;
    odeQuaternion[3] = z;
    dMatrix3 matrixA;
    dQtoR (odeQuaternion, matrixA);

    odeQuaternion[0] = k.w;
    odeQuaternion[1] = k.x;
    odeQuaternion[2] = k.y;
    odeQuaternion[3] = k.z;
    dMatrix3 matrixB;
    dQtoR (odeQuaternion, matrixB);

    dMatrix3 matrixC;
    for (int i = 0; i<3; i++)
        for (int j = 0; j<3; j++)
            matrixC[i*j] = matrixA[i*j] - matrixB[i*j];

    dRtoQ (matrixC, odeQuaternion);
    return odeQuaternion;
    */
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
    w -= k.w;
    x -= k.x;
    y -= k.y;
    z -= k.z;
    return *this;
}
Quaternion Quaternion::operator+ (const Quaternion & k) const
{
    /*
    dQuaternion odeQuaternion;

    odeQuaternion[0] = w;
    odeQuaternion[1] = x;
    odeQuaternion[2] = y;
    odeQuaternion[3] = z;
    dMatrix3 matrixA;
    dQtoR (odeQuaternion, matrixA);

    odeQuaternion[0] = k.w;
    odeQuaternion[1] = k.x;
    odeQuaternion[2] = k.y;
    odeQuaternion[3] = k.z;
    dMatrix3 matrixB;
    dQtoR (odeQuaternion, matrixB);

    dMatrix3 matrixC;
    for (int i = 0; i<3; i++)
        for (int j = 0; j<3; j++)
            matrixC[i*j] = matrixA[i*j] + matrixB[i*j];
            //matrixC(i,j) = matrixA(i,j) + matrixB(i,j);

    dRtoQ (matrixC, odeQuaternion);
    return odeQuaternion;
*/
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
    w += k.w;
    x += k.x;
    y += k.y;
    z += k.z;
    return *this;
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


