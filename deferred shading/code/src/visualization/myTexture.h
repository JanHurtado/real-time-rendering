#ifndef MYTEXTURE_H
#define MYTEXTURE_H

#include <GL/glew.h>
#include "CImg.h"

using namespace cimg_library;

class myTexture
{
	//CImg<unsigned char> image("lambertian.jpg");
public:
	GLuint m_textureID;
	GLsizei m_width;
	GLsizei m_height;
	GLsizei m_numLevels;
	GLubyte * m_texels;

	myTexture(const char * file_name)
	{
		CImg<unsigned char> image(file_name);

		m_width = image.width();
		m_height = image.height();
		m_numLevels = 1 + std::floor(std::log2(std::max(m_width, m_height)));

		m_texels = new GLubyte[m_width*m_height * 4];

		int pos = 0;
		for (int r = m_height - 1; r >= 0; r--)
		{
			for (int c = 0; c < m_width; c++)
			{

				m_texels[pos] = (int)image(c, r, 0, 0);
				pos++;
				m_texels[pos] = (int)image(c, r, 0, 1);
				pos++;
				m_texels[pos] = (int)image(c, r, 0, 2);
				pos++;
				m_texels[pos] = 255;
				pos++;
			}
		}
	}

	~myTexture()
	{
		glDeleteTextures(1, &m_textureID);
		delete[] m_texels;
	}

	void createTexture()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);
	}

	void setUpTexture()
	{
		// allocate immutable memory (including mipmaps)
		glTextureStorage2D(m_textureID, m_numLevels, GL_RGBA8, m_width, m_height);

		// transfer data to GPU
		glTextureSubImage2D(m_textureID, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, m_texels); // level = 0, xoffset = 0, yoffset = 0

		// generate the mipmaps
		glGenerateTextureMipmap(m_textureID);
	}
};

#endif // MYTEXTURE_H