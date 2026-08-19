// ovg.cpp: 定义应用程序的入口点。
//

#include "ovg_main.h" 
#include "ovg_renderer_sdl3.h"
#include <Windows.h>
#include <cmath>
using namespace std;
/* ---------- 颜色辅助 ---------- */
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
	cb->clear(vg);
	cb->set_fill_rule(vg, VG_FILL_RULE_NON_ZERO);
	glm::vec2 sf = surfsize;
	//sf *= 0.5;
	draw_grid_fill(vg, sf, glm::ivec2(-1, 0xffdfdfdf), 20);


	//cb->rounded_rectangle(vg, 20, 20, 600, 600, 10);
	//cb->clip(vg);
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
	cb->set_source_rgba(vg, 0, 0, 0, 1);	cb->stroke(vg); //描边

	//cb->rectangle(vg, 20, 150, 200, 100, 10);
	//cb->clip(vg, vg);// 圆角矩形裁剪
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
	cb->reset_clip(vg);
	//cb->rectangle(vg, 128,128, 300, 200);
	cb->translate(vg, 300, 0);
	{
		auto pat = cb->new_pattern_radial(vg, 150, 100, 25.6, 102.4, 102.4, 128.0, false);
		cb->pattern_add_color_stop(pat, 0, 0, 0, 1, 0);// 蓝
		cb->pattern_add_color_stop(pat, 0.5, 0, 1, 0, 1);
		cb->pattern_add_color_stop(pat, 1, 1, 0, 0, 1);// 红
		cb->set_source(vg, pat);
	}
	cb->arc(vg, 128, 128.0, 76.8, 0, 2 * M_PI);
	cb->fill(vg);// 填充

	cb->set_source_rgba(vg, 0.1f, 0.9f, 0.1f, 1.0f);
	cb->move_to(vg, 100, 100);
	cb->rel_line_to(vg, 200, 0);
	cb->rel_line_to(vg, 0, 150);
	cb->rel_line_to(vg, -200, 0);
	cb->close_path(vg);

	cb->move_to(vg, 150, 150);
	cb->rel_line_to(vg, 0, 50);
	cb->rel_line_to(vg, 100, 0);
	cb->rel_line_to(vg, 0, -50);
	cb->close_path(vg);

	cb->fill(vg);
}

int main()
{
	LoadLibraryA(R"(E:\Program Files\RenderDoc_1.37_64\renderdoc.dll)");
	cout << "Hello ovg." << endl;
	glm::ivec2 surfsize = { 1024,800 };
	auto cb = new_ctx_cb();
	auto vg = cb->new_rvg(cb->ac);


	VGState g[1] = {};

	if (!VG_Init(g, surfsize.x, surfsize.y)) {
		SDL_Log("Init failed: %s", SDL_GetError());
		return 1;
	}
	auto dev = new_sdl3gpu_device(g->device);
	assert(dev);
	auto format = SDL_GetGPUSwapchainTextureFormat(g->device, g->window);
	ovg_ctx_t* ctx = new_ovgctx_sdl3(dev, format ? format : SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT, SDL_GPU_SAMPLECOUNT_4);
	assert(ctx);

	vg_fbo_t fbo = new_vgfbo_sdl3(ctx, surfsize.x, surfsize.y, g->window);
	bool running = true;
	runtime_cx rtc = {};
	while (running) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_EVENT_QUIT) running = false;
		}
		if (ovg_get_window_swapchain(ctx, &fbo))
		{
			rtc.begin();
			draw(cb, vg, surfsize);// 录制图元
			int ms = rtc.end();
			/*if (ms > 0)
				printf("draw build ms: %d\n", ms);*/
			auto dlist = get_draw_list(vg);
			rtc.begin();
			ovg_draw_data(ctx, &fbo, &dlist);// 提交渲染
			ms = rtc.end();
			/*if (ms > 0)
				printf("submit draw ms: %d\n", ms);*/
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

	// 删除vg对象
	cb->destroy_rvg(vg);
	if (cb)free_ctx_cb(cb);
	return 0;
}
