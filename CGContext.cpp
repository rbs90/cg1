#include "CGContext.h"

#include "CG1Helper.h"    // We need to speak with the helper during initialization.
#include <iostream>       // If we need to print something to the console...

//---------------------------------------------------------------------------
// CGCONTEXT
//---------------------------------------------------------------------------
CGContext::CGContext(int width, int height)
{
	m_frameBuffer.allocate(width, height);

	// Default all internal states.
	for (int i=0; i<CG_ATTRIBUTE_COUNT; i++)
		m_pVertexAttributePointer[i] = NULL;
	m_clearColor[0] = 0;
	m_clearColor[1] = 0;
	m_clearColor[2] = 0;
	m_clearColor[3] = 1;
	m_polygonMode=CG_FILL;

	m_pipelineFragmentsCount = 0;

	m_capabilities.blend         = false;
	m_capabilities.depthTest     = false;
	m_capabilities.cullFace      = false;
	m_capabilities.useBresenham  = false;

	cgViewport(0,0,width,height);

	// And make our framebuffer known to the helper.
	CG1Helper::AttachSoftwareFrameBuffer(&m_frameBuffer);

	m_textures.texturePool = NULL;
	m_textures.texturePoolSize = 0;
	for (int i=0; i<CG_MAX_TEXTURE_UNITS; i++)
		m_textures.textureUnits[i]=-1;

	m_vertexProgram = NULL;
	m_fragmentProgram = NULL;
}

