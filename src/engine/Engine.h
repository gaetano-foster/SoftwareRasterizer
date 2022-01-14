#ifndef ENGINE_H
#define ENGINE_H

#include <cmath>
#include <vector>
#include "../olcPixelGameEngine.h"
#include "Dynamics.h"

#define SCREEN_WIDTH        256
#define SCREEN_HEIGHT       240
#define ASPECT_RATIO        (float)(SCREEN_HEIGHT / (float)SCREEN_WIDTH)
#define SWAP(x,y) do { (x)=(x)^(y); (y)=(x)^(y); (x)=(x)^(y); } while(0)

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

class Entity : public Dynamic
{
public:
    std::vector<Triangle> mesh;

public:
    void Update() override;
    void Render(Camera cCamera, Mat4x4 matProj, olc::PixelGameEngine *engine);

private:
    Mat4x4 m_matRotation;
};

void MultiplyMatrixVector(Vec3D *o, Vec3D i, Mat4x4 m);

#endif