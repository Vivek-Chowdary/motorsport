/******************************************************************************
*
* Copyright (C) 2004 Daniel Toplak (homer.simpson123@gmx.de)
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*   
* See the GNU General Public License for more details.
*   
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*   
******************************************************************************/

#ifndef VECTOR3D_CPP
#   define VECTOR3D_CPP

inline Vector3d::Vector3d () 
{
    x = y = z = 0;
}
inline Vector3d::Vector3d (double x, double y, double z) 
{
    this->x = x;
    this->y = y;
    this->z = z;
}
inline Vector3d::Vector3d (const Vector3d & cpy) 
{
    *this = cpy;
}
inline const Vector3d & Vector3d::operator= (const Vector3d & cpy) 
{
    x = cpy.x;
    y = cpy.y;
    z = cpy.z;
    return *this;
}
inline bool Vector3d::operator== (const Vector3d & cmp) const 
{
    return (x == cmp.x && y == cmp.y && z == cmp.z);
}
inline bool Vector3d::operator!= (const Vector3d & cmp) const 
{
    return (!operator== (cmp));
}
inline Vector3d & Vector3d::operator+ (const Vector3d & k) const 
{
    return Vector3d (*this) + k;
}
inline Vector3d & Vector3d::operator- (const Vector3d & k) const 
{
    return Vector3d (*this) - k;
}
inline const Vector3d & Vector3d::operator+= (const Vector3d & k) 
{
    x += k.x;
    y += k.y;
    z += k.y;
    return *this;
}
inline const Vector3d & Vector3d::operator-= (const Vector3d & k) 
{
    x -= k.x;
    y -= k.y;
    z -= k.y;
    return *this;
}
inline double Vector3d::distance () const 
{
    return sqrt (x * x + y * y + z * z);
}
inline double Vector3d::distance (const Vector3d & k) const 
{
    return (*this - k).distance ();
}

// cross product (normalized to 1 unit, is needed for OpenGL normal-vectors)
inline const Vector3d GetCrossProduct (const std::vector < Vector3d > &vec) 
{
    
        /*  Normalx = (vtx1y - vtx2y) * (vtx2z - vtx3z) - (vtx1z - vtx2z) * (vtx2y - vtx3y) Normaly = (vtx1z - vtx2z) * (vtx2x - vtx3x) - (vtx1x - vtx2x) * (vtx2z - vtx3z) Normalz = (vtx1x - vtx2x) * (vtx2y - vtx3y) - (vtx1y - vtx2y) * (vtx2x - vtx3x)  */ 
        Vector3d cross, ret;
    
        /*  * calc cross product */ 
        cross.x = (vec[0].y - vec[1].y) * (vec[1].z - vec[2].z) - (vec[0].z - vec[1].z) * (vec[1].y - vec[2].y);
    cross.y = (vec[0].z - vec[1].z) * (vec[1].x - vec[2].x) - (vec[0].x - vec[1].x) * (vec[1].z - vec[2].z);
    cross.z = (vec[0].x - vec[1].x) * (vec[1].y - vec[2].y) - (vec[0].y - vec[1].y) * (vec[1].x - vec[2].x);
    
        /*  * normalize vector */ 
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
        sqr = 1;               // wg. Division durch 0
    // vector ist normalized
    ret.x = cross.x / sqr;
    ret.y = cross.y / sqr;
    ret.z = cross.z / sqr;
    return ret;
}
inline const Vector3d GetAvgVector (const std::vector < Vector3d > &vec) 
{
    Vector3d ret;
    size_t i, size;
    size = vec.size ();
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
inline void Vector3d::degreesToRadians()
{
    const double piK = 3.14159265358979323846264338327950288419716939937510 / 180;
    x *= piK;
    y *= piK;
    z *= piK;
}

#endif  /*  */
    
