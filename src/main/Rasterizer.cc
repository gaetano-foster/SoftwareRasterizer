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
	if (!eMesh.mMesh.LoadFromObjectFile("res/castle.obj", true))
		return false;
	eMesh.z = 9;
	olc_SetMousePos(ScreenWidth() / 2, ScreenHeight() / 2);
	bRegisterMouse = true;

	eMesh.mMesh.sprTexture = new olc::Sprite("res/colors.png");
	
	return true;
}

bool Rasterizer::OnUserUpdate(float fElapsedTime)
{
	int nTempMouseX, nTempMouseY;

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
			cCamera.yRot += nTempMouseX * 2 * PX_SIZE * fElapsedTime;
			cCamera.xRot -= nTempMouseY * 2 * PX_SIZE * fElapsedTime;
			if (cCamera.xRot > 1.5707f) 
				cCamera.xRot = 1.5707f; 
			else if (cCamera.xRot < -1.5707f)
				cCamera.xRot = -1.5707f; 
			bRegisterMouse = false;
		}
		else
		{
			bRegisterMouse = true;
		}

		if (!GetKey(olc::ESCAPE).bHeld) {
			olc_SetMousePos(ScreenWidth() / 2, ScreenHeight() / 2);
		}
	}

    Clear(olc::BLACK);

	for (int i = 0; i < ScreenWidth()*ScreenHeight(); i++)
		pDepthBuffer[i] = 0;
		
	cCamera.Update();
	eMesh.Update();
	eMesh.Render(cCamera, matProj, this, pDepthBuffer);
	return true;
}