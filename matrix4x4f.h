//-----------------------------------------------------------------------------
//           Name: matrix4x4f.h
//         Author: Kevin Harris
//  Last Modified: March 13, 2005 by Manuel Menezes
//    Description: OpenGL compatible utility class for a 4x4 matrix of floats.
//                 NOTE: This class has been left unoptimized for readability.
//-----------------------------------------------------------------------------

#ifndef _MATRIX4X4F_H_
#define _MATRIX4X4F_H_

#include "vector3f.h"

#define DEGTORAD(degree) ((degree) * (3.141592654f / 180.0f))
#define RADTODEG(radian) ((radian) * (180.0f / 3.141592654f))

class matrix4x4f
{
    public:

        float m[16];

        matrix4x4f()  { identity(); }

        matrix4x4f( float m0, float m4, float  m8, float m12,
                float m1, float m5, float  m9, float m13,
                float m2, float m6, float m10, float m14,
                float m3, float m7, float m11, float m15 );

        void identity(void);

        void rotate(const float angle, vector3f axis);

        void transformVector( vector3f *vec );

        float* operator * (const float *v);

        matrix4x4f operator * (const matrix4x4f &o);
};

matrix4x4f::matrix4x4f( float m0, float m4, float  m8, float m12,
        float m1, float m5, float  m9, float m13,
        float m2, float m6, float m10, float m14,
        float m3, float m7, float m11, float m15 )
{
    m[0]=m0; m[4]=m4; m[8] =m8;  m[12]=m12;
    m[1]=m1; m[5]=m5; m[9] =m9;  m[13]=m13;
    m[2]=m2; m[6]=m6; m[10]=m10; m[14]=m14;
    m[3]=m3; m[7]=m7; m[11]=m11; m[15]=m15;
}

void matrix4x4f::identity( void )
{
    m[0]=1.0f; m[4]=0.0f; m[8] =0.0f; m[12]=0.0f;
    m[1]=0.0f; m[5]=1.0f; m[9] =0.0f; m[13]=0.0f;
    m[2]=0.0f; m[6]=0.0f; m[10]=1.0f; m[14]=0.0f;
    m[3]=0.0f; m[7]=0.0f; m[11]=0.0f; m[15]=1.0f;
}

void matrix4x4f::rotate( const float angle, vector3f axis )
{
    float s = sin(DEGTORAD(angle));
    float c = cos(DEGTORAD(angle));

    axis.normalize();

    float ux = axis.x;
    float uy = axis.y;
    float uz = axis.z;

    m[0]  = c + (1-c) * ux;
    m[1]  = (1-c) * ux*uy + s*uz;
    m[2]  = (1-c) * ux*uz - s*uy;
    m[3]  = 0;

    m[4]  = (1-c) * uy*ux - s*uz;
    m[5]  = c + (1-c) * pow(uy,2);
    m[6]  = (1-c) * uy*uz + s*ux;
    m[7]  = 0;

    m[8]  = (1-c) * uz*ux + s*uy;
    m[9]  = (1-c) * uz*uz - s*ux;
    m[10] = c + (1-c) * pow(uz,2);
    m[11] = 0;

    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1;
}

void matrix4x4f::transformVector( vector3f *vec )
{
    vector3f &v = *vec;

    float x = v.x;
    float y = v.y;
    float z = v.z;

    v.x = x * m[0] +
        y * m[4] +
        z * m[8];

    v.y = x * m[1] +
        y * m[5] +
        z * m[9];

    v.z = x * m[2] +
        y * m[6] +
        z * m[10];
}

float* matrix4x4f::operator * (const float *v)
{
    float *r = new float[4];
    r[0] = m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12] * v[3];
    r[1] = m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13] * v[3];
    r[2] = m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14] * v[3];
    r[3] = m[3] * v[0] + m[7] * v[1] + m[11] * v[2] + m[15] * v[3];
    return r;
}

matrix4x4f matrix4x4f::operator * (const matrix4x4f &o)
{
    matrix4x4f n(
            m[0] * o.m[0] + m[4] * o.m[1] + m[8] * o.m[2] + m[12] * o.m[3],
            m[0] * o.m[4] + m[4] * o.m[5] + m[8] * o.m[6] + m[12] * o.m[7],
            m[0] * o.m[8] + m[4] * o.m[9] + m[8] * o.m[10] + m[12] * o.m[11],
            m[0] * o.m[12] + m[4] * o.m[13] + m[8] * o.m[14] + m[12] * o.m[15],

            m[1] * o.m[0] + m[5] * o.m[1] + m[9] * o.m[2] + m[13] * o.m[3],
            m[1] * o.m[4] + m[5] * o.m[5] + m[9] * o.m[6] + m[13] * o.m[7],
            m[1] * o.m[8] + m[5] * o.m[9] + m[9] * o.m[10] + m[13] * o.m[11],
            m[1] * o.m[12] + m[5] * o.m[13] + m[9] * o.m[14] + m[13] * o.m[15],

            m[2] * o.m[0] + m[6] * o.m[1] + m[10] * o.m[2] + m[14] * o.m[3],
            m[2] * o.m[4] + m[6] * o.m[5] + m[10] * o.m[6] + m[14] * o.m[7],
            m[2] * o.m[8] + m[6] * o.m[9] + m[10] * o.m[10] + m[14] * o.m[11],
            m[2] * o.m[12] + m[6] * o.m[13] + m[10] * o.m[14] + m[14] * o.m[15],

            m[3] * o.m[0] + m[7] * o.m[1] + m[11] * o.m[2] + m[15] * o.m[3],
            m[3] * o.m[4] + m[7] * o.m[5] + m[11] * o.m[6] + m[15] * o.m[7],
            m[3] * o.m[8] + m[7] * o.m[9] + m[11] * o.m[10] + m[15] * o.m[11],
            m[3] * o.m[12] + m[7] * o.m[13] + m[11] * o.m[14] + m[15] * o.m[15]);
    return n;
}

#endif // _MATRIX4X4F_H_
