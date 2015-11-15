#include "CGFrameBuffer.h"
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy
#include <iostream>

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
	this->pBufferData = (unsigned char*) malloc(width * height * 4);

	return true;
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::get(int x, int y, float *rgba) const
{
	// Uebung 01, Aufgabe 1c)

	long pos = (y * this->width + x) * 4;
	rgba[0] = this->pBufferData[pos + 0] / 255.0f;
	rgba[1] = this->pBufferData[pos + 1] / 255.0f;
	rgba[2] = this->pBufferData[pos + 2] / 255.0f;
	rgba[3] = this->pBufferData[pos + 3] / 255.0f;
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::set(int x, int y, const float *rgba)
{
	// Uebung 01, Aufgabe 1b)
	//std::cout << "x: " << x << " y: " << y << " stelle: " << y * this->width + x << std::endl;

	long pos = (y * this->width + x) * 4;

	this->pBufferData[pos + 0] = (unsigned char)(int)(rgba[0] * 255);
	this->pBufferData[pos + 1] = (unsigned char)(int)(rgba[1] * 255);
	this->pBufferData[pos + 2] = (unsigned char)(int)(rgba[2] * 255);
	this->pBufferData[pos + 3] = (unsigned char)(int)(rgba[3] * 255);

	/*
	std::cout << "rgba in :"
		<< rgba[0] << " "
		<< rgba[1] << " "
		<< rgba[2] << " "
		<< rgba[3] << " " << std::endl;
	
	float* rgba2 = new float[4];

	get(x, y, rgba2);

	std::cout << "rgba out:"
		<< rgba2[0] << " "
		<< rgba2[1] << " "
		<< rgba2[2] << " "
		<< rgba2[3] << " " << std::endl;

	*/
}
//---------------------------------------------------------------------------
void CGFrameBuffer::CGBuffer4UB::clear(const float *rgba)
{
	// Uebung 01, Aufgabe 1d)
	for (int x = 0; x < this->width; x++)
		for (int y = 0; y < this->height; y++)
			set(x, y, rgba);
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
