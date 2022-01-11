#include "Engine.h"

void Mat4x4::Init()
{
    for (int r = 0; r < 4; r++) 
    {
        for (int c = 0; c < 4; c++) 
        {
            this->m[c][r] = ( r == c ? 1.0f : 0.0f);
        }
    }
}

void Mat4x4::SetProjectionMatrix(float near, float far, float fov)
{
    float fov_rad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);

    this->Init();
    this->m[0][0] = ASPECT_RATIO * fov_rad;
    this->m[1][1] = fov_rad;
    this->m[2][2] = far / (far - near);
    this->m[3][2] = (-far * near) / (far - near);
    this->m[2][3] = 1.0f;
    this->m[3][3] = 0.0f;
}

void MultiplyMatrixVector(Vec3D *o, Vec3D i, Mat4x4 m)
{
    o->x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o->y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o->z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f) 
    {
		o->x /= w; 
        o->y /= w; 
        o->z /= w;
    }
}