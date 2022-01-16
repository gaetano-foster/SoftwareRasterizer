#include "Maths.h"

using namespace vec;
using namespace mat4;

float Vec3D::Length() const
{
    return sqrtf(x * x + y * y + z * z);
}

Vec3D Vec3D::Normal()
{
    return { this->x / Length(), this->y / Length(), this->z / Length() };
}

void Vec3D::Normalize()
{
    *this = Normal();
}

Vec3D vec::FloatAsVec(float fVector)
{
    return { fVector, fVector, fVector }; 
}

float vec::DotProduct(Vec3D &v1, Vec3D &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3D vec::CrossProduct(Vec3D &v1, Vec3D &v2)
{
    Vec3D v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
    return v;
}

Vec3D vec::Vec3D_IntersectPlane(Vec3D &plane_p, Vec3D &plane_n, Vec3D &lineStart, Vec3D &lineEnd)
{
	plane_n.Normalize();
	float plane_d = -DotProduct(plane_n, plane_p);
	float ad = DotProduct(lineStart, plane_n);
	float bd = DotProduct(lineEnd, plane_n);
	float t = (-plane_d - ad) / (bd - ad);
	Vec3D lineStartToEnd = Vec3D_Sub(lineEnd, lineStart);
	Vec3D lineToIntersect = Vec3D_Mult(lineStartToEnd, FloatAsVec(t));
	return Vec3D_Add(lineStart, lineToIntersect);
}

Vec3D vec::Vec3D_Add(Vec3D v1, Vec3D v2)
{
    return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

Vec3D vec::Vec3D_Sub(Vec3D v1, Vec3D v2)
{
    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

Vec3D vec::Vec3D_Mult(Vec3D v1, Vec3D v2)
{
    return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
}

Vec3D vec::Vec3D_Div(Vec3D v1, Vec3D v2)
{
    return { v1.x / v2.x, v1.y / v2.y, v1.z / v2.z };
}