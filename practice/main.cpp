#include <cstdio>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <algorithm>
#include "CGVector3D.h"
#include "CGMatrix4D.h"

using namespace CCG;

//! 获取三维坐标在一维数组中的索引
#define IX(size, i, j, k) (i * j * k * size + i * j * size + i)

//! 生成0-1之间的随机浮点数
float drand48()
{
	return rand() / (RAND_MAX + 1.0);
}

//! 保存图像为ppm格式
void Save(const char *file, const CGVector3Df *v, const int &width, const int &height)
{
	std::ofstream ofs;
	ofs.open(file, std::ios::out | std::ios::binary);
	if(ofs.fail())
	{
		fprintf(stderr, "Error : can't save image to file %s\n", file);
	}
	else
	{
		ofs << "P6\n" << width << " " << height << "\n255\n";
		const CGVector3Df *pColor = v;
		for(int j = 0; j < height; ++j)
		{
			for (int i = 0; i < width; ++i)
			{
				char r = static_cast<char>(std::min(255.f, 255 * pColor->X() + 0.5f));
				char g = static_cast<char>(std::min(255.f, 255 * pColor->Y() + 0.5f));
				char b = static_cast<char>(std::min(255.f, 255 * pColor->Z() + 0.5f));
				ofs << r << g << b;
				pColor++;
			}
		}
	}
	ofs.close();
}

//! 双线性插值公式
CGVector3Df Bilinear(const float &tx, const float &ty,  
				const CGVector3Df &c00, const CGVector3Df &c10,
				const CGVector3Df &c01, const CGVector3Df &c11)
{
	CGVector3Df a = (1 - tx) * c00 + tx * c10;
	CGVector3Df b = (1 - tx) * c01 + tx * c11;
	return a * (1 - ty) + b * ty;
}

//! 测试二次线性插值
void TestBilinear()
{
	//! 生成插值图像
	int nImageWidth = 512;
	int nGridSizeX = 9, nGridSizeY = 9;
	CGVector3Df *pGrid = new CGVector3Df[(nGridSizeY + 1) * (nGridSizeX + 1)];
	for(int j = 0; j <= nGridSizeY; ++j)
	{
		for(int i = 0; i <= nGridSizeX; ++i)
		{
			pGrid[j*(nGridSizeX + 1) + i] = CGVector3Df(drand48(), drand48(), drand48());
			printf("%d %d %f\n", i, j, pGrid[j*(nGridSizeX+1) + i].X());
		}
	}

	CGVector3Df *pImageData = new CGVector3Df[nImageWidth * nImageWidth], *pPixel = pImageData;
	for(int j = 0; j < nImageWidth; ++j)
	{
		for(int i = 0; i < nImageWidth; ++i)
		{
			float fx = i / (float)nImageWidth * nGridSizeX;
			float fy = j / (float)nImageWidth * nGridSizeY;
			int nx = int(fx);
			int ny = int(fy);
			const CGVector3Df &c00 = pGrid[ny * (nGridSizeX + 1) + nx];
			const CGVector3Df &c10 = pGrid[ny * (nGridSizeX + 1) + (nx + 1)];
			const CGVector3Df &c01 = pGrid[(ny + 1) * (nGridSizeX + 1) + nx];
			const CGVector3Df &c11 = pGrid[(ny + 1) * (nGridSizeX + 1) + (nx + 1)];
			*(pPixel++) = Bilinear(fx - nx, fy - ny, c00, c10, c01, c11);
		}
	}
	Save("./bilinear.ppm", pImageData, nImageWidth, nImageWidth);


	//! 以下代码是生成原图像
	pPixel = pImageData;
	int nCellSize = nImageWidth / nGridSizeX;
	fprintf(stderr, "%d\n", nCellSize);
	for(int j = 0; j < nImageWidth; ++j)
	{
		for(int i = 0; i < nImageWidth; ++i)
		{
			float fx = (i + nCellSize / 2) / float(nImageWidth);
			float fy = (j + nCellSize / 2) / float(nImageWidth);
			int nx = static_cast<int>(fx * nGridSizeX);
			int ny = static_cast<int>(fy * nGridSizeY);
			*pPixel = pGrid[ny * (nGridSizeY + 1) + nx];
			int mx = (i + nCellSize / 2) % nCellSize;
			int my = (j + nCellSize / 2) % nCellSize;
			int ma = nCellSize / 2 + 2, mb = nCellSize / 2 - 2;
			if(mx < ma && mx > mb && my < ma && my > mb)
			{
				*pPixel = CGVector3Df(0, 0, 0);
			}
			pPixel++;
		}
	}
	Save("./input.ppm", pImageData, nImageWidth, nImageWidth);
}


