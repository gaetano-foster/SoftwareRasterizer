#include "Mesh.h"

bool Mesh::LoadFromObjectFile(std::string sFileName, bool bHasTexture = false)
{
	std::ifstream f(sFileName);
	if (!f.is_open())
		return false;

	// Local cache of verts
	std::vector<Vec3D> verts;
	std::vector<Vec2D> texs;

	while (!f.eof())
	{
		char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;

		char junk;

		if (line[0] == 'v')
		{
			if (line[1] == 't')
			{
				Vec2D v;
				s >> junk >> junk >> v.x >> v.y;
				// A little hack for the spyro texture
				//v.u = 1.0f - v.u;
				//v.v = 1.0f - v.v;
				texs.push_back(v);
			}
			else
			{
				Vec3D v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}
		}

		if (!bHasTexture)
		{
			if (line[0] == 'f')
			{
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				vecTris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}
		else
		{
			if (line[0] == 'f')
			{
				s >> junk;

				std::string tokens[6];
				int nTokenCount = -1;


				while (!s.eof())
				{
					char c = s.get();
					if (c == ' ' || c == '/')
						nTokenCount++;
					else
						tokens[nTokenCount].append(1, c);
				}

				tokens[nTokenCount].pop_back();


				vecTris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
					texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });

			}

		}
	}
	return true;
}

void RasterableTriangle::Raster(olc::PixelGameEngine *engine, olc::Sprite *sprTex, float *pDepthBuffer, bool wireframe = false)
{
	TexturedTriangle(tri.p[0].x, tri.p[0].y, tri.t[0].x, tri.t[0].y, tri.t[0].w,
					 tri.p[1].x, tri.p[1].y, tri.t[1].x, tri.t[1].y, tri.t[1].w,
					 tri.p[2].x, tri.p[2].y, tri.t[2].x, tri.t[2].y, tri.t[2].w, 
					 sprTex, engine, pDepthBuffer, fBrightness);
}

