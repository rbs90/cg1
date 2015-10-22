#ifndef  CG_MATH_H
#define  CG_MATH_H
#include  "CG.h"

#include <math.h>
#include <cmath>
#include <stdio.h>
//#include <assert.h>

#define ISZERO(x) (fabs(x)<1.0e-5)

class  CGMath
{
public:
	static float dot(const CGVec4& a, const CGVec4& b)
	{
		//assert(!(!ISZERO(a[3]) && !ISZERO(b[3]))); // scalar product only with vector(s)
		return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3];
	}
	static CGVec4 cross(const CGVec4& a, const CGVec4& b)
	{
		//assert(ISZERO(a[3]) && ISZERO(b[3])); // no cross product for points
		CGVec4 c;
		c[0]=a[1]*b[2]-a[2]*b[1];
		c[1]=a[2]*b[0]-a[0]*b[2];
		c[2]=a[0]*b[1]-a[1]*b[0];
		c[3]=0.0f;
		return c;
	}
	static float length(const CGVec4& a)
	{
		return sqrt(dot(a,a));
	}
	static CGVec4 normalize(const CGVec4& a)
	{
		return a / length(a);
	}
	static CGVec4 divw(const CGVec4& a)
	{
		CGVec4 c;
		//assert(!ISZERO(a[3]));
		float s=1.0f/a[3];
		c[0]=s*a[0]; c[1]=s*a[1]; c[2]=s*a[2]; c[3]=1.0f;
		return c;
	}
	static CGVec4 max(const CGVec4& a, float m)
	{
		CGVec4 c;
		for (int i=0; i<4; i++)
			c[i]=a[i]>m?a[i]:m;
		return c;
	}
	static CGVec4 min(const CGVec4& a, float n)
	{
		CGVec4 c;
		for (int i=0; i<4;i++)
			c[i]=a[i]>n?n:a[i];
		return c;
	}
	static CGVec4 max(const CGVec4& a, const CGVec4& b)
	{
		CGVec4 c;
		for (int i=0; i<4; i++)
			c[i]=a[i]>b[i]?a[i]:b[i];
		return c;
	}
	static CGVec4 min(const CGVec4& a, const CGVec4& b)
	{
		CGVec4 c;
		for (int i=0; i<4; i++)
			c[i]=a[i]>b[i]?b[i]:a[i];
		return c;
	}
	static CGVec4 clamp(const CGVec4& a, float m, float n)
	{
		return max(min(a,n),m);    
	}
	static CGVec4 clamp(const CGVec4& a, const CGVec4& m, const CGVec4& n)
	{
		return max(min(a,n),m);    
	}
	static CGVec4 abs(const CGVec4& a)
	{
		CGVec4 c;
		for (int i=0; i<4; i++)
			c[i]=fabs(a[i]);
		return c;
	}
	static CGVec4 ceil(const CGVec4& a)
	{
		CGVec4 c;
		for (int i=0; i<4; i++)
			c[i]=std::ceil(a[i]);
		return c;
	}
	static CGVec4 floor(const CGVec4& a)
	{
		CGVec4 c;
		for (int i=0; i<4; i++)
			c[i]=std::floor(a[i]);
		return c;
	}
	static CGVec4 fract(const CGVec4& a)
	{
		CGVec4 c;
		for (int i=0; i<4; i++)
			c[i]=a[i]-std::floor(a[i]);
		return c;
	}
	static float distance(const CGVec4& a, const CGVec4& b)
	{
		//assert(ISZERO(a[3]-b[3])); // a[3] must equal b[3]     
		return length(a-b);
	}
	static void debugPrint(const CGVec4& a, const char *prefix=NULL)
	{
		printf("%s: %f %f %f %f\n",prefix?prefix:"",a[0],a[1],a[2],a[3]);
	}
};
#endif  /*  CG_MATH_H  */