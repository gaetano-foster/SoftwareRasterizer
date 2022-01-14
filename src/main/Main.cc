#define OLC_PGE_APPLICATION
#include "Rasterizer.h"

int main(int argc, char **argv)
{
	Rasterizer demo;
	if (demo.Construct(SCREEN_WIDTH, SCREEN_HEIGHT, PX_SIZE, PX_SIZE))
		demo.Start();

	return 0;
}