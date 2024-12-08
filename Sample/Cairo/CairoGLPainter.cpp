#include "CairoGLPainter.h"
#include <GL/glew.h>

CairoGLPainter::CairoGLPainter(uint32_t width, uint32_t height)
	:
	CairoUIPainter(width, height)
{
	uint32_t texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_NativeTexture = texture;
}

CairoGLPainter::~CairoGLPainter()
{
	glDeleteTextures(1, &m_NativeTexture);
	m_NativeTexture = 0;
}

void CairoGLPainter::resize(uint32_t width, uint32_t height)
{
	CairoUIPainter::resize(width, height);

	glBindTexture(GL_TEXTURE_2D, m_NativeTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

uint32_t CairoGLPainter::getTexture() const
{
	glBindTexture(GL_TEXTURE_2D, m_NativeTexture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, getWidth(), getHeight(), GL_BGRA, GL_UNSIGNED_BYTE, getPixels().data());
	glBindTexture(GL_TEXTURE_2D, 0);
	return m_NativeTexture;
}
