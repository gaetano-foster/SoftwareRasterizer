#include "Maths.h"

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

bool Mesh::LoadFromObjectFile(std::string sFileName)
{
	std::ifstream f(sFileName);
	if (!f.is_open())
    {
        std::cout << "Error! Could not open file " << sFileName <<  "." << std::endl;
		return false;
    }

	// Local cache of verts
	std::vector<Vec3D> verts;

	while (!f.eof())
	{
		char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;

		char junk;

		if (line[0] == 'v')
		{
			Vec3D v;
			s >> junk >> v.x >> v.y >> v.z;
			verts.push_back(v);
		}

		if (line[0] == 'f')
		{
			int f[3];
			s >> junk >> f[0] >> f[1] >> f[2];
			vecTris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
		}
	}

	return true;
}