//---------------------------------------------------------------------------
CGContext::~CGContext()
{
	free(m_textures.texturePool);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// TEXTURE MODIFIERS
//---------------------------------------------------------------------------
int CGContext::cgGenTexture_cgBindTexture_cgTexImage2D(int width, int height, const unsigned char* data4ub)
{
	++m_textures.texturePoolSize;
	m_textures.texturePool = (CGTexture2D*) realloc(m_textures.texturePool,
	sizeof(CGTexture2D)*m_textures.texturePoolSize);
	m_textures.texturePool[m_textures.texturePoolSize-1] = CGTexture2D();
	m_textures.texturePool[m_textures.texturePoolSize-1].allocate(width, height, data4ub);
	return m_textures.texturePoolSize-1; // return index as texture name.
}
//---------------------------------------------------------------------------
void CGContext::cgActiveTexture_cgBindTexture_cgTexParameter(int textureUnit, int textureName, int filter, int wrap)
{
	if (textureUnit < 0 || textureUnit >= CG_MAX_TEXTURE_UNITS) {
		std::cerr << "invalid texture unit" << std::endl;
		return;
	}
	if (textureName >= m_textures.texturePoolSize) {
		std::cerr << "invalid texture name" << std::endl;
		return;
	}
	// Activate a texture unit and bind (name) a texture
	m_textures.textureUnits[textureUnit] = textureName;
	// set texture parameter
	if (textureName >= 0) {
		m_textures.texturePool[textureName].setFilterMode(filter);
		m_textures.texturePool[textureName].setWrapMode(wrap);
	}
}
//---------------------------------------------------------------------------
void CGContext::cgClearColor(float red, float green, float blue, float alpha)
{
	m_clearColor.set(red, green, blue, alpha);
}
//---------------------------------------------------------------------------
void CGContext::cgClear(int bitfield)
{
	if (bitfield&CG_COLOR_BUFFER_BIT) m_frameBuffer.colorBuffer.clear(m_clearColor.elements);

	// clear the depth buffer
	// ...
}
//---------------------------------------------------------------------------
void CGContext::cgVertexAttribPointer(CGenum attribute,
                                      const float *data)
{
	if (attribute < CG_ATTRIBUTE_COUNT)
		m_pVertexAttributePointer[attribute] = data;
}
//---------------------------------------------------------------------------
void CGContext::cgDrawArrays(CGenum mode, int first, int count)
{
	if(!m_cgValidateState())
		return;

	// Set primitive mode.
	m_primitiveMode=mode;

	// Reset rasterizer state.
	m_pipelineVerticesCount = 0;
  
	// CG RENDER PIPELINE
	// For all vertices.
	for(int iVertex=0; iVertex<count; iVertex++) {
    
		// Pull next vertex from vertex data.
		m_cgPullVertex(first+iVertex);
    
		// Start Pipeline (including rasterizer with fragment pipeline).
		m_cgVertexPipeline();
	}
}
//---------------------------------------------------------------------------
// STATE MODIFIERS
//---------------------------------------------------------------------------
void CGContext::cgEnable(CGenum cap)
{
	switch(cap) {
		case CG_CULL_FACE:      m_capabilities.cullFace     = true; break;
		case CG_DEPTH_TEST:     m_capabilities.depthTest    = true; break;
		case CG_BLEND:          m_capabilities.blend        = true; break;
		case CG_USE_BRESENHAM:  m_capabilities.useBresenham = true; break;
	}
}
//---------------------------------------------------------------------------
void CGContext::cgDisable(CGenum cap)
{
	switch(cap) {
		case CG_CULL_FACE:      m_capabilities.cullFace     = false; break;
		case CG_DEPTH_TEST:     m_capabilities.depthTest    = false; break;
		case CG_BLEND:          m_capabilities.blend        = false; break;
		case CG_USE_BRESENHAM:  m_capabilities.useBresenham = false; break;
	}
}
//---------------------------------------------------------------------------
void CGContext::cgPolygonMode(CGenum mode)
{
	if (mode == CG_FILL || mode == CG_LINE)
		m_polygonMode=mode;
}
//---------------------------------------------------------------------------
void CGContext::cgUseProgram(CGVertexProgram vertexProgram, CGFragmentProgram fragmentProgram)
{
	m_vertexProgram = vertexProgram;
	m_fragmentProgram = fragmentProgram;
}
//---------------------------------------------------------------------------
// UNIFORM MODIFIERS
//---------------------------------------------------------------------------
void CGContext::cgUniform1i(int location, int value)
{
	switch(location) {
		case CG_ULOC_SAMPLER: m_uniforms.sampler.unit=value; break;
		// case ULOC_?
	}
}
//---------------------------------------------------------------------------
void CGContext::cgUniform1fv(int location, int count, const float* value)
{
	if(count == 1) {
		switch(location) {
			case CG_ULOC_MATERIAL_SHININESS: m_uniforms.materialShininess=value[0]; break;
			// case ULOC_?
		}
	}
	else {
		// Loop over all values and assign to m_uniforms.???[i] = value[i]
	}
}
//---------------------------------------------------------------------------
void CGContext::cgUniform4fv(int location, int count, const float* value)
{
	if(count == 1) {
		CGVec4 vec4; vec4.set(value[0],value[1],value[2],value[3]);
		switch(location) {
			case CG_ULOC_LIGHT0_AMBIENT: m_uniforms.light0Ambient = vec4; break;
			case CG_ULOC_LIGHT0_DIFFUSE: m_uniforms.light0Diffuse = vec4; break;
			case CG_ULOC_LIGHT0_SPECULAR: m_uniforms.light0Specular = vec4; break;
			case CG_ULOC_LIGHT0_POSITION: m_uniforms.light0Position = vec4; break;
			case CG_ULOC_MATERIAL_AMBIENT: m_uniforms.materialAmbient = vec4; break;
			case CG_ULOC_MATERIAL_DIFFUSE: m_uniforms.materialDiffuse = vec4; break;
			case CG_ULOC_MATERIAL_SPECULAR: m_uniforms.materialSpecular = vec4; break;
			case CG_ULOC_MATERIAL_EMISSION: m_uniforms.materialEmission = vec4; break;
			// case ULOC_?
		}
	}
	else {
		// If count > 0 specified, create vec4 with value+4*i for each i<count
		// and assign to m_uniforms.???[i].
	}
}
//---------------------------------------------------------------------------
void CGContext::cgUniformMatrix4fv(int location, int count, bool transpose, const float* value)
{
	if(count == 1) {
		CGMatrix4x4 mat; mat.setFloatsFromColMajor(value);
		if(transpose) mat.transpose();
		switch(location) {
			case CG_ULOC_MODELVIEW_MATRIX: m_uniforms.modelviewMatrix = mat; break;
			case CG_ULOC_PROJECTION_MATRIX: m_uniforms.projectionMatrix = mat; break;
			case CG_ULOC_NORMAL_MATRIX: m_uniforms.normalMatrix = mat; break;
		}
	} else {
		// If count > 0 specified, create matrix with value+16*i for each i<count
		// and assign to m_uniforms.???[i].
	}
}
//---------------------------------------------------------------------------
// VIEWPORT MODIFIERS
//---------------------------------------------------------------------------
void CGContext::cgViewport(int x, int y, int w, int h)
{
	m_viewport[0]=x;
	m_viewport[1]=y;
	m_viewport[2]=w;
	m_viewport[3]=h;
}
//---------------------------------------------------------------------------
// FRAMEBUFFER PIXEL DATA READBACK
//---------------------------------------------------------------------------
void CGContext::cgReadPixels(unsigned char *data) const
{
	int W = m_frameBuffer.getWidth();
	int H = m_frameBuffer.getHeight();
	memcpy(data,m_frameBuffer.colorBuffer.getDataPointer(),W*H*4);
}
//---------------------------------------------------------------------------
// PIPELINE COMPONENTS: see separate CGContext_*.cpp files.
//---------------------------------------------------------------------------
bool CGContext::m_cgValidateState()
{
	// Have vertex positions?
	if(!m_pVertexAttributePointer[CG_POSITION_ATTRIBUTE])
		return false;

	// Have valid shader programs?
	if(!m_vertexProgram || !m_fragmentProgram)
		return false;

	// Prepare texture sampler.
	m_uniforms.sampler.texture = (m_uniforms.sampler.unit>=0 && 
		m_uniforms.sampler.unit<CG_MAX_TEXTURE_UNITS && 
		m_textures.textureUnits[m_uniforms.sampler.unit]>=0 && 
		m_textures.textureUnits[m_uniforms.sampler.unit]<m_textures.texturePoolSize) ? 
		&m_textures.texturePool[m_textures.textureUnits[m_uniforms.sampler.unit]] : NULL;

	return true;
}
