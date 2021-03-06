#include "CGContext.h"
#include <iostream>
#include <cassert>
#include <algorithm>
#include <utility>

//---------------------------------------------------------------------------
// CGCONTEXT (FRAGMENT OPERATIONS)
//---------------------------------------------------------------------------
void CGContext::m_cgPushFragment(CGFragmentData& fragment)
{
	// Wait for enough fragments to process.
	m_pipelineFragments[m_pipelineFragmentsCount++] = fragment;
	if(m_pipelineFragmentsCount == CG_MAX_FRAGMENTS_IN_PIPELINE)
		m_cgFlushFragments();
}
//---------------------------------------------------------------------------
void CGContext::m_cgFlushFragments()
{
	// Run fragment pipeline components for each fragment.
	// Uncomment following line for parallel fragment processing (if OpenMP is activated)

	//#pragma omp parallel for
	for(int i=0; i<m_pipelineFragmentsCount; ++i)
		m_cgFragmentPipeline(m_pipelineFragments[i]);

	// All fragments processed, clear pipeline.
	m_pipelineFragmentsCount = 0;
}
//---------------------------------------------------------------------------
void CGContext::m_cgFragmentPipeline(CGFragmentData& fragment)
{
	if(!m_cgFragmentClipping(fragment))
		return;

	m_cgFragmentProgram(fragment);
	if(m_capabilities.depthTest && !m_cgFragmentZTest(fragment))
		return;

	if(m_capabilities.blend)
		m_cgFragmentBlending(fragment);

	m_cgFragmentWriteBuffer(fragment);
}
//---------------------------------------------------------------------------
bool CGContext::m_cgFragmentClipping(const CGFragmentData& fragment)
{
	// returns true if fragment is inside the framebuffer bounds
	// ...
	return true;
}
//---------------------------------------------------------------------------
void CGContext::m_cgFragmentProgram(CGFragmentData& fragment)
{
	// Run programmable fragment processing.
	CGVec4 color;
	m_fragmentProgram(fragment, color, m_uniforms);
	fragment.varyings[CG_COLOR_VARYING] = color;
}
//---------------------------------------------------------------------------
bool CGContext::m_cgFragmentZTest(const CGFragmentData& fragment)
{
	//returns true if the fragment is visible
	// ...
	return true;
}
//---------------------------------------------------------------------------
void CGContext::m_cgFragmentBlending(CGFragmentData& fragment)
{
	// ...
}
//---------------------------------------------------------------------------
void CGContext::m_cgFragmentWriteBuffer(const CGFragmentData& fragment)
{
	// Write the current fragment into the framebuffer.
	// color into color buffer
	m_frameBuffer.colorBuffer.set(fragment.coordinates[0],
								  fragment.coordinates[1],
								  fragment.varyings[CG_COLOR_VARYING].elements);

	// window space z coordinate into depth buffer
	// ...
}
//---------------------------------------------------------------------------
