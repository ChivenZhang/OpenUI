// ovg.cpp: 定义应用程序的入口点。
//

#include "ovg_main.h" 
#include "ovg_renderer_sdl3.h"
#include <Windows.h>
#include <cmath>
#include <unordered_map>
#include "ovg_fonts.h"
using namespace std;


static inline uint32_t MAKE_RGBA(float r, float g, float b, float a) {
	return (((uint8_t)(a * 255)) << 24) | (((uint8_t)(r * 255)) << 16) | (((uint8_t)(g * 255)) << 8) | ((uint8_t)(b * 255));
}
#ifndef M_PI
#define M_PI 3.1415926
#endif
#define COL(r,g,b,a) MAKE_RGBA(r,g,b,a)
void draw_second_hand(ovg_ctx_cb* cb, rvg_t* ctx, float cx, float cy, int second, float len, float width)
{
	float angle = second * 6.0f * (M_PI / 180.0f);
	if (len < 1)
		len = 1;

	cb->save(ctx);
	cb->translate(ctx, cx, cy);
	cb->rotate(ctx, angle);

	cb->set_source_rgba(ctx, 1, 0, 0, 0.9f);
	cb->rounded_rectangle(ctx, -2, -len, width, len, width * 0.5);
	cb->fill(ctx);

	cb->restore(ctx);
}
void draw_clock(ovg_ctx_cb* cb, rvg_t* ctx, float cx, float cy, float R, int hour, int minute, int second)
{
	/* ========== 颜色表 ========== */
	uint32_t C_FACE = COL(0.98f, 0.97f, 0.93f, 1.0f);   /* 奶油表盘 */
	uint32_t C_BORDER = COL(0.25f, 0.25f, 0.30f, 1.0f);   /* 深灰边框 */
	uint32_t C_TICK_H = COL(0.15f, 0.915f, 0.20f, 1.0f);   /* 时刻度   */
	uint32_t C_TICK_M = COL(0.55f, 0.55f, 0.60f, 1.0f);   /* 分刻度   */
	uint32_t C_HOUR_HAND = COL(0.12f, 0.12f, 0.18f, 1.0f);   /* 时针     */
	uint32_t C_MIN_HAND = COL(0.18f, 0.18f, 0.24f, 1.0f);   /* 分针     */
	uint32_t C_SEC_HAND = COL(0.88f, 0.18f, 0.18f, 1.0f);   /* 秒针红   */
	uint32_t C_CENTER = COL(0.88f, 0.18f, 0.18f, 1.0f);   /* 中心圆点 */
	uint32_t C_SHADOW = COL(0.00f, 0.00f, 0.00f, 0.12f);  /* 阴影     */
	uint32_t C_NUM_DOT = COL(0.30f, 0.30f, 0.35f, 0.8f);   /* 数字圆点 */

	/* ========== 1. 表盘阴影 ========== */
	cb->new_path(ctx);
	cb->circle(ctx, cx + R * 0.025f, cy + R * 0.04f, R * 0.94f);
	cb->set_source_color(ctx, C_SHADOW);
	cb->fill(ctx);

	/* ========== 2. 表盘底色 + 边框 ========== */
	cb->new_path(ctx);
	float rr = R * 0.94f;
	cb->circle(ctx, cx, cy, rr);
	cb->set_source_color(ctx, C_FACE);
	cb->fill_preserve(ctx);
	cb->set_source_color(ctx, C_BORDER);
	cb->set_line_width(ctx, R * 0.022f);
	cb->set_line_cap(ctx, VG_LINE_CAP_ROUND);
	cb->stroke(ctx);

	/* ========== 3. 刻度线（60 条） ========== */
	float r_min_in = R * 0.83f;   /* 分刻度内端 */
	float r_min_out = R * 0.88f;   /* 分刻度外端 */
	float r_hr_in = R * 0.76f;   /* 时刻度内端 */
	float r_hr_out = R * 0.89f;   /* 时刻度外端 */

	cb->set_line_cap(ctx, VG_LINE_CAP_BUTT);

	for (int i = 0; i < 60; i++) {
		float a = (float)i * (2.0f * M_PI / 60.0f) - M_PI / 2.0f;
		float ca = cosf(a), sa = sinf(a);

		if (i % 5 == 0) {
			/* -- 时刻度（粗） -- */
			cb->move_to(ctx, cx + r_hr_in * ca, cy + r_hr_in * sa);
			cb->line_to(ctx, cx + r_hr_out * ca, cy + r_hr_out * sa);
			cb->set_source_color(ctx, C_TICK_H);
			cb->set_line_width(ctx, R * 0.020f);
			cb->stroke(ctx);
		}
		else {
			/* -- 分刻度（细） -- */
			glm::vec2 p0 = { cx + r_min_in * ca, cy + r_min_in * sa }, p1 = { cx + r_min_out * ca, cy + r_min_out * sa };
			cb->move_to(ctx, p0.x, p0.y);
			cb->line_to(ctx, p1.x, p1.y);
			cb->set_source_color(ctx, C_TICK_M);
			cb->set_line_width(ctx, 1);
			cb->stroke(ctx);
		}
	}
	/* ========== 4. 数字标记（12 个小圆点） ========== */
	float num_r = R * 0.866f;
	for (int i = 0; i < 12; i++) {
		float a = (float)i * (2.0f * M_PI / 12.0f) - M_PI / 2.0f;
		float nx = cx + num_r * cosf(a);
		float ny = cy + num_r * sinf(a);

		cb->new_path(ctx);
		cb->circle(ctx, nx, ny, 5);
		cb->set_source_color(ctx, C_NUM_DOT);
		cb->fill(ctx);
	}

	/* ========== 5. 指针角度 ========== */
	float hr_ang = ((float)(hour % 12) * 30.0f + (float)minute * 0.5f) * (M_PI / 180.0f) - M_PI / 2.0f;
	float min_ang = ((float)minute * 6.0f + (float)second * 0.1f) * (M_PI / 180.0f) - M_PI / 2.0f;
	float sec_ang = (float)second * 6.0f * (M_PI / 180.0f) - M_PI / 2.0f;

	/* ========== 6. 时针（粗短 rounded_rectangle） ========== */
	{
		float L = R * 0.48f, W = R * 0.028f;
		cb->save(ctx);
		cb->translate(ctx, cx, cy);
		cb->rotate(ctx, hr_ang + M_PI / 2.0f);
		cb->new_path(ctx);
		cb->rounded_rectangle(ctx, -W * 0.5f, -R * 0.04f, W, L + R * 0.04f, W * 0.4f);
		cb->set_source_color(ctx, C_HOUR_HAND);
		cb->fill(ctx);
		cb->restore(ctx);
	}

	/* ========== 7. 分针（中等） ========== */
	{
		float L = R * 0.68f, W = R * 0.018f;
		cb->save(ctx);
		cb->translate(ctx, cx, cy);
		cb->rotate(ctx, min_ang + M_PI / 2.0f);
		cb->new_path(ctx);
		cb->rounded_rectangle(ctx, -W * 0.5f, -R * 0.03f, W, L + R * 0.03f, W * 0.4f);
		cb->set_source_color(ctx, C_MIN_HAND);
		cb->fill(ctx);
		cb->restore(ctx);
	}

	/* ========== 8. 秒针（细长红色） ========== */
	{
		float L = R * 0.80f, W = R * 0.007f;
		//	cb->save(ctx);
		//	cb->translate(ctx, cx, cy);
		//	cb->rotate(ctx, sec_ang + M_PI / 2.0f);
		//	cb->new_path(ctx);
		//	cb->rounded_rectangle(ctx, -W * 0.5f, -R * 0.07f, W, L + R * 0.07f, W * 0.5f);
		//	cb->set_source_color(ctx, C_SEC_HAND);
		//	cb->fill(ctx);
		//	cb->restore(ctx);
		draw_second_hand(cb, ctx, cx, cy, second, L + R * 0.07f, 2);
	}

	/* ========== 9. 中心装饰圆点 ========== */
	cb->new_path(ctx);
	cb->circle(ctx, cx, cy, R * 0.030f);
	cb->set_source_color(ctx, C_CENTER);
	cb->fill(ctx);

	cb->new_path(ctx);
	cb->circle(ctx, cx, cy, R * 0.052f);
	cb->set_source_rgba(ctx, 0.88f, 0.18f, 0.18f, 0.20f);
	cb->fill(ctx);
}

