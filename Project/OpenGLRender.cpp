#include "OpenGLRender.h"
#include "OpenGLPainter.h"
#include <GL/glew.h>

OpenGLRender::OpenGLRender()
{
	auto vsource = R"(
		#version 450 core
		layout (location = 0) in vec2 _point;
		layout (location = 1) in vec2 _uv;
		out vec2 uv;

		void main()
		{
			uv = _uv;
			gl_Position = vec4(_point, 0.0, 1.0);
		}
	)";

	auto fsource = R"(
		#version 450 core
		in vec2 uv;
		layout (location = 0) out vec4 color;
		layout (binding = 0) uniform sampler2D texture0;

		void main()
		{
			color = vec4(texture(texture0, uv).rgb,1);
		}
	)";

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

	auto shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vshader);
	glAttachShader(shaderProgram, fshader);
	glLinkProgram(shaderProgram);

	// 检查链接错误  
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		GLchar infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "Shader program linking failed: " << infoLog << std::endl;
		glDeleteProgram(shaderProgram); // 删除程序，防止内存泄漏  
		::exit(-1);
	}

	// 删除着色器对象
	glDeleteShader(vshader);
	glDeleteShader(fshader);

	m_NativeProgram = shaderProgram;

	// 1. 生成VAO  
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// 3. 配置顶点缓冲区（假设你已经有了一个VBO和顶点数据）  
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(primitive_t) * 1024, nullptr, GL_DYNAMIC_DRAW);

	m_NativeBuffer = vbo;

	// 4. 设置顶点属性指针（例如，位置属性）  
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 5. 解绑VAO  
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

	m_PrimitiveList.clear();
	for (size_t i = 0; i < data.size(); ++i)
	{
		auto painter = RmCast<OpenGLPainter>(data[i].Painter);
		auto primitive = data[i].Primitive;
		if (painter == nullptr || primitive.size() == 0) continue;

		auto texture = painter->getTextureUpdated();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		for (size_t n = 0; n < primitive.size(); ++n)
		{
			m_PrimitiveList.emplace_back(primitive_t{
				2 * (primitive[n].P0.X / client.W) - 1.0f,2 * (primitive[n].P0.Y / client.H) - 1.0f,primitive[n].P0.U,primitive[n].P0.V,
				});
			m_PrimitiveList.emplace_back(primitive_t{
				2 * (primitive[n].P1.X / client.W) - 1.0f,2 * (primitive[n].P1.Y / client.H) - 1.0f,primitive[n].P1.U,primitive[n].P1.V,
				});
			m_PrimitiveList.emplace_back(primitive_t{
				2 * (primitive[n].P2.X / client.W) - 1.0f,2 * (primitive[n].P2.Y / client.H) - 1.0f,primitive[n].P2.U,primitive[n].P2.V,
				});
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_NativeBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(primitive_t) * m_PrimitiveList.size(), m_PrimitiveList.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDrawArrays(GL_TRIANGLES, 0, m_PrimitiveList.size());
	}
	glBindVertexArray(0);
	glUseProgram(0);
}