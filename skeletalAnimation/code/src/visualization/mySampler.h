#ifndef MYSAMPLER_H
#define MYSAMPLER_H

#include <GL/glew.h>

class mySampler
{
public:

	GLuint m_textureSamplerID;

	mySampler()
	{

	}
	~mySampler()
	{
		glDeleteSamplers(1,&m_textureSamplerID);
	}
	void createSampler()
	{
		glCreateSamplers(1, &m_textureSamplerID);
	}
	void setUpSampler()
	{
		glSamplerParameteri(m_textureSamplerID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(m_textureSamplerID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		// sharp texture
		//		glSamplerParameteri(_textureSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		//		glSamplerParameteri(_textureSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// blurred texture
		glSamplerParameteri(m_textureSamplerID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glSamplerParameteri(m_textureSamplerID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// improve blur quality by enabling anysotropic filtering
		GLint maxAnisotropy = 0;
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
		glSamplerParameteri(m_textureSamplerID, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
	}
	

};


#endif // MYSAMPLER_H