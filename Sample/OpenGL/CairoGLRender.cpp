/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#ifdef OPENUI_ENABLE_OPENGL
#include <gl/glew.h>
#include "CairoGLRender.h"
#include "../Cairo/CairoUIPainter.h"

static auto vsource = R"(
#version 330
layout (location = 0) in vec2 _point;
layout (location = 1) in uint _index;
out vec2 uv;
flat out uint index;
void main()
{
	index = _index;
	uv = vec2(_point.x, 1.0-_point.y);
	gl_Position = vec4(2*_point-1, 0.0, 1.0);
}
)";

static auto fsource = R"(
#version 330
in vec2 uv;
flat in uint index;
layout (location = 0) out vec4 color;
uniform sampler2D textureList[16];
void main()
{
	color = texture(textureList[index], uv);
}
)";

CairoGLRender::CairoGLRender(uint32_t width, uint32_t height)
{
	auto vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &vsource, nullptr);
	glCompileShader(vshader);
	GLint success;
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(vshader, 512, nullptr, infoLog);
		glDeleteShader(vshader);
		UI_FATAL("Shader compilation failed: %s", infoLog);
	}
	auto fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &fsource, nullptr);
	glCompileShader(fshader);
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(fshader, 512, nullptr, infoLog);
		glDeleteShader(fshader);
		UI_FATAL("Shader compilation failed: %s", infoLog);
	}
	auto shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vshader);
	glAttachShader(shaderProgram, fshader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		glDeleteProgram(shaderProgram); // 删除程序，防止内存泄漏
		UI_FATAL("Shader program linking failed: %s", infoLog);
	}
	glDeleteShader(vshader);
	glDeleteShader(fshader);
	m_NativeProgram = shaderProgram;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(primitive_t) * 1024, nullptr, GL_DYNAMIC_DRAW);
	m_NativeBuffer = vbo;

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(primitive_t), (void*)offsetof(primitive_t, X));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(primitive_t), (void*)offsetof(primitive_t, Index));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	m_NativePrimitive = vao;

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

CairoGLRender::~CairoGLRender()
{
	glDeleteTextures(1, &m_NativeTexture); m_NativeTexture = 0;
	glDeleteVertexArrays(1, &m_NativePrimitive); m_NativePrimitive = 0;
	glDeleteBuffers(1, &m_NativeBuffer); m_NativeBuffer = 0;
	glDeleteProgram(m_NativeProgram); m_NativeProgram = 0;
}

void CairoGLRender::render(UIRect client, UIListView<UIPrimitive> data)
{
	glUseProgram(m_NativeProgram);
	glBindVertexArray(m_NativePrimitive);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_NativeTexture);

	for (size_t i = 0; i < data.size(); ++i)
	{
		auto primitive = data[i].Primitive;
		auto painter = UICast<CairoUIPainter>(data[i].Painter);
		if (primitive.empty() || painter == nullptr) continue;

		auto width = painter->getWidth();
		auto height = painter->getHeight();
		auto pixels = painter->getPixels().data();
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pixels);

		m_PrimitiveList.clear();
		for (size_t k = 0; k < primitive.size(); ++k)
		{
			m_PrimitiveList.push_back({primitive[k].P0.X, primitive[k].P0.Y, 0});
			m_PrimitiveList.push_back({primitive[k].P1.X, primitive[k].P1.Y, 0});
			m_PrimitiveList.push_back({primitive[k].P2.X, primitive[k].P2.Y, 0});
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_NativeBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(primitive_t) * m_PrimitiveList.size(), m_PrimitiveList.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, m_PrimitiveList.size());
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}
#endif