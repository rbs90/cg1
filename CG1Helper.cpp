//This class is responsible for setting up and maintaining the actual application
//It should not be neccessary to change anything in here or to look at it at all.

#include "CG1Helper.h"
#include "glutil.h"

static const GLchar *vertexShaderTexturedQuad=
	"ATTR_IN vec2 pos;\n"
	"VAR_OUT vec2 v_tex;\n"
	"void main()\n"
	"{\n"
	"	v_tex=0.5*pos + vec2(0.5);\n"
	"	gl_Position=vec4(pos, 0.0, 1.0);\n"
	"}\n";

static const GLchar *fragmentShaderTexturedQuad=
	"VAR_IN vec2 v_tex;\n"
	"uniform sampler2D texSampler;\n"
	"FS_OUT_DECL\n"
	"void main()\n"
	"{\n"
	"	FS_OUT=TEX2D(texSampler, v_tex);\n"
	"}\n";

static const GLchar *vertexShaderGrid=
	"ATTR_IN vec2 pos;\n"
	"uniform vec2 dims;\n"
	"void main()\n"
	"{\n"
	"	//snap-in to pixel centers (move towards viewport center)\n"
	"	gl_Position=vec4(((floor((0.5*pos+vec2(0.5))*dims - 0.25*normalize(pos))+vec2(0.5))/dims)*2.0-vec2(1.0), 0.0, 1.0);\n"
	"}\n";

static const GLchar *fragmentShaderGrid=
	"FS_OUT_DECL\n"
	"void main()\n"
	"{\n"
	"	FS_OUT=vec4(1.0);\n"
	"}\n";

double GetMilliSec(void)
{ 
	return 1000.0*glfwGetTime();
}
double GetMicroSec(void)
{ 
	return 1000000.0*glfwGetTime();
}

static void
print_info_log(GLuint obj, bool program)
{
	char log[16384];
	if (program) {
		glGetProgramInfoLog(obj, 16383, 0, log);
	} else {
		glGetShaderInfoLog(obj, 16383, 0, log);
	}
	log[16383]=0;
	fprintf(stderr,"%s\n",log);
}

GLuint CG1Helper::shader_create_and_compile(GLenum type, const GLchar *source)
{
        static const GLchar *glsl110_header=
		"#define ATTR_IN attribute\n"
		"#define VAR_OUT varying\n"
		"#define VAR_IN varying\n"
		"#define FS_OUT_DECL\n"
		"#define FS_OUT gl_FragColor\n"
		"#define TEX2D texture2D\n\n";
        static const GLchar *glsl150_header=
		"#version 150 core\n\n"
		"#define ATTR_IN in\n"
		"#define VAR_OUT out\n"
		"#define VAR_IN in\n"
		"#define FS_OUT_DECL out vec4 color;\n"
		"#define FS_OUT color\n"
		"#define TEX2D texture\n\n";
	GLuint shader=0;
	GLint status;
	const GLchar *src[2];

	if (glcaps.supports(CGLCaps::SHADERS_150)) {
		src[0]=glsl150_header;
	} else {
		src[0]=glsl110_header;
	}
	src[1]=source;

	shader=glCreateShader(type);
	glShaderSource(shader, 2, src, NULL);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		fprintf(stderr,"Failed to compile shader\n");
		print_info_log(shader,false);
		glDeleteShader(shader);
		shader=0;
	}

	return shader;
}

GLuint CG1Helper::program_create(GLuint vertex_shader, GLuint fragment_shader)
{
	GLuint program=0;
	GLint status;

	program=glCreateProgram();

	if (vertex_shader)
		glAttachShader(program, vertex_shader);
	if (fragment_shader)
		glAttachShader(program, fragment_shader);

	glBindAttribLocation(program, TGL_ATTR_POSITION, "pos");

	if (glcaps.supports(CGLCaps::SHADERS_150)) {
		glBindFragDataLocation(program, 0, "color");
	}

	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		fprintf(stderr,"Failed to link program!\n");
		print_info_log(program,true);
		glDeleteProgram(program);
		return 0;
	}
	return program;
}

