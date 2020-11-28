#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include "CGVector3D.h"
#include "CGMatrix4d.h"
#include "CGVector2D.h"

using namespace CCG;

static const float fInchToMm = 25.4;

const CGVector3Df verts[146] = {
	{  -2.5703,   0.78053,  -2.4e-05}, { -0.89264,  0.022582,  0.018577}, 
    {   1.6878, -0.017131,  0.022032}, {   3.4659,  0.025667,  0.018577}, 
    {  -2.5703,   0.78969, -0.001202}, { -0.89264,   0.25121,   0.93573}, 
    {   1.6878,   0.25121,    1.1097}, {   3.5031,   0.25293,   0.93573}, 
    {  -2.5703,    1.0558, -0.001347}, { -0.89264,    1.0558,    1.0487}, 
    {   1.6878,    1.0558,    1.2437}, {   3.6342,    1.0527,    1.0487}, 
    {  -2.5703,    1.0558,         0}, { -0.89264,    1.0558,         0}, 
    {   1.6878,    1.0558,         0}, {   3.6342,    1.0527,         0}, 
    {  -2.5703,    1.0558,  0.001347}, { -0.89264,    1.0558,   -1.0487}, 
    {   1.6878,    1.0558,   -1.2437}, {   3.6342,    1.0527,   -1.0487}, 
    {  -2.5703,   0.78969,  0.001202}, { -0.89264,   0.25121,  -0.93573}, 
    {   1.6878,   0.25121,   -1.1097}, {   3.5031,   0.25293,  -0.93573}, 
    {   3.5031,   0.25293,         0}, {  -2.5703,   0.78969,         0}, 
    {   1.1091,    1.2179,         0}, {    1.145,     6.617,         0}, 
    {   4.0878,    1.2383,         0}, {  -2.5693,    1.1771, -0.081683}, 
    {  0.98353,    6.4948, -0.081683}, { -0.72112,    1.1364, -0.081683}, 
    {   0.9297,     6.454,         0}, {  -0.7929,     1.279,         0}, 
    {  0.91176,    1.2994,         0}
};

const uint32_t nTriangles = 51;

const uint32_t Triangles[nTriangles * 3] = {
	4,   0,   5,   0,   1,   5,   1,   2,   5,   5,   2,   6,   3,   7,   2, 
     2,   7,   6,   5,   9,   4,   4,   9,   8,   5,   6,   9,   9,   6,  10, 
     7,  11,   6,   6,  11,  10,   9,  13,   8,   8,  13,  12,  10,  14,   9, 
     9,  14,  13,  10,  11,  14,  14,  11,  15,  17,  16,  13,  12,  13,  16, 
    13,  14,  17,  17,  14,  18,  15,  19,  14,  14,  19,  18,  16,  17,  20, 
    20,  17,  21,  18,  22,  17,  17,  22,  21,  18,  19,  22,  22,  19,  23, 
    20,  21,   0,  21,   1,   0,  22,   2,  21,  21,   2,   1,  22,  23,   2, 
     2,  23,   3,   3,  23,  24,   3,  24,   7,  24,  23,  15,  15,  23,  19, 
    24,  15,   7,   7,  15,  11,   0,  25,  20,   0,   4,  25,  20,  25,  16, 
    16,  25,  12,  25,   4,  12,  12,   4,   8,  26,  27,  28,  29,  30,  31, 
    32,  34,  33 
};

/*float fFocalLength = 35;
float fFilmApertureWidth = 0.980;
float fFilmApertureHeight = 0.735;

float fNearClippingPlane = 0.1;
float fFarClippingPlane = 1000;*/


enum CGFitMode
{
	FILL = 0,
	OVERSCAN = 1
};

