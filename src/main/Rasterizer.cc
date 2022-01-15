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
	
	return true;
}

bool Rasterizer::OnUserUpdate(float fElapsedTime)
{
	eMesh.yRot += (GetKey(olc::D).bHeld) * fElapsedTime;
	eMesh.yRot -= (GetKey(olc::A).bHeld) * fElapsedTime;
	eMesh.xRot += (GetKey(olc::W).bHeld) * fElapsedTime;
	eMesh.xRot -= (GetKey(olc::S).bHeld) * fElapsedTime;

    Clear(olc::BLACK);
	cCamera.Update();
	eMesh.Update();
	eMesh.Render(cCamera, matProj, this);
	return true;
}