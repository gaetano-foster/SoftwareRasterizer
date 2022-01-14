#include "../Engine.h"

class Rasterizer : public olc::PixelGameEngine
{
public:
	Rasterizer();

public:
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;

private:
	Mat4x4 matProj;
    Camera cCamera;
    Entity eCube;
};