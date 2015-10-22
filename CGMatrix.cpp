#include "CGMatrix.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include <stdio.h>

// The code is given in UE8

//---------------------------------------------------------------------------
// CGMATRIX4X4 : Static (factory) functions.
//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getIdentityMatrix()
{
	CGMatrix4x4 m;
	// ...
	return m;
}
//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getTranslationMatrix(float x, float y, float z)
{
	CGMatrix4x4 m;
	// ...
	return m;
}
//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getRotationMatrixX(float angle)
{
	CGMatrix4x4 m;
	// ...
	return m;
}
//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getRotationMatrixY(float angle)
{
	CGMatrix4x4 m;
	// ...
	return m;
}
//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getRotationMatrixZ(float angle)
{
	CGMatrix4x4 m;
	// ...
	return m;
}
//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getScaleMatrix(float x, float y, float z)
{
	CGMatrix4x4 m;
	// ...
	return m;
}
//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getRotationMatrix(float angle, float x, float y, float z)
{
	CGMatrix4x4 m;
	// ...
	return m;
}
//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::getFrustum(float left, float right, float bottom, float top, float nearVal, float farVal)
{
	CGMatrix4x4 f;
	// ...
	return f;
}
//---------------------------------------------------------------------------
// CGMATRIX4X4 : Non-static operants.
//---------------------------------------------------------------------------
CGMatrix4x4::CGMatrix4x4()
{
	// ...
}
//---------------------------------------------------------------------------
void CGMatrix4x4::getFloatsToColMajor(float *floats) const
{
	// ...
}
//---------------------------------------------------------------------------
void CGMatrix4x4::setFloatsFromColMajor(const float *floats)
{
	// ...
}
//---------------------------------------------------------------------------
void CGMatrix4x4::getFloatsToRowMajor(float *floats) const
{
	// ...
}
//---------------------------------------------------------------------------
void CGMatrix4x4::setFloatsFromRowMajor(const float *floats)
{
	// ...
}
//---------------------------------------------------------------------------
void CGMatrix4x4::identity()
{
	// ...
}
//---------------------------------------------------------------------------
CGMatrix4x4 CGMatrix4x4::operator*(const CGMatrix4x4& b) const
{
	CGMatrix4x4 r;
	// ...
	return r;
}
//---------------------------------------------------------------------------
void CGMatrix4x4::debugPrint(const char *prefix) const
{
	// ...
}
//---------------------------------------------------------------------------
CGVec4 CGMatrix4x4::operator*(const CGVec4& b) const
{
	CGVec4 c;
	// ...

	// for now, just return 0 vector
	c.set(0.f, 0.f, 0.f, 0.f); // REMOVE THIS
	return c;
}
//---------------------------------------------------------------------------
void CGMatrix4x4::transpose()
{
	// ...
}
//---------------------------------------------------------------------------
void CGMatrix4x4::invert()
{
	// ...
}
//---------------------------------------------------------------------------
void CGMatrix4x4::frustum(float left, float right, float bottom, float top, float nearVal, float farVal)
{
	*this=*this * CGMatrix4x4::getFrustum(left,right,bottom,top,nearVal,farVal);
}
