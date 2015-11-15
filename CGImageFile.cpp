#define _CRT_SECURE_NO_WARNINGS
#include "CGImageFile.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream> 

//---------------------------------------------------------------------------
// GENERIC INPUT/OUTPUT HELPERS
//---------------------------------------------------------------------------

typedef unsigned char cg_u8_t;
typedef unsigned short cg_u16_t;

static int write_u8(cg_u8_t value, FILE *f)
{
	if (fwrite(&value, sizeof(value),1,f) != 1)
		return -1;
	return 0;
}

static int write_u16le(cg_u16_t value, FILE *f)
{
	cg_u8_t buf[2];

	// little endian: least significant byte first
	buf[0]=(cg_u8_t) (value&0xff);
	buf[1]=(cg_u8_t) ((value>>8)&0xff);
  
	if (fwrite(buf, 2*sizeof(cg_u8_t),1,f) != 1)
		return -1;
	return 0;
}

static int read_u8(cg_u8_t *value, FILE *f)
{
	if (fread(value, sizeof(*value),1,f) != 1)
		return -1;
	return 0;
}

static int read_u16le(cg_u16_t *value, FILE *f)
{
	cg_u8_t buf[2];
  
	if (fread(buf, 2*sizeof(cg_u8_t),1,f) != 1) {
		*value=0;
		return -1;
	}

	// little endian: least significant byte first
	*value=((cg_u16_t)buf[1]) << 8 | (cg_u16_t)buf[0];
	return 0;
}

//---------------------------------------------------------------------------
// CG1
//---------------------------------------------------------------------------

extern int cgImageWriteCG1(const char *filename, const unsigned char *data, int width, int height)
{
	// PVL 1
	FILE *file = fopen(filename, "wb");
	unsigned char kennung[] = { 0x43, 0x47, 0x31, 0x69 };
	
	int error = 0;

	//write file type:
	//fwrite(&kennung, 1, 4, file);
	error += write_u8(0x43, file);
	error += write_u8(0x47, file);
	error += write_u8(0x31, file);
	error += write_u8(0x69, file);

	//write img size:
	error += write_u16le(width, file);
	error += write_u16le(height, file);

	//write channel count:
	error += write_u8(4, file);

	//write bytes per color:
	error += write_u8(1, file);
	
	for (int c = 0; c < 4; c++)
		for (int y = height - 1; y >= 0; y--)
			for (int x = 0; x < width; x++)
				error += write_u8(data[(y*width + x) * 4 + c], file);
	
	fclose(file);

	if (error != 0)
		std::cout << "Error on saving screenshot..." << std::endl;

	return -1;
}

