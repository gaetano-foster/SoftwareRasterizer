#include "Mesh.h"

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