/* ============================================================
   场景入口：清背景 + 画时钟
   ============================================================ */
void draw_clock_scene(ovg_ctx_cb* cb, rvg_t* ctx, int w, int h, int hh, int mm, int ss)
{
	/* 清屏 */
	cb->new_path(ctx);
	cb->circle(ctx, w / 2, w / 2, (float)w * 0.5);
	cb->set_source_rgba(ctx, 0.90f, 0.91f, 0.94f, 1.0f);
	cb->fill(ctx);

	float margin = (float)fmin(w, h) * 0.07f;
	float cx = (float)w / 2.0f;
	float cy = (float)h / 2.0f;
	float R = (float)fmin(w, h) / 2.0f - margin;

	draw_clock(cb, ctx, cx, cy, R, hh, mm, ss);
}
void draw(ovg_ctx_cb* cb, rvg_t* vg, const glm::ivec2& surfsize)
{
	cb->save(vg);
	cb->rounded_rectangle(vg, 20, 20, 600, 600, 10);
	cb->clip(vg);
	cb->set_source_color(vg, 0xff0080ff);
	auto pat = cb->new_pattern_linear(vg, 0, 0, 0, 256);
	cb->pattern_add_color_stop(pat, 0, 0, 0, 1, 1);// 蓝
	cb->pattern_add_color_stop(pat, 0.5, 0, 1, 0, 1);// 绿
	cb->pattern_add_color_stop(pat, 1, 1, 0, 0, 1);// 红
	cb->set_source(vg, pat);
	cb->rectangle(vg, 20, 20, 300, 300);
	cb->fill(vg);
	cb->translate(vg, 0, 460);
	{
		auto pat = cb->new_pattern_sweep(vg, 160, 160, 0, 2);
		cb->pattern_add_color_stop(pat, 0, 0, 0, 1, 0.81);// 蓝
		cb->pattern_add_color_stop(pat, 0.5, 0, 1, 0, 0.81);// 绿
		cb->pattern_add_color_stop(pat, 1, 1, 0, 0, 0.91);// 红
		cb->set_source(vg, pat);
		cb->rectangle(vg, 20, 20, 300, 300);
		cb->fill(vg);
	}
	cb->reset_clip(vg, STENCIL_CLIP_BIT);// 重置裁剪
	cb->translate(vg, 0, -460);
	cb->translate(vg, 120, 250);
	cb->circle(vg, 150, 150, 160);
	pat = cb->new_pattern_radial(vg, 150, 150, 25.6, 102.4, 102.4, 128.0, false);
	cb->pattern_add_color_stop(pat, 0, 0, 0, 1, 0);// 蓝
	cb->pattern_add_color_stop(pat, 0.5, 0, 1, 0, 1);// 绿
	cb->pattern_add_color_stop(pat, 0.8, 1, 0, 0, 1);// 红
	cb->pattern_add_color_stop(pat, 1, 1, 1, 0, 0.61);// 橙
	cb->set_source(vg, pat);
	cb->fill(vg);
	cb->translate(vg, 300, -250);
	static glm::ivec3 ss = { 10,20,10 };
	static int ic = 0;
	//if (9 < ic++)
	{
		ss.z++; ic = 0;
	}
	if (ss.z > 60) {
		ss.z = 0;
		ss.y++;
	}
	if (ss.y > 60) {
		ss.y = 0;
		ss.x++;
	}
	if (ss.x > 24) { ss.x = 0; }
	draw_clock_scene(cb, vg, 300, 300, ss.x, ss.y, ss.z);

	cb->translate(vg, 0, 350);


	cb->rounded_rectangle(vg, 200, 12, 300, 200, 10);
	cb->set_line_width(vg, 6);	cb->set_source_rgba(vg, 0, 0.51, 1, 1);

	auto pat1 = cb->new_pattern_linear(vg, 0.0, 0.0, 0.0, 256.0);
	cb->pattern_add_color_stop(pat1, 0, 0, 0, 1, 0);// 蓝
	cb->pattern_add_color_stop(pat1, 1, 1, 0, 0, 1);// 红
	cb->set_source(vg, pat1);

	cb->fill_preserve(vg);// 填充
	cb->set_source_color(vg, 0xff1181f1);
	cb->stroke(vg);//描边

	cb->set_line_width(vg, 6);
	cb->rectangle(vg, 12, 12, 232, 70);
	cb->new_sub_path(vg);	cb->arc(vg, 64, 64, 40, 0, 2 * M_PI);
	cb->new_sub_path(vg);	cb->arc_negative(vg, 192, 64, 40, 0, -2 * M_PI);
	cb->set_fill_rule(vg, VG_FILL_RULE_EVEN_ODD);
	cb->set_source_rgba(vg, 0, 0.7, 0, 1);
	cb->fill_preserve(vg);//填充
	cb->set_source_rgba(vg, 0, 0, 0, 1);
	cb->stroke(vg); //描边

	cb->set_line_width(vg, 6);
	cb->save(vg);
	cb->translate(vg, 0, 128);
	cb->rectangle(vg, 12, 12, 232, 70);
	cb->new_sub_path(vg);
	cb->arc(vg, 64, 64, 40, 0, 2 * M_PI);
	cb->new_sub_path(vg);
	cb->arc_negative(vg, 192, 64, 40, 0, -2 * M_PI);
	cb->set_fill_rule(vg, VG_FILL_RULE_NON_ZERO);
	cb->set_source_rgba(vg, 0, 0, 0.9, 1);
	cb->fill_preserve(vg);// 填充
	float dashes[] = { 50.0,  /* ink */
				   10.0,  /* skip */
				   10.0,  /* ink */
				   10.0   /* skip*/
	};
	int    ndash = sizeof(dashes) / sizeof(dashes[0]);
	double offset = 10.0;

	cb->set_dash(vg, dashes, ndash, offset);

	cb->set_source_rgba(vg, 0, 0, 0, 1);
	cb->stroke(vg); //描边
	cb->restore(vg);
	//cb->translate(vg, 0, -128);
	//cb->reset_clip(vg, 0);

	cb->translate(vg, 300, -200);
	//cb->rectangle(vg, 20, 150, 200, 100, 10);
	//cb->clip(vg, vg);// 圆角矩形裁剪
	//cb->clip_rect(vg, 20, 150, 300, 500);// 矩形裁剪会接受当前矩阵影响
	{
		auto pat = cb->new_pattern_radial(vg, 150, 100, 25.6, 102.4, 102.4, 128.0, false);
		cb->pattern_add_color_stop(pat, 0, 0, 0, 1, 0);// 蓝
		cb->pattern_add_color_stop(pat, 0.5, 0, 1, 0, 1);
		cb->pattern_add_color_stop(pat, 1, 1, 0, 0, 1);// 红
		cb->set_source(vg, pat);
	}
	cb->arc(vg, 128, 128.0, 76.8, 0, 2 * M_PI);
	cb->fill(vg);// 填充


	cb->restore(vg);
}
#if 1
glm::vec3 polarToVector(float yaw, float pitch) {
	return glm::vec3(
		sinf(yaw) * sinf(pitch),  // x = sin(yaw) * sin(pitch)
		cosf(pitch),              // y = cos(pitch)
		cosf(yaw) * sinf(pitch)   // z = cos(yaw) * sin(pitch)
	);
}

