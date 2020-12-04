#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <cstring>
#include "CGVector3D.h"
#include "CGMatrix4d.h"
#include "CGVector2D.h"

#if 0
using namespace CCG;

bool BEdgeFunction(const CGVector2Df &a, const CGVector2Df &b, const CGVector2Df &c)
{
	return((c.x - a.x)*(b.y - a.y) - (c.y - a.y)*(b.x - a.x) >= 0);
}

bool TriangleEdgeFunction(
	const CGVector2Df &v0,
	const CGVector2Df &v1,
	const CGVector2Df &v2,
	const CGVector2Df &p
	)
{
	bool bIsInside = true;
	bIsInside &= BEdgeFunction(v0, v1, p);
	bIsInside &= BEdgeFunction(v1, v2, p);
	bIsInside &= BEdgeFunction(v2, v0, p);
	return bIsInside;
}

float EdgeFunction(const CGVector2Df &a, const CGVector2Df &b, const CGVector2Df &c)
{
	return (c.x - a.x) * (b.y - a.y) - (c.y - a.y) * (b.x - a.x);
}

void ComputeBarycentric(
	const CGVector2Df &p,
	const CGVector2Df &v0,
	const CGVector2Df &v1,
	const CGVector2Df &v2,
	float &w0, float &w1, float &w2
	)
{
	float fArea = EdgeFunction(v0, v1, v2);
	float fTri0 = EdgeFunction(v1, v2, p);
	float fTri1 = EdgeFunction(v2, v0, p);
	float fTri2 = EdgeFunction(v0, v1, p);
	float fInvertArea = 1 / fArea;
	if(fTri0 >= 0 && fTri1 >= 0 && fTri2 >= 0)
	{
		w0 = fTri0 * fInvertArea;
		w1 = fTri1 * fInvertArea;
		w2 = fTri2 * fInvertArea;
	}
}

typedef unsigned char RGB[3];

int main(int argc, char **argv)
{
#if 0
	CGVector2Df a(0, 0), b(0, 1), c(1, 0), p(0.5, 0.2);
	if(TriangleEdgeFunction(a, b, c, p))
	{
		std::cout << "The P point is inside the triangle.\n";
	}
	else
	{
		std::cout << "The P point is not inside the triangle.\n";
	}
#endif

	CGVector2Df v0(491.407, 411.407), v1(148.593, 68.5928), v2(148.593, 411.407);
	CGVector3Df c0(1, 0, 0), c1(0, 1, 0), c2(0, 0, 1);

	const uint32_t w = 512;
	const uint32_t h = 512;

	RGB *pFrameBuffer = new RGB[w*h];
	memset(pFrameBuffer, 0x0, w*h*3);

	float fArea = EdgeFunction(v0, v1, v2);
	fArea = 1.0f / fArea;

	for(uint32_t j = 0; j < h; ++j)
	{
		for(uint32_t i = 0; i < w; ++i)
		{
			CGVector2Df p = CGVector2Df(i + 0.5f, j + 0.5f);
			float w0 = EdgeFunction(v1, v2, p);
			float w1 = EdgeFunction(v2, v0, p);
			float w2 = EdgeFunction(v0, v1, p);

			if(w0 >= 0 && w1 >= 0 && w2 >= 0)
			{
				w0 *= fArea;
				w1 *= fArea;
				w2 *= fArea;

				float r = w0 * c0[0] + w1 * c1[0] + w2 * c2[0];
				float g = w0 * c0[1] + w1 * c1[1] + w2 * c2[1];
				float b = w0 * c0[2] + w1 * c1[2] + w2 * c2[2];
				pFrameBuffer[j * w + i][0] = (unsigned char)(r*255);
				pFrameBuffer[j * w + i][1] = (unsigned char)(g*255);
				pFrameBuffer[j * w + i][2] = (unsigned char)(b*255);
			}
		}
	}
	std::ofstream ofs;
	ofs.open("./raster.ppm");
	ofs << "P6\n" << w << " " << h << "\n255\n";
	ofs.write((char*)pFrameBuffer, w*h*3);
	ofs.close();
	delete [] pFrameBuffer;
	return 0;
}
#endif 

#include <cstdio> 
#include <cstdlib> 
#include <fstream> 
 
typedef float Vec2[2]; 
typedef float Vec3[3]; 
typedef unsigned char Rgb[3]; 
 
inline 
float edgeFunction(const Vec2 &a, const Vec2 &b, const Vec2 &c) 
{ return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]); } 
 
int main(int argc, char **argv) 
{ 
    Vec2 v0 = {491.407, 411.407}; 
    Vec2 v1 = {148.593, 68.5928}; 
    Vec2 v2 = {148.593, 411.407};
    Vec3 c0 = {1, 0, 0}; 
    Vec3 c1 = {0, 1, 0}; 
    Vec3 c2 = {0, 0, 1}; 
 
    const uint32_t w = 512; 
    const uint32_t h = 512; 
 
    Rgb *framebuffer = new Rgb[w * h]; 
    memset(framebuffer, 0x0, w * h * 3); 
 
    float area = edgeFunction(v0, v1, v2); 
 
    for (uint32_t j = 0; j < h; ++j) { 
        for (uint32_t i = 0; i < w; ++i) { 
            Vec2 p = {i + 0.5f, j + 0.5f}; 
            float w0 = edgeFunction(v1, v2, p); 
            float w1 = edgeFunction(v2, v0, p); 
            float w2 = edgeFunction(v0, v1, p); 
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) { 
                w0 /= area; 
                w1 /= area; 
                w2 /= area; 
                float r = w0 * c0[0] + w1 * c1[0] + w2 * c2[0]; 
                float g = w0 * c0[1] + w1 * c1[1] + w2 * c2[1]; 
                float b = w0 * c0[2] + w1 * c1[2] + w2 * c2[2]; 
                framebuffer[j * w + i][0] = (unsigned char)(r * 255); 
                framebuffer[j * w + i][1] = (unsigned char)(g * 255); 
                framebuffer[j * w + i][2] = (unsigned char)(b * 255); 
            } 
        } 
    } 
 
    std::ofstream ofs; 
    ofs.open("./raster2d.ppm"); 
    ofs << "P6\n" << w << " " << h << "\n255\n"; 
    ofs.write((char*)framebuffer, w * h * 3); 
    ofs.close(); 
 
    delete [] framebuffer; 
 
    return 0; 
} 