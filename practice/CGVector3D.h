#ifndef CGVECTOR3D_H
#define CGVECTOR3D_H

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <assert.h>

#define PI 3.1415926535897932385

namespace CCG
{


template<typename T>
class CGVector3D
{
public:
    CGVector3D() : x(0), y(0), z(0) {}
    CGVector3D(T _x) : x(_x), y(_x), z(_x) {}
    CGVector3D(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
    ~CGVector3D() {}

    CGVector3D<T> &operator=(const CGVector3D<T> &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }

    CGVector3D<T> operator - () const { return CGVector3D<T>(-x, -y, -z); }

    CGVector3D<T> operator + (const T &s) const { return CGVector3D<T>(x + s, y + s, z + s); }
    CGVector3D<T> operator - (const T &s) const { return CGVector3D<T>(x - s, y - s, z - s); }
    CGVector3D<T> operator * (const T &s) const { return CGVector3D<T>(x * s, y * s, z * s); }
    CGVector3D<T> operator / (const T &s) const { return CGVector3D<T>(x / s, y / s, z / s); }

    CGVector3D<T> operator + (const CGVector3D<T> &v) const { return CGVector3D<T>(x+v.x, y+v.y, z+v.z); }
    CGVector3D<T> operator - (const CGVector3D<T> &v) const { return CGVector3D<T>(x-v.x, y-v.y, z-v.z); }
    CGVector3D<T> operator * (const CGVector3D<T> &v) const { return CGVector3D<T>(x*v.x, y*v.y, z*v.z); }

    CGVector3D<T> &operator += (const T &s)
    {
        x += s;
        y += s;
        z += s;
        return *this;
    }

    CGVector3D<T> &operator -= (const T &s)
    {
        x -= s;
        y -= s;
        z -= s;
        return *this;
    }

    CGVector3D<T> &operator *= (const T &s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    CGVector3D<T> &operator /= (const T &s)
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }

    const T &operator [] (uint8_t i) const { assert(i>=0 && i<3); return (&x)[i]; }
    T &operator [] (uint8_t i) { assert(i>=0 && i<3); return (&x)[i]; }

    friend CGVector3D<T> operator * (const T &s, const CGVector3D<T> &v)
    {
        return CGVector3D<T>(s*v.x, s*v.y, s*v.z);
    }

    friend CGVector3D<T> operator / (const T &s, const CGVector3D<T> &v)
    {
        return CGVector3D<T>(s/v.x, s/v.y, s/v.z);
    }

    friend std::ostream &operator<<(std::ostream &out, const CGVector3D<T> &v)
    {
        out << "[" << v.x << " " << v.y << " " << v.z << "]";
        return out;
    }

    T &X() { return x; }
    const T &X() const { return x; }

    T &Y() { return y; }
    const T &Y() const { return y; }

    T &Z() { return z; }
    const T &Z() const { return z; }

    T Length() const
    {
        return sqrt(Norm());
    }

    T Norm() const
    {
        return x*x +  y*y + z*z;
    }

    CGVector3D<T> &Normalize()
    {
        T len = Length();
        if(len > 0)
        {
            T dominator = 1 / len;
            x *= dominator;
            y *= dominator;
            z *= dominator;
        }
        return *this;
    }

    T Dot(const CGVector3D<T> &v) const
    {
        return x*v.x + y*v.y + z*v.z;
    }

    CGVector3D<T> Cross(const CGVector3D<T> &v) const
    {
        T _x = y*v.z - z*v.y;
        T _y = z*v.x - x*v.z;
        T _z = x*v.y - y*v.x;
        return CGVector3D<T>(_x, _y, _z);
    }

public:
    T x, y, z;
};

typedef CGVector3D<int> CGVector3Di;
typedef CGVector3D<float> CGVector3Df;
typedef CGVector3D<double> CGVector3Dd;

template<typename T>
T Length(const CGVector3D<T> &v)
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

template<typename T>
void Normalize(CGVector3D<T> &v)
{
    T len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    if(len > 0)
    {
        T dtr = 1 / len;
        v.x *= dtr;
        v.y *= dtr;
        v.z *= dtr;
    }
}

template<typename T>
CGVector3D<T> Normalize(const CGVector3D<T> &v)
{
    T len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
    if(len > 0)
    {
        T dtr = 1 / len;
        T nx = v.x * dtr;
        T ny = v.y * dtr;
        T nz = v.z * dtr;
        return CGVector3D<T>(nx, ny, nz);
    }
    return CGVector3D<T>(v);
}

template<typename T>
T Dot(const CGVector3D<T> &a, const CGVector3D<T> &b)
{
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

template<typename T>
CGVector3D<T> Cross(const CGVector3D<T> &a, const CGVector3D<T> &b)
{
    return CGVector3D<T>(
                         a.y*b.z - a.z*b.y,
                         a.z*b.x - a.x*b.z,
                         a.x*b.y - a.y*b.x);
}


//球坐标与笛卡尔坐标的相互转换
template<typename T>
CGVector3D<T> SphericalToCartesian(const T &theta, const T &phi)
{
    return CGVector3D<T>(cos(phi)*sin(theta), sin(phi)*sin(theta), cos(theta));
}

template<typename T>
inline T SphericalTheta(const CGVector3D<T> &v)
{
    return acos(CLAMP(v[2], -1, 1));
}

template<typename T>
inline T SphericalPhi(const CGVector3D<T> &v)
{
    T p = atan2(v[1], v[0]);
    return p < 0 ? p + 2 * PI : p;
}

template<typename T>
inline T CosTheta(const CGVector3D<T> &v)
{
    return v[2];
}

template<typename T>
inline T SinTheta2(const CGVector3D<T> &v)
{
    return std::max(T(0), 1 - CosTheta(v)*CosTheta(v));
}

template<typename T>
inline T SinTheta(const CGVector3D<T> &v)
{
    return sqrt(SinTheta2(v));
}

template<typename T>
inline T CosPhi(const CGVector3D<T> &v)
{
    T len = SinTheta(v);
    if(len == 0) return 1;
    return CLAMP(v[0]/len, -1, 1);
}

template<typename T>
inline T SinPhi(const CGVector3D<T> &v)
{
    T len = SinTheta(v);
    if(len == 0) return 0;
    return CLAMP(v[1]/len, -1, 1);
}

}
#endif // CGVECTOR3D_H
