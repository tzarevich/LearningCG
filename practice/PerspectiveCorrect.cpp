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

#define PERSP_CORRECT

inline float EdgeFunction(const CGVector3Df &a, const CGVector3Df &b, const CGVector3Df &c)
{
	return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

#if 0
int main(int argc, char **argv)
{
	CGVector3Df v0(13, 34, 114), v2(29, -15, 44), v1(-48, -10, 82);
	CGVector3Df c0(1, 0, 0), c1(0, 1, 0), c2(0, 0, 1);

	const uint32_t w = 512;
	const uint32_t h = 512;

	v0.x = v0.x / v0.z;
	v0.y = v0.y / v0.z;
	v1.x = v1.x / v1.z;
	v1.y = v1.y / v1.z;
	v2.x = v2.x / v2.z;
	v2.y = v2.y / v2.z;

	v0.x = (v0.x + 1) * 0.5 * w;
	v0.y = (v0.y + 1) * 0.5 * h;
	v1.x = (v1.x + 1) * 0.5 * w;
	v1.y = (v1.y + 1) * 0.5 * h;
	v2.x = (v2.x + 1) * 0.5 * w;
	v2.y = (v2.y + 1) * 0.5 * h;

#ifdef PERSP_CORRECT
	c0 /= v0.z;
	c1 /= v1.z;
	c2 /= v2.z;
	v0.z = 1 / v0.z;
	v1.z = 1 / v1.z;
	v2.z = 1 / v2.z;
#endif

	char *pFrameBuffer = new char[w * h * 3];
	memset(pFrameBuffer, 0, w*h*3);

	float fArea = EdgeFunction(v0, v1, v2);
	fArea = 1 / fArea;

	for(uint32_t j = 0; j < h; ++j)
	{
		for(uint32_t i = 0; i < w; ++i)
		{
			CGVector3Df p(i+0.5f, j+0.5f, 0);
			float w0 = EdgeFunction(v1, v2, p);
			float w1 = EdgeFunction(v2, v0, p);
			float w2 = EdgeFunction(v0, v1, p);
			if(w0 >= 0 && w1 >= 0 && w2 >= 0)
			{
				w0 *= fArea;
				w1 *= fArea;
				w2 *= fArea;
				CGVector3Df c = w0 * c0 + w1 * c1 + w2 * c2;
				#ifdef PERSP_CORRECT
					float z = 1 / (w0 * v0.z + w1 * v1.z + w2 * v2.z);
					c *= z; 
				#endif
				pFrameBuffer[(j * w + i) * 3 + 0] = (char)(c[0] * 255);
				pFrameBuffer[(j * w + i) * 3 + 1] = (char)(c[1] * 255);
				pFrameBuffer[(j * w + i) * 3 + 2] = (char)(c[2] * 255);
			}
		}
	}

	std::ofstream ofs;
	ofs.open("./PersCorrect.ppm", std::ios::out | std::ios::binary);
	if(ofs.fail())
	{
		std::cerr << "Failed to open the file!\n";
		return 0;
	}
	ofs << "P6\n" << w << " " << h << "\n255\n";
	ofs.write((char*)pFrameBuffer, w*h*3);
	ofs.close();

	delete pFrameBuffer;

	return 0;
}
#endif

int main(int argc, char **argv)
{
	return 0;
}