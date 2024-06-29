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
		uniform sampler2D textureList[32];

		void main()
		{
			color = texture(textureList[index], uv);
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

	// 2. 绑定VAO  
	glBindVertexArray(vao);

	// 3. 配置顶点缓冲区（假设你已经有了一个VBO和顶点数据）  
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, size, GL_STATIC_DRAW);

	// 4. 设置顶点属性指针（例如，位置属性）  
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// ... 可以设置更多的顶点属性 ...  

	// 5. 解绑VAO  
	glBindVertexArray(0);

	// 6. 在渲染时使用VAO  
	// 在渲染循环中：  
	glBindVertexArray(vao);
	// ... 绘制调用（如glDrawArrays或glDrawElements）...  
	glBindVertexArray(0);
}

OpenGLRender::~OpenGLRender()
{
	glDeleteProgram(m_NativeProgram);
	m_NativeProgram = 0;
}

void OpenGLRender::render(RmArrayView<RmPrimitive> data)
{
	int32_t maxTextureUnits = 16;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

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