void generateSphere(int sides, std::vector<uint32_t>& outIndices, std::vector<glm::vec3>& outVertices) {
	outIndices.clear();
	outVertices.clear();
	outVertices.reserve((sides + 1) * (sides + 1));  // 调整预留大小 
	outIndices.reserve(sides * sides * 6);          // 每个网格生成 2 个三角形（6 个索引）

	for (int roll = 0; roll <= sides; roll++) {    // 改为闭合区间 
		float phi = roll * (glm::pi<float>() * 2.0f) / sides;  // yaw 覆盖 0-2π
		for (int theta = 0; theta <= sides; theta++) {        // pitch 覆盖 0-π
			float thetaRad = theta * glm::pi<float>() / sides;
			outVertices.push_back(polarToVector(phi, thetaRad));
		}
	}

	// 生成索引（参考球面经纬网格连接逻辑）
	for (int i = 0; i < sides; i++) {
		for (int j = 0; j < sides; j++) {
			uint32_t currentRow = i * (sides + 1);
			uint32_t nextRow = (i + 1) * (sides + 1);
			outIndices.push_back(currentRow + j);
			outIndices.push_back(nextRow + j);
			outIndices.push_back(nextRow + j + 1);
			outIndices.push_back(currentRow + j);
			outIndices.push_back(nextRow + j + 1);
			outIndices.push_back(currentRow + j + 1);
		}
	}
}
void generateCube(std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices, float r, const glm::vec3& pos)
{
	std::vector<glm::vec3> v = {
		glm::vec3{ -r, -r, -r }, glm::vec3{ r, -r, -r },
			glm::vec3{ -r,  r, -r }, glm::vec3{ r,  r, -r },
			glm::vec3{ -r, -r,  r }, glm::vec3{ r, -r,  r },
			glm::vec3{ -r,  r,  r }, glm::vec3{ r,  r,  r }
	};
	std::vector<uint32_t> idx = {
		0, 2, 3, 1,  4,5,7,6 ,
		 0,1,5,4 ,  1,3,7,5 ,
		 3,2,6,7 ,  2,0,4,6 ,
	};
	std::vector<uint32_t> triangles = {
	0,3,2,  0,1,3,     // 第一个四边形 0,2,3,1 
	4,7,5,  4,6,7,     // 第二个四边形 4,5,7,6 
	0,5,1,  0,4,5,     // 第三个四边形 0,1,5,4 
	1,7,3,  1,5,7,     // 第四个四边形 1,3,7,5 
	3,6,2,  3,7,6,     // 第五个四边形 3,2,6,7 
	2,4,0,  2,6,4      // 第六个四边形 2,0,4,6 
	};
	vertices = v;
	for (auto& it : vertices)it += pos;
	indices = triangles;
}
void generateTorus(std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& texCoords, std::vector<unsigned int>& indices,
	float R, float r, int numSegments, int numRings)
{
	auto idx = vertices.size();
	auto idxs = indices.size();
	vertices.reserve(idx + (numRings + 1) * (numSegments + 1));
	indices.reserve(idxs + numRings * numSegments * 6);
	normals.reserve(idx + (numRings + 1) * (numSegments + 1));
	texCoords.reserve(idx + (numRings + 1) * (numSegments + 1));
	for (int i = 0; i <= numRings; ++i) {
		float ringAngle = 2 * glm::pi<float>() * i / numRings;
		glm::mat4 ringRotate = glm::rotate(glm::mat4(1.0f), ringAngle, glm::vec3(0, 1, 0));

		for (int j = 0; j <= numSegments; ++j) {
			float segAngle = 2 * glm::pi<float>() * j / numSegments;
			glm::vec3 basePoint(r * cos(segAngle), r * sin(segAngle), R);

			// 顶点位置（应用旋转变换）
			glm::vec4 worldPos = ringRotate * glm::vec4(basePoint, 1.0f);
			vertices.push_back(glm::vec3(worldPos));

			// 法向量（圆心到顶点的方向）
			auto n = glm::vec3(worldPos - ringRotate * glm::vec4(0, 0, R, 1));
			glm::vec3 normal = glm::normalize(n);
			normals.push_back(normal);

			// 纹理坐标（避免接缝处拉伸）
			float u = static_cast<float>(i) / numRings;
			float v = static_cast<float>(j) / numSegments;
			texCoords.push_back(glm::vec2(u, v));
		}
	}
	for (int i = 0; i < numRings; ++i) {
		for (int j = 0; j < numSegments; ++j) {
			int current = i * (numSegments + 1) + j;
			int next = current + numSegments + 1;
			current += idx;
			next += idx;
			// 四边形拆分为两个三角形 
			indices.push_back(current);
			indices.push_back(next);
			indices.push_back(current + 1);

			indices.push_back(next);
			indices.push_back(next + 1);
			indices.push_back(current + 1);
		}
	}
}

