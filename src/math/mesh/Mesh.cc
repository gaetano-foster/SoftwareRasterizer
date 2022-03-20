#include <algorithm>
#include <list>
#include "Mesh.h"

bool Mesh::LoadFromObjectFile(std::string file_name, bool has_texture = false)
{
	std::ifstream f(file_name);
	if (!f.is_open())
		return false;

	// Local cache of verts
	std::vector<Vec3D> verts;
	std::vector<Vec2D> texs;

	while (!f.eof()) {
		char line[128];
		f.getline(line, 128);

		std::strstream s;
		s << line;

		char junk;

		if (line[0] == 'v') {
			if (line[1] == 't') {
				Vec2D v;
				s >> junk >> junk >> v.x >> v.y;
				// A little hack for the spyro texture
				//v.u = 1.0f - v.u;
				//v.v = 1.0f - v.v;
				texs.push_back(v);
			}
			else {
				Vec3D v;
				s >> junk >> v.x >> v.y >> v.z;
				verts.push_back(v);
			}
		}

		if (!has_texture) {
			if (line[0] == 'f') {
				int f[3];
				s >> junk >> f[0] >> f[1] >> f[2];
				tris.push_back({ verts[f[0] - 1], verts[f[1] - 1], verts[f[2] - 1] });
			}
		}
		else {
			if (line[0] == 'f') {
				s >> junk;

				std::string tokens[6];
				int token_count = -1;

				while (!s.eof()) {
					char c = s.get();
					if (c == ' ' || c == '/')
						token_count++;
					else
						tokens[token_count].append(1, c);
				}

				tokens[token_count].pop_back();

				tris.push_back({ verts[stoi(tokens[0]) - 1], verts[stoi(tokens[2]) - 1], verts[stoi(tokens[4]) - 1],
					texs[stoi(tokens[1]) - 1], texs[stoi(tokens[3]) - 1], texs[stoi(tokens[5]) - 1] });
			}
		}
	}
	return true;
}

