#ifndef CGMATRIX4D_H_INCLUDED
#define CGMATRIX4D_H_INCLUDED

#include <iostream>
#include <cstdlib>
#include <cmath>
#include <iomanip>
#include <assert.h>

namespace CCG
{

template<typename T>
class CGMatrix4D
{
public:
    CGMatrix4D() {}

    CGMatrix4D(T a, T b, T c, T d, T e, T f, T g, T h,
               T i, T j, T k, T l, T z, T n, T o, T p)
    {
        m[0][0] = a;
        m[0][1] = b;
        m[0][2] = c;
        m[0][3] = d;
        m[1][0] = e;
        m[1][1] = f;
        m[1][2] = g;
        m[1][3] = h;
        m[2][0] = i;
        m[2][1] = j;
        m[2][2] = k;
        m[2][3] = l;
        m[3][0] = z;
        m[3][1] = n;
        m[3][2] = o;
        m[3][3] = p;
    }

    CGMatrix4D<T> operator * (const CGMatrix4D<T> &rhs) const
    {
        CGMatrix4D<T> t;
        Multiply(*this, rhs, t);
        return t;
    }

    CGMatrix4D<T> operator / (const T &s) const
    {
        assert(s != 0);
        return CGMatrix4D<T>(m[0][0]/s, m[0][1]/s, m[0][2]/s, m[0][3]/s,
                             m[1][0]/s, m[1][1]/s, m[1][2]/s, m[1][3]/s,
                             m[2][0]/s, m[2][1]/s, m[2][2]/s, m[2][3]/s,
                             m[3][0]/s, m[3][1]/s, m[3][2]/s, m[3][3]/s);
    }

    const T *operator [] (uint8_t i) const { return m[i]; }
    T *operator [] (uint8_t i) { return m[i]; }

    static void Multiply(const CGMatrix4D<T> &a, const CGMatrix4D<T> &b, CGMatrix4D<T> &c)
    {
        for(uint8_t i = 0; i < 4; ++i)
        {
            for(uint8_t j = 0; j < 4; ++j)
            {
                c[i][j] = a[i][0]*b[0][j] + a[i][1]*b[1][j] +
                          a[i][2]*b[2][j] + a[i][3]*b[3][j];
            }
        }
    }


    void multVecMatrix(const CGVector3D<T> &src, CGVector3D<T> &dest) const
    {
        dest.x = src.x*m[0][0] + src.y*m[1][0] + src.z*m[2][0] + m[3][0];
        dest.y = src.x*m[0][1] + src.y*m[1][1] + src.z*m[2][1] + m[3][1];
        dest.z = src.x*m[0][2] + src.y*m[1][2] + src.z*m[2][2] + m[3][2];
        T w = src.x*m[0][3] + src.y*m[1][3] + src.z*m[2][3] + m[3][3];
        if(w != 1 && w != 0)
        {
            dest.x /= w;
            dest.y /= w;
            dest.z /= w;
        }
    }

    void multDirMatrix(const CGVector3D<T> &src, CGVector3D<T> &dest) const
    {
        dest.x = src.x*m[0][0] + src.y*m[1][0] + src.z*m[2][0];
        dest.y = src.x*m[0][1] + src.y*m[1][1] + src.z*m[2][1];
        dest.z = src.x*m[0][2] + src.y*m[1][2] + src.z*m[2][2];
    }

    CGMatrix4D<T> Transpose() const
    {
        CGMatrix4D<T> x;
        for(int i = 0; i < 4; ++i)
        {
            for(int j = 0; j < 4; ++j)
            {
                x[i][j] = m[j][i];
            }
        }
        return x;
    }

    CGMatrix4D<T> &Transposed()
    {
        CGMatrix4D<T> x;
        for(int i = 0; i < 4; ++i)
        {
            for(int j = 0; j < 4; ++j)
            {
                x[i][j] = m[j][i];
            }
        }
        *this = x;
        return *this;
    }

    T MINOR(int r0, int r1, int r2, int c0, int c1, int c2) const
    {
        T t1 = m[r0][c0] * (m[r1][c1]*m[r2][c2] - m[r1][c2]*m[r2][c1]);
        T t2 = m[r0][c1] * (m[r1][c0]*m[r2][c2] - m[r1][c2]*m[r2][c0]);
        T t3 = m[r0][c2] * (m[r1][c0]*m[r2][c1] - m[r1][c1]*m[r2][c0]);
        return t1 + t2 + t3;
    }

