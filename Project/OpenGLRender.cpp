#include "OpenGLRender.h"
#include "OpenGLPainter.h"
#include <GL/glew.h>

OpenGLRender::OpenGLRender()
{
	auto vsource = R"(
		#version 330 core  
		layout (location = 0) in vec3 aPos;  
  
		void main()  
		{  
			gl_Position = vec4(aPos, 1.0);  
		}
	)";

	auto fsource = R"(
		#version 330 core  
		out vec4 FragColor;  
  
		void main()  
		{  
			FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);  
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
}

OpenGLRender::~OpenGLRender()
{
	glDeleteProgram(m_NativeProgram);
	m_NativeProgram = 0;
}

void OpenGLRender::render(RmArrayView<RmPrimitive> data)
{
	for (size_t i = 0; i < data.size(); ++i)
	{
		auto painter = RmCast<OpenGLPainter>(data[i].Painter);
		auto primitive = data[i].Primitive;
		if (painter == nullptr || primitive.size() == 0) continue;
		auto texture = painter->getTextureUpdated();

		glUseProgram(m_NativeProgram);
		// TODO
		glUseProgram(0);
	}
}