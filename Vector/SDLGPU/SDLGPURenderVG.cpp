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
#include "SDLGPURenderVG.h"
#include <GL/glew.h>
#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_rect_pack.h>
#define OPENVG_MAX_TRIANGLE (1024000)
#define OPENVG_MAX_VERTEIES (3*OPENVG_MAX_TRIANGLE)

SDLGPURenderVG::SDLGPURenderVG()
{
	auto commons = R"(
		#define MAX_STOP_COUNT 16
		#define VG_FLAGS_IMAGE_COLOR 0x0001
		#define VG_FLAGS_IMAGE_GLYPH 0x0002
		#define VG_FLAGS_IMAGE_HWGPU 0x0004
		#define VG_FLAGS_STYLE_LINEAR 0x0010
		#define VG_FLAGS_STYLE_RADIAL 0x0020

		struct vertex_t
		{
			float X, Y, U, V;
			int Style, Matrix;
		};
		struct style_t
		{
			vec4 Color;
			int Flags;
			int Image;
			int Linear;
			int Radial;
		};
		struct linear_t
		{
			vec4  NumStops;
			vec2  GradStartPos;
			vec2  GradEndPos;
			vec4  StopPoints[MAX_STOP_COUNT / 4];
			vec4  StopColors[MAX_STOP_COUNT];
		};
		struct radial_t
		{
			vec4  NumStops;
			vec2  CenterPos;
			vec2  Radius;
			vec4  StopPoints[MAX_STOP_COUNT / 4];
			vec4  StopColors[MAX_STOP_COUNT];
		};
		struct matrix_t
		{
			vec4 Scissor;
			mat3 Transform;
		};

		layout(std430, binding=0) buffer STYLE_BLOCK
		{
			style_t StyleList[];
		};

		layout(std430, binding=1) buffer LINEAR_BLOCK
		{
			linear_t LinearList[];
		};

		layout(std430, binding=2) buffer RADIAL_BLOCK
		{
			radial_t RadialList[];
		};

		layout(std430, binding=3) buffer MATRIX_BLOCK
		{
			matrix_t MatrixList[];
		};

		layout (binding = 0) uniform sampler2D TextureList[16];

		uniform vec2 Viewport;
	)";

	auto vsource = VGString(R"(
		#version 450
		layout (location = 0) in vec2 _point;
		layout (location = 1) in vec2 _uv;
		layout (location = 2) in int _style;
		layout (location = 3) in int _matrix;
		out vec2 uv;
		out vec2 point;
		flat out int style;
		flat out int matrix;
		)") + commons + R"(
		void main()
		{
			style = _style;
			matrix = _matrix;
			mat3 transform = MatrixList[_matrix].Transform;
			point = vec2(transform * vec3(_point, 1.0));
			int flags = StyleList[style].Flags;
			if((flags & VG_FLAGS_IMAGE_COLOR) == VG_FLAGS_IMAGE_COLOR) uv = _uv;
			else uv = vec2(point.x * Viewport.x, 1.0 - point.y * Viewport.y);

			gl_Position = vec4(2 * point * Viewport - 1, 0.0, 1.0);
		}
	)";

	auto fsource = VGString(R"(
		#version 450
		in vec2 uv;
		in vec2 point;
		flat in int style;
		flat in int matrix;
		layout (location = 0) out vec4 color;
		)") + commons + R"(
		void main()
		{
			vec4 bounds = MatrixList[matrix].Scissor;
			if(0 <= bounds[2] || 0 <= bounds[3])
			{
				if(bounds[0] <= point.x && point.x <= bounds[0]+bounds[2] 
				&& bounds[1] <= point.y && point.y <= bounds[1]+bounds[3]);
				else discard;
			}
			color = StyleList[style].Color;
			int flags = StyleList[style].Flags;
			if((flags & VG_FLAGS_IMAGE_COLOR) == VG_FLAGS_IMAGE_COLOR) color *= texture(TextureList[0], uv);
		}
	)";

	// 检查编译错误 
	auto vshader = glCreateShader(GL_VERTEX_SHADER);
	auto source = vsource.c_str();
	glShaderSource(vshader, 1, &source, NULL);
	glCompileShader(vshader);
	GLint success;
	glGetShaderiv(vshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(vshader, 512, NULL, infoLog);
		glDeleteShader(vshader); // 删除着色器，防止内存泄漏  
		VG_FATAL("Shader compilation failed: %s", infoLog);
	}

	// 检查编译错误 
	auto fshader = glCreateShader(GL_FRAGMENT_SHADER);
	source = fsource.c_str();
	glShaderSource(fshader, 1, &source, NULL);
	glCompileShader(fshader);
	glGetShaderiv(fshader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		GLchar infoLog[512];
		glGetShaderInfoLog(fshader, 512, NULL, infoLog);
		glDeleteShader(fshader); // 删除着色器，防止内存泄漏  
		VG_FATAL("Shader compilation failed: %s", infoLog);
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
		glDeleteProgram(shaderProgram); // 删除程序，防止内存泄漏  
		VG_FATAL("Shader program linking failed: %s", infoLog);
	}
	glDeleteShader(vshader);
	glDeleteShader(fshader);

	m_NativeProgram = shaderProgram;

	// 配置顶点缓冲区
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	m_NativePrimitive = vao;

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VGPrimitive::point_t) * OPENVG_MAX_VERTEIES, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VGPrimitive::point_t), (void*)offsetof(VGPrimitive::point_t, X));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VGPrimitive::point_t), (void*)offsetof(VGPrimitive::point_t, U));
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(2, 1, GL_INT, sizeof(VGPrimitive::point_t), (void*)offsetof(VGPrimitive::point_t, Style));
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(3, 1, GL_INT, sizeof(VGPrimitive::point_t), (void*)offsetof(VGPrimitive::point_t, Matrix));
	glEnableVertexAttribArray(3);

	m_NativeVertexBuffer = vbo;

	// 配置全局缓冲区
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VGPrimitive::style_t) * OPENVG_MAX_TRIANGLE, nullptr, GL_DYNAMIC_DRAW);

	m_NativeStyleBuffer = vbo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VGPrimitive::linear_t) * OPENVG_MAX_TRIANGLE, nullptr, GL_DYNAMIC_DRAW);

	m_NativeLinearBuffer = vbo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VGPrimitive::radial_t) * OPENVG_MAX_TRIANGLE, nullptr, GL_DYNAMIC_DRAW);

	m_NativeRadialBuffer = vbo;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VGPrimitive::matrix_t) * OPENVG_MAX_TRIANGLE, nullptr, GL_DYNAMIC_DRAW);

	m_NativeMatrixBuffer = vbo;
}