//! 测试三次线性插值
void TestTrilinear()
{
	int nGridSize = 10;
	int nVertices = nGridSize + 1;
	CGVector3Df *pGrid = new CGVector3Df[nVertices*nVertices*nVertices];
	for(int k = 0; k < nVertices + 1; ++k)
	{
		for(int j = 0; j < nVertices + 1; ++j)
		{
			for(int i = 0; i < nVertices + 1; ++i)
			{
				pGrid[IX(nVertices, i, j, k)] = CGVector3Df(drand48(), drand48(), drand48());
			}
		}
	}
	float rx, ry, rz;
	float fx, fy, fz;
	int nx, ny, nz;
	float tx, ty, tz;
	for(int i = 0; i < 10e2; ++i)
	{
		rx = drand48();
		ry = drand48();
		rz = drand48();
		fx = rx*nGridSize; nx = int(fx); tx = fx - nx;
		fy = ry*nGridSize; ny = int(fy); ty = fy - ny;
		fz = rz*nGridSize; nz = int(fz); tz = fz - nz;
		const CGVector3Df &c000 = pGrid[IX(nVertices, nx, ny, nz)];
		const CGVector3Df &c100 = pGrid[IX(nVertices, nx+1, ny, nz)];
		const CGVector3Df &c010 = pGrid[IX(nVertices, nx, ny+1, nz)];
		const CGVector3Df &c110 = pGrid[IX(nVertices, nx+1, ny+1, nz)];
		const CGVector3Df &c001 = pGrid[IX(nVertices, nx, ny, nz+1)];
		const CGVector3Df &c101 = pGrid[IX(nVertices, nx+1, ny, nz+1)];
		const CGVector3Df &c011 = pGrid[IX(nVertices, nx, ny+1, nz+1)];
		const CGVector3Df &c111 = pGrid[IX(nVertices, nx+1, ny+1, nz+1)];

		CGVector3Df e = Bilinear(tx, ty, c000, c100, c010, c110);
		CGVector3Df f = Bilinear(tx, ty, c001, c101, c011, c111);
		CGVector3Df g = e * (1 - tz) + tz * f;
	}
	delete pGrid;
}

//! 构建相机矩阵
CGMatrix4Df LookAt(const CGVector3Df &from, const CGVector3Df &to, const CGVector3Df &tmp = CGVector3Df(0, 1, 0))
{
	CGVector3Df forward = Normalize<float>(from - to);
	CGVector3Df right = Cross<float>(Normalize<float>(tmp), forward);
	CGVector3Df up = Cross<float>(forward, right);

	CGMatrix4Df camToWorld;
	camToWorld[0][0] = right.x;
	camToWorld[0][1] = right.y;
	camToWorld[0][2] = right.z;
	camToWorld[1][0] = up.x;
	camToWorld[1][1] = up.y;
	camToWorld[1][2] = up.z;
	camToWorld[2][0] = forward.x;
	camToWorld[2][1] = forward.y;
	camToWorld[2][2] = forward.z;
	camToWorld[3][0] = from.x;
	camToWorld[3][1] = from.y;
	camToWorld[3][2] = from.z;

	return camToWorld;
}

float F(float x) { return 2 * x * x; }
float f(float x) { return 4 * x; }

int main(int argc, char **argv)
{
	srand(1308);
	//! 测试双线性插值
	//TestBilinear();

	//! 黎曼和估计定积分
	float a = atof(argv[1]);
	float b = atof(argv[2]);
	int N = atoi(argv[3]);
	float fExact = F(b) - F(a);

	float dt = (b - a) / N, time = dt, sum = 0;
	for(int i = 1; i <= N; ++i)
	{
		float delta = f(time) * dt;
		sum += delta;
		time += dt;
	}
	printf("Result, exact %f, approximation %f, diff %f\n", fExact, sum, sum - fExact);

	system("pause");
	return 0;
}