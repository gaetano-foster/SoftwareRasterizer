#ifndef MESH_H
#define MESH_H

#include "../Maths.h"

struct Mesh
{
    std::vector<Triangle> vecTris;

    bool LoadFromObjectFile(std::string sFileName);
};

struct RasterableTriangle
{
    Triangle tri;
    olc::Pixel pColor;

    void Raster(olc::PixelGameEngine *engine, bool wireframe);
};

int TriangleClipAgainstPlane(Vec3D plane_p, Vec3D plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2);

#endif