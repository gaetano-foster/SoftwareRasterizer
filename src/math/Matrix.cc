#include "Maths.h"

using namespace vec;
using namespace mat4;

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

Mat4x4 mat4::MultiplyMatrices(Mat4x4 &m1, Mat4x4 &m2)
{
	Mat4x4 matrix;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
	return matrix;
}

void mat4::MatrixMultiplyVector(Vec3D *o, Vec3D i, Mat4x4 m)
{
	o->x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
	o->y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
	o->z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
	o->w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
}

Mat4x4 mat4::MatrixPointAt(Vec3D pos, Vec3D target, Vec3D up)
{
    // new forward direction
    Vec3D vNewForward = Vec3D_Sub(target, pos);
    vNewForward.Normalize(); 

    // new up direction
    Vec3D a = Vec3D_Mult(vNewForward, FloatAsVec(DotProduct(up, vNewForward)));
    Vec3D vNewUp = Vec3D_Sub(up, a);
    vNewUp.Normalize();

    // new right direction
    Vec3D vNewRight = CrossProduct(vNewUp, vNewForward);

	// Construct Dimensioning and Translation Matrix	
	Mat4x4 matrix;
	matrix.m[0][0] = vNewRight.x;	matrix.m[0][1] = vNewRight.y;	matrix.m[0][2] = vNewRight.z;	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = vNewUp.x;		matrix.m[1][1] = vNewUp.y;		matrix.m[1][2] = vNewUp.z;		matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = vNewForward.x;	matrix.m[2][1] = vNewForward.y;	matrix.m[2][2] = vNewForward.z;	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
	return matrix;
}

Mat4x4 mat4::MatrixInverse(Mat4x4 m)
{
	Mat4x4 matrix;
	matrix.m[0][0] = m.m[0][0]; matrix.m[0][1] = m.m[1][0]; matrix.m[0][2] = m.m[2][0]; matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = m.m[0][1]; matrix.m[1][1] = m.m[1][1]; matrix.m[1][2] = m.m[2][1]; matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = m.m[0][2]; matrix.m[2][1] = m.m[1][2]; matrix.m[2][2] = m.m[2][2]; matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = -(m.m[3][0] * matrix.m[0][0] + m.m[3][1] * matrix.m[1][0] + m.m[3][2] * matrix.m[2][0]);
	matrix.m[3][1] = -(m.m[3][0] * matrix.m[0][1] + m.m[3][1] * matrix.m[1][1] + m.m[3][2] * matrix.m[2][1]);
	matrix.m[3][2] = -(m.m[3][0] * matrix.m[0][2] + m.m[3][1] * matrix.m[1][2] + m.m[3][2] * matrix.m[2][2]);
	matrix.m[3][3] = 1.0f;
	return matrix;
}