#include "Rasterizer.h"

Rasterizer::Rasterizer() 
{
	sAppName = "SoftwareRasterizer";
}

bool Rasterizer::OnUserCreate()
{
    matProj.SetProjectionMatrix(0.1f, 1000.0f, cCamera.fFov);
    eCube.mesh = {
	    // SOUTH
	    { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
	    { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },
	    // EAST                                                      
	    { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
	    { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },
	    // NORTH                                                     
	    { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
	    { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },
	    // WEST                                                      
	    { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
	    { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },
	    // TOP                                                       
	    { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
	    { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },
	    // BOTTOM                                                    
	    { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
	    { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },
    };
	eCube.z = 6;
	
	return true;
}

bool Rasterizer::OnUserUpdate(float fElapsedTime)
{
	eCube.yRot += (GetKey(olc::D).bHeld) * fElapsedTime;
	eCube.yRot -= (GetKey(olc::A).bHeld) * fElapsedTime;
	eCube.xRot += (GetKey(olc::W).bHeld) * fElapsedTime;
	eCube.xRot -= (GetKey(olc::S).bHeld) * fElapsedTime;

    Clear(olc::BLACK);
	cCamera.Update();
	eCube.Update();
	eCube.Render(cCamera, matProj, this);
	return true;
}