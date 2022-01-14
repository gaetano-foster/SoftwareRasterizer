#ifndef DYNAMICS_H
#define DYNAMICS_H

class Dynamic
{
public:
    float x = 0, y = 0, z = 1; // to be added to positions in tris
    float xRot = 0, yRot = 0, zRot = 0;

public:
    virtual void Update() = 0;
};

class Camera : public Dynamic
{
public:
    float fFov = 90.0f;

public:
    void Update() override;
};

#endif