int TriangleClipAgainstPlane(Vec3D plane_p, Vec3D plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2)
{
	// Make sure plane normal is indeed normal
	plane_n.Normalize();

	// Return signed shortest distance from point to plane, plane normal must be normalised
	auto dist = [&](Vec3D &p)
	{
		Vec3D n = p.Normal();
		return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - DotProduct(plane_n, plane_p));
	};

	// Create two temporary storage arrays to classify points either side of plane
	// If distance sign is positive, point lies on "inside" of plane
	Vec3D* inside_points[3];  int nInsidePointCount = 0;
	Vec3D* outside_points[3]; int nOutsidePointCount = 0;
	Vec2D* inside_tex[3]; int nInsideTexCount = 0;
	Vec2D* outside_tex[3]; int nOutsideTexCount = 0;


	// Get signed distance of each point in triangle to plane
	float d0 = dist(in_tri.p[0]);
	float d1 = dist(in_tri.p[1]);
	float d2 = dist(in_tri.p[2]);

	if (d0 >= 0) { inside_points[nInsidePointCount++] = &in_tri.p[0]; inside_tex[nInsideTexCount++] = &in_tri.t[0]; }
	else {
		outside_points[nOutsidePointCount++] = &in_tri.p[0]; outside_tex[nOutsideTexCount++] = &in_tri.t[0];
	}
	if (d1 >= 0) {
		inside_points[nInsidePointCount++] = &in_tri.p[1]; inside_tex[nInsideTexCount++] = &in_tri.t[1];
	}
	else {
		outside_points[nOutsidePointCount++] = &in_tri.p[1];  outside_tex[nOutsideTexCount++] = &in_tri.t[1];
	}
	if (d2 >= 0) {
		inside_points[nInsidePointCount++] = &in_tri.p[2]; inside_tex[nInsideTexCount++] = &in_tri.t[2];
	}
	else {
		outside_points[nOutsidePointCount++] = &in_tri.p[2];  outside_tex[nOutsideTexCount++] = &in_tri.t[2];
	}

	// Now classify triangle points, and break the input triangle into 
	// smaller output triangles if required. There are four possible
	// outcomes...

	if (nInsidePointCount == 0)
	{
		// All points lie on the outside of plane, so clip whole triangle
		// It ceases to exist

		return 0; // No returned triangles are valid
	}

	if (nInsidePointCount == 3)
	{
		// All points lie on the inside of plane, so do nothing
		// and allow the triangle to simply pass through
		out_tri1 = in_tri;

		return 1; // Just the one returned original triangle is valid
	}

	if (nInsidePointCount == 1 && nOutsidePointCount == 2)
	{
		// Triangle should be clipped. As two points lie outside
		// the plane, the triangle simply becomes a smaller triangle
		// The inside point is valid, so keep that...
		out_tri1.p[0] = *inside_points[0];
		out_tri1.t[0] = *inside_tex[0];

		// but the two new points are at the locations where the 
		// original sides of the triangle (lines) intersect with the plane
		float t;
		out_tri1.p[1] = Vec3D_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
		out_tri1.t[1].x = t * (outside_tex[0]->x - inside_tex[0]->x) + inside_tex[0]->x;
		out_tri1.t[1].y = t * (outside_tex[0]->y - inside_tex[0]->y) + inside_tex[0]->y;
		out_tri1.t[1].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

		out_tri1.p[2] = Vec3D_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[1], t);
		out_tri1.t[2].x = t * (outside_tex[1]->x - inside_tex[0]->x) + inside_tex[0]->x;
		out_tri1.t[2].y = t * (outside_tex[1]->y - inside_tex[0]->y) + inside_tex[0]->y;
		out_tri1.t[2].w = t * (outside_tex[1]->w - inside_tex[0]->w) + inside_tex[0]->w;

		return 1; // Return the newly formed single triangle
	}

	if (nInsidePointCount == 2 && nOutsidePointCount == 1)
	{
		// Triangle should be clipped. As two points lie inside the plane,
		// the clipped triangle becomes a "quad". Fortunately, we can
		// represent a quad with two new triangles
		// The first triangle consists of the two inside points and a new
		// point determined by the location where one side of the triangle
		// intersects with the plane
		out_tri1.p[0] = *inside_points[0];
		out_tri1.p[1] = *inside_points[1];
		out_tri1.t[0] = *inside_tex[0];
		out_tri1.t[1] = *inside_tex[1];

		float t;
		out_tri1.p[2] = Vec3D_IntersectPlane(plane_p, plane_n, *inside_points[0], *outside_points[0], t);
		out_tri1.t[2].x = t * (outside_tex[0]->x - inside_tex[0]->x) + inside_tex[0]->x;
		out_tri1.t[2].y = t * (outside_tex[0]->y - inside_tex[0]->y) + inside_tex[0]->y;
		out_tri1.t[2].w = t * (outside_tex[0]->w - inside_tex[0]->w) + inside_tex[0]->w;

		// The second triangle is composed of one of he inside points, a
		// new point determined by the intersection of the other side of the 
		// triangle and the plane, and the newly created point above
		out_tri2.p[0] = *inside_points[1];
		out_tri2.t[0] = *inside_tex[1];
		out_tri2.p[1] = out_tri1.p[2];
		out_tri2.t[1] = out_tri1.t[2];
		out_tri2.p[2] = Vec3D_IntersectPlane(plane_p, plane_n, *inside_points[1], *outside_points[0], t);
		out_tri2.t[2].x = t * (outside_tex[0]->x - inside_tex[1]->x) + inside_tex[1]->x;
		out_tri2.t[2].y = t * (outside_tex[0]->y - inside_tex[1]->y) + inside_tex[1]->y;
		out_tri2.t[2].w = t * (outside_tex[0]->w - inside_tex[1]->w) + inside_tex[1]->w;
		return 2; // Return two newly formed triangles which form a quad
	}
}

