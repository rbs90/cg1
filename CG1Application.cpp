// Standard includes.
#include <stdlib.h>         // for rand()
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>           // for time() in srand()
#include <string> 

// Our includes.
#include "CG1Helper.h"
#include "CGContext.h"
#include "CGImageFile.h"
#include "CGQuadric.h"
#include "CGMath.h"
//---------------------------------------------------------------------------
// Defines, globals, etc.
#define FRAME_WIDTH  50   // Framebuffer width.
#define FRAME_HEIGHT 15   // Framebuffer height.
#define FRAME_SCALE  15     // Integer scaling factors (zoom).
CGContext *ourContext;

int segments[10][7] = { // a, b, c, d, e, f, g (like https://upload.wikimedia.org/wikipedia/commons/thumb/1/18/7Segment_24.svg/100px-7Segment_24.svg.png)
		{ 1, 1, 1, 1, 1, 1, 0 }, //0
		{ 0, 0, 0, 0, 1, 1, 0 }, //1
		{ 1, 1, 0, 1, 1, 0, 1 }, //2
		{ 1, 1, 1, 1, 0, 0, 1 }, //3
		{ 0, 1, 1, 0, 0, 1, 1 }, //4
		{ 1, 0, 1, 1, 0, 1, 1 }, //5
		{ 1, 0, 1, 1, 1, 1, 1 }, //6
		{ 1, 1, 1, 0, 0, 0, 0 }, //7
		{ 1, 1, 1, 1, 1, 1, 1 }, //8
		{ 1, 1, 1, 1, 1, 0, 1 }, //9
};

int segments_pos[7][4] = { // x1, x2, y1, y2
		{ 1, 4, 0, 0 }, //a
		{ 5, 5, 1, 4 }, //b
		{ 5, 5, 6, 9 }, //c
		{ 1, 4, 10, 10 }, //d
		{ 0, 0, 6, 9 }, //e
		{ 0, 0, 1, 4 }, //f
		{ 1, 4, 5, 5 }, //g

};

// Vertex Attribute Arrays.
#define VERTEX_COUNT 2
float vertex[VERTEX_COUNT][3];	// x,y,z
float color[VERTEX_COUNT][4];	  // r,g,b,a

//---------------------------------------------------------------------------
// generic "passthorugh" vertex program
void passthroughVertexProgram(const CGVertexAttributes& in,
							  CGVertexVaryings& out,
							  const CGUniformData& uniforms)
{
	out.varyings[CG_POSITION_VARYING] = in.attributes[CG_POSITION_ATTRIBUTE];
	out.varyings[CG_NORMAL_VARYING]   = in.attributes[CG_NORMAL_ATTRIBUTE];
	out.varyings[CG_COLOR_VARYING]    = in.attributes[CG_COLOR_ATTRIBUTE];
	out.varyings[CG_TEXCOORD_VARYING] = in.attributes[CG_TEXCOORD_ATTRIBUTE];
}
//---------------------------------------------------------------------------
// generic "passthorugh" fragment program
void passthroughFragmentProgram(const CGFragmentData& in,
								CGVec4& out,
								const CGUniformData& uniforms)
{
	out = in.varyings[CG_COLOR_VARYING];
}
///---------------------------------------------------------------------------
// This is something specifically needed for this application.
typedef struct
{
	float dx;
	float dy;
	float x;
	float y;
} SpringenderPunkt;
SpringenderPunkt a={1.0f, 1.0f, 20.0f, 0.0f};
SpringenderPunkt b={0.5f, -0.75f, 70.0f, 50.0f};



//---------------------------------------------------------------------------
void animateSpringenderPunkt(SpringenderPunkt& p)
{
	if (p.x >=  FRAME_WIDTH-1) p.dx = -fabsf(p.dx);
	if (p.x <=           0.0f) p.dx =  fabsf(p.dx);
	if (p.y >= FRAME_HEIGHT-1) p.dy = -fabsf(p.dy);
	if (p.y <=           0.0f) p.dy =  fabsf(p.dy);
	p.x+=p.dx;
	p.y+=p.dy;
}

