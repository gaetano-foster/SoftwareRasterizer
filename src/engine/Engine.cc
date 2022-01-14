#include "Engine.h"

void Mat4x4::Init()
{
    for (int r = 0; r < 4; r++) 
    {
        for (int c = 0; c < 4; c++) 
        {
            this->m[c][r] = ( r == c ? 1.0f : 0.0f);
        }
    }
}

void Mat4x4::SetProjectionMatrix(float fNear, float fFar, float fFov)
{
    float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

    this->Init();
    this->m[0][0] = ASPECT_RATIO * fFovRad;
    this->m[1][1] = fFovRad;
    this->m[2][2] = fFar / (fFar - fNear);
    this->m[3][2] = (-fFar * fNear) / (fFar - fNear);
    this->m[2][3] = 1.0f;
    this->m[3][3] = 0.0f;
}

void MultiplyMatrixVector(Vec3D *o, Vec3D i, Mat4x4 m)
{
    o->x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
	o->y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
	o->z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
	float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

	if (w != 0.0f) 
    {
		o->x /= w; 
        o->y /= w; 
        o->z /= w;
    }
}

void Entity::Update()
{
    m_matRotation.Init();
    
    m_matRotation.m[0][0] = cosf(zRot) * cosf(yRot);
    m_matRotation.m[1][0] = cosf(zRot) * sinf(yRot) * sinf(xRot) - sinf(zRot) * cosf(xRot); // row 1
    m_matRotation.m[2][0] = cosf(zRot) * sinf(yRot) * cosf(xRot) + sinf(zRot) * sinf(xRot);
    m_matRotation.m[0][1] = sinf(zRot) * cosf(yRot);
    m_matRotation.m[1][1] = sinf(zRot) * sinf(yRot) * sinf(xRot) + cosf(zRot) * cosf(xRot); // row 2
    m_matRotation.m[2][1] = sinf(zRot) * sinf(yRot) * cosf(xRot) - cosf(zRot) * sinf(xRot);
    m_matRotation.m[0][2] = -sinf(yRot);
    m_matRotation.m[1][2] = cosf(yRot) * sinf(xRot); // row 3
    m_matRotation.m[2][2] = cosf(yRot) * cosf(xRot);
}

void Entity::Render(Camera cCamera, Mat4x4 matProj, olc::PixelGameEngine *engine)
{
    for (auto tri : mesh)
    {
        Triangle triProj, triTrans, triRot;

        for (int n = 0; n < 3; n++)
		    MultiplyMatrixVector(&triRot.p[n], tri.p[n], m_matRotation);

        triTrans = triRot;
        for (int n = 0; n < 3; n++)// translate the triangle
        {
            triTrans.p[n].x = triRot.p[n].x + x - cCamera.x;
            triTrans.p[n].y = triRot.p[n].y + y - cCamera.y;
            triTrans.p[n].z = triRot.p[n].z + z - cCamera.z; // we will probably not be rotating the camera along the z axis
        }

        // Use Cross-Product to Get Surface Normal 
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
        
        if  (vNormal.x * (triTrans.p[0].x - cCamera.x) +
             vNormal.y * (triTrans.p[0].y - cCamera.y) +                                        
             vNormal.z * (triTrans.p[0].z - cCamera.z) < 0) 
        {
            triProj = triTrans;
            for (int n = 0; n < 3; n++) // apply perspective/projection to triangle
                MultiplyMatrixVector(&triProj.p[n], triTrans.p[n], matProj);

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
            Vec3D vLightDir = (Vec3D) { 0.0f, 0.0f, -1.0f }; // spaghetti
            float ld = sqrtf(vLightDir.x * vLightDir.x + vLightDir.y * vLightDir.y + vLightDir.z * vLightDir.z);
		    vLightDir.x /= ld; 
            vLightDir.y /= ld; 
            vLightDir.z /= ld;

		    // How similar is normal to light direction?
		    float dp = vNormal.x * vLightDir.x + vNormal.y * vLightDir.y + vNormal.z * vLightDir.z;

            engine->FillTriangle(triProj.p[0].x, triProj.p[0].y,
                                 triProj.p[1].x, triProj.p[1].y, 
                                 triProj.p[2].x, triProj.p[2].y,
                                 olc::Pixel(dp * 255, dp * 255, dp * 255, 255));
        }
    }
}

void Camera::Update()
{

}