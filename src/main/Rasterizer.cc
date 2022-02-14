#include "Rasterizer.h"

Rasterizer::Rasterizer() 
{
	sAppName = "SoftwareRasterizer";
	olc_hideCursor = true;
}

bool Rasterizer::OnUserCreate()
{
	pDepthBuffer = new float[ScreenWidth() * ScreenHeight()];
    matProj.MakeProjection(0.1f, 1000.0f, cCamera.fFov);
	if (!eMesh.mMesh.LoadFromObjectFile("res/man.obj", true))
		return false;
	eMesh.z = 9;

	eMesh.mMesh.sprTexture = new olc::Sprite("res/man.png");
	
	return true;
}

bool Rasterizer::OnUserUpdate(float fElapsedTime)
{
	if (GetKey(olc::W).bHeld)
	{
		cCamera.z += cosf(cCamera.yRot) * 8 * fElapsedTime;
		cCamera.x += sinf(cCamera.yRot) * 8 * fElapsedTime;
	}

	if (GetKey(olc::S).bHeld)
	{
		cCamera.z -= cosf(cCamera.yRot) * 8 * fElapsedTime;
		cCamera.x -= sinf(cCamera.yRot) * 8 * fElapsedTime;
	}

	if (GetKey(olc::A).bHeld)
	{
		cCamera.z -= sinf(cCamera.yRot) * 8 * fElapsedTime;
		cCamera.x += cosf(cCamera.yRot) * 8 * fElapsedTime;
	}

	if (GetKey(olc::D).bHeld)
	{
		cCamera.z += sinf(cCamera.yRot) * 8 * fElapsedTime;
		cCamera.x -= cosf(cCamera.yRot) * 8 * fElapsedTime;
	}

	cCamera.y -= (GetKey(olc::SHIFT).bHeld) * 8 * fElapsedTime;
	cCamera.y += (GetKey(olc::SPACE).bHeld) * 8 * fElapsedTime;

	cCamera.yRot += (GetKey(olc::RIGHT).bHeld) * 2 * fElapsedTime;
	cCamera.yRot -= (GetKey(olc:: LEFT).bHeld) * 2 * fElapsedTime;
	cCamera.xRot -= (GetKey(olc::   UP).bHeld) * 2 * fElapsedTime;
	cCamera.xRot += (GetKey(olc:: DOWN).bHeld) * 2 * fElapsedTime;
	
	if (cCamera.xRot > 1.5707f) 
		cCamera.xRot = 1.5707f; 
	else if (cCamera.xRot < -1.5707f)
		cCamera.xRot = -1.5707f;

    Clear(olc::BLACK);

	for (int i = 0; i < ScreenWidth()*ScreenHeight(); i++)
		pDepthBuffer[i] = 0;

	cCamera.Update();
	eMesh.Update();
	eMesh.Render(cCamera, matProj, this, pDepthBuffer);
	return true;
}