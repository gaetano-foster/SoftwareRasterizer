#ifndef MATHS_H
#define MATHS_H

#include <cmath>
#include <vector>
#include <string>
#include <fstream>
#include <strstream>
#include "../Definitions.h"

struct Vec3D 
{
    float x, y, z, w;

    Vec3D();
    Vec3D(float a, float b, float c);

    float Length() const;
    Vec3D Normal();
};

struct Triangle 
{
    Vec3D p[3];
};

struct Mat4x4 
{
    float m[4][4] = { 0 };
    void MakeIdentity();
    void MakeProjection(float near, float far, float fov);
    void MakeRotation(float xRot, float yRot, float zRot);
    void MakeTranslation(float x, float y, float z);
};

void MatrixMultiplyVector(Vec3D *o, Vec3D i, Mat4x4 m);
float DotProduct(Vec3D &v1, Vec3D &v2);
Vec3D CrossProduct(Vec3D &v1, Vec3D &v2);
Mat4x4 MultiplyMatrices(mat4x4 &m1, mat4x4 &m2);

#endif