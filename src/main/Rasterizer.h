#include "../dynamics/Dynamics.h"

class Rasterizer : public olc::PixelGameEngine {
public:
	Rasterizer();

public:
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;

private:
	Mat4x4 matProj;
    Camera camera;
    Entity mesh;
	float *depth_buffer;
};