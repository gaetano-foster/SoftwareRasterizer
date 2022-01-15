#include "Maths.h"

void Mat4x4::MakeIdentity()
{
    for (int r = 0; r < 4; r++) 
    {
        for (int c = 0; c < 4; c++) 
        {
            this->m[c][r] = ( r == c ? 1.0f : 0.0f);
        }
    }
}

void Mat4x4::MakeProjection(float fNear, float fFar, float fFov)
{
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

    this->MakeIdentity();
    this->m[0][0] = ASPECT_RATIO * fFovRad;
    this->m[1][1] = fFovRad;
    this->m[2][2] = fFar / (fFar - fNear);
    this->m[3][2] = (-fFar * fNear) / (fFar - fNear);
    this->m[2][3] = 1.0f;
    this->m[3][3] = 0.0f;
}

void Mat4x4::MakeRotation(float xRot, float yRot, float zRot)
{
    MakeIdentity();

    m[0][0] = cosf(zRot) * cosf(yRot);
    m[1][0] = cosf(zRot) * sinf(yRot) * sinf(xRot) - sinf(zRot) * cosf(xRot); // row 1
    m[2][0] = cosf(zRot) * sinf(yRot) * cosf(xRot) + sinf(zRot) * sinf(xRot);

    m[0][1] = sinf(zRot) * cosf(yRot);
    m[1][1] = sinf(zRot) * sinf(yRot) * sinf(xRot) + cosf(zRot) * cosf(xRot); // row 2
    m[2][1] = sinf(zRot) * sinf(yRot) * cosf(xRot) - cosf(zRot) * sinf(xRot);

    m[0][2] = -sinf(yRot);
    m[1][2] = cosf(yRot) * sinf(xRot);                                        // row 3
    m[2][2] = cosf(yRot) * cosf(xRot);
}

void Mat4x4::MakeTranslation(float x, float y, float z)
{
    MakeIdentity();
    m[3][0] = x;
    m[3][1] = y;
    m[3][2] = z;
}

Mat4x4 MultiplyMatrices(Mat4x4 &m1, Mat4x4 &m2)
{
	Mat4x4 matrix;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
	return matrix;
}

float Vec3D::Length() const
{
    return sqrtf(x * x + y * y + z * z);
}

Vec3D Vec3D::Normal()
{
    return { this->x / Length(), this->y / Length(), this->z / Length() };
}

void MatrixMultiplyVector(Vec3D *o, Vec3D i, Mat4x4 m)
{
	o->x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
	o->y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
	o->z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
	o->w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
}

float DotProduct(Vec3D &v1, Vec3D &v2)
{
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

Vec3D CrossProduct(Vec3D &v1, Vec3D &v2)
{
    Vec3D v;
	v.x = v1.y * v2.z - v1.z * v2.y;
	v.y = v1.z * v2.x - v1.x * v2.z;
	v.z = v1.x * v2.y - v1.y * v2.x;
    return v;
}

Vec3D Vec3D_Add(Vec3D v1, Vec3D v2)
{
    return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z };
}

Vec3D Vec3D_Sub(Vec3D v1, Vec3D v2)
{
    return { v1.x - v2.x, v1.y - v2.y, v1.z - v2.z };
}

Vec3D Vec3D_Mult(Vec3D v1, Vec3D v2)
{
    return { v1.x * v2.x, v1.y * v2.y, v1.z * v2.z };
}

Vec3D Vec3D_Div(Vec3D v1, Vec3D v2)
{
    return { v1.x / v2.x, v1.y / v2.y, v1.z / v2.z };
}