bool IsNeedFit(const float fFilmAspectRatio, const float fDeviceAspectRatio, float &fXScale, float &fYScale)
{
	if(fFilmAspectRatio == fDeviceAspectRatio)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void ComputeScale(
	const float fFilmAspectRatio,
	const float fDeviceAspectRatio,
	float &fXScale,
	float &fYScale,
	CGFitMode eFitMode
	)
{
	fXScale = 1;
	fYScale = 1;

	switch(eFitMode)
	{
	case FILL:
		if(fFilmAspectRatio > fDeviceAspectRatio)
		{
			fXScale = fDeviceAspectRatio / fFilmAspectRatio;
		}
		else
		{
			fYScale = fFilmAspectRatio / fDeviceAspectRatio; 
		}
		break;

	case OVERSCAN:
		if(fFilmAspectRatio > fDeviceAspectRatio)
		{
			fYScale = fFilmAspectRatio / fDeviceAspectRatio;
		}
		else
		{
			fXScale = fDeviceAspectRatio / fFilmAspectRatio;
		}
		break;

	default:
		break;
	}
}

bool ComputePixelCoordinats(
	const CGVector3Df &ptWorld,
	const CGMatrix4Df &matWorldToCamera,
	const float &bottom,
	const float &top,
	const float &left,
	const float &right,
	const float &fNear,
	const uint32_t &nImageWidth,
	const uint32_t &nImageHeight,
	CGVector2Di &ptRaster
	)
{
	CGVector3Df ptCamera;
	matWorldToCamera.multVecMatrix(ptWorld, ptCamera);

	CGVector2Df ptScreen;
	ptScreen.x = (ptCamera.x / -ptCamera.z) * fNear;
	ptScreen.y = (ptCamera.y / -ptCamera.z) * fNear;

	CGVector2Df ptNDC;
	ptNDC.x = (ptScreen.x + right) / (2 * right);
	ptNDC.y = (ptScreen.y + top) / (2 * top);

	ptRaster.x = int(ptNDC.x * nImageWidth);
	ptRaster.y = int((1 - ptNDC.y) * nImageHeight);

	bool bVisible = true;
	if(ptRaster.x < left || ptRaster.x > right || ptRaster.y < bottom || ptRaster.y > top)
	{
		bVisible = false;
	}
	return bVisible;
}

float fFocalLength = 35;
float fFilmApertureWidth = 0.825;
float fFilmApertureHeight = 0.446;

float fNearClippingPlane = 0.1;
float fFarClippingPlane = 1000;

uint32_t nImageWidth = 512;
uint32_t nImageHeight = 512;

CGFitMode eFitMode = OVERSCAN;

int main(int agrv, char **argc)
{
#if 0
#if 0
	float fAngleOfViewHorizontal = 2 * atan((fFilmApertureWidth * fInchToMm / 2) / fFocalLength);
	float right = tan(fAngleOfViewHorizontal / 2) * fNearClippingPlane;

	float fAngleOfViewVertical = 2 * atan((fFilmApertureHeight * fInchToMm / 2) / fFocalLength);
	float top = tan(fAngleOfViewVertical / 2) * fNearClippingPlane;
#else
	float right = (fFilmApertureWidth * fInchToMm / 2) / fFocalLength * fNearClippingPlane;
	
	float top = (fFilmApertureHeight * fInchToMm / 2) / fFocalLength * fNearClippingPlane;
#endif
	float left = -right;
	float bottom = -top;

	std::cout << "Screen window left-right coordinates " << left << "\t" << right << "\n";
	std::cout << "Screen window bottom-top coordinates " << bottom << "\t" << top << "\n";

	std::cout << "Using film aspect ratio to compute four coordinates.\n";
	float top = (fFilmApertureHeight * fInchToMm / 2) / fFocalLength * fNearClippingPlane;
	float bottom = -top;

	float fFilmAspectRatio = fFilmApertureWidth / fFilmApertureHeight;
	float right = top * fFilmAspectRatio;
	float left = -right;

	std::cout << "Screen window left-right coordinates " << left << "\t" << right << "\n";
	std::cout << "Screen window bottom-top coordinates " << bottom << "\t" << top << "\n";
#endif

	float fFilmAspectRatio = fFilmApertureWidth / fFilmApertureHeight;
	float fDeviceAspectRatio = nImageWidth / (float)nImageHeight;

	float top = (fFilmApertureHeight * fInchToMm / 2) / fFocalLength * fNearClippingPlane;
	float right = (fFilmApertureWidth * fInchToMm / 2) / fFocalLength * fNearClippingPlane;

	float fXScale = 1, fYScale = 1;

	switch(eFitMode)
	{
	case FILL:
		if(fFilmAspectRatio > fDeviceAspectRatio)
		{
			fXScale = fDeviceAspectRatio / fFilmAspectRatio;
		}
		else
		{
			fYScale = fFilmAspectRatio / fDeviceAspectRatio;
		}
		break;

	case OVERSCAN:
		if(fFilmAspectRatio > fDeviceAspectRatio)
		{
			fYScale = fFilmAspectRatio / fDeviceAspectRatio;
		}
		else
		{
			fXScale = fDeviceAspectRatio / fFilmAspectRatio;
		}
		break;

	default:
		break;
	}

	right *= fXScale;
	top *= fYScale;
	float bottom = -top;
	float left = -right;

	std::cerr << "Screen window coordinates : " << bottom << " " << left << " " << top << " " << right << "\n";
	std::cerr << "Film Aspect Ratio : " << fFilmAspectRatio << "\nDevice Aspect Ratio : " << fDeviceAspectRatio << "\n";
	std::cerr << "Angle of view : " << 2 * atan((fFilmApertureWidth * fInchToMm / 2) / fFocalLength * 180 / PI) << "deg\n";

	std::ofstream ofs;
	ofs.open("./pinhole2.svg");
	ofs << "<svg version=\"1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns=\"http://www.w3.org/2000/svg\" width=\"" << nImageWidth << "\" height=\"" << nImageHeight << "\">" << std::endl;

	CGMatrix4Df matCameraToWorld(-0.95424, 0, 0.299041, 0, 0.0861242, 0.95763, 0.274823, 0, -0.28637, 0.288002, -0.913809, 0, -3.734612, 7.610426, -14.152769, 1);
	CGMatrix4Df matWorldToCamera = matCameraToWorld.Inverse();
	float fCanvasWidth = 2, fCanvasHeight = 2;
	for(uint32_t i = 0; i < nTriangles; ++i)
	{
		const CGVector3Df &ptWorld0 = verts[Triangles[i*3]];
		const CGVector3Df &ptWorld1 = verts[Triangles[i*3 + 1]];
		const CGVector3Df &ptWorld2 = verts[Triangles[i*3 + 2]];

		CGVector2Di ptRaster0, ptRaster1, ptRaster2;

		bool bVisible = true;
		bVisible &= ComputePixelCoordinats(ptWorld0, matWorldToCamera, bottom, top, left, right, fNearClippingPlane, nImageWidth, nImageHeight, ptRaster0);
		bVisible &= ComputePixelCoordinats(ptWorld1, matWorldToCamera, bottom, top, left, right, fNearClippingPlane, nImageWidth, nImageHeight, ptRaster1);
		bVisible &= ComputePixelCoordinats(ptWorld2, matWorldToCamera, bottom, top, left, right, fNearClippingPlane, nImageWidth, nImageHeight, ptRaster2);

		int nVal = bVisible ? 0 : 255;
		ofs << "<line x1=\"" << ptRaster0.x << "\" y1=\"" << ptRaster0.y << "\" x2=\"" << ptRaster1.x << "\" y2=\"" << ptRaster1.y << "\" style=\"stroke:rgb(" << nVal << ",0,0);stroke-width:1\" />\n"; 
        ofs << "<line x1=\"" << ptRaster1.x << "\" y1=\"" << ptRaster1.y << "\" x2=\"" << ptRaster2.x << "\" y2=\"" << ptRaster2.y << "\" style=\"stroke:rgb(" << nVal << ",0,0);stroke-width:1\" />\n"; 
        ofs << "<line x1=\"" << ptRaster2.x << "\" y1=\"" << ptRaster2.y << "\" x2=\"" << ptRaster0.x << "\" y2=\"" << ptRaster0.y << "\" style=\"stroke:rgb(" << nVal << ",0,0);stroke-width:1\" />\n";
	}

	ofs << "</svg>\n";
	ofs.close();

	return 0;
}