    CGMatrix4D<T> Adjugate() const
    {
        return CGMatrix4D<T>(MINOR(1,2,3,1,2,3), -MINOR(0,2,3,1,2,3), MINOR(0,1,3,1,2,3), -MINOR(0,1,2,1,2,3),
                             -MINOR(1,2,3,0,2,3), MINOR(0,2,3,0,2,3), -MINOR(0,1,3,0,2,3), MINOR(0,1,2,0,2,3),
                             MINOR(1,2,3,0,1,3), -MINOR(0,2,3,0,1,3), MINOR(0,1,3,0,1,3), -MINOR(0,1,2,0,1,3),
                             -MINOR(1,2,3,0,1,2), MINOR(0,2,3,0,1,2), -MINOR(0,1,3,0,1,2), MINOR(0,1,2,0,1,2));
    }

    T Determinant() const
    {
        return m[0][0]*MINOR(1,2,3,1,2,3) + m[0][1]*MINOR(1,2,3,0,2,3) + m[0][2]*MINOR(1,2,3,0,1,3) + m[0][3]*MINOR(1,2,3,0,1,2);
    }

    CGMatrix4D<T> Inverse() const
    {
        T d = Determinant();
        if(d != 0)
        {
            return Adjugate() / Determinant();
        }
        return *this;
    }

    CGMatrix4D<T> &Inversed()
    {
        CGMatrix4D<T> x = Inverse();
        //if(*this != &x)
        {
            *this = x;
        }
        return *this;
    }

    CGMatrix4D<T> Orthorize() const
    {
        CGVector3D<T> v1(m[0][0], m[0][1], m[0][2]);
        CGVector3D<T> v2(m[1][0], m[1][1], m[1][2]);
        CGVector3D<T> v3(m[2][0], m[2][1], m[2][2]);
        CGVector3D<T> v11, v22, v33;
        CGMatrix4D<T> x;
        v11 = v1;
        v22 = v2 - v11*(v2.Dot(v11)/v11.Dot(v11));
        v33 = v3 - v11*(v3.Dot(v11)/v11.Dot(v11)) - v22*(v3.Dot(v22)/v22.Dot(v22));
        v11.Normalize();
        v22.Normalize();
        v33.Normalize();
        x[0][0] = v11[0];
        x[0][1] = v11[1];
        x[0][2] = v11[2];
        x[1][0] = v22[0];
        x[1][1] = v22[1];
        x[1][2] = v22[2];
        x[2][0] = v33[0];
        x[2][1] = v33[1];
        x[2][2] = v33[2];
        return x;
    }

    friend std::ostream &operator<<(std::ostream &out, const CGMatrix4D<T> &x)
    {
        std::ios_base::fmtflags oldFlags = out.flags();
		int width = 12;
		out.precision(5);
		out.setf(std::ios_base::fixed);

		out << "[" << std::setw(width) << x[0][0] <<
			" " << std::setw(width) << x[0][1] <<
			" " << std::setw(width) << x[0][2] <<
			" " << std::setw(width) << x[0][3] << "\n" <<

			" " << std::setw(width) << x[1][0] <<
			" " << std::setw(width) << x[1][1] <<
			" " << std::setw(width) << x[1][2] <<
			" " << std::setw(width) << x[1][3] << "\n" <<

			" " << std::setw(width) << x[2][0] <<
			" " << std::setw(width) << x[2][1] <<
			" " << std::setw(width) << x[2][2] <<
			" " << std::setw(width) << x[2][3] << "\n" <<

			" " << std::setw(width) << x[3][0] <<
			" " << std::setw(width) << x[3][1] <<
			" " << std::setw(width) << x[3][2] <<
			" " << std::setw(width) << x[3][3] << "]";

		out.flags(oldFlags);
		return out;
    }

    CGVector3D<T> multVecMatrix(const CGVector3D<T> &v)
    {
#ifdef ROWMAJOR
        return CGVector3D<T>(v.x*m[0][0] + v.y*m[1][0] + v.z*m[2][0],
                             v.x*m[0][1] + v.y*m[1][1] + v.z*m[2][1],
                             v.x*m[0][2] + v.y*m[1][2] + v.z*m[2][2]);
#else
        return CGVector3D<T>(v.x*m[0][0] + v.y*m[0][1] + v.z*m[0][2],
                             v.x*m[1][0] + v.y*m[1][1] + v.z*m[1][2],
                             v.x*m[2][0] + v.y*m[2][1] + v.z*m[2][2]);
#endif
    }

public:
    T m[4][4] = { {1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1} };
};

}

#endif // CGMATRIX4D_H_INCLUDED
