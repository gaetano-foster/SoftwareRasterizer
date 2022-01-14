#define OLC_PGE_APPLICATION
#include "engine/Engine.h"

int main(int argc, char **argv)
{
	Rasterizer demo;
	if (demo.Construct(SCREEN_WIDTH, SCREEN_HEIGHT, 2, 2))
		demo.Start();

	return 0;
}