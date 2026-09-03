#pragma once
/*
字体处理管理


2026/8/19 创建

*/
#ifdef __cplusplus 
#include <set>
#include <map>
#include <unordered_map>
extern "C" {
#endif
	typedef struct hb_raster_image_t hb_raster_image_t;
	typedef struct hb_glyph_position_t hb_glyph_position_t;
	typedef struct hb_buffer_t hb_buffer_t;
	typedef struct hb_draw_funcs_t hb_draw_funcs_t;

	typedef struct _FcConfig    FcConfig;

#ifdef __cplusplus 
}
#endif
enum class SubpixelLayout {
	NONE = 0,   // 未知 / 非标准 → 强制灰阶 AA
	RGB,         // 水平 R-G-B（默认，最常见）
	BGR,         // 水平 B-G-R（常见于部分笔记本/外接屏）
	VRGB,        // 垂直 R-G-B
	VBGR,        // 垂直 B-G-R
};

struct vg_font;
struct FontStyle;
class usp_ac_cx;

// 纹理图像打包器接口
class packer_base
{
public:
	int width = 0, height = 0;
public:
	packer_base();
	virtual ~packer_base();
	virtual void init_target(int width, int height, int heuristic);
	virtual void clear();
	virtual size_t push_rect(glm::ivec4* rc, int n, size_t stride);
	virtual bool push_rect(const glm::ivec2& rc, glm::ivec2* pos);
};
// 创建空装箱对象
packer_base* new_packer(int width, int height);
void free_packer(packer_base* p);

class stb_packer;
#ifndef OVG_H
struct ovg_image_data :public vg_image_t {
	int format;			// 0 rgba, 1 bgra
	int stride;			// 像素宽度
	uint32_t* data;
	bool multiply;		// 是否预乘  
};
#endif
class image_cache_cx
{
public:
	std::vector<stb_packer*> _packer;
	std::vector<ovg_image_data*> _data;
	int width = 1024;					// 纹理宽高
	int height = 1024;
public:
	image_cache_cx();
	~image_cache_cx();
	// 重置大小，会清空原有内容
	void resize(int w, int h);
	// 装箱矩形并填充颜色
	glm::ivec2 fill_color(int w, int h, uint32_t color);
	// 装箱一个矩形，返回坐标/图像
	ovg_image_data* push_cache_size(const glm::ivec2& ss, glm::ivec2* pos, int linegap = 0);
	// 复制像素到装箱，从pos返回坐标
	ovg_image_data* push_cache_bitmap(hb_raster_image_t* img, glm::ivec2* pos, int linegap = 0);
	// 清空所有缓存
	void clear();
private:
	stb_packer* get_last_packer(bool isnew);

};
struct glyph_atlas_entry {
	enum { RASTER, VECTOR } type = RASTER;
	int advance;			// 水平步进
	vg_image_t* atlas_img;	// 指向字体 atlas 纹理
	glm::ivec4 uv_rect;		// (x, y, w, h) 在 atlas 中的像素区域
	glm::ivec2 offset;		// 字形偏移（bearing）x_bearing/y_bearing
	float* path_data;		// 路径数据，不同字号共用一份
	size_t path_size;
	int em_units = 1;
	bool has_color = false;
};
struct path_builder {
	std::vector<float> data;
	float upem;
	float scale;        // 1.0 / upem，用于归一化或直接存原始 upem
	float pen_x = 0;    // 跟踪当前点，处理 close_path 时可能需要
	float pen_y = 0;
	void clear();
	void move_to(float x, float y);
	void line_to(float x, float y);
	void quad_to(float cx, float cy, float x, float y);
	void close();
};

class font_cache_cx
{
public:
	struct user_data_key_t {
		/*< private >*/
		char unused;
	};
	union glyph_key {
		struct {
			glm::u16vec2 k;
			uint32_t glyph_index;
		}s;
		uint64_t v;
	};
	std::map<std::string, std::vector<FontStyle*>> _familys;
	// 自定义加载的字体
	std::map<std::string, std::vector<FontStyle*>> _familys_name;
	std::vector<FontStyle*> _emojis;
	std::vector<FontStyle*> _temp;
	// key=[uint16字体id，uint16字号，uint32字形id]
	std::unordered_map<uint64_t, glyph_atlas_entry> glyph_cache;
	std::unordered_map<uint64_t, glyph_atlas_entry> _sub_glyph_cache;
	// 位图缓存
	image_cache_cx image_cache;
	path_builder temp_path;
	hb_draw_funcs_t* funcs = 0;
	FcConfig* cfg = 0;
	// 内存分配器
	vg_alloc_cx* ac = 0;
	uint32_t next_font_id = 1;
	int max_raster_size = 256;
	int min_subpixel = 0;
	int references = 1;
public:
	font_cache_cx();
	~font_cache_cx();
	void set_alloc_ptr(vg_alloc_cx* p);
	void get_sys_family();
	void clear_sys();
	void clear_load();
	vg_font* get_font(const char* family, const char* style, int weight, int slant);

