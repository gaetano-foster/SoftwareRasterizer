#include "Rasterizer.h"

Rasterizer::Rasterizer()  {
	sAppName = "SoftwareRasterizer";
	olc_hideCursor = true;
}

bool Rasterizer::OnUserCreate() {
	depth_buffer = new float[ScreenWidth() * ScreenHeight()];
    matProj.MakeProjection(0.1f, 1000.0f, camera.fov);
	if (!mesh.mesh.LoadFromObjectFile("res/castle.obj", true))
		return false;
	mesh.z = 9;

	mesh.mesh.texture = new olc::Sprite("res/man.png");
	return true;
}

bool Rasterizer::OnUserUpdate(float fElapsedTime) {
	if (GetKey(olc::W).bHeld) {
		camera.z += cosf(camera.roty) * 8 * fElapsedTime;
		camera.x += sinf(camera.roty) * 8 * fElapsedTime;
	}

	if (GetKey(olc::S).bHeld) {
		camera.z -= cosf(camera.roty) * 8 * fElapsedTime;
		camera.x -= sinf(camera.roty) * 8 * fElapsedTime;
	}

	if (GetKey(olc::A).bHeld) {
		camera.z -= sinf(camera.roty) * 8 * fElapsedTime;
		camera.x += cosf(camera.roty) * 8 * fElapsedTime;
	}

	if (GetKey(olc::D).bHeld) {
		camera.z += sinf(camera.roty) * 8 * fElapsedTime;
		camera.x -= cosf(camera.roty) * 8 * fElapsedTime;
	}

	camera.y -= (GetKey(olc::SHIFT).bHeld) * 8 * fElapsedTime;
	camera.y += (GetKey(olc::SPACE).bHeld) * 8 * fElapsedTime;

	camera.roty -= (GetKey(olc::RIGHT).bHeld) * 2 * fElapsedTime;
	camera.roty += (GetKey(olc:: LEFT).bHeld) * 2 * fElapsedTime;
	camera.rotx -= (GetKey(olc::   UP).bHeld) * 2 * fElapsedTime;
	camera.rotx += (GetKey(olc:: DOWN).bHeld) * 2 * fElapsedTime;
	
	if (camera.rotx > 1.5707f) 
		camera.rotx = 1.5707f; 
	else if (camera.rotx < -1.5707f)
		camera.rotx = -1.5707f;

    Clear(olc::BLACK);

	for (int i = 0; i < ScreenWidth()*ScreenHeight(); i++)
		depth_buffer[i] = 0;

	camera.Update();
	mesh.Update();
	mesh.Render(camera, matProj, this, depth_buffer);
	return true;
}