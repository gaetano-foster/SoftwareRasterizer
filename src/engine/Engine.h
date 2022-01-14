#ifndef ENGINE_H
#define ENGINE_H

#include <cmath>
#include <vector>
#include "../olcPixelGameEngine.h"

#define SCREEN_WIDTH        640
#define SCREEN_HEIGHT       360
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


class Dynamic
{
public:
    float x = 0, y = 0, z = 1; // to be added to positions in tris
    float xRot = 0, yRot = 0, zRot = 0;
public:
    virtual void Update() = 0;
};

class Entity : public Dynamic
{
public:
    std::vector<Triangle> mesh;
public:
    void Update() override;
    void Render(Vec3D vCamera, Mat4x4 matProj, olc::PixelGameEngine *engine);
private:
    Mat4x4 m_matRotation;
};


class Rasterizer : public olc::PixelGameEngine
{
public:
	Rasterizer();

public:
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;

private:
	Mat4x4 matProj;
    Vec3D vCamera;
    Entity eCube;
};

void MultiplyMatrixVector(Vec3D *o, Vec3D i, Mat4x4 m);

#endif