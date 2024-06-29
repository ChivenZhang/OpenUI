#include "OpenGLRender.h"
#include "OpenGLPainter.h"
#include <GL/glew.h>

OpenGLRender::OpenGLRender()
{
	auto vsource = R"(
		#version 450 core
		layout (location = 0) in vec2 _point;
		layout (location = 1) in vec2 _uv;
		layout (location = 2) in uint _index;
		out vec2 uv;
		flat out uint index;

		void main()
		{
			uv = _uv;
			index = _index;
			gl_Position = vec4(_point, 0.0, 1.0);
		}
	)";

	auto fsource = R"(
		#version 450 core
		in vec2 uv;
		flat in uint index;
		layout (location = 0) out vec4 color;
		layout (binding = 0) uniform sampler2D textureList[32];

		void main()
		{
			color = texture(textureList[index], uv);
		}
	)";

	// 检查编译错误 
	auto vshader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vshader, 1, &vsource, NULL);
	glCompileShader(vshader);
	GLint success;
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(vshader, 512, NULL, infoLog);
		std::cerr << "Shader compilation failed: " << infoLog << std::endl;
		glDeleteShader(vshader); // 删除着色器，防止内存泄漏  
		::exit(-1);
	}

	// 检查编译错误 
	auto fshader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fshader, 1, &fsource, NULL);
	glCompileShader(fshader);
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(fshader, 512, NULL, infoLog);
		std::cerr << "Shader compilation failed: " << infoLog << std::endl;
		glDeleteShader(fshader); // 删除着色器，防止内存泄漏  
		::exit(-1);
	}

	// 检查链接错误  
	auto shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vshader);
	glAttachShader(shaderProgram, fshader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "Shader program linking failed: " << infoLog << std::endl;
		glDeleteProgram(shaderProgram); // 删除程序，防止内存泄漏  
		::exit(-1);
	}
	glDeleteShader(vshader);
	glDeleteShader(fshader);

	m_NativeProgram = shaderProgram;

	// 3. 配置顶点缓冲区
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(primitive_t) * 1024, nullptr, GL_DYNAMIC_DRAW);

	m_NativeBuffer = vbo;

	// 4. 设置顶点属性指针 
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(primitive_t), (void*)offsetof(primitive_t, XY));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(primitive_t), (void*)offsetof(primitive_t, UV));
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_INT, sizeof(primitive_t), (void*)offsetof(primitive_t, Index));
	glBindVertexArray(0);

	m_NativePrimitive = vao;
}

OpenGLRender::~OpenGLRender()
{
	glDeleteBuffers(1, &m_NativeBuffer); m_NativeBuffer = 0;
	glDeleteVertexArrays(1, &m_NativePrimitive); m_NativePrimitive = 0;
	glDeleteProgram(m_NativeProgram); m_NativeProgram = 0;
}

void OpenGLRender::render(RmRect client, RmArrayView<RmPrimitive> data)
{
	glUseProgram(m_NativeProgram);
	glBindVertexArray(m_NativePrimitive);

	int32_t maxTextureUnits = 16;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);
	for (size_t i = 0; i < data.size(); i += maxTextureUnits)
	{
		for (size_t k = 0; k < maxTextureUnits && i + k < data.size(); ++k)
		{
			auto painter = RmCast<OpenGLPainter>(data[i + k].Painter);
			auto primitive = data[i].Primitive;
			if (painter == nullptr || primitive.size() == 0) continue;

			// 绑定到纹理数组
			auto texture = painter->getTextureUpdated();
			glActiveTexture(GL_TEXTURE0 + k);
			glBindTexture(GL_TEXTURE_2D, texture);

			for (size_t n = 0; n < primitive.size(); ++n)
			{
				m_PrimitiveList.emplace_back(primitive_t{
					2 * (primitive[n].P0.X / client.W) - 1.0f, 2 * (primitive[n].P0.Y / client.H) - 1.0f, primitive[n].P0.U, primitive[n].P0.V, (uint32_t)k,
					});
				m_PrimitiveList.emplace_back(primitive_t{
					2 * (primitive[n].P1.X / client.W) - 1.0f, 2 * (primitive[n].P1.Y / client.H) - 1.0f, primitive[n].P1.U, primitive[n].P1.V, (uint32_t)k,
					});
				m_PrimitiveList.emplace_back(primitive_t{
					2 * (primitive[n].P2.X / client.W) - 1.0f, 2 * (primitive[n].P2.Y / client.H) - 1.0f, primitive[n].P2.U, primitive[n].P2.V, (uint32_t)k,
					});
			}
		}

		// 更新顶点缓冲区
		glBindBuffer(GL_ARRAY_BUFFER, m_NativeBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(primitive_t) * m_PrimitiveList.size(), m_PrimitiveList.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// 渲染控件视图
		glDrawArrays(GL_TRIANGLES, 0, m_PrimitiveList.size());
	}

	glBindVertexArray(0);
	glUseProgram(0);
}