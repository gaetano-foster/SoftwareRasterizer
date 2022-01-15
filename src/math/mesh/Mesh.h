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

    void Raster(olc::PixelGameEngine *engine);
};

#endif