void Mesh::Render(Vec3D vCamera, Mat4x4 matProj, olc::PixelGameEngine *engine, float *depth_buffer, Mat4x4 matWorld, Mat4x4 matView, bool wireframe = false) {
	std::vector<RasterableTriangle> triangles_to_raster;

    // get triangles to be rastered
    for (auto tri : tris) {
        Triangle triProj, triTrans, triViewed;

        for (int n = 0; n < 3; n++) {
		    MatrixMultiplyVector(&triTrans.p[n], tri.p[n], matWorld);
            triTrans.t[n] = tri.t[n];
        }

        // Use Cross-Product to Get Surface normal 
		Vec3D vNormal, line1, line2;
		line1 = Vec3D_Sub(triTrans.p[1], triTrans.p[0]);
        line2 = Vec3D_Sub(triTrans.p[2], triTrans.p[0]);

	    vNormal = CrossProduct(line1, line2);
	    vNormal.normalize(); // it's normally normal to normalize a normal
        Vec3D vCameraRay = Vec3D_Sub(triTrans.p[0], vCamera);
        
        if  (DotProduct(vNormal, vCameraRay) < 0) {
            // Illumination 
            Vec3D vLightDir = (Vec3D) { 0.5f, 0.0f, -1.0f }; 
            vLightDir.normalize();

            // How similar is normal to light direction?
		    float fLightingVal = (DotProduct(vLightDir, vNormal)) * 255; // we multiply by 255 for illumination
            fLightingVal = (fLightingVal > 230) ? fLightingVal = 230 : fLightingVal = (fLightingVal < 20) ? fLightingVal = 20 : fLightingVal = fLightingVal;
            fLightingVal = 240 - fLightingVal;
            fLightingVal /= 10.0f;

            triViewed = triTrans;
            for (int n = 0; n < 3; n++) { // world space -> view space
                MatrixMultiplyVector(&triViewed.p[n], triTrans.p[n], matView);
                triViewed.t[n] = triTrans.t[n];
            }

            int num_clipped_tris = 0;
            Triangle clipped[2];
            num_clipped_tris = TriangleClipAgainstPlane({ 0, 0, 0.1f }, { 0, 0, 1 }, triViewed, clipped[0], clipped[1]);

            for (int i = 0; i < num_clipped_tris; i++) {
                triProj = triViewed;
                for (int n = 0; n < 3; n++) { // 3D -> 2D
                    MatrixMultiplyVector(&triProj.p[n], clipped[i].p[n], matProj);
                    triProj.t[n] = clipped->t[n];
                }

                for (int n = 0; n < 3; n++) {
                    triProj.t[n].x /= triProj.p[n].w;
                    triProj.t[n].y /= triProj.p[n].w;
                    triProj.t[n].w = 1.0f / triProj.p[n].w;
                }

                for (int n = 0; n < 3; n++)
                    triProj.p[n] = Vec3D_Div(triProj.p[n], FloatAsVec(triProj.p[n].w));

               	// X/Y are inverted so put them back
				triProj.p[0].x *= -1.0f;
				triProj.p[1].x *= -1.0f;
				triProj.p[2].x *= -1.0f;
				triProj.p[0].y *= -1.0f;
				triProj.p[1].y *= -1.0f;
				triProj.p[2].y *= -1.0f;

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

                triangles_to_raster.push_back({ triProj, fLightingVal });
            }
        }
    }

    // sort triangles back to front
    /*sort(triangles_to_raster.begin(), triangles_to_raster.end(), [](RasterableTriangle &t1, RasterableTriangle &t2) 
    {
        float z1 = (t1.tri.p[0].z + t1.tri.p[1].z + t1.tri.p[2].z) / 3.0f;
        float z2 = (t2.tri.p[0].z + t2.tri.p[1].z + t2.tri.p[2].z) / 3.0f;
        return z1 > z2;
    });*/

    // raster triangles
    for (auto &triToRaster : triangles_to_raster) {
		// Clip triangles against all four screen edges, this could yield
		// a bunch of triangles, so create a queue that we traverse to 
		//  ensure we only test new triangles generated against planes
		Triangle clipped[2];
		std::list<Triangle> triangles;

		// Add initial triangle
		triangles.push_back(triToRaster.tri);
		int new_triangles = 1;

		for (int p = 0; p < 4; p++) {
			int tris_to_add = 0;
			while (new_triangles > 0) {
				// Take triangle from front of queue
				Triangle test = triangles.front();
				triangles.pop_front();
				new_triangles--;

				// Clip it against a plane. We only need to test each 
				// subsequent plane, against subsequent new triangles
				// as all triangles after a plane clip are guaranteed
				// to lie on the inside of the plane. I like how this
				// comment is almost completely and utterly justified
				switch (p)
				{
				case 0:	tris_to_add = TriangleClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 1:	tris_to_add = TriangleClipAgainstPlane({ 0.0f, (float)SCREEN_HEIGHT - 1, 0.0f }, { 0.0f, -1.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 2:	tris_to_add = TriangleClipAgainstPlane({ 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				case 3:	tris_to_add = TriangleClipAgainstPlane({ (float)SCREEN_WIDTH - 1, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }, test, clipped[0], clipped[1]); break;
				}

				// Clipping may yield a variable number of triangles, so
				// add these new ones to the back of the queue for subsequent
				// clipping against next planes
				for (int w = 0; w < tris_to_add; w++)
					triangles.push_back(clipped[w]);
			}
			new_triangles = triangles.size();
		}

		// Draw the transformed, viewed, clipped, projected, sorted, clipped triangles
		for (auto &t : triangles) {
            RasterableTriangle tri = { t, triToRaster.brightness };
			tri.Raster(engine, texture, depth_buffer, wireframe);
		}
    }
}

void RasterableTriangle::Raster(olc::PixelGameEngine *engine, olc::Sprite *sprTex, float *depth_buffer, bool wireframe = false) {
	ShadeTriangle(tri.p[0].x, tri.p[0].y, tri.t[0].x, tri.t[0].y, tri.t[0].w,
				  tri.p[1].x, tri.p[1].y, tri.t[1].x, tri.t[1].y, tri.t[1].w,
				  tri.p[2].x, tri.p[2].y, tri.t[2].x, tri.t[2].y, tri.t[2].w, 
				  sprTex, engine, depth_buffer, brightness);

	if (wireframe) {
		engine->DrawTriangle(tri.p[0].x, tri.p[0].y,
					 	 	 tri.p[1].x, tri.p[1].y, 
					 	 	 tri.p[2].x, tri.p[2].y);
	}
}

int TriangleClipAgainstPlane(Vec3D plane_p, Vec3D plane_n, Triangle &in_tri, Triangle &out_tri1, Triangle &out_tri2)
{
	// Make sure plane normal is indeed normal
	plane_n.normalize();

	// Return signed shortest distance from point to plane, plane normal must be normalised
	auto dist = [&](Vec3D &p)
	{
		Vec3D n = p.normal();
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

void ShadeTriangle(int x1, int y1, float u1, float v1, float w1,
				   int x2, int y2, float u2, float v2, float w2,
				   int x3, int y3, float u3, float v3, float w3,
				   olc::Sprite *tex, olc::PixelGameEngine *engine,
				   float *depth_buffer, float brightness)
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

				if (tex_w > depth_buffer[i * SCREEN_WIDTH + j])
				{
					// olc::Pixel drawPixel = olc::Pixel(tex->Sample(tex_u / tex_w, tex_v / tex_w).r / brightness, tex->Sample(tex_u / tex_w, tex_v / tex_w).g / brightness, tex->Sample(tex_u / tex_w, tex_v / tex_w).b / brightness);
					olc::Pixel drawPixel = olc::Pixel(std::clamp(i - 300, 0, 255) / brightness, std::clamp(j - 300, 0, 255) / brightness, 255 / brightness);
					engine->Draw(j, i, drawPixel);
					depth_buffer[i * SCREEN_WIDTH + j] = tex_w;
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

				if (tex_w > depth_buffer[i * SCREEN_WIDTH + j])
				{
					// olc::Pixel drawPixel = olc::Pixel(tex->Sample(tex_u / tex_w, tex_v / tex_w).r / brightness, tex->Sample(tex_u / tex_w, tex_v / tex_w).g / brightness, tex->Sample(tex_u / tex_w, tex_v / tex_w).b / brightness);
					olc::Pixel drawPixel = olc::Pixel(std::clamp(i - 300, 0, 255) / brightness, std::clamp(j - 300, 0, 255) / brightness, 255 / brightness);
					engine->Draw(j, i, drawPixel);
					depth_buffer[i * SCREEN_WIDTH + j] = tex_w;
				}
				t += tstep;
			}
		}	
	}		
}