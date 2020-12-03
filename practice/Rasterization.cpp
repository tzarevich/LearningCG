#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include "CGVector3D.h"
#include "CGMatrix4d.h"
#include "CGVector2D.h"

using namespace CCG;

bool EdgeFunction(const CGVector2Df &a, const CGVector2Df &b, const CGVector2Df &c)
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
	bIsInside &= EdgeFunction(v0, v1, p);
	bIsInside &= EdgeFunction(v1, v2, p);
	bIsInside &= EdgeFunction(v2, v0, p);
	return bIsInside;
}

int main(int argc, char **argv)
{
	CGVector2Df a(0, 0), b(0, 1), c(1, 0), p(0.5, 0.2);
	if(TriangleEdgeFunction(a, b, c, p))
	{
		std::cout << "The P point is inside the triangle.\n";
	}
	else
	{
		std::cout << "The P point is not inside the triangle.\n";
	}

	return 0;
}