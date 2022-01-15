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
    float x, y, z;
};

struct Triangle 
{
    Vec3D p[3];
};

struct Mat4x4 
{
    float m[4][4] = { 0 };
    void Init();
    void SetProjectionMatrix(float near, float far, float fov);
};

struct Mesh
{
    std::vector<Triangle> vecTris;

    bool LoadFromObjectFile(std::string sFileName);
};

void MultiplyMatrixVector(Vec3D *o, Vec3D i, Mat4x4 m);

#endif