GLenum CG1Helper::program_create_vs_fs(const GLchar *vs, const GLchar *fs)
{
	GLuint id_vs=shader_create_and_compile(GL_VERTEX_SHADER, vs);
	GLuint id_fs=shader_create_and_compile(GL_FRAGMENT_SHADER, fs);
	GLuint program=program_create(id_vs,id_fs);
	glDeleteShader(id_vs);
	glDeleteShader(id_fs);
	return program;
}

bool CG1Helper::initApplication(CGContext* &_context, int width, int height, int windowScale)
{
	GLenum err;
	const GLubyte *vendor, *renderer;
	GLint gl_version[2],glsl_version[2];
	int wh, hh;

	if (!glfwInit()) {
		return false;
	}
	int scale = windowScale>0?windowScale:1;
	wh=width*scale;
	hh=height*scale;
	if (scale >= CG_GRID_MIN_SCALE) {
		wh++;
		hh++;
	}

	// request a moden GL 3.2 core context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	smWindow=glfwCreateWindow( wh, hh, "CG1 Template", NULL, NULL);
	if (!smWindow) {
		// request a legacy GL context
		fprintf(stderr, "Failed to create modern OpenGL >= 3.2 core context, trying fallback\n");
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_FALSE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
		smWindow=glfwCreateWindow( wh, hh, "CG1 Template", NULL, NULL);
	}
	if (!smWindow) {
		fprintf(stderr, "Failed to create OpenGL context\n");
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(smWindow);

	// check what we got 
	CGLHelper::GetGLVersion(gl_version, glsl_version);
	vendor=glGetString(GL_VENDOR);
	renderer=glGetString(GL_RENDERER);
	fprintf(stderr, "OpenGL %d.%d: %s %s\n",gl_version[0],gl_version[1],vendor,renderer);
	if (gl_version[0] < 2) {
		fprintf(stderr," OpenGL 2.0 is required\n");
		glfwDestroyWindow(smWindow);
		glfwTerminate();
		return false;
	}
	if (glsl_version[0] < 1 || (glsl_version[0] == 1 && glsl_version[1] < 10)) {
		fprintf(stderr," GLSL version 1.10 is required\n");
		glfwDestroyWindow(smWindow);
		glfwTerminate();
		return false;
	}

	glfwSetWindowSizeCallback(smWindow, m_reshapeFunc);
	glfwSetFramebufferSizeCallback(smWindow, m_frambufferResizeFunc);
	glfwSetKeyCallback(smWindow, m_keyboardFunc);

	glewExperimental=GL_TRUE;
	err=glewInit();
	if (err != GLEW_OK) {
		fprintf(stderr,"failed to intialize GLEW: 0x%x\n",(unsigned)err);
		return false;
	}
	CGLHelper::CheckGLErrors("glew initialization", false); // glew might create an GL error...

	glcaps.set(gl_version, glsl_version);

	_context = new CGContext(width,height);
	smContext = _context;

	// setup texture which is used as CG framebuffer
	glGenTextures(1, &FBTexture);
	glBindTexture(GL_TEXTURE_2D,FBTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D,0);
	CGLHelper::CheckGLErrors("GL initialization: texture");

	// setup state for drawing grid: shader + VAO + VBO
	programGrid=program_create_vs_fs(vertexShaderGrid, fragmentShaderGrid);
	glUseProgram(programGrid);
	loc_dims=glGetUniformLocation(programGrid, "dims");

	
	vaoGrid.create(glcaps.supports(CGLCaps::VAO));
	vaoGrid.bind();
	glGenBuffers(1,&vboGrid);
	vaoGrid.BindBuffer(GL_ARRAY_BUFFER, vboGrid);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*4*(width+1 + height+1), NULL, GL_STATIC_DRAW);
	GLfloat *buf=(GLfloat*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	for (int dx=0; dx<=width; dx++) {
		buf[dx*4+0]=-1.0f + 2.0f * (float)dx/(float)width;
		buf[dx*4+1]=-1.0f;
		buf[dx*4+2]=-1.0f + 2.0f * (float)dx/(float)width;;
		buf[dx*4+3]= 1.0f;
	}
	buf += (width+1)*4;
	for (int dy=0; dy<=height; dy++) {
		buf[dy*4+0]=-1.0f;
		buf[dy*4+1]=-1.0f + 2.0f * (float)dy/(float)height;
		buf[dy*4+2]= 1.0f;
		buf[dy*4+3]=-1.0f + 2.0f * (float)dy/(float)height;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	vaoGrid.VertexAttribPointer(TGL_ATTR_POSITION,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),0);
	vaoGrid.EnableVertexAttribArray(TGL_ATTR_POSITION);
	vaoGrid.unbind();
	CGLHelper::CheckGLErrors("GL initialization: grid");

	// setup state for drawing a textured quad: shader + VAO + VBO
	vaoQuad.create(glcaps.supports(CGLCaps::VAO));
	vaoQuad.bind();
	glGenBuffers(1,&vboQuad);
	vaoQuad.BindBuffer(GL_ARRAY_BUFFER, vboQuad);
	static const GLfloat quadVertices[]={
		-1.0f, -1.0f,
		 1.0f, -1.0f,
		-1.0f,  1.0f,
		 1.0f,  1.0f};
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*2*4, quadVertices, GL_STATIC_DRAW);
	vaoGrid.VertexAttribPointer(TGL_ATTR_POSITION,2,GL_FLOAT,GL_FALSE,2*sizeof(GLfloat),0);
	vaoGrid.EnableVertexAttribArray(TGL_ATTR_POSITION);
	programTexturedQuad=program_create_vs_fs(vertexShaderTexturedQuad, fragmentShaderTexturedQuad);
	glUseProgram(programTexturedQuad);
	GLint loc_texSampler=glGetUniformLocation(programTexturedQuad, "texSampler");
	glUniform1i(loc_texSampler, 0);

	CGLHelper::CheckGLErrors("GL initialization: quad");

	m_reshapeFunc(smWindow, wh, hh);
	m_frambufferResizeFunc(smWindow, wh, hh);
	return true;
}

