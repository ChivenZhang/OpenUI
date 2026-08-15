#include <iostream>
#ifndef GLM_FORCE_XYZW_ONLY 
#define GLM_ENABLE_EXPERIMENTAL
//#define GLM_FORCE_ALIGNED
//#define GLM_FORCE_INTRINSICS
// 定义glm启用simd
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_XYZW_ONLY
#include <glm/glm.hpp>  

#include <glm/gtx/intersect.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/closest_point.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp> 
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/euler_angles.hpp>
#endif

#include <vulkan/vulkan.h>
#include "ovg.h"
#include "vg_renderer.h"

#include <Windows.h>
int main()
{
	LoadLibraryA(R"(E:\Program Files\RenderDoc_1.37_64\renderdoc.dll)");
	std::cout << "Hello OpenVG!" << std::endl;
	VGState g[1] = {};

	ovg_canvas_cb* cav = new_canvas_cb();
	static ovg_draw_data dlist = {};
	if (cav)
	{
		auto vg = cav->new_rvg(cav->ac);
		auto path = cav->new_path(cav->ac);
		cav->rectangle(path, 20, 20, 300, 200);
		vg_state_save_t* sst = cav->new_state(cav->ac);
		cav->set_source_color(sst, 0xff0080ff);
		cav->set_fill_rule(sst, VG_FILL_RULE_EVEN_ODD);
		auto pat = cav->new_pattern_linear(cav->ac, 0, 0, 100, 256);
		cav->pattern_add_color_stop(pat, 0, 0, 0, 1, 1);// 蓝
		cav->pattern_add_color_stop(pat, 0.5, 0, 1, 0, 1);// 绿
		cav->pattern_add_color_stop(pat, 1, 1, 0, 0, 1);// 红
		cav->set_source(sst, pat);
		cav->set_path(vg, path, sst);
		cav->fill(vg);
		pat = cav->new_pattern_radial(cav->ac, 150, 100, 25.6, 102.4, 102.4, 128.0, false);
		cav->pattern_add_color_stop(pat, 0, 0, 0, 1, 0);// 蓝
		cav->pattern_add_color_stop(pat, 0.5, 0, 1, 0, 1);// 绿
		cav->pattern_add_color_stop(pat, 1, 1, 0, 0, 1);// 红
		cav->set_source(sst, pat);
		cav->rectangle(path, 120, 50, 300, 200);
		cav->set_path(vg, path, sst);
		cav->fill(vg);
		dlist = get_draw_list(vg);
	}
	if (!VG_Init(g, 800, 600)) {
		SDL_Log("Init failed: %s", SDL_GetError());
		return 1;
	}
	bool running = true;
	while (running) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_EVENT_QUIT) running = false;
		}
		VG_RenderFrame(g, &dlist);
		SDL_Delay(16);  /* ~60 FPS */
	}

	/* Cleanup */
	SDL_ReleaseGPUBuffer(g->device, g->vertexBuffer);
	SDL_ReleaseGPUSampler(g->device, g->linearSampler);
	SDL_ReleaseGPUGraphicsPipeline(g->device, g->pipeline);
	SDL_DestroyGPUDevice(g->device);
	SDL_DestroyWindow(g->window);
	SDL_Quit();
	free_canvas_cb(cav);
	return 0;
}