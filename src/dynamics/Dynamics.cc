#include <algorithm>
#include <list>
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
    Vec3D vCamera = { cCamera.x, cCamera.y, cCamera.z };
    Mat4x4 matView = cCamera.matView;

    // get triangles to be rastered
    for (auto tri : mMesh.vecTris)
    {
        Triangle triProj, triTrans, triViewed;

        for (int n = 0; n < 3; n++)
		    MatrixMultiplyVector(&triTrans.p[n], tri.p[n], m_matWorld);

        // Use Cross-Product to Get Surface Normal 
		Vec3D vNormal, line1, line2;
		line1 = Vec3D_Sub(triTrans.p[1], triTrans.p[0]);
        line2 = Vec3D_Sub(triTrans.p[2], triTrans.p[0]);

	    vNormal = CrossProduct(line1, line2);
	    vNormal.Normalize(); // it's normally normal to normalize a normal
        Vec3D vCameraRay = Vec3D_Sub(triTrans.p[0], vCamera);
        
        if  (DotProduct(vNormal, vCameraRay) < 0) 
        {
            // Illumination 
            Vec3D vLightDir = (Vec3D) { 0.5f, 0.0f, -1.0f }; 
            vLightDir.Normalize();

            // How similar is normal to light direction?
		    float fLightingVal = (DotProduct(vLightDir, vNormal)) * 255; // we multiply by 255 for illumination
            fLightingVal = (fLightingVal > 255) ? fLightingVal = 255 : fLightingVal = (fLightingVal < 10) ? fLightingVal = 10 : fLightingVal = fLightingVal;
            olc::Pixel pColor = olc::Pixel(fLightingVal, fLightingVal, fLightingVal, 255);

            triViewed = triTrans;
            for (int n = 0; n < 3; n++) // world space -> view space
                MatrixMultiplyVector(&triViewed.p[n], triTrans.p[n], matView);

            int nClippedTriangles = 0;
            Triangle clipped[2];
            nClippedTriangles = TriangleClipAgainstPlane({ 0, 0, 0.1f }, { 0, 0, 1 }, triViewed, clipped[0], clipped[1]);

            for (int i = 0; i < nClippedTriangles; i++)
            {
                triProj = triViewed;
                for (int n = 0; n < 3; n++) // 3D -> 2D
                    MatrixMultiplyVector(&triProj.p[n], clipped[i].p[n], matProj);

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

                vecTrianglesToRaster.push_back({ triProj, pColor });
            }
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
    for (auto &triToRaster : vecTrianglesToRaster)
    {
		// Clip triangles against all four screen edges, this could yield
		// a bunch of triangles, so create a queue that we traverse to 
		//  ensure we only test new triangles generated against planes
		Triangle clipped[2];
		std::list<Triangle> listTriangles;

		// Add initial triangle
		listTriangles.push_back(triToRaster.tri);
		int nNewTriangles = 1;

		for (int p = 0; p < 4; p++)
		{
			int nTrisToAdd = 0;
			while (nNewTriangles > 0)
			{
				// Take triangle from front of queue
				Triangle test = listTriangles.front();
				listTriangles.pop_front();
				nNewTriangles--;

				// Clip it against a plane. We only need to test each 
				// subsequent plane, against subsequent new triangles
				// as all triangles after a plane clip are guaranteed
				// to lie on the inside of the plane. I like how this
				// comment is almost completely and utterly justified
				switch (p)
				{
				case 0:	nTrisToAdd = TriangleClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 1:	nTrisToAdd = TriangleClipAgainstPlane({ 0.0f, (float)SCREEN_HEIGHT - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 2:	nTrisToAdd = TriangleClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 3:	nTrisToAdd = TriangleClipAgainstPlane({ (float)SCREEN_WIDTH - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				}

				// Clipping may yield a variable number of triangles, so
				// add these new ones to the back of the queue for subsequent
				// clipping against next planes
				for (int w = 0; w < nTrisToAdd; w++)
					listTriangles.push_back(clipped[w]);
			}
			nNewTriangles = listTriangles.size();
		}

		// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
		for (auto &t : listTriangles)
		{
            RasterableTriangle tri = { t, triToRaster.pColor };
			tri.Raster(engine, false);
		}
    }
}

void Camera::Update()
{
    Vec3D vCamera = { x, y, z };

    Vec3D vUp = { 0, 1, 0 };
    Vec3D vTarget = { 0, 0, 1 }; 
    matRot.MakeRotation(xRot, yRot, zRot);
    MatrixMultiplyVector(&vLookDir, vTarget, matRot);
    vTarget = Vec3D_Add(vCamera, vLookDir);

    matView = MatrixInverse(MatrixPointAt(vCamera, vTarget, vUp));
}