SDLGPURenderVG::~SDLGPURenderVG()
{
	glDeleteProgram(m_NativeProgram); m_NativeProgram = 0;
	glDeleteBuffers(1, &m_NativeVertexBuffer); m_NativeVertexBuffer = 0;
	glDeleteBuffers(1, &m_NativeStyleBuffer); m_NativeStyleBuffer = 0;
	glDeleteBuffers(1, &m_NativeLinearBuffer); m_NativeLinearBuffer = 0;
	glDeleteBuffers(1, &m_NativeRadialBuffer); m_NativeRadialBuffer = 0;
	glDeleteBuffers(1, &m_NativeMatrixBuffer); m_NativeMatrixBuffer = 0;
	glDeleteVertexArrays(1, &m_NativePrimitive); m_NativePrimitive = 0;
}

void SDLGPURenderVG::render(VGRect client, VGListView<const VGPrimitive> data)
{
	m_PointList.clear(); m_StyleList.clear();
	m_LinearList.clear(); m_RadialList.clear();
	m_MatrixList.clear(); m_TextureList.clear();
	static int32_t maxTextureUnits = 16;
	if (maxTextureUnits == 0) glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnits);

	for (size_t i = 0, k = 0; i < data.size(); ++i)
	{
		k += data[i].PointList.size();
		if (i + 1 == data.size()) m_PointList.resize(k);
	}
	if (m_PointList.empty()) return;

	for (size_t i = 0; i < data.size(); ++i)
	{
		auto& points = data[i].PointList;
		auto& styles = data[i].StyleList;
		auto& images = data[i].ImageList;

		for (size_t k = 0; k < styles.size(); ++k)
		{
			auto& style = styles[k];
			if ((style.Flags & VG_FLAGS_IMAGE_COLOR) == VG_FLAGS_IMAGE_COLOR && style.Image != -1)
			{
				if ((style.Flags & VG_FLAGS_IMAGE_HWGPU) == VG_FLAGS_IMAGE_HWGPU) continue;
				bool isGlyph = (style.Flags & VG_FLAGS_IMAGE_GLYPH) == VG_FLAGS_IMAGE_GLYPH;
				OpenVGTextures::Instance()->getTexture(images[styles[k].Image], isGlyph);
			}
		}
	}
	for (size_t i = 0, p = 0; i < data.size(); ++i)
	{
		auto pointIndex = p;
		auto styleIndex = m_StyleList.size();
		auto linearIndex = m_LinearList.size();
		auto radialIndex = m_RadialList.size();
		auto matrixIndex = m_MatrixList.size();
		auto textureIndex = m_TextureList.size();
		auto& points = data[i].PointList;
		auto& styles = data[i].StyleList;
		auto& linears = data[i].LinearList;
		auto& radials = data[i].RadialList;
		auto& matrixs = data[i].MatrixList;
		auto& images = data[i].ImageList;

		m_MatrixList.insert(m_MatrixList.end(), matrixs.begin(), matrixs.end());
		m_LinearList.insert(m_LinearList.end(), linears.begin(), linears.end());
		m_RadialList.insert(m_RadialList.end(), radials.begin(), radials.end());
		m_StyleList.insert(m_StyleList.end(), styles.begin(), styles.end());
		::memcpy(m_PointList.data() + pointIndex, points.data(), sizeof(VGPrimitive::point_t) * points.size());

		for (size_t k = 0, t = 0; k < styles.size(); ++k)
		{
			auto& style = m_StyleList[styleIndex + k];
			if (style.Linear != -1) style.Linear += linearIndex;
			if (style.Radial != -1) style.Radial += radialIndex;
			if (style.Image != -1) style.Image += textureIndex;
			if ((style.Flags & VG_FLAGS_IMAGE_COLOR) == VG_FLAGS_IMAGE_COLOR && style.Image != -1)
			{
				auto image = images[styles[k].Image];
				if ((style.Flags & VG_FLAGS_IMAGE_HWGPU) == VG_FLAGS_IMAGE_HWGPU)
				{
					OpenVGTextures::texture_t texture;
					texture.Handle = image.Data;
					texture.Bounds = { 0, 0, (float)image.Width, (float)image.Height };
					m_TextureList.push_back(texture);
				}
				else
				{
					bool isGlyph = (style.Flags & VG_FLAGS_IMAGE_GLYPH) == VG_FLAGS_IMAGE_GLYPH;
					auto texture = OpenVGTextures::Instance()->getTexture(image, isGlyph);
					m_TextureList.push_back(texture);
				}
			}
		}

		for (size_t k = 0; k < points.size(); ++k)
		{
			auto& point = m_PointList[pointIndex + k];
			if (point.Style != -1) point.Style += styleIndex;
			if (point.Matrix != -1) point.Matrix += matrixIndex;
		}

		p += points.size();
	}

	for (size_t i = 0; i < m_PointList.size(); ++i)
	{
		auto& point = m_PointList[i];
		auto& style = m_StyleList[point.Style];
		if (style.Image != -1 && (style.Flags & VG_FLAGS_IMAGE_GLYPH) == VG_FLAGS_IMAGE_GLYPH)
		{
			auto bounds = m_TextureList[style.Image].Bounds;
			if (point.U == 0.0f) point.U = bounds.X;
			else point.U = bounds.X + bounds.W;
			if (point.V == 0.0f) point.V = bounds.Y;
			else point.V = bounds.Y + bounds.H;
		}
	}

	// 更新顶点缓冲区
	if (m_PointList.size())
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_NativeVertexBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VGPrimitive::point_t) * m_PointList.size(), m_PointList.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (m_StyleList.size())
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_NativeStyleBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VGPrimitive::style_t) * m_StyleList.size(), m_StyleList.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (m_LinearList.size())
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_NativeLinearBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VGPrimitive::linear_t) * m_LinearList.size(), m_LinearList.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (m_RadialList.size())
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_NativeRadialBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VGPrimitive::radial_t) * m_RadialList.size(), m_RadialList.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	if (m_MatrixList.size())
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_NativeMatrixBuffer);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(VGPrimitive::matrix_t) * m_MatrixList.size(), m_MatrixList.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	// 渲染顶点缓冲区
	glUseProgram(m_NativeProgram);
	glBindVertexArray(m_NativePrimitive);
	glUniform2f(glGetUniformLocation(m_NativeProgram, "Viewport"), 1.0f / client.W, 1.0f / client.H);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_NativeStyleBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_NativeLinearBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_NativeRadialBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_NativeMatrixBuffer);

	size_t offset = 0;
	for (size_t i = 0, s = 0, t = 0; i + 3 <= m_PointList.size(); i += 3)
	{
		auto& style = m_StyleList[m_PointList[i].Style];
		if (style.Image != -1)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_TextureList[style.Image].Handle);
		}
		glDrawArrays(GL_TRIANGLES, i, 3);
	}

	glBindVertexArray(0);
	glUseProgram(0);
}

