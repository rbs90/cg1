//This implements some OpenGL-related helper functionality.
//It should not be neccessary to change anything in here or to look at it at all.
#ifndef GLUTIL_H
#define GLUTIL_H

#include <iostream>
#include <cstring>

#include "glew.h"

/* we use a global attribute pointer mapping */
typedef enum {
	TGL_ATTR_POSITION=0,
	TGL_ATTR_NORMAL,
	TGL_ATTR_COLOR,
	TGL_ATTR_TEXCOORD,
	// always add new attributes here
	// ..
	TGL_ATTR_MAX
} TGLAttr;

class CGLHelper {
	private:
		static void version_maj_min(const char *str, GLint *version) {
			version[0]=0;
			version[1]=0;
			if (str) {
				char *ptr;
				version[0]=(GLint)strtoul((char*)str, &ptr, 10);
				if (*ptr) {
					if (*ptr=='.') {
						version[1]=(GLint)strtoul(ptr+1, &ptr, 10);
					}
				}
			}
		}
	public:
		/* Check for GL errors */
		static GLenum CheckGLErrors(const char *prefix=NULL, bool warn=true) {
			GLenum glerr;
			GLenum first=GL_NO_ERROR;

			while ( (glerr = glGetError()) != GL_NO_ERROR ) {
				if (first == GL_NO_ERROR) {
					first = glerr;
				}
				if (warn) {
					std::cerr << ((prefix)?prefix:"") << " GL error " << glerr << " occured" << std::endl;
				}
			}
			return first;
		}
		/* Check the GL version */
		static void GetGLVersion(GLint *gl_version, GLint *glsl_version)
		{
			const GLubyte *version;
			version=glGetString(GL_VERSION);
			version_maj_min((const char *)version, gl_version);
			version=glGetString(GL_SHADING_LANGUAGE_VERSION);
			version_maj_min((const char *)version, glsl_version);
		}

		/* Check if an extension is supported 
		 * we cannot use GLEW, since glew in experimental mode will return bullshit
		 * in core profiles...
		 */
		static bool CheckGLExtension(const char *name, const GLint *gl_version=NULL) {
			GLint max_extensions=-1;
			bool skip_check=false;
			CheckGLErrors("before extension check");
			if (gl_version) {
			       if (gl_version[0] < 3) {
				       // the newer extension stuff is not available
				       skip_check=true;
			       }
			}
			if (!skip_check) {
				glGetIntegerv(GL_NUM_EXTENSIONS, &max_extensions);
				if (CheckGLErrors(NULL,false) != GL_NO_ERROR) {
					// probably to old for glGetStringi
					max_extensions=-1;
				}
			}
			if (!glGetStringi) {
				max_extensions=-1;
			}
			if (max_extensions > 0) {
				GLint i;
				for (i=0; i<max_extensions; i++) {
					const char *ext=(const char *)glGetStringi(GL_EXTENSIONS, i);
					if (ext && !strcmp(ext, name)) {
						return true;
					}
				}
			} else {
				// deprecated method
				const char *exts=(const char*)glGetString(GL_EXTENSIONS);
				if (exts && strstr(exts,name)) {
					return true;
				}
			}
			return false;
		}
};

/* Wrapper class to track the state a vertex attribute array */
class CGLAttribPtr {
	public:
		bool enabled;
		GLuint index;
		GLint size;
		GLenum type;
		GLboolean normalized;
		GLsizei stride;
		const GLvoid *ptr;
		GLuint vbo;
		CGLAttribPtr() {
			enabled=false;
			vbo=0;
		}
		void set_index(GLuint idx) {
			index=idx;
		}
		void AttribPointer(GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *ptr, GLuint vbo)
		{
			this->size=size;
			this->type=type;
			this->normalized=normalized;
			this->stride=stride;
			this->ptr=ptr;
			this->vbo=vbo;	
		}
		void applyPointer(GLuint& cur_buf) const {
			if (cur_buf != vbo) {
				glBindBuffer(GL_ARRAY_BUFFER, vbo);
				cur_buf=vbo;
			}
			glVertexAttribPointer(index, size, type, normalized, stride, ptr);
		}
		void applyEnable() const {
			glEnableVertexAttribArray(index);
		}
		void applyDisable() const {
			glDisableVertexAttribArray(index);
		}
		void apply(GLuint& cur_buf) const {
			if (enabled) {
				applyPointer(cur_buf);
				applyEnable();
			} else {
				applyDisable();
			}
		}
		void disable() {
			enabled=false;
		}
		void enable() {
			enabled=true;
		}
		void set_enable(bool v) {
			enabled=v;
		}
};

/* Wrapper class around VAOs
 * Will use VAOs internally if allowed to.
 * usage: (at setup)
 *     create(); // will implicitely bind vao
 *     BindArrayBuffer(...);
 *     VertexAttribPointer(...);
 *     EnableVerrextAttribArray(...);
 *     [...]
 *     unbind();
 *
 * later at use:
 *     bind();
 *     [... glDraw ...]
 *     [unbind();]
 */
