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
	olc_SetMousePos(ScreenWidth() / 2, ScreenHeight() / 2);
	bRegisterMouse = true;
	
	return true;
}

bool Rasterizer::OnUserUpdate(float fElapsedTime)
{
	int nTempMouseX, nTempMouseY;
	olc_hideCursor = true;
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

	cCamera.y -= (GetKey(olc::SHIFT).bHeld) * 8 * fElapsedTime;
	cCamera.y += (GetKey(olc::SPACE).bHeld) * 8 * fElapsedTime;

	nMouseX = GetMouseX();
	nMouseY = GetMouseY();
	nTempMouseX = nMouseX - nLastMouseX;
	nTempMouseY = nMouseY - nLastMouseY;
	nLastMouseX = nMouseX;
	nLastMouseY = nMouseY;

	if (nTempMouseX != 0 || nTempMouseY != 0)
	{
		if (bRegisterMouse)
		{
			cCamera.yRot += ((nTempMouseX != 0) ? (nTempMouseX) : 0) * 2 * fElapsedTime;
			cCamera.xRot -= ((nTempMouseY != 0) ? (nTempMouseY) : 0) * 2 * fElapsedTime;
			bRegisterMouse = false;
		}
		else
		{
			bRegisterMouse = true;
		}

		olc_SetMousePos(ScreenWidth() / 2, ScreenHeight() / 2);
	}

    Clear(olc::BLACK);
	cCamera.Update();
	eMesh.Update();
	eMesh.Render(cCamera, matProj, this);
	return true;
}