struct mesh3d_vi {
	std::vector<glm::vec3> vertices;
	std::vector<uint32_t> indices; // 三角形索引，每三个一组表示一个三角形 
};


// 获取边的唯一键 
inline size_t getEdgeKey(size_t i0, size_t i1) {
	if (i0 > i1) std::swap(i0, i1);
	return (i0 << 32) | i1;
}

// 获取或创建边中点 
size_t getMidpoint(std::unordered_map<size_t, size_t>& midCache,
	std::vector<glm::vec3>& vertices,
	size_t i0, size_t i1) {
	size_t key = getEdgeKey(i0, i1);
	auto it = midCache.find(key);
	if (it != midCache.end())
		return it->second;

	glm::vec3 mid = glm::normalize((vertices[i0] + vertices[i1]) * 0.5f);
	size_t index = vertices.size();
	vertices.push_back(mid);
	midCache[key] = index;
	return index;
}

void createIcosphere(mesh3d_vi* shphere, size_t subdivisions, float radius, const glm::vec3& pos)
{
	const float t = (1.0f + std::sqrt(5.0f)) / 2.0f;

	if (!(radius > 1.0f)) {
		shphere->vertices.clear();
		shphere->indices.clear();
		return;
	}
	// 初始二十面体的12个顶点 
	std::vector<glm::vec3> verts = {
		glm::normalize(glm::vec3(-1,  t,  0)),
		glm::normalize(glm::vec3(1,  t,  0)),
		glm::normalize(glm::vec3(-1, -t,  0)),
		glm::normalize(glm::vec3(1, -t,  0)),
		glm::normalize(glm::vec3(0, -1,  t)),
		glm::normalize(glm::vec3(0,  1,  t)),
		glm::normalize(glm::vec3(0, -1, -t)),
		glm::normalize(glm::vec3(0,  1, -t)),
		glm::normalize(glm::vec3(t,  0, -1)),
		glm::normalize(glm::vec3(t,  0,  1)),
		glm::normalize(glm::vec3(-t,  0, -1)),
		glm::normalize(glm::vec3(-t,  0,  1))
	};

	// 初始二十面体的20个三角形面 
	std::vector<unsigned int> triangles = {
		0,11,5,  0,5,1,  0,1,7,  0,7,10,  0,10,11,
		1,5,9,  5,11,4,  11,10,2,  10,7,6,  7,1,8,
		3,9,4,  3,4,2,  3,2,6,  3,6,8,  3,8,9,
		4,9,5,  2,4,11, 6,2,10, 8,6,7,  9,8,1
	};
	auto length = triangles.size() / 3;
	for (size_t i = 0; i < length; i++)
	{
		std::swap(triangles[i * 3 + 1], triangles[i * 3 + 2]); // 翻转三角形顶点顺序
	}
	std::unordered_map<size_t, size_t> midCache;
	std::vector<unsigned int> newTriangles;
	// 递归细分 
	for (unsigned int i = 0; i < subdivisions; ++i) {
		midCache.clear();
		newTriangles.clear();
		for (size_t j = 0; j < triangles.size(); j += 3) {
			size_t v0 = triangles[j];
			size_t v1 = triangles[j + 1];
			size_t v2 = triangles[j + 2];

			size_t a = getMidpoint(midCache, verts, v0, v1);
			size_t b = getMidpoint(midCache, verts, v1, v2);
			size_t c = getMidpoint(midCache, verts, v2, v0);

			newTriangles.insert(newTriangles.end(), {
				(unsigned)v0, (unsigned)a, (unsigned)c,
				(unsigned)v1, (unsigned)b, (unsigned)a,
				(unsigned)v2, (unsigned)c, (unsigned)b,
				(unsigned)a, (unsigned)b, (unsigned)c
				});
		}
		triangles.swap(newTriangles);
	}
	shphere->vertices = verts;
	shphere->indices = triangles;
	glm::vec3 sc = { 1,1,1 };
	if ((radius > 0.0f && radius < 1.0f) || radius > 1.0f)
	{
		sc = { radius,radius,radius };
	}
	for (auto& it : shphere->vertices)it = pos + it * sc;
}

