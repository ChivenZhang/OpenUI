#pragma once
/*
扩展接口
*/
#ifndef OVG_E_H
#define OVG_E_H 
#include "ovg_c.h"
// 对象模式
struct ovg_canvas_cb {
	mem_resource_t* ac;
	// 路径操作
	ovg_path_t* (*new_path)(mem_resource_t* ac);
	void(*destroy_path)(ovg_path_t* path);
	void(*clear_path)(ovg_path_t* path);
	void(*close_path)(ovg_path_t* path);
	void(*new_sub_path)(ovg_path_t* path);
	void(*path_extents)(ovg_path_t* path, float* x1, float* y1, float* x2, float* y2);
	void(*get_current_point)(ovg_path_t* path, float* x, float* y);
	size_t(*get_segment_count)(ovg_path_t* path);
	void(*set_segment_color)(ovg_path_t* path, size_t idx, uint32_t color);
	// 添加数据到当前路径，参考path_type_e
	void(*add_path)(ovg_path_t* path, float* data, size_t count);
	void(*move_to)(ovg_path_t* path, float x, float y);
	void(*rel_move_to)(ovg_path_t* path, float x, float y);
	void(*line_to)(ovg_path_t* path, float x, float y);
	void(*rel_line_to)(ovg_path_t* path, float dx, float dy);
	void(*arc)(ovg_path_t* path, float xc, float yc, float radius, float a1, float a2);
	void(*arc_negative)(ovg_path_t* path, float xc, float yc, float radius, float a1, float a2);
	// 有缩放时，先执行set_path一次再执行curve_to
	void(*curve_to)(ovg_path_t* path, float x1, float y1, float x2, float y2, float x3, float y3);
	void(*rel_curve_to)(ovg_path_t* path, float x1, float y1, float x2, float y2, float x3, float y3);
	void(*quadratic_to)(ovg_path_t* path, float x1, float y1, float x2, float y2);
	void(*rel_quadratic_to)(ovg_path_t* path, float x1, float y1, float x2, float y2);
	void(*rectangle)(ovg_path_t* path, float x, float y, float w, float h);
	void(*rounded_rectangle)(ovg_path_t* path, float x, float y, float w, float h, float radius);
	void(*rounded_rectangle2)(ovg_path_t* path, float x, float y, float w, float h, float rx, float ry);
	void(*ellipse)(ovg_path_t* path, float radiusX, float radiusY, float x, float y, float rotationAngle);
	void(*elliptic_arc_to)(ovg_path_t* path, float x, float y, bool large_arc_flag, bool sweep_flag, float rx, float ry, float phi);
	void(*rel_elliptic_arc_to)(ovg_path_t* path, float x, float y, bool large_arc_flag, bool sweep_flag, float rx, float ry, float phi);
	void(*circle)(ovg_path_t* path, float x, float y, float radius);
	// 配置
	vg_state_save_t* (*new_state)(mem_resource_t* ac);
	void (*state_destroy)(vg_state_save_t* p);
	void(*set_opacity)(vg_state_save_t* ctx, float opacity);
	void(*set_source_color)(vg_state_save_t* ctx, uint32_t c);
	void(*set_source_rgba)(vg_state_save_t* ctx, float r, float g, float b, float a);
	void(*set_source_rgb)(vg_state_save_t* ctx, float r, float g, float b);
	void(*set_line_width)(vg_state_save_t* ctx, float width);
	void(*set_miter_limit)(vg_state_save_t* ctx, float limit);
	void(*set_line_cap)(vg_state_save_t* ctx, int cap);
	void(*set_line_join)(vg_state_save_t* ctx, int join);
	void(*set_source_surface)(vg_state_save_t* ctx, vg_image_t* surf, float x, float y);
	void(*set_source)(vg_state_save_t* ctx, vg_pattern_t* pat);
	void(*set_operator)(vg_state_save_t* ctx, int op);
	void(*set_fill_rule)(vg_state_save_t* ctx, int fr);
	void(*set_dash)(vg_state_save_t* ctx, const float* dashes, uint32_t num_dashes, float offset);		// 虚线
	void(*set_dash8)(vg_state_save_t* ctx, uint64_t dashes, uint32_t num_dashes, float offset);								// 虚线,用uint8_t v8[8]表示
	void(*translate)(vg_state_save_t* ctx, float dx, float dy);
	void(*scale)(vg_state_save_t* ctx, float sx, float sy);
	void(*rotate)(vg_state_save_t* ctx, float radians);
	void(*transform)(vg_state_save_t* ctx, const void* matrix);
	void(*set_matrix)(vg_state_save_t* ctx, const void* matrix);
	void(*get_matrix)(vg_state_save_t* ctx, void* matrix);
	void(*identity_matrix)(vg_state_save_t* ctx);
	void(*matrix_init)(void* mat, float xx, float yx, float xy, float yy, float x0, float y0);

