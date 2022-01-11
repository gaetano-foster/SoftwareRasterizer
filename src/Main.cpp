#define OLC_PGE_APPLICATION
#include "engine/Engine.h"
#include "olcPixelGameEngine.h"

class Rasterizer : public olc::PixelGameEngine
{
public:
	Rasterizer() 
    {
		sAppName = "SoftwareRasterizer";
	}

public:
	bool OnUserCreate() override
	{
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		return true;
	}
};


int main(int argc, char **argv)
{
	Rasterizer demo;
	if (demo.Construct(SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1))
		demo.Start();

	return 0;
}