glm::mat4 ortho(float width, float height, float znear, float zfar, bool is_top)
{
	return is_top ? glm::ortho(0.0f, width, height, 0.0f, znear, zfar) : glm::ortho(0.0f, width, 0.0f, height, znear, zfar);
}
#endif // 1

void draw_test3d(vg_fbo_t* fbo, ovg_ctx_cb* cb, rvg_t* vg) {
	gem_info_t info = {};
	info.blendMode = (uint8_t)blendMode_e::normal;
	info.topology = 0;
	info.flags = (uint8_t)depth_stencil_State::d_stenciltest_enable;
	info.frontFace = 0;
	info.cullMode = 0;
	auto info2d = info;
	glm::vec2 surfSize = { (float)fbo->width, (float)fbo->height };

	uint32_t color[3] = { 0x8f0080FF,0xFF80FF00,0xFF555555 };
	uint32_t indices[15] = { 0,1,2,0,2,3,0,3,4,0,4,5,0,5,1 };
	glm::vec3 v[6] = {};
	glm::vec2 uv[6] = {};
	double r = 100.0;
	glm::vec3 pos = { 126,126,0 };
	v[0] = glm::vec3(0.0f) + pos;
	auto t = v;
	t++;
	for (int i = 0; i < 5; ++i) {
		double angle = 2.0 * M_PI * i / 5; // 五边形角度
		double x = r * cos(angle + M_PI / 2); // 旋转90°使顶点朝上
		double y = r * sin(angle + M_PI / 2);
		*t++ = glm::vec3(x, y, 0.0f) + pos;
	}
	// 中心
	uv[0] = glm::vec2(0.5f, 0.5f);
	// 外圈 5 个顶点
	for (int i = 0; i < 5; ++i) {
		// v[i+1] 是世界/局部坐标，减去 pos 得到以中心为原点的坐标
		glm::vec2 local = glm::vec2(v[i + 1].x, v[i + 1].y) - glm::vec2(pos.x, pos.y);

		// 归一化到 [0, 1]
		uv[i + 1] = glm::vec2(
			0.5f + local.x / (2.0f * r),
			0.5f + local.y / (2.0f * r)
		);
	}
	std::vector<uint32_t> indices3; std::vector<glm::vec3> vertices3;
	generateSphere(16, indices3, vertices3);
	std::vector<glm::vec3>vertices, normals; std::vector<glm::vec2>  texCoords; std::vector<unsigned int> indices4;
	//generateTorus(vertices3, normals, texCoords, indices3, 2.0f, 1.0f, 32, 64);
	generateCube(vertices, indices4, 1.2f, glm::vec3(2.5, 0, 0));

	mesh3d_vi shphere;
	createIcosphere(&shphere, 2, 2, glm::vec3(-3.2, 0, 0));

	glm::mat4 model = glm::mat4(1.0f);
	//model = glm::translate(glm::mat4(1.0), glm::vec3(2, 0, 0));
	float fov = 45;
	glm::mat4 projection = glm::perspective(glm::radians(fov), (float)(surfSize.x * 1.0 / surfSize.y), 0.1f, 1000.0f);
	// 视图矩阵：摄像机位于(0,1,5)，看向原点，上方向为(0,1,0)
	glm::mat4 view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 8.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	glm::mat4 mat = projection * view * model;
	cb->set_geom_state(vg, &info, &mat);
	info.lineWidth = 1;
	//gem_info_s info;
	info.blendMode = (uint8_t)blendMode_e::normal;
	info.topology = 0;
	info.shader = ST_DOUBLESIDED;
	info.flags = ((uint8_t)depth_stencil_State::d_depthtest_enable | (uint8_t)depth_stencil_State::d_depthwrite_enable);

	cb->set_geom_state(vg, &info, &mat);
	cb->add_geometry3d(vg, nullptr, (float*)vertices3.data(), sizeof(glm::vec3), &color[1], 0, (float*)uv, sizeof(glm::vec2), vertices3.size(), indices3.data(), indices3.size(), sizeof(uint32_t), 1);

	cb->add_geometry3d(vg, nullptr, (float*)vertices.data(), sizeof(glm::vec3), &color[1], 0, (float*)uv, sizeof(glm::vec2), vertices.size(), indices4.data(), indices4.size(), sizeof(uint32_t), 1);

	uint32_t color2[2] = { 0xFF9678B4,0xFFf55555 };

	{
		glm::mat4 ins[5] = {};
		ins[0] = glm::mat4(1.0);
		ins[1] = glm::translate(glm::vec3(0.5, 1.0, 0));
		cb->set_instance_mat(vg, ins, 2);
		info.shader = ST_INSTANCE;
		cb->set_geom_state(vg, &info, &mat);
	}
	cb->add_geometry3d(vg, nullptr, (float*)shphere.vertices.data(), sizeof(glm::vec3), color2, 0, (float*)uv, sizeof(glm::vec2), shphere.vertices.size(), shphere.indices.data(), shphere.indices.size(), sizeof(uint32_t), 1);

	cb->set_instance_mat(vg, 0, 0);

	info.polygon = 1;// 线框模式  
	info.shader = ST_NONE;
	cb->set_geom_state(vg, &info, &mat);
	uint32_t color1[2] = { 0xFF2c2c2c,0xFFf55555 };
	cb->add_geometry3d(vg, nullptr, (float*)vertices3.data(), sizeof(glm::vec3), color1, 0, (float*)uv, sizeof(glm::vec2), vertices3.size(), indices3.data(), indices3.size(), sizeof(uint32_t), 1);
	cb->add_geometry3d(vg, nullptr, (float*)vertices.data(), sizeof(glm::vec3), color1, 0, (float*)uv, sizeof(glm::vec2), vertices.size(), indices4.data(), indices4.size(), sizeof(uint32_t), 1);
	cb->add_geometry3d(vg, nullptr, (float*)shphere.vertices.data(), sizeof(glm::vec3), color1, 0, (float*)uv, sizeof(glm::vec2), shphere.vertices.size(), shphere.indices.data(), shphere.indices.size(), sizeof(uint32_t), 1);

	info2d.shader = ST_INSTANCE;
	mat = ortho(surfSize.x, surfSize.y, -1.0f, 1.0f, 0);
	info2d.flags = ((uint8_t)depth_stencil_State::d_depthtest_enable);
	cb->set_geom_state(vg, &info2d, &mat);
	glm::mat4 ins[5] = {};
	ins[0] = glm::mat4(1.0);
	ins[1] = glm::translate(glm::vec3(200, 10, 0));
	cb->set_instance_mat(vg, ins, 2);
	static vg_image_t img[1] = {};
	uint32_t pxcolord2[2] = { 0xffffffff,0xffffffff, };
	uint32_t pxcolor2[16] = { 0xFFf55555,0xFF2c2c2c, 0xFF9678B4,0xFFf55555,0xFF2c2c2c,0xFFf55555, 0xFF9678B4,0xFFf55555,0xFF2c2c2c,0xFFf55555, 0xFF9678B4,0xFFf55555,0xFF2c2c2c,0xFFf55555, 0xFF9678B4,0xFFf55555, };

	vg_image_desc_t desc = {};
	desc.width = 4;
	desc.height = 4;
	desc.format = VG_FORMAT_RGBA8;
	desc.stride = desc.width * sizeof(int);
	desc.pixels = pxcolor2;
	desc.x = 0, desc.y = 0, desc.w = 4, desc.h = 4;		// 更新矩形区域
	desc.is_copy = true;
	auto pss = sizeof(pxcolor2);
	if (img->valid)
	{
		img->valid = false;
		cb->image_update(vg, img, &desc);
	}
	auto vm = mat * glm::vec4(v[0], 1.0f);
	//cb->add_geometry(vg, img, (float*)v, sizeof(glm::vec3), pxcolord2, 0, (float*)uv, sizeof(glm::vec2), 6, indices, 15, sizeof(uint32_t), 1);
	ovg_image_r rimg = {};
	rimg.img = img;
	rimg.dst = { 10,10,100,100 };
	rimg.rc = { 0,0,4,4 };
	rimg.color = -1;
	cb->add_image(vg, &rimg);
}

