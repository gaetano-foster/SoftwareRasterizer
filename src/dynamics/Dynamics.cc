#include <algorithm>
#include "Dynamics.h"

using namespace vec;
using namespace mat4;

void Entity::Update()
{
    Mat4x4 matTrans, matRot;
    m_matWorld.MakeIdentity();
    matTrans.MakeTranslation(x, y, z);
    matRot.MakeRotation(xRot, yRot, zRot);

    m_matWorld = MultiplyMatrices(matRot, matTrans);
}

void Entity::Render(Camera cCamera, Mat4x4 matProj, olc::PixelGameEngine *engine)
{
    std::vector<RasterableTriangle> vecTrianglesToRaster;

    // get triangles to be rastered
    for (auto tri : mMesh.vecTris)
    {
        Triangle triProj, triTrans;

        for (int n = 0; n < 3; n++)
		    MatrixMultiplyVector(&triTrans.p[n], tri.p[n], m_matWorld);

        // Use Cross-Product to Get Surface Normal 
		Vec3D vNormal, line1, line2;
		line1 = Vec3D_Sub(triTrans.p[1], triTrans.p[0]);
        line2 = Vec3D_Sub(triTrans.p[2], triTrans.p[0]);

	    vNormal = CrossProduct(line1, line2);
	    vNormal.Normalize(); // it's normally normal to normalize a normal
        Vec3D vCameraRay = Vec3D_Sub(triTrans.p[0], { cCamera.x, cCamera.y, cCamera.z });
        
        if  (DotProduct(vNormal, vCameraRay) < 0) 
        {
            triProj = triTrans;
            for (int n = 0; n < 3; n++) // apply perspective/projection to triangle
                MatrixMultiplyVector(&triProj.p[n], triTrans.p[n], matProj);
            
            for (int n = 0; n < 3; n++)
                triProj.p[n] = Vec3D_Div(triProj.p[n], FloatAsVec(triProj.p[n].w));

            // Scale mesh into view             
	        triProj.p[0].x *= 0.5f * (float)SCREEN_WIDTH;
		    triProj.p[0].y *= 0.5f * (float)SCREEN_HEIGHT;
	        triProj.p[1].x *= 0.5f * (float)SCREEN_WIDTH;
	        triProj.p[1].y *= 0.5f * (float)SCREEN_HEIGHT;
	        triProj.p[2].x *= 0.5f * (float)SCREEN_WIDTH;
	        triProj.p[2].y *= 0.5f * (float)SCREEN_HEIGHT;

            triProj.p[0].x += SCREEN_WIDTH / 2; 
            triProj.p[0].y += SCREEN_HEIGHT / 2;
		    triProj.p[1].x += SCREEN_WIDTH / 2;
            triProj.p[1].y += SCREEN_HEIGHT / 2;
		    triProj.p[2].x += SCREEN_WIDTH / 2; 
            triProj.p[2].y += SCREEN_HEIGHT / 2;

            // Illumination 
            Vec3D vLightDir = (Vec3D) { 0.5f, 0.0f, -1.0f }; 
            vLightDir.Normalize();

		    // How similar is normal to light direction?
		    float fLightingVal = (DotProduct(vLightDir, vNormal)) * 255; // we multiply by 255 for illumination
            fLightingVal = (fLightingVal > 255) ? fLightingVal = 255 : fLightingVal = (fLightingVal < 20) ? fLightingVal = 20 : fLightingVal = fLightingVal;

            vecTrianglesToRaster.push_back({ triProj, olc::Pixel(fLightingVal, fLightingVal, fLightingVal, 255) });
        }
    }

    // sort triangles back to front
    sort(vecTrianglesToRaster.begin(), vecTrianglesToRaster.end(), [](RasterableTriangle &t1, RasterableTriangle &t2) 
    {
        float z1 = (t1.tri.p[0].z + t1.tri.p[1].z + t1.tri.p[2].z) / 3.0f;
        float z2 = (t2.tri.p[0].z + t2.tri.p[1].z + t2.tri.p[2].z) / 3.0f;
        return z1 > z2;
    });

    // raster triangles
    for (auto &triangle : vecTrianglesToRaster)
    {
        triangle.Raster(engine);
    }
}

void Camera::Update()
{

}