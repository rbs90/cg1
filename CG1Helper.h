//This class is responsible for setting up and maintaining the actual application
//It should not be neccessary to change anything in here or to look at it at all.


#ifndef CG1HELPER_H
#define CG1HELPER_H

#include "glew.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include "CG.h"
#include "CGContext.h"
#include "glutil.h"

#ifndef M_PI    // for Windows
#define M_PI 3.14159265358979323846
#endif

#define CG_GRID_MIN_SCALE 5


class CG1Helper
{
public:
	CG1Helper(){};
	~CG1Helper(){};
  
	static void AttachSoftwareFrameBuffer(CGFrameBuffer* _buffer)
	{
		smFrameBuffer = _buffer;
	}
	static bool initApplication(CGContext* &_context, int width, int height, int windowScale=1);
	static void runApplication();
	static void setProgramStep(void (*progLoop)());
	static bool isKeyPressed(int key) { return (key < 0 || key > GLFW_KEY_LAST) ? false:pressedKeys[key];};
	static bool isKeyReleased(int key){ if (key < 0 || key > GLFW_KEY_LAST) return false; bool t = releasedKeys[key]; releasedKeys[key] = false; return t;};

private:
	static GLuint shader_create_and_compile(GLenum type, const GLchar *source);
	static GLuint program_create(GLuint vertex_shader, GLuint fragment_shader);
	static GLenum program_create_vs_fs(const GLchar *vs, const GLchar *fs);
	static void m_displayFunc();
	static void m_reshapeFunc(GLFWwindow *win, int w, int h);
	static void m_frambufferResizeFunc(GLFWwindow *win, int w, int h);
	static void m_keyboardFunc (GLFWwindow *win, int key, int scancode, int action, int mods);
	static GLFWwindow *smWindow;
	static CGFrameBuffer* smFrameBuffer;
	static CGContext* smContext;
	static void (*smProgramStep)();
	static GLuint FBTexture;
	static GLuint programTexturedQuad;
	static CGLVAO vaoQuad;
	static GLuint vboQuad;
	static GLuint programGrid;
	static CGLVAO vaoGrid;
	static GLuint vboGrid;
	static GLint loc_dims;
	static int glcontextWidth,glcontextHeight;
	static bool pressedKeys[GLFW_KEY_LAST+1];
	static bool releasedKeys[GLFW_KEY_LAST+1];
	static CGLCaps glcaps;
};



#endif
