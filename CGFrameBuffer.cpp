#include "CGFrameBuffer.h"
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy

//---------------------------------------------------------------------------
// CGFRAMEBUFFER
//---------------------------------------------------------------------------
bool CGFrameBuffer::allocate(int width, int height)
{
	bool retval = colorBuffer.reserve(width,height);
	// also reserve a depth buffer
	// ...
	return retval;
}
//---------------------------------------------------------------------------
int CGFrameBuffer::getWidth() const
{
	return colorBuffer.getWidth();
}
//---------------------------------------------------------------------------
int CGFrameBuffer::getHeight() const
{
	return colorBuffer.getHeight();
}
//---------------------------------------------------------------------------
// CGFRAMEBUFFER::CGBUFFER4UB
//---------------------------------------------------------------------------
CGFrameBuffer::CGBuffer4UB::CGBuffer4UB()
{
	pBufferData = NULL;
}
//---------------------------------------------------------------------------
CGFrameBuffer::CGBuffer4UB::~CGBuffer4UB()
{
	free(pBufferData);
}
//---------------------------------------------------------------------------
bool CGFrameBuffer::CGBuffer4UB::reserve(int width, int height)
{
	this->width = width;
	this->height = height;

	// Uebung 01, Aufgabe 1a)
	pBufferData = (unsigned char*) malloc(height * width * 4);
	return true;
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::get(int x, int y, float *rgba) const
{
	// Uebung 01, Aufgabe 1c)
	for (int i = 0; i < 4; i++)
		rgba[i] = 255 * pBufferData[(y * height + x) * 4 + i];
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::set(int x, int y, const float *rgba)
{
	// Uebung 01, Aufgabe 1b)
  	for (int i = 0; i < 4; i++)
		pBufferData[(y * height + x) * 4 + i] = 255 * rgba[i];
	
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::clear(const float *rgba)
{
	// Uebung 01, Aufgabe 1d)
	for (int h = 0; h < height; h++)
		for (int w = 0; w < width; w++)
			set(h, w, rgba);
}
//---------------------------------------------------------------------------
unsigned char* CGFrameBuffer::CGBuffer4UB::getDataPointer() const
{
	return pBufferData;
}
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer4UB::getWidth() const
{
	return width;
}
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer4UB::getHeight() const
{
	return height;
}
//---------------------------------------------------------------------------
// CGFRAMEBUFFER::CGBUFFER1F
//---------------------------------------------------------------------------
CGFrameBuffer::CGBuffer1f::CGBuffer1f()
{
	pBufferData = NULL;
}
//---------------------------------------------------------------------------
CGFrameBuffer::CGBuffer1f::~CGBuffer1f()
{
	free(pBufferData);
}
//---------------------------------------------------------------------------
bool CGFrameBuffer::CGBuffer1f::reserve(int width, int height)
{
	// ...
	return true;
}
//---------------------------------------------------------------------------
float CGFrameBuffer::CGBuffer1f::get(int x, int y) const
{
	// ...
	return 0.0f;
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer1f::set(int x, int y, const float z)
{
	// ...
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer1f::clear()
{
	// ...
}
//---------------------------------------------------------------------------
float* CGFrameBuffer::CGBuffer1f::getDataPointer() const
{
	// ...
	return NULL;
}
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer1f::getWidth() const
{
	// ...
	return 0;
}
//---------------------------------------------------------------------------
int CGFrameBuffer::CGBuffer1f::getHeight() const
{
	// ...
	return 0;
}
//---------------------------------------------------------------------------