void CG1Helper::runApplication()
{
	while (!glfwWindowShouldClose(smWindow)) {
		m_displayFunc();
		CGLHelper::CheckGLErrors("main loop");
		glfwPollEvents();

	}

	vaoQuad.destroy();
	vaoGrid.destroy();

	delete smContext;
	smContext=NULL;

	glfwDestroyWindow(smWindow);
	smWindow=NULL;
	glfwTerminate();
}
void CG1Helper::setProgramStep(void (*progLoop)())
{
	smProgramStep = progLoop;
}
double current_time;
void CG1Helper::m_displayFunc()
{
	char WinTitle [100];
  
	double Framerate = 0;
	double MSPF = 0;
  
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	double m = GetMicroSec();
 
	smProgramStep();

	Framerate = 1000000.0/(GetMicroSec()-current_time);
	MSPF = (GetMicroSec()-m)*0.001;
	current_time=GetMicroSec();
#ifdef WIN32
	sprintf_s(WinTitle, sizeof(WinTitle),"Computergraphik I Template /// %3.1f FPS (%3.4f ms per actual draw)", Framerate, MSPF);
#else
	snprintf(WinTitle, sizeof(WinTitle),"Computergraphik I Template /// %3.1f FPS (%3.4f ms per actual draw)", Framerate, MSPF);
#endif
	glfwSetWindowTitle(smWindow, WinTitle);

	if(smFrameBuffer->colorBuffer.getDataPointer()) {
		glBindTexture(GL_TEXTURE_2D,FBTexture);
		glTexSubImage2D(GL_TEXTURE_2D,0,0,0,smFrameBuffer->getWidth(),smFrameBuffer->getHeight(),GL_RGBA,GL_UNSIGNED_BYTE,smFrameBuffer->colorBuffer.getDataPointer());

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		int scale = glcontextWidth/smFrameBuffer->getWidth();
		if (scale>=CG_GRID_MIN_SCALE) {
			// draw the grid
			vaoGrid.bind();
			glUseProgram(programGrid);
			glDrawArrays(GL_LINES, 0, 2*(smFrameBuffer->getWidth()+1+smFrameBuffer->getHeight()+1));
			vaoQuad.bind();
			glUseProgram(programTexturedQuad);
		}
	}

	glfwSwapBuffers(smWindow);
}

