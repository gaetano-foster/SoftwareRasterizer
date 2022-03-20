#include "Maths.h"

void Mat4x4::MakeIdentity() {
    for (int r = 0; r < 4; r++) 
        for (int c = 0; c < 4; c++) 
            this->m[c][r] = ( r == c ? 1.0f : 0.0f);
}

void Mat4x4::MakeProjection(float near, float far, float fov) {
    float fov_rad = 1.0f / tanf(fov * 0.5f / 180.0f * 3.14159f);

    this->MakeIdentity();
    this->m[0][0] = ASPECT_RATIO * fov_rad;
    this->m[1][1] = fov_rad;
    this->m[2][2] = far / (far - near);
    this->m[3][2] = (-far * near) / (far - near);
    this->m[2][3] = 1.0f;
    this->m[3][3] = 0.0f;
}

void Mat4x4::MakeRotation(float rotx, float roty, float rotz) {
    MakeIdentity();

    m[0][0] = cosf(rotz) * cosf(roty);
    m[1][0] = cosf(rotz) * sinf(roty) * sinf(rotx) - sinf(rotz) * cosf(rotx); // row 1
    m[2][0] = cosf(rotz) * sinf(roty) * cosf(rotx) + sinf(rotz) * sinf(rotx);

    m[0][1] = sinf(rotz) * cosf(roty);
    m[1][1] = sinf(rotz) * sinf(roty) * sinf(rotx) + cosf(rotz) * cosf(rotx); // row 2
    m[2][1] = sinf(rotz) * sinf(roty) * cosf(rotx) - cosf(rotz) * sinf(rotx);

    m[0][2] = -sinf(roty);
    m[1][2] = cosf(roty) * sinf(rotx);                                        // row 3
    m[2][2] = cosf(roty) * cosf(rotx);
}

void Mat4x4::MakeTranslation(float x, float y, float z) {
    MakeIdentity();
    m[3][0] = x;
    m[3][1] = y;
    m[3][2] = z;
}

Mat4x4 MultiplyMatrices(Mat4x4 &m1, Mat4x4 &m2) {
	Mat4x4 matrix;
	for (int c = 0; c < 4; c++)
		for (int r = 0; r < 4; r++)
			matrix.m[r][c] = m1.m[r][0] * m2.m[0][c] + m1.m[r][1] * m2.m[1][c] + m1.m[r][2] * m2.m[2][c] + m1.m[r][3] * m2.m[3][c];
	return matrix;
}

void MatrixMultiplyVector(Vec3D *o, Vec3D i, Mat4x4 m) {
	o->x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + i.w * m.m[3][0];
	o->y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + i.w * m.m[3][1];
	o->z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + i.w * m.m[3][2];
	o->w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + i.w * m.m[3][3];
}

Mat4x4 MatrixPointAt(Vec3D pos, Vec3D target, Vec3D up) {
    // new forward direction
    Vec3D new_forward = Vec3D_Sub(target, pos);
    new_forward.normalize(); 

    // new up direction
    Vec3D a = Vec3D_Mult(new_forward, FloatAsVec(DotProduct(up, new_forward)));
    Vec3D new_up = Vec3D_Sub(up, a);
    new_up.normalize();

    // new right direction
    Vec3D new_right = CrossProduct(new_up, new_forward);

	// Construct Dimensioning and Translation Matrix	
	Mat4x4 matrix;
	matrix.m[0][0] = new_right.x;	matrix.m[0][1] = new_right.y;	matrix.m[0][2] = new_right.z;	matrix.m[0][3] = 0.0f;
	matrix.m[1][0] = new_up.x;		matrix.m[1][1] = new_up.y;		matrix.m[1][2] = new_up.z;		matrix.m[1][3] = 0.0f;
	matrix.m[2][0] = new_forward.x;	matrix.m[2][1] = new_forward.y;	matrix.m[2][2] = new_forward.z;	matrix.m[2][3] = 0.0f;
	matrix.m[3][0] = pos.x;			matrix.m[3][1] = pos.y;			matrix.m[3][2] = pos.z;			matrix.m[3][3] = 1.0f;
	return matrix;
}

Mat4x4 MatrixInverse(Mat4x4 m) {
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