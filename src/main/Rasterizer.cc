#include "Rasterizer.h"

Rasterizer::Rasterizer() 
{
	sAppName = "SoftwareRasterizer";
}

bool Rasterizer::OnUserCreate()
{
    matProj.MakeProjection(0.1f, 1000.0f, cCamera.fFov);
	if (!eMesh.mMesh.LoadFromObjectFile("res/castle.obj"))
		return false;
	eMesh.z = 9;
	eMesh.zRot = 3.14159;
	
	return true;
}

bool Rasterizer::OnUserUpdate(float fElapsedTime)
{
	eMesh.yRot += fElapsedTime;

	if (GetKey(olc::W).bHeld)
	{
		cCamera.z += (GetKey(olc::W).bHeld) * cosf(cCamera.yRot) * 8 * fElapsedTime;
		cCamera.x += (GetKey(olc::W).bHeld) * sinf(cCamera.yRot) * 8 * fElapsedTime;
	}

	if (GetKey(olc::S).bHeld)
	{
		cCamera.z -= (GetKey(olc::S).bHeld) * cosf(cCamera.yRot) * 8 * fElapsedTime;
		cCamera.x -= (GetKey(olc::S).bHeld) * sinf(cCamera.yRot) * 8 * fElapsedTime;
	}

	cCamera.yRot += (GetKey(olc::D).bHeld) * fElapsedTime;
	cCamera.yRot -= (GetKey(olc::A).bHeld) * fElapsedTime;

	cCamera.y += (GetKey(olc::SHIFT).bHeld) * 8 * fElapsedTime;
	cCamera.y -= (GetKey(olc::SPACE).bHeld) * 8 * fElapsedTime;

    Clear(olc::BLACK);
	cCamera.Update();
	eMesh.Update();
	eMesh.Render(cCamera, matProj, this);
	return true;
}