//---------------------------------------------------------------------------
//draws a 7-segment display at a given position with a given value
void draw7segmentArray(int value, int x_start, int y_start){

	y_start += 10; // begin from top

	for (int i = 0; i < 7; i++){
		vertex[0][0] = x_start + segments_pos[i][0];
		vertex[0][1] = y_start - segments_pos[i][2];
		vertex[0][2] = 0.0f;

		vertex[1][0] = x_start + segments_pos[i][1];
		vertex[1][1] = y_start - segments_pos[i][3];
		vertex[1][2] = 0.0f;

		if (segments[value][i]) {
			color[0][0] = 1.0f;
			color[0][1] = 0.1f;
			color[0][2] = 0.1f;
			color[0][3] = 1.0f;
		}
		else {
			color[0][0] = 0.8f;
			color[0][1] = 0.8f;
			color[0][2] = 0.8f;
			color[0][3] = 1.0f;
		}

		

		ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
		ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);
		ourContext->cgUseProgram(passthroughVertexProgram, passthroughFragmentProgram);
		ourContext->cgDrawArrays(CG_LINES, 0, 2);
	}
}

void drawLong7segmentNumber(long number, int start_x, int start_y) {
	std::string numberString = std::to_string(number);

	int count = 0;
	for (char& c : numberString) {
		int val = (int)c - 48; //convert current char back to int
		draw7segmentArray(val, start_x + count * 8, start_y);
		count++;
	}
}

void makeScreenShot(){
	std::cout << "Screenshot!" << std::endl;
	unsigned char* color_buffer_copy = (unsigned char*) malloc(FRAME_WIDTH * FRAME_HEIGHT * 4);
	ourContext->cgReadPixels(color_buffer_copy); 
	cgImageWriteCG1("screenshot.cg1", color_buffer_copy, FRAME_WIDTH, FRAME_HEIGHT);
	free(color_buffer_copy);
}


void programStep_7Segments() {
	if (CG1Helper::isKeyReleased(83) || CG1Helper::isKeyReleased(115)) makeScreenShot(); // s or S released -> Screenshot

	ourContext->cgClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT);

	drawLong7segmentNumber(242583, 2, 2);
}

//---------------------------------------------------------------------------
// programStep_* functions are always our main application.
void programStep_SpringenderPunkt()
{
	if (CG1Helper::isKeyReleased(CG_KEY_LEFT))  a.dx = -1.0f;
	if (CG1Helper::isKeyReleased(CG_KEY_RIGHT)) a.dx =  1.0f;
	if (CG1Helper::isKeyReleased(CG_KEY_UP))    a.dy =  1.0f;
	if (CG1Helper::isKeyReleased(CG_KEY_DOWN))  a.dy = -1.0f;
	animateSpringenderPunkt(a);
	animateSpringenderPunkt(b);
	
	// render 
	// Uebung01, Aufgabe 2a)
	ourContext->cgClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	ourContext->cgClear(CG_COLOR_BUFFER_BIT);
	// ourContext->cgClearColor(0.5f, 0.0f, 0.0f, 1.0f);
	//ourContext->cgClearColor(1.0f, 1.0f, 0.0f, 1.0f);
	//ourContext->cgClearColor(0.0f, 1.0f, 1.0f, 1.0f);
	//ourContext->cgClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	//ourContext->cgClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	
	// prepare vertex array for point a
	color[0][0]=1.0f;
	color[0][1]=1.0f;
	color[0][2]=1.0f;
	color[0][3]=1.0f;

	vertex[0][0]=a.x;
	vertex[0][1]=a.y;
	vertex[0][2]=0.0f;
	
	// prepare vertex array for point b
	color[1][0]=0.0f;
	color[1][1]=1.0f;
	color[1][2]=0.0f;
	color[1][3]=1.0f;

	vertex[1][0]=b.x;
	vertex[1][1]=b.y;
	vertex[1][2]=0.0f;
	
	ourContext->cgVertexAttribPointer(CG_POSITION_ATTRIBUTE, &vertex[0][0]);
	ourContext->cgVertexAttribPointer(CG_COLOR_ATTRIBUTE, &color[0][0]);
	
	ourContext->cgUseProgram(passthroughVertexProgram, passthroughFragmentProgram);
	// Uebung 01, Aufgabe 3b)
	ourContext->cgDrawArrays(CG_LINES, 0, 2);
	
	_sleep(50);
}



//---------------------------------------------------------------------------
int main(int argc, char** argv)
{
	srand((unsigned)time(0));           //init random seed
	if (!CG1Helper::initApplication(ourContext, FRAME_WIDTH, FRAME_HEIGHT, FRAME_SCALE)) {
		fprintf(stderr,"failed to intialize CG1Helper!\n");
		return -1;
	}
	CG1Helper::setProgramStep(programStep_7Segments);
	CG1Helper::runApplication();
	return 0;
}
//---------------------------------------------------------------------------