// ===============================================================================

VGRaw<OpenVGTextures> OpenVGTextures::Instance()
{
	static OpenVGTextures s_Instance;
	return &s_Instance;
}

OpenVGTextures::OpenVGTextures()
{
}

OpenVGTextures::~OpenVGTextures()
{
}

OpenVGTextures::texture_t OpenVGTextures::getTexture(VGImage image, bool glyph)
{
	if (glyph == false)
	{
		auto result = m_ImageTextureMap.find(image);
		if (result == m_ImageTextureMap.end())
		{
			context_t context;
			auto imageFormat = image.Type == VGImage::Float || image.Type == VGImage::HWFloat;
			renderCreate(context, imageFormat, image.Width, image.Height);
			int rect[4]{ 0, 0, (int)image.Width, (int)image.Height };
			renderUpdate(context, imageFormat, rect, (uint8_t*)image.Pixel);
			result = m_ImageTextureMap.emplace(image, context).first;
		}
		return texture_t{ result->second.Texture, {0, 0, 1, 1} };
	}
	else
	{
		if (m_FontStashTexture.Texture == 0)
		{
			renderCreate(m_FontStashTexture, 0, 512, 512);
		}
		auto result = m_GlyphTextureMap.find(image);
		if (result == m_GlyphTextureMap.end())
		{
			result = m_GlyphTextureMap.emplace(image, context_t{ m_FontStashTexture.Texture, 0, 0, 0, image.Width, image.Height }).first;
			VGList<VGRaw<context_t>> textureList;
			VGList<VGRaw<const VGImage>> imageList;
			for (auto& e : m_GlyphTextureMap)
			{
				imageList.push_back(&e.first);
				textureList.push_back(&e.second);
			}

			VGList<stbrp_rect> rects;
			for (size_t i = 0; i < textureList.size(); ++i)
			{
				auto& texture = textureList[i];
				auto& rect = rects.emplace_back();
				rect.id = (int32_t)i;
				rect.w = texture->W;
				rect.h = texture->H;
			}

			do
			{
				stbrp_context context;
				stbrp_node* nodes = new stbrp_node[m_FontStashTexture.W];
				stbrp_init_target(&context, m_FontStashTexture.W, m_FontStashTexture.H, nodes, m_FontStashTexture.W);
				if (stbrp_pack_rects(&context, rects.data(), (int32_t)rects.size()) == 0)
				{
					delete[] nodes;
					m_FontStashTexture.W *= 2;
					m_FontStashTexture.H *= 2;
					renderResize(m_FontStashTexture, 0, m_FontStashTexture.W, m_FontStashTexture.H);
					continue;
				}
				for (size_t i = 0; i < rects.size(); ++i)
				{
					textureList[i]->X = rects[i].x;
					textureList[i]->Y = rects[i].y;
					int rect[4]{ rects[i].x, rects[i].y, rects[i].w, rects[i].h };
					renderUpdate(m_FontStashTexture, 0, rect, (uint8_t*)imageList[i]->Pixel);
				}

				delete[] nodes;
				break;
			} while (true);
		}

		auto x = result->second.X * 1.0f / m_FontStashTexture.W;
		auto y = result->second.Y * 1.0f / m_FontStashTexture.H;
		auto w = result->second.W * 1.0f / m_FontStashTexture.W;
		auto h = result->second.H * 1.0f / m_FontStashTexture.H;
		return texture_t{ result->second.Texture, {x, y, w, h} };
	}
}