int main()
{
	//LoadLibraryA(R"(E:\Program Files\RenderDoc_1.37_64\renderdoc.dll)");
	cout << "Hello ovg." << endl;
	glm::ivec2 surfsize = { 1024,800 };

	ovg_sdl3_ctx g[1] = {};
	font_cache_cx* font_ctx = new_font_cache();
	font_familys_t* familys = new_font_family(font_ctx, (char*)u8"新宋体,Segoe UI Emoji,Times New Roman,Consolas", 0);

	auto cb = new_ctx_cb(font_ctx);
	auto vg = cb->new_rvg(cb->ac);
	if (!vg_sdl3_init(g, surfsize.x, surfsize.y, true)) {
		SDL_Log("Init failed: %s", SDL_GetError());
		return 1;
	}
	auto dev = new_sdl3gpu_device(g->device);
	assert(dev);
	auto format = SDL_GetGPUSwapchainTextureFormat(g->device, g->window);
	ovg_ctx_t* ctx = new_ovgctx_sdl3(dev, format ? format : SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT, SDL_GPU_SAMPLECOUNT_4);
	assert(ctx);
	ovg_canvas_cb* can = new_canvas_cb(font_ctx);

	vg_fbo_t fbo = new_vgfbo_sdl3(ctx, surfsize.x, surfsize.y, g->window);
	bool running = true;
	runtime_cx rtc = {};
	auto str1 = u8"agyh🍕☂️按钮";
	auto str = u8"➗🍕☂️6bg太妹";
	auto rst = glm::mat3x2(1.0);
	auto canvg = can->new_rvg(can->ac);
	auto path = can->new_path(can->ac);
	auto st = can->new_state(can->ac);
	can->set_path(canvg, path, st);

	//vg_text_run_cx run;

	//// 设置字体
	//run.set_font(0, 18);
	//run.set_font_families(familys);
	////gen_text(familys, u8"我123abc➗🍕☂️", -1, 18);
	//// 设置文本 → 自动 shape
	//run.set_text(str, -1);
	//run.shape();

	// 渲染 


	bool testvg = true;
	while (running) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_EVENT_QUIT) running = false;
		}
		if (ovg_get_window_swapchain(ctx, &fbo))
		{
			rtc.begin();
			cb->clear(vg);
			cb->clear(canvg);
			cb->set_fill_rule(vg, VG_FILL_RULE_NON_ZERO);
			glm::vec2 sf = fbo.display_size;
			draw_grid_fill(vg, sf, glm::ivec2(-1, 0xffdfdfdf), 20);
			cb->reset_clip(vg, 1);
			if (testvg)
				draw(cb, vg, fbo.display_size);// 录制图元

			vg->width = fbo.display_size.x; vg->height = fbo.display_size.y;
			canvg->width = fbo.display_size.x; canvg->height = fbo.display_size.y;

			//draw_test3d(&fbo, cb, vg);
			text_style_t style4 = {};
			style4.family = familys;
			style4.fontsize = 100;
			style4.color = 0xFFf2120F;
			style4.color_stroke = 0xFF0000f0;
			style4.stroke = 2;
			style4.color_shadow = 0x86000000;
			style4.shadow_pos = { 5.0f, 5.0f };

			text_st_t text4 = {};
			text4.text = (char*)u8"➗🍕☂️abg描边+阴影";
			text4.text_len = -1;

			text4.pos = { 10.0f, 200.0f };

			cb->move_to(vg, 0, text4.pos.y + 0.5);
			cb->rel_line_to(vg, 1800, 0);
			cb->set_source_color(vg, 0xff00ff00);
			cb->set_line_width(vg, 1);
			cb->stroke(vg);

			cb->add_text(vg, &text4, &style4, nullptr);

			style4.stroke = -2;
			text4.pos = { 10.0f, 120 + 200.0f };

			cb->move_to(vg, 0, text4.pos.y + 0.5);
			cb->rel_line_to(vg, 1800, 0);
			cb->set_source_color(vg, 0xff00ff00);
			cb->set_line_width(vg, 1);
			cb->stroke(vg);
			cb->add_text(vg, &text4, &style4, nullptr);

			int ms = rtc.end();
			//if (ms > 0)
			//	printf("draw build ms: %d\n", ms);
			ovg_draw_data_t dlist[] = { get_draw_list(vg), get_draw_list(canvg) };
			rtc.begin();
			ovg_draw_data(ctx, &fbo, dlist, sizeof(dlist) / sizeof(ovg_draw_data_t));// 提交渲染 
			ms = rtc.end();
			//if (ms > 0)
			//	printf("submit draw ms: %d\n", ms);
		}
		SDL_Delay(16);  /* ~60 FPS */
	}

	SDL_WaitForGPUIdle(g->device);
	/* Cleanup */

	free_vgfbo_sdl3(&fbo);
	free_ovgctx_sdl3(ctx);
	free_sdl3gpu_device(dev);

	SDL_DestroyGPUDevice(g->device);
	SDL_DestroyWindow(g->window);
	SDL_Quit();

	delete_font_family(familys);
	free_font_cache(font_ctx);
	// 删除vg对象
	cb->destroy_rvg(vg);
	if (cb)free_ctx_cb(cb);
	if (can)free_canvas_cb(can);
	return 0;
}