class CGLVAO {
	private:
		GLuint vao;
		GLuint ibo;
		GLuint cur_vbo;
		CGLAttribPtr attr[TGL_ATTR_MAX];
	public:
		CGLVAO() {
			vao=0;
			ibo=0;
			cur_vbo=0;
			for (GLuint i=0; i<(GLuint)TGL_ATTR_MAX; i++) {
				attr[i].set_index(i);
			}	
		}
		void destroy() {
			if (vao) {
				glBindVertexArray(0);
				glDeleteVertexArrays(1,&vao);
				vao=0;
			}
			for (int i=0; i<(int)TGL_ATTR_MAX; i++) {
				attr[i].disable();
			}	
		}
		void create(bool have_vao) {
			if (have_vao) {
				glGenVertexArrays(1,&vao);
				if (vao) {
					glBindVertexArray(vao);
				}
			}
		}
		void bind() const {
			if (vao) {
				glBindVertexArray(vao);
			} else {
				// fallback code
				GLuint buf=0;
				for (int i=0; i<(int)TGL_ATTR_MAX; i++) {
					attr[i].apply(buf);
				}	
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
			}
		}
		void BindVertexArray() const {
			bind();
		}
		void unbind() const {
			if (vao) {
				glBindVertexArray(0);
			} else {
				// fallback code
				for (GLuint i=0; i<(GLuint)TGL_ATTR_MAX; i++) {
					glDisableVertexAttribArray(i);
				}
				glBindBuffer(GL_ARRAY_BUFFER, 0);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		}
		void BindArrayBuffer(GLuint name) {
			cur_vbo=name;
			glBindBuffer(GL_ARRAY_BUFFER, cur_vbo);
		}
		void BindElementBuffer(GLuint name) {
			ibo=name;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		}
		void BindBuffer(GLenum target, GLuint name) {
			if (target == GL_ARRAY_BUFFER) {
				BindArrayBuffer(name);
			} else if (target == GL_ELEMENT_ARRAY_BUFFER) {
				BindElementBuffer(name);
			}
		}
		void VertexAttribPointer(GLuint index=0, GLint size=3, GLenum type=GL_FLOAT, GLboolean normalized=true, GLsizei stride=0, size_t offset=0)
		{
			attr[index].AttribPointer(size, type, normalized, stride, (const GLvoid*)((const char*)offset), cur_vbo);
			attr[index].applyPointer(cur_vbo);
		}
		void EnableVertexAttribArray(GLint index)
		{
			attr[index].enable();
			attr[index].applyEnable();
		}
		void DisableVertexAttribArray(GLint index)
		{
			attr[index].disable();
			attr[index].applyDisable();
		}
		~CGLVAO() {
			// destroy(); // disabled
		}
};

/* We use this to define which GL capabilities are available */

class CGLCaps {
	private:
		void info(unsigned int i, const char *str) const
	       	{
			if (i & (1<<SHADERS_150)) {
				std::cout << "GLSL 1.50 " << str << std::endl;
			} else if (i & (1<<SHADERS_110)) {
				std::cout << "legacy GLSL 1.10 " << str << std::endl;
			} else {
				std::cout << "no shaders " << str << std::endl;
			}
			std::cout << "VAOs " << ((i&(1<<VAO))?"":"NOT ") << str << std::endl;
		}

	public:
		typedef enum {
			SHADERS_150=0,
			SHADERS_110,
			VAO
		} CapType;
		GLint version[2];
		GLint glsl_version[2];
		unsigned int caps;
		unsigned int caps_potentially;
		CGLCaps() {
			version[0]=1;
			version[1]=0;
			glsl_version[0]=0;
			glsl_version[1]=0;
			caps=caps_potentially=0;
		}
		void set(const GLint *v, const GLint *glsl_v) {
			version[0]=v[0];
			version[1]=v[1];
			glsl_version[0]=glsl_v[0];
			glsl_version[1]=glsl_v[1];
			caps_potentially=0;
			if (glsl_version[0] > 1 || (glsl_version[0] == 1 && glsl_version[1] >= 10)) {
				caps_potentially |= (1 << SHADERS_110);
			}
			if (glsl_version[0] > 1 || (glsl_version[0] == 1 && glsl_version[1] >= 50)) {
				caps_potentially |= (1 << SHADERS_150);
			}
			if (version[0] >= 3) {
				caps_potentially |= (1 << VAO);
			} else {
				if (CGLHelper::CheckGLExtension("GL_ARB_vertex_array_object", version)) {
					caps_potentially |= (1 << VAO);
				}
			}
			if (caps_potentially & (1<<VAO)) {
				if (!glGenVertexArrays) {
					std::cout << "glGenVertexArrays missing although it should be available" << std::endl;
					caps_potentially &= ~(1 << VAO);
				}
			}
			info(caps_potentially, "available");
			caps=caps_potentially;
		}
		void disable(CapType t) {
			caps &= ~(1<<t);
		}
		void enable(CapType t)
		{
			unsigned int v=(1<<t);
			if (caps_potentially & v) {
				caps |= v;
			}
		}
		void setEnabled(CapType t, bool enabled=true)
		{
			if (enabled) {
				enable(t);
			} else {
				disable(t);
			}
		}
		bool supports(CapType t) const {
			return  (caps & (1 << t))?true:false;
		}
		void info() const {
			info(caps,"enabled");
		}
};

#endif /* GL_UTIL_H */

