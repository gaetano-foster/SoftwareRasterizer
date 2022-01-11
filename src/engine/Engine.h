#ifndef ENGINE_H
    #define ENGINE_H

    #include <cmath>

    #define SCREEN_WIDTH        1200
    #define SCREEN_HEIGHT       650
    #define ASPECT_RATIO        (float)SCREEN_HEIGHT / (float)SCREEN_WIDTH
    #define SWAP(x,y) do { (x)=(x)^(y); (y)=(x)^(y); (x)=(x)^(y); } while(0)

    struct Vec3D 
    {
        float x, y, z;
    };

    struct Triangle 
    {
        Vec3D p[3];
    };

    struct Mat4x4 
    {
        float m[4][4] = { 0 };
        void Init();
        void SetProjectionMatrix(float near, float far, float fov);
    };

    typedef Triangle* Mesh;

    void MultiplyMatrixVector(Vec3D *o, Vec3D i, Mat4x4 m);

#endif