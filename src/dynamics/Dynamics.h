#ifndef DYNAMICS_H
#define DYNAMICS_H

#include "../math/mesh/Mesh.h"

class Dynamic
{
public:
    float x = 0, y = 0, z = 1; 
    float xRot = 0, yRot = 0, zRot = 0;

public:
    virtual void Update() = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

class Camera : public Dynamic
{
public:
    float fFov = 90.0f;

public:
    void Update() override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

class Entity : public Dynamic
{
public:
    Mesh mMesh;

public:
    void Update() override;
    void Render(Camera cCamera, Mat4x4 matProj, olc::PixelGameEngine *engine);

private:
    Mat4x4 m_matRotation;
};

#endif