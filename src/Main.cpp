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
        vCamera = { 0, 0, 0 };
        matProj.SetProjectionMatrix(0.1f, 1000.0f, 90.0f);
        mMesh = {
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
        fTheta = 0;
		
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
        Clear(olc::BLACK);

        Mat4x4 matRotZ, matRotX;
        fTheta += fElapsedTime;

        matRotZ.Init();
		matRotZ.m[0][0] = cosf(fTheta);
		matRotZ.m[0][1] = sinf(fTheta);
		matRotZ.m[1][0] = -sinf(fTheta);
		matRotZ.m[1][1] = cosf(fTheta);
		matRotZ.m[2][2] = 1;
		matRotZ.m[3][3] = 1;

        matRotX.Init();
		matRotX.m[0][0] = 1;
		matRotX.m[1][1] = cosf(fTheta * 0.5f); // different speed to avoid gimbal lock
		matRotX.m[1][2] = sinf(fTheta * 0.5f);
		matRotX.m[2][1] = -sinf(fTheta * 0.5f);
		matRotX.m[2][2] = cosf(fTheta * 0.5f);
		matRotX.m[3][3] = 1;

        for (auto tri : mMesh)
        {
            Triangle triProj, triTrans, triRotZ, triRotZX;

            // Rotate in Z-Axis
            for (int n = 0; n < 3; n++)
			    MultiplyMatrixVector(&triRotZ.p[n], tri.p[n], matRotZ);

			// Rotate in X-Axis
            for (int n = 0; n < 3; n++)
			    MultiplyMatrixVector(&triRotZX.p[n], triRotZ.p[n], matRotX);

            triTrans = triRotZX;
            for (int n = 0; n < 3; n++) // translate the triangle
                triTrans.p[n].z = triRotZX.p[n].z + 3.0f;

            /* Use Cross-Product to Get Surface Normal */
			Vec3D vNormal, line1, line2;
			line1.x = triTrans.p[1].x - triTrans.p[0].x;
			line1.y = triTrans.p[1].y - triTrans.p[0].y;
			line1.z = triTrans.p[1].z - triTrans.p[0].z;

			line2.x = triTrans.p[2].x - triTrans.p[0].x;
			line2.y = triTrans.p[2].y - triTrans.p[0].y;
		    line2.z = triTrans.p[2].z - triTrans.p[0].z;

		    vNormal.x = line1.y * line2.z - line1.z * line2.y;
		    vNormal.y = line1.z * line2.x - line1.x * line2.z;
		    vNormal.z = line1.x * line2.y - line1.y * line2.x;

            // it's normally normal to normalize a normal
		    float l = sqrtf(vNormal.x * vNormal.x + vNormal.y * vNormal.y + vNormal.z * vNormal.z);
		    vNormal.x /= l;
            vNormal.y /= l; 
            vNormal.z /= l;
            
            if  (vNormal.x * (triTrans.p[0].x - vCamera.x) +
                 vNormal.y * (triTrans.p[0].y - vCamera.y) +                                        
                 vNormal.z * (triTrans.p[0].z - vCamera.z) < 0) 
            {
                triProj = triRotZX;
                for (int n = 0; n < 3; n++) // apply perspective/projection to triangle
                    MultiplyMatrixVector(&triProj.p[n], triTrans.p[n], matProj);

                /* Scale mesh into view */
                triProj.p[0].x += 1.0f; 
                triProj.p[0].y += 1.0f;
			    triProj.p[1].x += 1.0f;
                triProj.p[1].y += 1.0f;
			    triProj.p[2].x += 1.0f; 
                triProj.p[2].y += 1.0f;
		        triProj.p[0].x *= 0.5f * (float)SCREEN_WIDTH;
			    triProj.p[0].y *= 0.5f * (float)SCREEN_HEIGHT;
		        triProj.p[1].x *= 0.5f * (float)SCREEN_WIDTH;
		        triProj.p[1].y *= 0.5f * (float)SCREEN_HEIGHT;
		        triProj.p[2].x *= 0.5f * (float)SCREEN_WIDTH;
		        triProj.p[2].y *= 0.5f * (float)SCREEN_HEIGHT;

                /* Illumination */
                Vec3D vLightDir = (Vec3D) { 0.0f, 0.0f, -1.0f }; // spaghetti
                float ld = sqrtf(vLightDir.x * vLightDir.x + vLightDir.y * vLightDir.y + vLightDir.z * vLightDir.z);
			    vLightDir.x /= ld; 
                vLightDir.y /= ld; 
                vLightDir.z /= ld;

			    // How similar is normal to light direction?
			    float dp = vNormal.x * vLightDir.x + vNormal.y * vLightDir.y + vNormal.z * vLightDir.z;

                FillTriangle(triProj.p[0].x, triProj.p[0].y,
                             triProj.p[1].x, triProj.p[1].y, 
                             triProj.p[2].x, triProj.p[2].y,
                             olc::Pixel(dp * 255, dp * 255, dp * 255, 255));

                DrawTriangle(triProj.p[0].x, triProj.p[0].y,
                             triProj.p[1].x, triProj.p[1].y, 
                             triProj.p[2].x, triProj.p[2].y,
                             olc::BLACK);
            }
        }

		return true;
	}

private:
    Mat4x4 matProj;
    Vec3D vCamera;
    Mesh mMesh;
    float fTheta;
};


int main(int argc, char **argv)
{
	Rasterizer demo;
	if (demo.Construct(SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1))
		demo.Start();

	return 0;
}