	// 图案：渐变/图片 
	vg_pattern_t* (*new_pattern_linear)(mem_resource_t* ac, float x0, float y0, float x1, float y1);
	vg_pattern_t* (*new_pattern_radial)(mem_resource_t* ac, float cx0, float cy0, float radius0, float cx1, float cy1, float radius1, bool is_ellipse);
	vg_pattern_t* (*new_pattern_sweep)(mem_resource_t* ac, float cx, float cy, float start_angle, float end_angle);
	int (*pattern_add_color_stop)(vg_pattern_t* pat, float o, float r, float g, float b, float a);
	int (*pattern_set_color_stop)(vg_pattern_t* pat, int idx, float o, float r, float g, float b, float a);
	void(*pattern_set_matrix)(vg_pattern_t* pat, const void* matrix);	// mat3x2
	void(*pattern_set_extend)(vg_pattern_t* pat, int extend);
	void(*pattern_set_filter)(vg_pattern_t* pat, int filter);
	void(*pattern_destroy)(vg_pattern_t* pat);
	// 更新纹理，vg_image_t指针由用户创建管理
	void (*image_update)(rvg_t* p, vg_image_t* img, vg_image_desc_t* desc);
	//标记图片不再使用（后端延迟释放 GPU 纹理）
	void (*image_destroy)(rvg_t* p, vg_image_t* img);

	// 渲染操作，rvg_t可以多次执行fill或stroke/clip
	rvg_t* (*new_rvg)(mem_resource_t* ac);
	void (*destroy_rvg)(rvg_t* p);
	void(*clear)(rvg_t* v);			// 清空画布
	void(*set_path)(rvg_t* v, ovg_path_t* path, vg_state_save_t* st);// 绑定路径和状态
	void(*stroke)(rvg_t* v);
	void(*stroke_preserve)(rvg_t* v);
	void(*fill)(rvg_t* v);
	void(*fill_preserve)(rvg_t* v);
	void(*paint)(rvg_t* v);			// 全屏渲染
	void(*reset_clip)(rvg_t* v, uint8_t ref);	// 重置裁剪
	void(*clip)(rvg_t* v);			// 路径裁剪，清空当前路径
	void(*clip_preserve)(rvg_t* v);	// 路径裁剪
	void(*clip_rect)(rvg_t* v, int x, int y, int width, int height);	// 矩形裁剪
	void(*set_clip_rect)(rvg_t* v, void* rc);	// 矩形裁剪,int[4]
	void(*get_clip_rect)(rvg_t* v, void* rc);	// 获取矩形裁剪

	// 添加文本，风格，渲染区可选
	void (*add_text)(rvg_t* dc, text_st_t* p, text_style_t* ts, text_box_rt* box);
	// 普通图片，支持九宫格、混合颜色
	void (*add_image)(rvg_t* dc, ovg_image_r* r);
	// 原始三角形
	// gem_info_t或矩阵输入空指针则不修改该值
	void (*set_geom_state)(rvg_t* dc, gem_info_t* info, const void* matrix4x4);
	// 实例化
	size_t(*set_instance_mat)(rvg_t* dc, const void* instance_mat, size_t instance_count);
	// 添加几何数据到缓冲区，xy顶点坐标，color顶点颜色，uv顶点纹理坐标，indices索引数据，color_type=0表示float4，1表示uint32_t
	void (*add_geometry)(rvg_t* dc, vg_image_t* texture, const float* xy, int xy_stride, const void* color, int color_stride, const float* uv, int uv_stride, int num_vertices, const void* indices, int num_indices, int size_indices, int color_type);
	// 添加3D几何数据到缓冲区，xyz顶点坐标，color顶点颜色（双面则要双倍），uv顶点纹理坐标，indices索引数据
	void (*add_geometry3d)(rvg_t* dc, vg_image_t* texture, const float* xyz, int xyz_stride, const void* color, int color_stride, const float* uv, int uv_stride, int num_vertices, const void* indices, int num_indices, int size_indices, int color_type);

};
 
// 对象模式接口，如果没字体ctx则无法渲染文本
ovg_canvas_cb* new_canvas_cb();
void free_canvas_cb(ovg_canvas_cb*); 
#endif // !OVG_E_H
