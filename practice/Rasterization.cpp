#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <memory.h>
#include "CGVector3D.h"
#include "CGMatrix4d.h"
#include "CGVector2D.h"

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
	//CGVector2Df v0(450, 420), v1(100, 100), v2(100, 420);
	CGVector3Df c0(1, 0, 0), c1(0, 1, 0), c2(0, 0, 1);

	const uint32_t w = 512;
	const uint32_t h = 512;

	RGB *pFrameBuffer = new RGB[w*h];
	memset(pFrameBuffer, 0, w*h*3);

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

				CGVector3Df c = w0 * c0 + w1 * c1 + w2 * c2;
				pFrameBuffer[j * w + i][0] = (unsigned char)(c[0]*255);
				pFrameBuffer[j * w + i][1] = (unsigned char)(c[1]*255);
				pFrameBuffer[j * w + i][2] = (unsigned char)(c[2]*255);
			}
		}
	}
	std::ofstream ofs;
	//颜色错位的原因就是存储的颜色值超过了位数，多余的值分到其他点上
	ofs.open("./raster.ppm", std::ios::binary | std::ios::out);
	ofs << "P6\n" << w << " " << h << "\n255\n";
	ofs.write((char*)pFrameBuffer, w*h*3);
	ofs.close();
	delete [] pFrameBuffer;
	return 0;
}

