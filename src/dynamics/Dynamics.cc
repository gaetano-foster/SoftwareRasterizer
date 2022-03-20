#include "Dynamics.h"

void Entity::Update() {
    Mat4x4 matTrans, matRot;
    m_matWorld.MakeIdentity();
    matTrans.MakeTranslation(x, y, z);
    matRot.MakeRotation(rotx, roty, rotz);

    m_matWorld = MultiplyMatrices(matRot, matTrans);
}

void Entity::Render(Camera camera, Mat4x4 matProj, olc::PixelGameEngine *engine, float *depth_buffer) {
    mesh.Render({ camera.x, camera.y, camera.z }, matProj, engine, depth_buffer, m_matWorld, (offset ? camera.matView : (Mat4x4)IDENTITY_MATRIX()), wireframe);
}

void Camera::Update() {
    Vec3D vCamera = { x, y, z };

    Vec3D vUp = { 0, 1, 0 };
    Vec3D vTarget = { 0, 0, 1 }; 
    matRot.MakeRotation(rotx, roty, rotz);
    MatrixMultiplyVector(&look_dir, vTarget, matRot);
    vTarget = Vec3D_Add(vCamera, look_dir);

    matView = MatrixInverse(MatrixPointAt(vCamera, vTarget, vUp));
}