	void select_font_face(const char* family, const char* style, int weight, int slant);
	// name自定义名称，可空
	bool load_font_from_path(const char* path, const char* name);
	bool add_font_dir(const char* dir);
	bool load_font_from_memory(unsigned char* fontBuffer, long fontBufferByteSize, const char* name);
	const char* weight_to_string(int w);
	const char* slant_to_string(int s);
	glyph_atlas_entry* get_cache_lookup_glyph(hb_font_t* font, uint32_t glyph_id, int fontsize);
private:

	size_t mk_font(std::map<std::string, std::vector<FontStyle*>>* p, const char* family, const char* style, int weight, int slant);
};

//vgText text_run_new(const font_familys_t* familys, int font_size, const char* text);
//vgText text_run_new_with_length(const font_familys_t* familys, int font_size, const char* text, uint32_t length);
//void text_run_destroy(vgText textRun);
// 设置字体，字号，0则不改
//void text_run_set_font(vgText textRun, const font_familys_t* familys, int font_size);
// 重新设置文本
//void show_text_set(vgText textRun, const char* text, uint32_t length);
//void show_text_run(vgText textRun);
//void show_text_run_path(vgText textRun);
//void text_run_get_extents(vgText textRun, vg_text_extents_t* extents);
//uint32_t text_run_get_glyph_count(vgText textRun);
//void text_run_get_glyph_position(vgText textRun, uint32_t index, vg_glyph_info_t* pGlyphInfo);

struct vg_glyph_info_t {
	uint32_t  glyph_id;
	float     x_offset;
	float     y_offset;
	float     x_advance;
	float     y_advance;
	// 指向缓存条目（位图或矢量）
	glyph_atlas_entry* cache_entry;
};

struct vg_text_extents_t {
	float width;
	float height;
	float x_advance;
	float y_advance;
	float x_bearing;
	float y_bearing;
};
struct glyph_draw_cmd {
	enum { RASTER, VECTOR } type;
	glyph_atlas_entry* entry;
	glm::vec2         pos;      // 字形位置（已加 pen offset）
	glm::vec2         size;     // 像素尺寸
	glm::vec4         uv_rect;  // 归一化 UV
	uint32_t          color;

	int fontsize = 18;
};

struct text_draw_list {
	std::vector<glyph_draw_cmd> cmds;
	vg_text_extents_t           extents;
	int fontsize = 18;
	bool has_color = false;	// 彩色启用
	void clear();
	void push_raster(glyph_atlas_entry* e, float x, float y, float w, float h, const glm::vec4& uv, uint32_t c);
	void push_vector(glyph_atlas_entry* e, float x, float y, uint32_t c);
};
class vg_text_run_cx
{
public:
	enum render_mode { RASTER_FIRST, VECTOR_ONLY };
private:
	const font_familys_t* _ffs = nullptr;
	hb_font_t* _primary_font = nullptr;
	int                   _fontsize = 16;
	std::vector<uint32_t> _utf32;

	// 当前 shaping 结果
	hb_buffer_t* _buf = nullptr;
	vg_text_extents_t     _extents{};
	std::vector<vg_glyph_info_t> _glyphs;
	uint32_t              _glyph_count = 0;
	int min_subpixel = 32;
	// 缓存引用
	font_cache_cx* _cache = nullptr;

	// 多 run 支持（不同 font fallback）
	struct text_run {
		hb_buffer_t* buf = nullptr;
		hb_font_t* font = nullptr;
		int          fontsize = 0;
		uint32_t     start_cp = 0;
		uint32_t     end_cp = 0;
	};
	std::vector<text_run> _runs;
public:
	vg_text_run_cx();
	~vg_text_run_cx();
	void set_min_subpixel(int sp);
	// 设置文本（UTF-8），触发重新 shape
	void set_text(const void* str8, size_t len = -1);

	// 设置字体参数
	void set_font(hb_font_t* font, int fontsize);

	// 设置字体集（多 family fallback）
	void set_font_families(const font_familys_t* ffs, int fontsize);

	// 执行 shape + 缓存 lookup（内部调 set_text/set_font 后自动调）
	void shape();

	// 清除所有缓存引用（字体变了时调用，不释放 atlas 数据）
	void clear_glyphs();

	void populate_draw_list(text_draw_list& list, float origin_x, float origin_y, uint32_t color, render_mode m);
	// 访问结果
	const vg_text_extents_t& extents() const { return _extents; }
	const std::vector<vg_glyph_info_t>& glyphs() const { return _glyphs; }
	uint32_t glyph_count() const { return _glyph_count; }

private:
	void free_buffer();
	void shape_run(size_t run_start, size_t run_end, hb_font_t* font, int fontsize);

};

bool write_png_bgra(const char* path, const uint8_t* bgra, int w, int h);
