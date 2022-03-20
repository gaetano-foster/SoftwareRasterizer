#ifndef DYNAMICS_H
#define DYNAMICS_H

#include "../math/mesh/Mesh.h"

class Dynamic {
public:
    float x = 0, y = 0, z = 1; 
    float rotx = 0, roty = 0, rotz = 0;

public:
    virtual void Update() = 0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////

class Camera : public Dynamic {
public:
    float fov = 90.0f;
    Vec3D look_dir = { 0, 0, 1 };
    Mat4x4 matRot;
    Mat4x4 matView; 

public:
    void Update() override;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

class Entity : public Dynamic {
public:
    Mesh mesh;
    bool wireframe = false;
    bool offset = true;

public:
    void Update() override;
    void Render(Camera camera, Mat4x4 matProj, olc::PixelGameEngine *engine, float *depth_buffer);

private:
    Mat4x4 m_matWorld;
};

#endif