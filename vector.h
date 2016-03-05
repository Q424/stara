/*
*  Created on: Dec 02, 2004
*  Last modified on: Nov 18, 2009
*  Author: Changwei Xiong
*  
*  Copyright (C) 2009, Changwei Xiong, 
*  axcw@hotmail.com, <http://www.cs.utah.edu/~cxiong/>
*
*  Licence: Revised BSD License
*/
 
#ifndef vector_H
#define vector_H

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
using namespace std;

class xvector   {
public:

	xvector () {}
	xvector (float e0, float e1, float e2) {
		x=e0; y=e1; z=e2;
	}

	xvector (const xvector  &v) {
		x = v.x; y = v.y; z = v.z;
	}

	const xvector & operator+() const { return *this; }
	xvector  operator-() const { return xvector (-x, -y, -z); }

	xvector & operator+=(const xvector  &v2);
	xvector & operator-=(const xvector  &v2);
	xvector & operator*=(const float t);
	xvector & operator/=(const float t);

	//inner product
	inline float operator%(const xvector  &v){return x *v.x + y *v.y  + z *v.z;}

	float length() const { return sqrt(x*x + y*y + z*z); }
	float squaredLength() const { return x*x + y*y + z*z; }

	void normalize();

	float x,y,z;
};

inline bool operator==(const xvector  &t1, const xvector  &t2) {
	return ((t1.x==t2.x)&&(t1.y==t2.y)&&(t1.z==t2.z));
}

inline bool operator!=(const xvector  &t1, const xvector  &t2) {
	return ((t1.x!=t2.x)||(t1.y!=t2.y)||(t1.z!=t2.z));
}

inline istream &operator>>(istream &is, xvector  &t) {
	is >> t.x >> t.y >> t.z;
	return is;
}

inline ostream &operator<<(ostream &os, const xvector  &t) {
	os << t.x << " " << t.y << " " <<  t.z;
	return os;
}

inline xvector  unit_xvector (const xvector & v) {
	float k = 1.0 / sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	return xvector (v.x*k, v.y*k, v.z*k);
}

inline void xvector ::normalize() {
	float k = 1.0 / sqrt(x*x + y*y + z*z);
	x *= k; y *= k; z *= k;
}

inline xvector  operator+(const xvector  &v1, const xvector  &v2) {
	return xvector ( v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline xvector  operator-(const xvector  &v1, const xvector  &v2) {
	return xvector ( v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline xvector  operator*(float t, const xvector  &v) {
	return xvector (t*v.x, t*v.y, t*v.z);
}

inline xvector  operator*(const xvector  &v, float t) {
	return xvector (t*v.x, t*v.y, t*v.z);
}


inline xvector  operator*(const xvector  &v1, const xvector  &v2) {
	return xvector (v1.x*v2.x, v1.y*v2.y, v1.z*v2.z);
}


inline xvector  operator/(const xvector  &v, float t) {
	return xvector (v.x/t, v.y/t, v.z/t);
}


inline float operator%(const xvector  &v1, const xvector  &v2) {
	return v1.x *v2.x + v1.y *v2.y  + v1.z *v2.z;
}

inline xvector  operator^(const xvector  &v1, const xvector  &v2) {
	return xvector ( (v1.y*v2.z - v1.z*v2.y),
		(-(v1.x*v2.z - v1.z*v2.x)),
		(v1.x*v2.y - v1.y*v2.x));
}


inline xvector & xvector ::operator+=(const xvector  &v){
	x  += v.x;
	y  += v.y;
	z  += v.z;
	return *this;
}

inline xvector & xvector ::operator-=(const xvector & v) {
	x  -= v.x;
	y  -= v.y;
	z  -= v.z;
	return *this;
}

inline xvector & xvector ::operator*=(const float t) {
	x  *= t;
	y  *= t;
	z  *= t;
	return *this;
}

inline xvector & xvector ::operator/=(const float t) {
	x  /= t;
	y  /= t;
	z  /= t;
	return *this;
}


#endif