int OpenVGTextures::renderCreate(context_t& context, int format, int width, int height)
{
	renderDelete(context);

	if (format == 0)
	{
		glGenTextures(1, &context.Texture);
		if (!context.Texture) return 0;
		context.W = width;
		context.H = height;
		context.Format = format;
		glBindTexture(GL_TEXTURE_2D, context.Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, context.W, context.H, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		VGFloat4 color{ 0, 0, 0, 0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color.Value);
	}
	else
	{
		glGenTextures(1, &context.Texture);
		if (!context.Texture) return 0;
		context.W = width;
		context.H = height;
		context.Format = format;
		glBindTexture(GL_TEXTURE_2D, context.Texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, context.W, context.H, 0, GL_RGBA, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	return 1;
}

int OpenVGTextures::renderResize(context_t& context, int format, int width, int height)
{
	return renderCreate(context, format, width, height);
}

void OpenVGTextures::renderUpdate(context_t& context, int format, int* rect, const unsigned char* data)
{
	if (context.Texture == 0) return;

	if (format == 0)
	{
		glBindTexture(GL_TEXTURE_2D, context.Texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, rect[0], rect[1], rect[2], rect[3], GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, context.Texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, rect[0], rect[1], rect[2], rect[3], GL_RGBA, GL_FLOAT, data);
	}
}

void OpenVGTextures::renderDelete(context_t& context)
{
	if (context.Texture != 0)
		glDeleteTextures(1, &context.Texture);
	context.Texture = 0;
}