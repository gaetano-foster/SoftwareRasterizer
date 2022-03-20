#ifndef MESH_H
#define MESH_H

#include "../Maths.h"

struct Mesh {
    std::vector<Triangle> tris;
    olc::Sprite *texture;

    bool LoadFromObjectFile(std::string file_name, bool has_texture = false);
    void Render(Vec3D vCamera, Mat4x4 matProj, olc::PixelGameEngine *engine, float *pDepthBuffer, Mat4x4 matWorld, Mat4x4 matView, bool wireframe = false);
};

struct RasterableTriangle {
    Triangle tri;
    float brightness;

    void Raster(olc::PixelGameEngine *engine, olc::Sprite *texture, float *depth_buffer, bool wireframe = false);
};

int TriangleClipAgainstPlane(Vec3D plane_p, Vec3D plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2);
void ShadeTriangle(int x1, int y1, float u1, float v1, float w1,
				   int x2, int y2, float u2, float v2, float w2,
				   int x3, int y3, float u3, float v3, float w3,
				   olc::Sprite *tex, olc::PixelGameEngine *engine,
                   float *depth_buffer, float brightness);

#endif