void CG1Helper::m_reshapeFunc(GLFWwindow *win, int w, int h)
{
	// We allow only full multiples of the framebuffer size smaller than the given size:
	int scale = w/smFrameBuffer->getWidth();
	int fw=smFrameBuffer->getWidth();
	int fh=smFrameBuffer->getHeight();
	if(scale<=0)
	       scale = 1;

	glcontextWidth  = scale*fw;
	glcontextHeight = scale*fh;
	if (scale >= CG_GRID_MIN_SCALE) {
		glcontextWidth++;
		glcontextHeight++;
	}

	if ( (w != glcontextWidth) || (h != glcontextHeight) ) {
		glfwSetWindowSize(win, glcontextWidth, glcontextHeight);
	}
}

void CG1Helper::m_frambufferResizeFunc(GLFWwindow *win, int w, int h)
{
	glViewport (0, 0, w, h);
	glUseProgram(programGrid);
	glUniform2f(loc_dims, (GLfloat)w,(GLfloat)h);
	glUseProgram(programTexturedQuad);
}

void CG1Helper::m_keyboardFunc (GLFWwindow *win, int key, int scancode, int action, int mods)
{
	if (key < 0 || key > GLFW_KEY_LAST) {
		return;
	}

	switch (key) {
		case GLFW_KEY_ESCAPE:
			key=CG_KEY_ESCAPE;
			break;
		case GLFW_KEY_LEFT:
			key=CG_KEY_LEFT;
			break;
		case GLFW_KEY_RIGHT:
			key=CG_KEY_RIGHT;
			break;
		case GLFW_KEY_UP:
			key=CG_KEY_UP;
			break;
		case GLFW_KEY_DOWN:
			key=CG_KEY_DOWN;
			break;
		case GLFW_KEY_PAGE_UP:
			key=CG_KEY_PAGE_UP;
			break;
		case GLFW_KEY_PAGE_DOWN:
			key=CG_KEY_PAGE_DOWN;
			break;
		case GLFW_KEY_HOME:
			key=CG_KEY_HOME;
			break;
		case GLFW_KEY_INSERT:
			key=CG_KEY_INSERT;
			break;
		case GLFW_KEY_LEFT_SHIFT:
		case GLFW_KEY_RIGHT_SHIFT:
			key=CG_KEY_SHIFT;
			break;
		case GLFW_KEY_LEFT_ALT:
		case GLFW_KEY_RIGHT_ALT:
			key=CG_KEY_ALT;
			break;
		case GLFW_KEY_LEFT_CONTROL:
		case GLFW_KEY_RIGHT_CONTROL:
			key=CG_KEY_CONTROL;
			break;
	}

	if ( !(mods & GLFW_MOD_SHIFT) && key > 0 && key < 256) {
		key = tolower(key);
	}

	if (action == GLFW_RELEASE) {
		pressedKeys[key] = false;
		releasedKeys[key] = true;
	} else {
		if (!pressedKeys[key]) {
			switch (key) {
				case CG_KEY_ESCAPE:
					glfwSetWindowShouldClose(win,1);
					break;
			}
		}
		pressedKeys[key] = true;
	}

}

GLFWwindow *CG1Helper::smWindow=NULL;
CGFrameBuffer* CG1Helper::smFrameBuffer = 0;
CGContext* CG1Helper::smContext = 0;
void (*CG1Helper::smProgramStep)() = 0;
GLuint CG1Helper::FBTexture = 0;
GLuint CG1Helper::programTexturedQuad = 0;
GLuint CG1Helper::vboQuad = 0;
GLuint CG1Helper::programGrid = 0;
GLuint CG1Helper::vboGrid = 0;
GLint CG1Helper::loc_dims = -1;
int CG1Helper::glcontextWidth = 0;
int CG1Helper::glcontextHeight = 0;
bool CG1Helper::pressedKeys[GLFW_KEY_LAST+1] = {0,};
bool CG1Helper::releasedKeys[GLFW_KEY_LAST+1] = {0,};
CGLCaps CG1Helper::glcaps;
CGLVAO CG1Helper::vaoQuad;
CGLVAO CG1Helper::vaoGrid;