void TexturedTriangle(int x1, int y1, float u1, float v1, float w1,
					  int x2, int y2, float u2, float v2, float w2,
					  int x3, int y3, float u3, float v3, float w3,
					  olc::Sprite *tex, olc::PixelGameEngine *engine,
					  float *pDepthBuffer, float fBrightness)
{
	if (y2 < y1)
	{
		std::swap(y1, y2);
		std::swap(x1, x2);
		std::swap(u1, u2);
		std::swap(v1, v2);
		std::swap(w1, w2);
	}	
	if (y3 < y1)
	{
		std::swap(y1, y3);
		std::swap(x1, x3);
		std::swap(u1, u3);
		std::swap(v1, v3);
		std::swap(w1, w3);
	}	
	if (y3 < y2)
	{
		std::swap(y2, y3);
		std::swap(x2, x3);
		std::swap(u2, u3);
		std::swap(v2, v3);
		std::swap(w2, w3);
	}	
	int dy1 = y2 - y1;
	int dx1 = x2 - x1;
	float dv1 = v2 - v1;
	float du1 = u2 - u1;
	float dw1 = w2 - w1;	
	int dy2 = y3 - y1;
	int dx2 = x3 - x1;
	float dv2 = v3 - v1;
	float du2 = u3 - u1;
	float dw2 = w3 - w1;	
	float tex_u, tex_v, tex_w;	
	float dax_step = 0, dbx_step = 0,
		du1_step = 0, dv1_step = 0,
		du2_step = 0, dv2_step = 0,
		dw1_step=0, dw2_step=0;	
	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);	
	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);	
	if (dy2) du2_step = du2 / (float)abs(dy2);
	if (dy2) dv2_step = dv2 / (float)abs(dy2);
	if (dy2) dw2_step = dw2 / (float)abs(dy2);	
	if (dy1)
	{
		for (int i = y1; i <= y2; i++)
		{
			int ax = x1 + (float)(i - y1) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;	
			float tex_su = u1 + (float)(i - y1) * du1_step;
			float tex_sv = v1 + (float)(i - y1) * dv1_step;
			float tex_sw = w1 + (float)(i - y1) * dw1_step;	
			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;	
			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_su, tex_eu);
				std::swap(tex_sv, tex_ev);
				std::swap(tex_sw, tex_ew);
			}	
			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;	
			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;	
			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;

				if (tex_w > pDepthBuffer[i * SCREEN_WIDTH + j])
				{
					olc::Pixel drawPixel = olc::Pixel(tex->Sample(tex_u / tex_w, tex_v / tex_w).r / fBrightness, tex->Sample(tex_u / tex_w, tex_v / tex_w).g / fBrightness, tex->Sample(tex_u / tex_w, tex_v / tex_w).b / fBrightness);
					engine->Draw(j, i, drawPixel);
					pDepthBuffer[i * SCREEN_WIDTH + j] = tex_w;
				}
				t += tstep;
			}	
		}
	}	
	dy1 = y3 - y2;
	dx1 = x3 - x2;
	dv1 = v3 - v2;
	du1 = u3 - u2;
	dw1 = w3 - w2;	
	if (dy1) dax_step = dx1 / (float)abs(dy1);
	if (dy2) dbx_step = dx2 / (float)abs(dy2);	
	du1_step = 0, dv1_step = 0;
	if (dy1) du1_step = du1 / (float)abs(dy1);
	if (dy1) dv1_step = dv1 / (float)abs(dy1);
	if (dy1) dw1_step = dw1 / (float)abs(dy1);	
	if (dy1)
	{
		for (int i = y2; i <= y3; i++)
		{
			int ax = x2 + (float)(i - y2) * dax_step;
			int bx = x1 + (float)(i - y1) * dbx_step;	
			float tex_su = u2 + (float)(i - y2) * du1_step;
			float tex_sv = v2 + (float)(i - y2) * dv1_step;
			float tex_sw = w2 + (float)(i - y2) * dw1_step;	
			float tex_eu = u1 + (float)(i - y1) * du2_step;
			float tex_ev = v1 + (float)(i - y1) * dv2_step;
			float tex_ew = w1 + (float)(i - y1) * dw2_step;	
			if (ax > bx)
			{
				std::swap(ax, bx);
				std::swap(tex_su, tex_eu);
				std::swap(tex_sv, tex_ev);
				std::swap(tex_sw, tex_ew);
			}	
			tex_u = tex_su;
			tex_v = tex_sv;
			tex_w = tex_sw;	
			float tstep = 1.0f / ((float)(bx - ax));
			float t = 0.0f;	
			for (int j = ax; j < bx; j++)
			{
				tex_u = (1.0f - t) * tex_su + t * tex_eu;
				tex_v = (1.0f - t) * tex_sv + t * tex_ev;
				tex_w = (1.0f - t) * tex_sw + t * tex_ew;	

				if (tex_w > pDepthBuffer[i * SCREEN_WIDTH + j])
				{
					olc::Pixel drawPixel = olc::Pixel(tex->Sample(tex_u / tex_w, tex_v / tex_w).r / fBrightness, tex->Sample(tex_u / tex_w, tex_v / tex_w).g / fBrightness, tex->Sample(tex_u / tex_w, tex_v / tex_w).b / fBrightness);
					engine->Draw(j, i, drawPixel);
					pDepthBuffer[i * SCREEN_WIDTH + j] = tex_w;
				}
				t += tstep;
			}
		}	
	}		
}