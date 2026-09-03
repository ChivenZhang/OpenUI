
/*
字体处理管理


2026/8/19 创建

*/

#include <map>
#include <string>
#include <set>
#include <stack>
#include <unordered_map>
#include <SDL3/SDL.h>
#include <vector>

#include <locale.h>
#include <string.h>
#include <wchar.h>

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#define VG_USE_FONTCONFIG

#include <fontconfig/fontconfig.h> 

#ifndef GLM_FORCE_XYZW_ONLY 
#define GLM_ENABLE_EXPERIMENTAL 
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
//#include "mapView.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ICU */
#include <unicode/ubrk.h>
#include <unicode/ubidi.h>
#include <unicode/ustring.h>
#include <unicode/utext.h>

/* HarfBuzz */
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ot.h> 
#include <harfbuzz/hb-raster.h> 

#include "ovg.h"
#include "ovg_fonts.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#include <stb_image_write.h>
#include <stb_rect_pack.h>

struct vg_font :public font_family_t {
	hb_buffer_t* hb_buffer;
	hb_language_t hb_language;
	hb_raster_draw_t* rdr;
	hb_raster_paint_t* pnt;
};

struct FontStyle {
	uint32_t id;
	std::string family;
	std::set<std::string> alias;
	std::string style;
	std::string file;
	vg_font font;
	int weight;
	int slant;
	int index;
	bool slnt_applied = false;       // 是否已设置变量 slnt
};
void hb_res_init(vg_font* hp, hb_font_t* font);
void free_hb_res(vg_font* hp);
bool gfont_copy_image(ovg_image_data* dst, int rx, int ry, uint32_t color, hb_raster_image_t* img_src, const glm::ivec4& ow, bool origin, bool type, SubpixelLayout pixelLayout);
namespace ovg {
	uint64_t align_up(uint64_t val, uint64_t alignment)
	{
		return (val + alignment - (uint64_t)1) & ~(alignment - (uint64_t)1);
	}
	// align val to the previous multiple of alignment
	uint64_t align_down(uint64_t val, uint64_t alignment)
	{
		return val & ~(alignment - (uint64_t)1);
	}

	static int utf8_to_utf16(const char* utf8, UChar* out16, int cap16, UErrorCode* st)
	{
		int32_t len = 0;
		u_strFromUTF8(out16, cap16, &len, utf8, -1, st);
		return len;
	}
	static void utf16_slice_to_utf8(const UChar* u16, int32_t start, int32_t len, char* out, int cap)
	{
		UErrorCode st = U_ZERO_ERROR;
		u_strToUTF8(out, cap, NULL, u16 + start, len, &st);
	}
}
//!ovg
static hb_font_t* load_font(const char* family, const char* style)
{
	/* Fontconfig：只负责“找文件” */
	FcConfig* cfg = FcInitLoadConfigAndFonts();
	FcPattern* pat = FcPatternCreate();

	FcPatternAddString(pat, FC_FAMILY, (FcChar8*)family);
	if (style)
		FcPatternAddString(pat, FC_STYLE, (FcChar8*)style);

	FcConfigSubstitute(cfg, pat, FcMatchPattern);
	FcDefaultSubstitute(pat);

	FcResult result;
	FcPattern* match = FcFontMatch(cfg, pat, &result);
	if (!match) {
		fprintf(stderr, "Fontconfig: no match for %s\n", family);
		FcPatternDestroy(pat);
		FcConfigDestroy(cfg);
		return NULL;
	}

	FcChar8* file = NULL;
	int idx = 0;
	FcPatternGetString(match, FC_FILE, 0, &file);
	FcPatternGetInteger(match, FC_INDEX, 0, &idx);

	/* HarfBuzz：从文件创建 face */
	hb_blob_t* blob = hb_blob_create_from_file((const char*)file);
	if (hb_blob_get_length(blob) == 0) {
		fprintf(stderr, "HarfBuzz: failed to load %s\n", file);
		hb_blob_destroy(blob);
		FcPatternDestroy(match);
		FcPatternDestroy(pat);
		FcConfigDestroy(cfg);
		return NULL;
	}

	hb_face_t* hb_face = hb_face_create(blob, idx);
	hb_blob_destroy(blob);   /* face 已引用 blob */

	if (hb_face_get_glyph_count(hb_face) == 0) {
		fprintf(stderr, "HarfBuzz: invalid face %s\n", file);
		hb_face_destroy(hb_face);
		FcPatternDestroy(match);
		FcPatternDestroy(pat);
		FcConfigDestroy(cfg);
		return NULL;
	}

	/* 创建 font（带 scale） */
	hb_font_t* hb_font = hb_font_create(hb_face);
	hb_font_set_scale(hb_font, 16 * 64, 16 * 64); // 16px
	hb_ot_font_set_funcs(hb_font);                 // ✅ 用 OT 回调

	hb_face_destroy(hb_face);  /* font 已引用 face */

	FcPatternDestroy(match);
	FcPatternDestroy(pat);
	FcConfigDestroy(cfg);
	return hb_font;
}
font_cache_cx::font_cache_cx()
{
	get_sys_family();
}

font_cache_cx::~font_cache_cx()
{
	if (funcs)
		hb_draw_funcs_destroy(funcs); funcs = 0;
	clear_load();
	clear_sys();
	if (cfg)
		FcConfigDestroy(cfg);
	cfg = 0;
	_emojis.clear();
}

void font_cache_cx::set_alloc_ptr(vg_alloc_cx* p)
{
	if (p)ac = p;
}

void font_cache_cx::clear_sys()
{
	_temp.clear();
	for (auto& [k, v] : _familys) {
		for (auto it : v) {
			if (it) {
				if (it->font.font)
					free_hb_res(&it->font);
				delete it;
			}
		}
	}
	_familys.clear();
}

void font_cache_cx::clear_load()
{
	_temp.clear();
	for (auto& [k, v] : _familys_name) {
		for (auto it : v) {
			if (it) {
				if (it->font.font)
					free_hb_res(&it->font);
				delete it;
			}
		}
	}
	_familys_name.clear();
}

hb_font_t* load_font(const char* file, int idx) {
	hb_font_t* font = 0; hb_face_t* face = 0;
	hb_blob_t* blob = hb_blob_create_from_file((const char*)file);
	do {
		if (hb_blob_get_length(blob) == 0) {
			break;
		}
		face = hb_face_create(blob, idx);
		if (!face)break;
		auto gcont = hb_face_get_glyph_count(face);
		if (!gcont) {
			break;
		}
		font = hb_font_create(face);
	} while (0);
	if (face)
		hb_face_destroy(face);
	if (blob)
		hb_blob_destroy(blob);
	return font;
}
std::string get_pat_str(FcPattern* font, const char* o, int n)
{
	FcChar8* s = nullptr;
	std::string r;
	if (o && ::FcPatternGetString(font, o, n, &s) == FcResultMatch)
	{
		if (s)
		{
			r = (char*)s;
		}
	}
	return r;
}
std::set<std::string> get_pat_strs(FcPattern* font, const char* o)
{
	std::set<std::string> rv;
	FcChar8* s = nullptr;
	int n = 0;
	do {
		if (o && ::FcPatternGetString(font, o, n, &s) == FcResultMatch)
		{
			if (s)
			{
				rv.insert((char*)s);
			}
		}
		else { break; }
		n++;
	} while (1);
	return rv;
}
void get_pat_strs(FcPattern* font, const char* o, std::set<std::string>& rv)
{
	FcChar8* s = nullptr;
	int n = 0;
	do {
		if (o && ::FcPatternGetString(font, o, n, &s) == FcResultMatch)
		{
			if (s)
			{
				rv.insert((char*)s);
			}
		}
		else { break; }
		n++;
	} while (1);
	return;
}

inline float font_get_slant_angle(hb_font_t* font) {
	return hb_style_get_value(font, HB_STYLE_TAG_SLANT_ANGLE);
}

inline float font_get_italic_value(hb_font_t* font) {
	return hb_style_get_value(font, HB_STYLE_TAG_ITALIC);
}
// Fontconfig slant → 期望的 slnt 角度（右倾为负）
inline float fc_slant_to_slant_angle(int fc_slant) noexcept {
	switch (fc_slant) {
	case FC_SLANT_ITALIC:
		return -12.0f;   // 典型 italic 倾斜
	case FC_SLANT_OBLIQUE:
		return -10.0f;   // oblique 通常略小于 italic
	case FC_SLANT_ROMAN:
	default:
		return 0.0f;
	}
}
//SLANT_ANGLE：OpenType slnt 轴值
//ITALIC：0 = Roman，1 = Italic（COLRv1 / STAT 表）
//四、关键逻辑：是否设置 slnt（变量字体才设）
bool font_supports_slnt_axis(hb_font_t* font) {
	hb_face_t* face = hb_font_get_face(font);
	unsigned int axis_count = hb_ot_var_get_axis_count(face);
	auto axc = axis_count;
	for (unsigned i = 0; i < axis_count; i++) {
		hb_ot_var_axis_info_t info;
		hb_ot_var_get_axis_infos(face, i, &axc, &info);
		if (info.tag == HB_STYLE_TAG_SLANT_ANGLE)
			return true;
	}
	return false;
}
void font_cache_cx::get_sys_family()
{
	if (!cfg)
		cfg = FcInitLoadConfigAndFonts();
	FcPattern* pat = FcPatternCreate();
	FcObjectSet* os = FcObjectSetBuild(
		FC_FAMILY, FC_STYLE, FC_WEIGHT, FC_SLANT, FC_FILE, FC_FULLNAME, FC_INDEX, NULL);
	FcFontSet* fs = FcFontList(cfg, pat, os);

	for (int i = 0; i < fs->nfont; i++) {
		FcPattern* p = fs->fonts[i];
		FcChar8* family = NULL, * style = NULL;
		int weight = 0, slant = 0;
		char* file = NULL;
		int index = 0;
		FcPatternGetString(p, FC_FAMILY, 0, &family);
		FcPatternGetString(p, FC_STYLE, 0, &style);
		FcPatternGetInteger(p, FC_WEIGHT, 0, &weight);
		FcPatternGetInteger(p, FC_SLANT, 0, &slant);
		FcPatternGetString(p, FC_FILE, 0, (FcChar8**)&file);
		FcPatternGetInteger(p, FC_INDEX, 0, &index);
		if (!style)style = (FcChar8*)"";
		if (!file || !family || !(*file) || !(*family))continue;
		hb_font_t* font = 0;
		auto it = new FontStyle();
		if (it)
		{
			get_pat_strs(p, FC_FAMILY, it->alias);
			get_pat_strs(p, FC_FULLNAME, it->alias);
			it->family = (char*)family;
			std::string fname = it->family;
			if (fname.find("moji") != std::string::npos)
			{
				_emojis.push_back(it);
			}
			it->style = (char*)style;
			it->file = file;
			it->weight = weight;
			it->slant = slant;
			it->index = index;

			_familys[(char*)family].push_back(it);
		}
	}
	FcFontSetDestroy(fs);
	FcObjectSetDestroy(os);
	FcPatternDestroy(pat);

}
vg_font* font_cache_cx::get_font(const char* family, const char* style, int weight, int slant)
{
	bool mb = mk_font(&_familys_name, family, style, weight, slant);
	if (_temp.size()) {
		return &_temp[0]->font;
	}
	bool ab = mk_font(&_familys, family, style, weight, slant);
	if (_temp.size()) {
		return &_temp[0]->font;
	}
	return nullptr;
}

void font_cache_cx::select_font_face(const char* family, const char* style, int weight, int slant)
{}

bool font_cache_cx::load_font_from_path(const char* path, const char* name)
{
	if (!path || !*path)return false;
	bool hr = FcConfigAppFontAddFile(cfg, (FcChar8*)path);
	if (hr)
	{

	}
	return hr;
}

bool font_cache_cx::add_font_dir(const char* dir)
{
	if (!dir || !*dir)return false;
	return FcConfigAppFontAddDir(cfg, (FcChar8*)dir);
}

bool font_cache_cx::load_font_from_memory(unsigned char* fontBuffer, long fontBufferByteSize, const char* name)
{
	return false;
}

static hb_user_data_key_t fontStyleKey = {};
static hb_user_data_key_t g_font_cache_key = {};

size_t font_cache_cx::mk_font(std::map<std::string, std::vector<FontStyle*>>* pt, const char* family, const char* style, int weight, int slant)
{
	size_t n = 0;
	_temp.clear();
	if (!family || !(*family) || !pt)return n;
	{
		auto it = pt->find(family);
		if (it != pt->end()) {
			auto& v = it->second;
			for (auto& vt : v)
			{
				bool bst = !style || !(*style);
				if (!bst) {
					bst = (style && *style && vt->style == style);
				}
				bool bw = vt->weight == weight || weight < 1;
				bool bsl = vt->slant == slant || slant < 1;
				if (bst && bw && bsl) {
					_temp.push_back(vt);
				}
			}
		}
	}
	if (_temp.empty()) {
		for (auto& [k, v] : *pt) {
			for (auto vt : v) {
				if (vt) {
					if (vt->alias.find(family) != vt->alias.end())
					{
						bool bst = !style || !(*style);
						if (!bst) {
							bst = (style && *style && vt->style == style);
						}
						bool bw = vt->weight == weight || weight < 1;
						bool bsl = vt->slant == slant || slant < 1;
						if (bst && bw && bsl) {
							_temp.push_back(vt);
						}
					}
				}
			}
		}
	}
	for (auto it : _temp) {
		if (!it->font.font)
		{
			it->font.font = load_font(it->file.c_str(), it->index);
		}
		if (it->font.font)
		{
			it->id = next_font_id;
			hb_font_set_user_data(it->font.font, (hb_user_data_key_t*)&fontStyleKey, (void*)(uintptr_t)it, nullptr, true);
			hb_font_set_user_data(it->font.font, (hb_user_data_key_t*)&g_font_cache_key, (void*)(uintptr_t)this, nullptr, true);
			next_font_id++;
			hb_res_init(&it->font, 0);
			if (font_supports_slnt_axis(it->font.font)) {
				//float desired = fc_slant_to_slant_angle(slant);
				//hb_font_set_variation(font, HB_OT_TAG_VAR_AXIS_SLANT, desired);
				it->slnt_applied = true;
			}
			n++;
		}
	}
	return n;
}

/* ─────────────────────────────────────────────
 * 对一行逻辑文本做 Bidi 重排 + HarfBuzz 整形
 * ───────────────────────────────────────────── */
static void
shape_line(hb_font_t* hb_font,
	const UChar* line_u16,
	int32_t       line_len,
	UBiDiDirection base_dir)
{
	UErrorCode st = U_ZERO_ERROR;

	/* 1) ICU Bidi：设置段落方向 */
	UBiDi* bidi = ubidi_open();
	ubidi_setPara(bidi, line_u16, line_len, base_dir, NULL, &st);
	if (U_FAILURE(st)) {
		fprintf(stderr, "ubidi_setPara failed: %s\n", u_errorName(st));
		ubidi_close(bidi);
		return;
	}

	/* 2) 获取视觉顺序的 run 列表 */
	int32_t run_count = ubidi_countRuns(bidi, &st);
	if (U_FAILURE(st)) {
		ubidi_close(bidi);
		return;
	}

	/* 3) 按视觉顺序拼接逻辑 UTF-16 */
	UChar visual_u16[512];
	int32_t vis_len = 0;

	for (int32_t r = 0; r < run_count; r++) {
		int32_t run_start, run_len;
		UBiDiDirection run_dir = ubidi_getVisualRun(bidi, r, &run_start, &run_len);
		run_start = ubidi_getVisualRun(bidi, r, &run_start, &run_len); /* 返回逻辑起点 */

		if (run_dir == UBIDI_LTR) {
			for (int32_t i = 0; i < run_len; i++)
				visual_u16[vis_len++] = line_u16[run_start + i];
		}
		else {
			for (int32_t i = run_len - 1; i >= 0; i--)
				visual_u16[vis_len++] = line_u16[run_start + i];
		}
	}

	/* 4) UTF-16 → UTF-8（给 HarfBuzz） */
	char utf8_buf[1024];
	u_strToUTF8(utf8_buf, sizeof(utf8_buf), NULL,
		visual_u16, vis_len, &st);

	/* 5) HarfBuzz 整形 */
	hb_buffer_t* buf = hb_buffer_create();
	hb_buffer_add_utf8(buf, utf8_buf, -1, 0, -1);

	/* 设置 script / direction（用 Bidi 推断的段落方向） */
	hb_buffer_set_script(buf, HB_SCRIPT_COMMON); /* 让 HB 自动检测 */
	hb_buffer_set_direction(buf,
		(base_dir == UBIDI_RTL) ? HB_DIRECTION_RTL : HB_DIRECTION_LTR);
	hb_buffer_set_language(buf, hb_language_from_string("en", -1));
	hb_buffer_guess_segment_properties(buf);

	hb_shape(hb_font, buf, NULL, 0);

	/* 6) 打印 glyph 结果 */
	unsigned int glyph_count;
	hb_glyph_info_t* infos = hb_buffer_get_glyph_infos(buf, &glyph_count);
	hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(buf, &glyph_count);

	printf("  ├─ glyphs (%u): ", glyph_count);
	for (unsigned int i = 0; i < glyph_count; i++) {
		printf("[gid=%u x=%d y=%d dx=%d dy=%d] ",
			infos[i].codepoint,
			pos[i].x_offset, pos[i].y_offset,
			pos[i].x_advance, pos[i].y_advance);
	}
	printf("\n");

	hb_buffer_destroy(buf);
	ubidi_close(bidi);
}
UBiDiDirection get_dir(UChar32 c)
{
	UBiDiDirection dir = (UBiDiDirection)UBIDI_DEFAULT_LTR;
	if (u_charDirection(c) == U_RIGHT_TO_LEFT || u_charDirection(c) == U_ARABIC_NUMBER) {
		dir = UBIDI_RTL;
	}
	return dir;
}
hb_font_t* find_face_for_codepoint(hb_font_t* font, uint32_t cp, uint32_t variation_selector)
{
	hb_codepoint_t glyph{};

	if (variation_selector != 0) {
		if (hb_font_get_variation_glyph(font,
			cp,
			variation_selector,
			&glyph) ||
			hb_font_get_nominal_glyph(font, cp, &glyph)) {
			return font;
		}
	}
	else {
		if (hb_font_get_nominal_glyph(font, cp, &glyph)) {
			return font;
		}
	}
	return 0;
}

const char* font_cache_cx::weight_to_string(int w) {
	switch (w) {
	case FC_WEIGHT_THIN:		return "Thin";
	case FC_WEIGHT_EXTRALIGHT:	return "ExtraLight";
	case FC_WEIGHT_LIGHT:		return "Light";
	case FC_WEIGHT_DEMILIGHT:	return "DemiLight";
	case FC_WEIGHT_BOOK:		return "Book";
	case FC_WEIGHT_REGULAR:		return "Regular";
	case FC_WEIGHT_MEDIUM:		return "Medium";
	case FC_WEIGHT_DEMIBOLD:	return "DemiBold";
	case FC_WEIGHT_BOLD:		return "Bold";
	case FC_WEIGHT_EXTRABOLD:	return "ExtraBold";
	case FC_WEIGHT_BLACK:		return "Black";
	case FC_WEIGHT_EXTRABLACK:	return "ExtraBlack";
	}
	return "Black";
}

const char* font_cache_cx::slant_to_string(int s) {
	switch (s) {
	case FC_SLANT_ITALIC:  return "Italic";
	case FC_SLANT_OBLIQUE: return "Oblique";
	default:               return "Roman";
	}
}

void path_builder::clear()
{
	data.clear();
}
void path_builder::move_to(float x, float y) {
	data.push_back((float)path_type_et::e_vmove);  // MOVE
	data.push_back(x);
	data.push_back(-y); // ← y 翻转
	pen_x = x; pen_y = -y;
}
void path_builder::line_to(float x, float y) {
	data.push_back((float)path_type_et::e_vline);  // LINE
	data.push_back(x);
	data.push_back(-y); // ← y 翻转
	pen_x = x; pen_y = -y;
}
void path_builder::quad_to(float cx, float cy, float x, float y) {
	data.push_back((float)path_type_et::e_quadratic);  // QUAD
	data.push_back(cx);
	data.push_back(-cy); // ← y 翻转
	data.push_back(x);
	data.push_back(-y);  // ← y 翻转
	pen_x = x; pen_y = -y;
}
void path_builder::close() {
	data.push_back((float)path_type_et::e_close);  // CLOSE
	// close 不存坐标，ovg_path_t 内部补 终点→起点 直线
}
static void _hb_move_to(hb_draw_funcs_t* funcs, void* draw_data, hb_draw_state_t* st, float to_x, float to_y, void* user_data)
{
	auto* b = (path_builder*)draw_data;
	b->move_to(to_x, to_y);
}

static void _hb_line_to(hb_draw_funcs_t* funcs, void* draw_data, hb_draw_state_t* st, float to_x, float to_y, void* user_data)
{
	auto* b = (path_builder*)draw_data;
	b->line_to(to_x, to_y);
}

static void _hb_quad_to(hb_draw_funcs_t* funcs, void* draw_data, hb_draw_state_t* st, float cx, float cy, float to_x, float to_y, void* user_data)
{
	auto* b = (path_builder*)draw_data;
	b->quad_to(cx, cy, to_x, to_y);
}

static void _hb_close_path(hb_draw_funcs_t* funcs, void* draw_data, hb_draw_state_t* st, void* user_data)
{
	auto* b = (path_builder*)draw_data;
	b->close();
}

hb_raster_image_t* build_glyph_image_hb(vg_font* hp, uint32_t gid, int font_size, glm::ivec4* ot, const glm::vec2& scale);

glyph_atlas_entry* font_cache_cx::get_cache_lookup_glyph(hb_font_t* font, uint32_t glyph_id, int fontsize)
{
	glyph_atlas_entry* ret = 0;
	auto font_ptr = (FontStyle*)hb_font_get_user_data(font, (hb_user_data_key_t*)&fontStyleKey);
	uint32_t font_id = font_ptr->id;
	hb_font_set_scale(font, fontsize, fontsize);
	hb_glyph_extents_t extents;
	hb_font_get_glyph_extents(font, glyph_id, &extents);

	int em = hb_face_get_upem(hb_font_get_face(font));
	float scale = fontsize / (float)em;

	auto px_w = (extents.width);
	auto px_h = (-extents.height);

	bool use_raster = (px_w <= max_raster_size && px_h <= max_raster_size && px_w > 0 && px_h > 0);

	glyph_key gk;
	gk.s.k.x = (uint16_t)font_id;
	gk.s.glyph_index = glyph_id;

	{
		gk.s.k.y = 0;
		auto it = glyph_cache.find(gk.v);
		if (it == glyph_cache.end()) {
			if (!funcs)
			{
				funcs = hb_draw_funcs_create();
				hb_draw_funcs_set_move_to_func(funcs, _hb_move_to, nullptr, nullptr);
				hb_draw_funcs_set_line_to_func(funcs, _hb_line_to, nullptr, nullptr);
				hb_draw_funcs_set_quadratic_to_func(funcs, _hb_quad_to, nullptr, nullptr);
				hb_draw_funcs_set_close_path_func(funcs, _hb_close_path, nullptr, nullptr);
			}
			temp_path.clear();
			hb_font_set_scale(font, em, em);
			hb_font_draw_glyph(font, glyph_id, funcs, &temp_path);

			// 拷贝到持久内存
			size_t bytes = temp_path.data.size() * sizeof(float);
			float* path_copy = (float*)ac->alloc(bytes, 0);
			memcpy(path_copy, temp_path.data.data(), bytes);

			glyph_atlas_entry entry{};
			entry.type = glyph_atlas_entry::VECTOR;
			entry.atlas_img = nullptr;
			entry.uv_rect = glm::ivec4(0);
			entry.offset = glm::ivec2(0);
			entry.advance = 0;
			entry.path_data = path_copy;
			entry.path_size = temp_path.data.size();
			entry.em_units = em;
			auto [it2, ok] = glyph_cache.emplace(gk.v, entry);
			ret = &it2->second;
		}
		else {
			ret = &it->second;
		}
		if (!use_raster)return ret;
	}

	gk.s.k.y = (uint16_t)use_raster ? fontsize : 0;// 大号字符统一绑定0字号
	glm::ivec2 fsc = { 1,1 };
	if (fontsize <= min_subpixel && !font_ptr->font.pnt)
	{
		fsc.x = 3;
	}
	auto gca = fsc.x > 1 ? &_sub_glyph_cache : &glyph_cache;
	auto it = gca->find(gk.v);
	if (it != gca->end())
		return &it->second;
	assert(ret);
	// ═══════════════════════════════════════════
	// 位图路径
	// ═══════════════════════════════════════════
	if (use_raster) {

		// ── 缓存未命中：光栅化 ──
		glm::ivec4 rc = {};
		hb_raster_image_t* img = build_glyph_image_hb(&font_ptr->font, glyph_id, fontsize, &rc, fsc);
		// 装箱进 image_cache_cx
		glm::ivec2 pos = {};
		int ow = rc.z / fsc.x;
		ovg_image_data* img_data = image_cache.push_cache_size({ ow,rc.w }, &pos);
		gfont_copy_image(img_data, pos.x, pos.y, -1, (hb_raster_image_t*)img, rc, false, fsc.x > 1, SubpixelLayout::RGB);

		glyph_atlas_entry entry{};
		entry.atlas_img = img_data;  // ovg_image_data 继承自 vg_image_t
		entry.uv_rect = glm::ivec4(pos.x, pos.y, ow, rc.w);
		entry.offset = glm::ivec2(rc.x, rc.y);
		entry.advance = 0;
		entry.path_data = ret->path_data;
		entry.path_size = ret->path_size;
		entry.em_units = em;

		entry.has_color = font_ptr->font.pnt ? true : false;
		auto [it2, ok] = gca->emplace(gk.v, entry);
		ret = &it2->second;
	}

	return ret;
}
struct draw_ctx_f {
	ovg_ctx_cb* cb = 0;
	rvg_t* cr = 0;
};

void hb_ovg_move_to(hb_draw_funcs_t* dfuncs, void* draw_data, hb_draw_state_t* st, float to_x, float to_y, void* user_data)
{
	auto ctx = (draw_ctx_f*)draw_data;
	ctx->cb->move_to(ctx->cr, (double)to_x, (double)to_y);
}

void hb_ovg_line_to(hb_draw_funcs_t* dfuncs, void* draw_data, hb_draw_state_t* st, float to_x, float to_y, void* user_data)
{
	auto ctx = (draw_ctx_f*)draw_data;
	ctx->cb->line_to(ctx->cr, (double)to_x, (double)to_y);
}

void hb_ovg_quadratic_to(hb_draw_funcs_t*, void* data, hb_draw_state_t*, float cx1, float cy1, float to_x, float to_y, void*) {
	auto* c = static_cast<draw_ctx_f*>(data);
	c->cb->quadratic_to(c->cr, cx1, cy1, to_x, to_y);
}
void hb_ovg_cubic_to(hb_draw_funcs_t* dfuncs, void* draw_data, hb_draw_state_t* st, float control1_x, float control1_y, float control2_x, float control2_y, float to_x, float to_y, void* user_data)
{
	auto ctx = (draw_ctx_f*)draw_data;
	ctx->cb->curve_to(ctx->cr, (double)control1_x, (double)control1_y, (double)control2_x, (double)control2_y, (double)to_x, (double)to_y);
}

void hb_ovg_close_path(hb_draw_funcs_t* dfuncs, void* draw_data, hb_draw_state_t* st, void* user_data)
{
	auto ctx = (draw_ctx_f*)draw_data;
	ctx->cb->close_path(ctx->cr);
}
// 文本渲染

hb_draw_funcs_t* create_ovg_draw_funcs() {
	hb_draw_funcs_t* funcs = hb_draw_funcs_create();
	hb_draw_funcs_set_move_to_func(funcs, hb_ovg_move_to, nullptr, nullptr);
	hb_draw_funcs_set_line_to_func(funcs, hb_ovg_line_to, nullptr, nullptr);
	hb_draw_funcs_set_quadratic_to_func(funcs, hb_ovg_quadratic_to, nullptr, nullptr);
	hb_draw_funcs_set_cubic_to_func(funcs, hb_ovg_cubic_to, nullptr, nullptr);
	hb_draw_funcs_set_close_path_func(funcs, hb_ovg_close_path, nullptr, nullptr);
	return funcs;
}

const font_family_t* resolve_family(const font_familys_t* ffs, uint32_t cp)
{
	for (int i = 0; i < ffs->count; i++) {
		if (hb_set_has(ffs->familys[i]->coverage, cp))
			return ffs->familys[i];
	}
	return ffs->familys[0]; // fallback
}

static uint32_t utf8_next(const uint8_t*& p, const uint8_t* end) {
	if (p >= end) return 0;
	uint8_t c = *p++;
	uint32_t r = 0;
	if (c < 0x80) return c;
	if ((c & 0xE0) == 0xC0 && p + 1 <= end)
		r = ((c & 0x1F) << 6) | (*p++ & 0x3F);
	if ((c & 0xF0) == 0xE0 && p + 2 <= end)
		r = ((c & 0x0F) << 12) | ((p[0] & 0x3F) << 6) | (p[1] & 0x3F), p += 2;
	if ((c & 0xF8) == 0xF0 && p + 3 <= end)
		r = ((c & 0x07) << 18) | ((p[0] & 0x3F) << 12) | ((p[1] & 0x3F) << 6) | (p[2] & 0x3F), p += 3;
	return r ? r : 0xFFFD;
}

void vg_utf8_to_utf32(const void* str8, size_t len, std::vector<uint32_t>* ot)
{
	const uint8_t* p = (const uint8_t*)str8;
	const uint8_t* end = p + len;
	ot->reserve(len + ot->size());
	while (p < end)
		ot->push_back(utf8_next(p, end));
}

bool write_png_bgra(const char* path, const uint8_t* bgra, int w, int h) {
	std::vector<uint8_t> rgb(size_t(w) * h * 4);
	for (int y = 0; y < h; ++y) {
		const uint8_t* src = bgra + y * w * 4;
		uint8_t* dst = rgb.data() + y * w * 4;
		for (int x = 0; x < w; ++x) {
			dst[x * 4 + 0] = src[x * 4 + 2]; // R
			dst[x * 4 + 1] = src[x * 4 + 1]; // G
			dst[x * 4 + 2] = src[x * 4 + 0]; // B
			auto a = src[x * 4 + 3];
			dst[x * 4 + 3] = a; // A
		}
	}
	return stbi_write_png(path, w, h, 4, rgb.data(), w * 4) != 0;
}


void hb_res_init(vg_font* hp, hb_font_t* font) {
	if (!hp)return;
	if (font)
		hp->font = font;
	if (!hp->hb_language)
		hp->hb_language = hb_language_from_string("", -1);
	if (!hp->hb_buffer)
		hp->hb_buffer = hb_buffer_create();
	auto face = hb_font_get_face(hp->font);
	bool has_color = hb_ot_color_has_paint(face) || hb_ot_color_has_layers(face) || hb_ot_color_has_svg(face);
	auto bp = hb_ot_color_has_png(face);
	if (!hp->rdr)
		hp->rdr = hb_raster_draw_create_or_fail();
	if (!hp->pnt)
		hp->pnt = has_color ? hb_raster_paint_create_or_fail() : nullptr;

	hb_font_extents_t extents;
	hb_font_get_extents_for_direction(hp->font, HB_DIRECTION_LTR, &extents);
	hp->ascender = extents.ascender;
	hp->descender = extents.descender;
	hp->line_gap = extents.line_gap;
	hp->coverage = hb_set_create();
	hb_face_collect_unicodes(hb_font_get_face(hp->font), hp->coverage);
	hp->upem = hb_face_get_upem(face);
}
void free_hb_res(vg_font* hp) {
	if (hp->hb_buffer)
		hb_buffer_destroy(hp->hb_buffer);

	if (hp->pnt)
		hb_raster_paint_destroy(hp->pnt);
	if (hp->rdr)
		hb_raster_draw_destroy(hp->rdr);
	hb_set_destroy(hp->coverage);
	if (hp->font) {
		//hb_face_t* face = hb_font_get_face(hp->hb_font);
		//if (face)hb_face_destroy(face);
		hb_font_destroy(hp->font);
	}
	hp->font = 0;
	hp->coverage = 0;
	hp->hb_language = 0;
	hp->hb_buffer = 0;
	hp->rdr = 0;
	hp->pnt = 0;
}

hb_raster_image_t* build_glyph_image_hb(vg_font* hp, uint32_t gid, int font_size, glm::ivec4* ot, const glm::vec2& scale)
{
	hb_raster_image_t* img = nullptr;
	hb_glyph_extents_t gext = {};
	hb_glyph_extents_t gext0 = {};
	hb_font_extents_t extents[2] = {};
	auto rdr = hp->rdr;
	auto font = hp->font;
	auto pnt = hp->pnt;
	hb_font_set_scale(font, font_size, font_size);
	hb_font_get_glyph_extents(font, gid, &gext0);
	hb_raster_draw_reset(rdr);
	hb_bool_t bhe = hb_font_get_h_extents(hp->font, &extents[0]);
	hb_bool_t bve = hb_font_get_v_extents(hp->font, &extents[1]);
	float x, y;
	hb_raster_draw_get_scale_factor(rdr, &x, &y);
	int pad = std::max(2.0, font_size / 4.0);
	do {
		hb_font_set_scale(font, font_size * scale.x, font_size * scale.y);
		bool bext = hb_font_get_glyph_extents(font, gid, &gext);
		gext.x_bearing -= pad;
		gext.y_bearing += pad;       // 顶部扩大
		gext.width += 2 * pad;
		gext.height -= 2 * pad;      // height 为负，向下也扩大
		if (pnt)
		{
			hb_raster_paint_set_foreground(pnt, HB_COLOR(255, 255, 255, 255));
			if (bext && hb_raster_paint_set_glyph_extents(pnt, &gext))
			{
				hb_raster_paint_set_transform(pnt, 1.f, 0.f, 0.f, 1.f, 0.f, 0.f);
				hb_raster_paint_glyph(pnt, font, gid);	// 光栅化颜色字形
				img = hb_raster_paint_render(pnt);
			}
			if (img)
			{
				hb_raster_paint_recycle_image(pnt, img);
				break;
			}
		}
		{
			hb_raster_draw_set_glyph_extents(rdr, &gext);
			hb_raster_draw_set_transform(rdr, 1.f, 0.f, 0.f, 1.f, 0.0f, 0.0f);
			hb_raster_draw_glyph(rdr, font, gid);	// 单色
			img = hb_raster_draw_render(rdr);
			if (img) { hb_raster_draw_recycle_image(rdr, img); }
		}
	} while (0);
	hb_raster_extents_t ext = {};
	if (img) {
		hb_raster_image_get_extents(img, &ext);
		if (!ext.width || !ext.height) { img = 0; return img; }
		if (ot)
		{
			ot->x = ext.x_origin;
			ot->y = (ext.height + ext.y_origin);
			ot->z = ext.width;
			ot->w = ext.height;
		}
	}
	return img;
}
#if 1
// 像素写入函数类型
// src_ptr: 指向当前源像素的指针（字节）
// 返回: 要写入 dst 的 uint32_t 像素值
// 如果返回 false，表示跳过（不写入）
using PixelWriter = bool (*)(const uint8_t* src_ptr, uint32_t& out);

static void rgba_blit(
	ovg_image_data* dst,
	int dst_x, int dst_y,
	int src_x, int src_y,
	int src_w, int src_h,
	const uint8_t* src_data,
	int src_stride,        // 字节
	int src_bpp,           // 每像素字节数
	bool flip_y,
	PixelWriter write_pixel
)
{
	if (!dst || !src_data || src_w <= 0 || src_h <= 0 || src_stride < src_w * src_bpp)
		return;

	// 裁剪到 dst 边界
	int d_x = std::max(0, dst_x);
	int d_y = std::max(0, dst_y);
	int s_x = src_x;
	int s_y = src_y;

	int copy_w = std::min(src_w - s_x, dst->width - d_x);
	int copy_h = std::min(src_h - s_y, dst->height - d_y);

	if (copy_w <= 0 || copy_h <= 0)
		return;

	for (int iy = 0; iy < copy_h; ++iy) {
		int src_row_y = flip_y ? (src_h - 1 - (s_y + iy)) : (s_y + iy);
		const uint8_t* src_row = src_data + src_row_y * src_stride + s_x * src_bpp;
		uint32_t* dst_row = dst->data + (d_y + iy) * dst->width + d_x;

		for (int ix = 0; ix < copy_w; ++ix) {
			uint32_t pixel;
			if (write_pixel(src_row + ix * src_bpp, pixel)) {
				dst_row[ix] = pixel;
			}
		}
	}
}

// --- 灰度 → 指定颜色（gray 作 alpha）---
static bool write_gray_to_color(const uint8_t* src, uint32_t& out)
{
	uint32_t color = -1;
	uint8_t gray = src[0];
	if (gray == 0) return false;
	out = (color & 0x00FFFFFF) | (uint32_t(gray) << 24);
	return true;
}

// --- 灰度 → 预乘白色 ---
static bool write_gray_mul(const uint8_t* src, uint32_t& out)
{
	uint8_t gray = src[0];
	if (gray == 0) return false;
	out = (uint32_t(gray) << 24) | (uint32_t(gray) << 16) | (uint32_t(gray) << 8) | gray;
	return true;
}

// --- BGRA → 直接拷贝 ---
static bool write_bgra_raw(const uint8_t* src, uint32_t& out)
{
	out = *(const uint32_t*)src;
	return true;
}

// --- BGRA → RGBA（swap R/B）---
static bool write_bgra_to_rgba(const uint8_t* src, uint32_t& out)
{
	uint32_t pixel = *(const uint32_t*)src;
	uint8_t b = pixel & 0xFF;
	uint8_t g = (pixel >> 8) & 0xFF;
	uint8_t r = (pixel >> 16) & 0xFF;
	uint8_t a = (pixel >> 24) & 0xFF;
	out = (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | b;
	return true;
}

void rgba_copy2gray(ovg_image_data* dst, int x, int y, int w, int h,
	uint32_t c, const uint8_t* dt, int stride, bool fy)
{
	if (stride < 1) stride = w;

	rgba_blit(dst, x, y, 0, 0, w, h, dt, stride, 1, fy, write_gray_to_color);
}

void rgba_copy2gray_mul(ovg_image_data* dst, int x, int y, int w, int h,
	const uint8_t* dt, int stride, bool fy)
{
	if (stride < 1) stride = w;
	rgba_blit(dst, x, y, 0, 0, w, h, dt, stride, 1, fy, write_gray_mul);
}

void rgba_copy_bgra(ovg_image_data* dst, int x, int y, int w, int h,
	const uint32_t* dt, int stride, bool fy)
{
	if (stride < 1) stride = w * 4;
	rgba_blit(dst, x, y, 0, 0, w, h, (const uint8_t*)dt, stride, 4, fy, write_bgra_raw);
}

void rgba_copy_bgra2rgba(ovg_image_data* dst, int x, int y, int w, int h,
	const uint32_t* dt, int stride, bool fy)
{
	if (stride < 1) stride = w * 4;
	rgba_blit(dst, x, y, 0, 0, w, h, (const uint8_t*)dt, stride, 4, fy, write_bgra_to_rgba);
}
#endif // 1


struct A8Buf {
	int      w, h;
	uint8_t* rows; // 行主序，pitch = w
	std::vector<uint8_t> _data;
	A8Buf(int w_, int h_) : w(w_), h(h_) {
		_data.resize(w_ * h_); rows = _data.data();
		memset(rows, 0, w * h);
	}
	~A8Buf() { rows = 0; }
	uint8_t* row(int y) { return rows + y * w; }
	const uint8_t* row(int y) const { return rows + y * w; }
};
struct A8Glyph {
	uint8_t* coverage = nullptr;   // 长度 = width * height
	int      width = 0;         // 像素宽度（已含 3x 水平放大）
	int      height = 0;
	int      dx = 0;         // 相对字形原点的水平偏移（像素）
	int      dy = 0;         // 相对字形原点的垂直偏移（像素）
	std::vector<uint8_t> _data;

	A8Glyph() {}
	A8Glyph(int w, int h) {
		if (w > 0 && h > 0) {
			width = w; height = h; _data.resize(w * h);
			coverage = _data.data();
			memset(coverage, 0, w * h);
		}
	}
	~A8Glyph() { coverage = 0; }

};
/* ==================== 子像素排列（subpixel layout）====================
* LCD 亚像素渲染必须知道显示器的子像素物理排列，否则彩色边缘会反向/偏色。
*   RGB  / BGR   ：水平排列，可做三通道着色（最常见）
*   VRGB / VBGR  ：垂直排列，水平方向无额外分辨率 → 退化为灰阶 AA
*   NONE         ：非标准/未知排列（PenTile、旋转屏、高 DPI）→ 强制灰阶
*
* 取值应与操作系统报告一致：
*   Windows : GetDeviceCaps(hdc, COLORMGM) / DWM
*   X11     : xrandr --prop  →  "Subpixel: rgb"
*   Wayland : wl_output.subpixel
*   macOS   : 系统内部决定，不暴露，通常按 RGB 处理或默认灰阶 */

/* ==================== 2. 5-tap FIR 低通（FT_LcdFilter 经典系数）====================
	对每个颜色分量用相邻 3 个「通道」平滑，抑制 LCD 色边条纹。
	系数 {0x08, 0x4D, 0x56, 0x4D, 0x08} 与 FreeType 默认一致。
	水平 5-tap，作用于「3x 宽」的 A8 缓冲（每像素 3 个采样点）。 */
void fir_horizontal(A8Buf& src, A8Buf& dst) {
	static const uint8_t FIR[5] = { 0x08, 0x4D, 0x56, 0x4D, 0x08 };
	static const uint16_t FIR_SUM = 0x08 + 0x4D + 0x56 + 0x4D + 0x08; // = 0x140
	for (int y = 0; y < src.h; ++y) {
		uint8_t* s = src.row(y);
		uint8_t* d = dst.row(y);
		for (int x = 0; x < src.w; ++x) {
			int v = 0;
			for (int k = -2; k <= 2; ++k) {
				int xi = x + k;
				if (xi < 0) xi = 0;
				if (xi >= src.w) xi = src.w - 1;
				v += FIR[k + 2] * s[xi];
			}
			d[x] = uint8_t(v / FIR_SUM); // 归一化
		}
	}
}
inline void blend_lcd_bgra_premul(
	uint8_t* dst,        // 目标 BGRA（dst RGB 也必须是预乘格式）
	const uint8_t* src)  // a8_to_lcd 输出的 BGRA（预乘）
{
	uint8_t src_a = src[3];
	if (src_a == 0) return;

	if (src_a == 255) {
		*(uint32_t*)dst = *(const uint32_t*)src;
		return;
	}
	uint8_t dst_a = dst[3];
	uint8_t ia = 255 - src_a;
	dst[0] = (src[0] + (uint16_t)dst[0] * ia / 255);
	dst[1] = (src[1] + (uint16_t)dst[1] * ia / 255);
	dst[2] = (src[2] + (uint16_t)dst[2] * ia / 255);
	dst[3] = src_a + (uint16_t)dst_a * ia / 255;
}
void a8_to_lcd(const A8Buf& filtered, int height, uint8_t* rgba_out, int out_w, int out_stride, SubpixelLayout layout, uint32_t c) {
	glm::vec4 c4 = *(glm::u8vec4*)&c;
	c4 *= c4.w / 255.0;
	glm::u8vec4 color = c4;
	/* gamma 校正：覆盖度是线性面积，需转 sRGB 观感 */
	auto gamma = [](int v) -> uint8_t {
		double n = v / 255.0;
		n = std::pow(n, 1.0 / 1.8); // 1.8 接近典型 LCD gamma
		return uint8_t(glm::clamp(n * 255.0, 0.0, 255.0));
		};

	/* 是否为水平排列（可做真正的三通道 LCD 着色） */
	const bool horizontal = (layout == SubpixelLayout::RGB || layout == SubpixelLayout::BGR);
	int ow = std::min(filtered.w / 3, out_w);
	for (int y = 0; y < height; ++y) {
		const uint8_t* a = filtered.row(y);
		uint8_t* out = rgba_out + y * out_stride;
		for (int px = 0; px < ow; ++px) {
			int c0 = a[px * 3 + 0]; // 左通道
			int c1 = a[px * 3 + 1]; // 中通道
			int c2 = a[px * 3 + 2]; // 右通道

			uint8_t r, g, b;
			uint8_t gray = gamma((c0 + c1 + c2) / 3);
			if (horizontal) {
				/* 水平排列：三个采样点 → R/G/B，BGR 时翻转。
				 * 各通道独立乘颜色 → 产生 LCD 彩色边缘。 */
				r = gamma(c0); g = gamma(c1); b = gamma(c2);
				if (layout == SubpixelLayout::BGR) std::swap(r, b);
				r = uint8_t((r * color.x) >> 8);
				g = uint8_t((g * color.y) >> 8);
				b = uint8_t((b * color.z) >> 8);
			}
			else {
				/* 垂直 / 未知排列：退化为灰阶 AA（不产生彩色 fringe）。
				 * 用三个采样的平均覆盖度，乘上「颜色的亮度」——
				 * 关键是三通道用同一个系数，保证 r==g==b 仍是灰阶。
				 * 亮度系数采用 BT.601 (0.299R+0.587G+0.114B)，接近人眼感知。 */
				uint8_t lum = uint8_t((color.x * 77 + color.y * 150 + color.z * 29) >> 8);
				r = g = b = gray;// uint8_t((gray * lum) >> 8);
				r = uint8_t((r * color.x) >> 8);
				g = uint8_t((g * color.y) >> 8);
				b = uint8_t((b * color.z) >> 8);
			}

			{
				uint32_t cc = 0;
				auto dc = (uint8_t*)&cc;
				dc[0] = b;
				dc[1] = g;
				dc[2] = r;
				dc[3] = gray;
				*(uint32_t*)(out + px * 4) = cc;
			}
		}
	}
}

void rgba_copy2gray(A8Glyph* dst, const glm::ivec2& pos, const glm::ivec4& src, const uint8_t* dt, int stride, bool fy)
{
	if (stride < 1)stride = src.z;
	if (!dst || !dt || src.z <= 0 || src.w <= 0 || stride < src.z) return;
	// 计算实际绘制区域（处理边界裁剪）
	int dst_x = std::max(0, pos.x);
	int dst_y = std::max(0, pos.y);
	int src_x = src.x;
	int src_y = src.y;
	int copy_w = std::min(src.z - src_x, dst->width - dst_x);
	int copy_h = std::min(src.w - src_y, dst->height - dst_y);
	for (int iy = 0; iy < copy_h; ++iy) {
		const uint8_t* src_row = fy ? dt + (src.w - 1 - (src_y + iy)) * stride : dt + (src_y + iy) * stride + src_x;
		auto dst_row = dst->coverage + ((dst_y + iy) * dst->width + dst_x);
		for (int ix = 0; ix < copy_w; ++ix) {
			uint8_t gray = src_row[ix];
			if (gray > 0)
			{
				dst_row[ix] = gray;
			}
		}
	}
}

void subpixel_lcd(ovg_image_data* dst, int px, int py, hb_raster_image_t* img_src, const glm::ivec2& size, SubpixelLayout pixelLayout, bool flipY)
{
	hb_raster_extents_t ext = {};
	hb_raster_image_get_extents(img_src, &ext);
	glm::ivec2 ws = { ovg::align_up(ext.width,2), ovg::align_up(ext.height,2) };

	A8Buf src(ws.x, ws.y);
	A8Buf filtered(ws.x, ws.y);
	auto coverage = hb_raster_image_get_buffer(img_src);
#if 1
	const uint8_t* src_row = coverage + (flipY ? (ext.height - 1) * ext.stride : 0);
	int row_step = flipY ? -ext.stride : ext.stride;
	for (size_t i = 0; i < ext.height; i++)
	{
		std::memcpy(src.rows + src.w * i, src_row, ext.width);
		src_row += row_step;
	}
#else
	A8Glyph g(ws.x, ws.y);
	rgba_copy2gray(&g, { 0, 0 }, { 0,0,ext.width, ext.height }, coverage, ext.stride, true);
	std::memcpy(src.rows, g.coverage, g.width * g.height);
#endif
	uint32_t c = -1;
	/* ② 5-tap FIR 低通 */
	fir_horizontal(src, filtered);
	/* ③ A8(3x) → LCD 三通道 + gamma */
	if (py < 0)
		py = 0;
	if (px < 0)px = 0;
	a8_to_lcd(filtered, ext.height, (uint8_t*)(dst->data + px + py * dst->width), size.x, dst->stride, pixelLayout, c);
}

bool gfont_copy_image(ovg_image_data* dst, int rx, int ry, uint32_t color, hb_raster_image_t* img_src, const glm::ivec4& orc, bool origin, bool type, SubpixelLayout pixelLayout)
{
	auto img = img_src;
	bool has_color = false;
	hb_raster_extents_t ext = {};
	hb_raster_image_get_extents(img_src, &ext);
	hb_raster_format_t fmt = hb_raster_image_get_format(img_src);
	dst->multiply = 1;
	if (origin)
	{
		rx += orc.x;
		ry += orc.y;
	}
	if (fmt == HB_RASTER_FORMAT_A8)
	{
		if (type)
			subpixel_lcd(dst, rx, ry, img, { orc.z,orc.w }, pixelLayout, true);
		else
			rgba_copy2gray(dst, rx, ry, ext.width, ext.height, color, hb_raster_image_get_buffer(img_src), ext.stride, true);
	}
	else if (fmt == HB_RASTER_FORMAT_BGRA32)
	{
		has_color = true;
		if (dst->format == 1)
			rgba_copy_bgra(dst, rx, ry, ext.width, ext.height, (uint32_t*)hb_raster_image_get_buffer(img_src), ext.stride, true);
		else
			rgba_copy_bgra2rgba(dst, rx, ry, ext.width, ext.height, (uint32_t*)hb_raster_image_get_buffer(img_src), ext.stride, true);
	}
	return has_color;
}
// todo packer


packer_base::packer_base()
{}
packer_base::~packer_base()
{}

void packer_base::init_target(int width, int height, int heuristic) {}
void packer_base::clear() {}
size_t packer_base::push_rect(glm::ivec4* rc, int n, size_t stride) { return 0; }
bool packer_base::push_rect(const glm::ivec2& rc, glm::ivec2* pos) { return false; }

class image_packer :public packer_base
{
public:
	stbrp_context _ctx = {};
	std::vector<stbrp_node> _rpns;
public:
	image_packer() {}
	~image_packer() {}
	// BL = 0 “从下向左塞”（快速降低高度）
	// BF = 1 “精打细算”（最小化空间浪费）
	void init_target(int width, int height, int heuristic) {
		assert(!(width < 10 || height < 10));
		if (width < 10 || height < 10)return;
		this->width = width;
		this->height = height;
		_rpns.resize(width);
		memset(_rpns.data(), 0, _rpns.size() * sizeof(stbrp_node));
		stbrp_init_target(&_ctx, width, height, _rpns.data(), _rpns.size());
		stbrp_setup_heuristic(&_ctx, heuristic);
		stbrp_setup_allow_out_of_mem(&_ctx, 0);
	}
	void clear() {
		init_target(_ctx.width, _ctx.height, 0);
	}
	size_t push_rect(glm::ivec4* rc, int n, size_t stride)
	{
		if (!rc || n < 1)return false;
		std::vector<stbrp_rect> rct(n);
		auto r = rc;
		auto t = (char*)rc;
		if (stride < sizeof(glm::ivec4))
			stride = sizeof(glm::ivec4);
		for (auto& it : rct)
		{
			r = (glm::ivec4*)t;
			it.w = r->z; it.h = r->w;
			t += stride;
		}
		t = (char*)rc;
		int ret = stbrp_pack_rects(&_ctx, rct.data(), n);
		size_t cx = 0;
		for (auto& it : rct)
		{
			r = (glm::ivec4*)t;
			r->x = it.x; r->y = it.y;
			if (!it.was_packed)
			{
				cx++;
			}
			t += stride;
		}
		return cx;
	}
	bool push_rect(const glm::ivec2& rc, glm::ivec2* pos)
	{
		stbrp_rect rct[2] = {};
		rct->w = rc.x;
		rct->h = rc.y;
		int ret = stbrp_pack_rects(&_ctx, rct, 1);
		if (pos)
		{
			*pos = { rct->x,rct->y };
		}
		return ret;
	}
public:
	// todo stb结构
	int pack_rects(stbrp_rect* rects, int num_rects)
	{
		return stbrp_pack_rects(&_ctx, rects, num_rects);
	}
	void setup_allow_out_of_mem(int allow_out_of_mem)
	{
		stbrp_setup_allow_out_of_mem(&_ctx, allow_out_of_mem);
	}
	//可以选择库应该使用哪个打包启发式方法。不同启发式方法将为不同的数据集生成更好/更差的结果。 如果再次调用init，将重置为默认值。	
	void setup_heuristic(int heuristic = 1)
	{
		stbrp_setup_heuristic(&_ctx, heuristic);
	}
private:

};
packer_base* new_packer(int width, int height)
{
	auto p = new image_packer();
	if (p)
	{
		p->init_target(width, height, 0);
	}
	return p;
}

void free_packer(packer_base* p)
{
	if (p)
	{
		delete p;
	}
}

class stb_packer :public packer_base
{
public:
	stbrp_context _ctx = {};
	ovg_image_data img = {};
	std::vector<uint32_t> ptr;
	std::vector<stbrp_node> _rpns;
public:
	stb_packer() {}
	~stb_packer() {}
	ovg_image_data* get() {
		return &img;
	}
	// BL = 0 “从下向左塞”（快速降低高度）
	// BF = 1 “精打细算”（最小化空间浪费）
	void init_target(int width, int height, int heuristic) {
		assert(!(width < 10 || height < 10));
		if (width < 10 || height < 10)return;
		ptr.resize(width * height);
		auto img = get();
		img->format = 1;
		img->width = width;
		img->height = height;
		img->valid = 1;
		img->multiply = true;
		img->stride = width * sizeof(uint32_t);
		img->data = ptr.data();
		_rpns.resize(width);
		memset(_rpns.data(), 0, _rpns.size() * sizeof(stbrp_node));
		stbrp_init_target(&_ctx, width, height, _rpns.data(), _rpns.size());
		stbrp_setup_heuristic(&_ctx, heuristic);
		stbrp_setup_allow_out_of_mem(&_ctx, 0);
	}
	void clear() {
		init_target(_ctx.width, _ctx.height, 0);
	}
	size_t push_rect(glm::ivec4* rc, int n, size_t stride)
	{
		if (!rc || n < 1)return false;
		std::vector<stbrp_rect> rct(n);
		auto r = rc;
		auto t = (char*)rc;
		if (stride < sizeof(glm::ivec4))
			stride = sizeof(glm::ivec4);
		for (auto& it : rct)
		{
			r = (glm::ivec4*)t;
			it.w = r->z; it.h = r->w;
			t += stride;
		}
		int ret = stbrp_pack_rects(&_ctx, rct.data(), n);
		for (auto& it : rct)
		{
			r = (glm::ivec4*)t;
			r->x = it.x; r->y = it.y;
			t += stride;
		}
		auto img = get();
		img->valid = 1;
		return ret;
	}
	bool push_rect(const glm::ivec2& rc, glm::ivec2* pos)
	{
		stbrp_rect rct[2] = {};
		rct->w = rc.x;
		rct->h = rc.y;
		int ret = stbrp_pack_rects(&_ctx, rct, 1);
		if (pos)
		{
			*pos = { rct->x,rct->y };
		}
		auto img = get();
		img->valid = 1;
		return ret;
	}
public:
	int pack_rects(stbrp_rect* rects, int num_rects)
	{
		return stbrp_pack_rects(&_ctx, rects, num_rects);
	}
	void setup_allow_out_of_mem(int allow_out_of_mem)
	{
		stbrp_setup_allow_out_of_mem(&_ctx, allow_out_of_mem);
	}
	//可以选择库应该使用哪个打包启发式方法。不同启发式方法将为不同的数据集生成更好/更差的结果。 如果再次调用init，将重置为默认值。	
	void setup_heuristic(int heuristic = 1)
	{
		stbrp_setup_heuristic(&_ctx, heuristic);
	}
private:

};



image_cache_cx::image_cache_cx()
{
	resize(width, height);
}

image_cache_cx::~image_cache_cx()
{
	clear();
}

void image_cache_cx::resize(int w, int h)
{
	if (w < 10 || h < 10 || (w == width && h == height))return;
	width = w;
	height = h;
	clear();
}

glm::ivec2 image_cache_cx::fill_color(int w, int h, uint32_t color)
{
	glm::ivec2 pos = {};
	auto pt = get_last_packer(false);
	if (!pt || w < 1 || h < 1)return pos;
	auto ret = pt->push_rect({ w, h }, &pos);
	auto ptr = pt->get();
	auto px = ((uint32_t*)ptr->data) + pos.x;
	px += pos.y * width;
	for (size_t i = 0; i < h; i++)
	{
		for (size_t x = 0; x < w; x++)
		{
			px[x] = color;
		}
		px += width;
	}
	return pos;
}

ovg_image_data* image_cache_cx::push_cache_size(const glm::ivec2& ss, glm::ivec2* pos, int linegap)
{
	int width = ovg::align_up(ss.x + linegap, 2), height = ovg::align_up(ss.y + linegap, 2);
	glm::ivec4 rc4 = { 0, 0, ss.x,ss.y };
	auto pt = get_last_packer(false);
	if (!pt)return 0;
	auto ret = pt->push_rect({ width, height }, pos);
	if (!ret)
	{
		pt = get_last_packer(true);
		ret = pt->push_rect({ width, height }, pos);
	}
	if (!ret)
	{
		return 0;
	}
	return pt->get();
}

ovg_image_data* image_cache_cx::push_cache_bitmap(hb_raster_image_t* img, glm::ivec2* pos, int linegap)
{
	hb_raster_extents_t ext = {};
	hb_raster_image_get_extents(img, &ext);
	int width = ovg::align_up(ext.width + linegap, 2), height = ovg::align_up(ext.height + linegap, 2);
	glm::ivec4 rc4 = { 0, 0, ext.width, ext.height };
	auto pt = get_last_packer(false);
	if (!pt)return 0;
	auto ret = pt->push_rect({ width, height }, pos);
	if (!ret)
	{
		pt = get_last_packer(true);
		ret = pt->push_rect({ width, height }, pos);
	}
	if (ret)
	{
		rc4.x = pos->x;
		rc4.y = pos->y;

		auto dst = pt->get();

		hb_raster_format_t fmt = hb_raster_image_get_format(img);
		int rx = pos->x; int ry = pos->y;
		bool origin = false;
		if (origin)
		{
			rx += ext.x_origin;
			ry += -(ext.height + ext.y_origin);
		}
		auto imgd = hb_raster_image_get_buffer(img);
		if (fmt == HB_RASTER_FORMAT_A8)
		{
			rgba_copy2gray(dst, rx, ry, ext.width, ext.height, -1, imgd, ext.stride, true);
		}
		else if (fmt == HB_RASTER_FORMAT_BGRA32)
		{
			if (dst->format == 1)
				rgba_copy_bgra(dst, rx, ry, ext.width, ext.height, (uint32_t*)imgd, ext.stride, true);
			else
				rgba_copy_bgra2rgba(dst, rx, ry, ext.width, ext.height, (uint32_t*)imgd, ext.stride, true);
		}

		dst->valid = 1; // 更新缓存标志
	}
	return pt->get();
}


void image_cache_cx::clear()
{
	for (auto it : _packer)
	{
		if (it)delete it;
	}
	_packer.clear();
	_data.clear();
}

stb_packer* image_cache_cx::get_last_packer(bool isnew)
{
	if (_packer.empty() || isnew)
	{
		auto p = new stb_packer();
		if (!p)return 0;
		_packer.push_back(p);
		p->init_target(width, height, 0);
		_data.push_back(p->get());
	}
	return *_packer.rbegin();
}

void text_draw_list::clear() { cmds.clear(); }
void text_draw_list::push_raster(glyph_atlas_entry* e, float x, float y, float w, float h, const glm::vec4& uv, uint32_t c) {
	cmds.push_back({ glyph_draw_cmd::RASTER, e, {x,y}, {w,h}, uv, c ,fontsize });
}
void text_draw_list::push_vector(glyph_atlas_entry* e, float x, float y, uint32_t c) {
	cmds.push_back({ glyph_draw_cmd::VECTOR, e, {x,y}, {}, {}, c ,fontsize });
}
vg_text_run_cx::vg_text_run_cx() {}

vg_text_run_cx::~vg_text_run_cx() {
	free_buffer();
	for (auto& r : _runs) {
		if (r.buf) hb_buffer_destroy(r.buf);
	}
}

void vg_text_run_cx::set_min_subpixel(int sp)
{
	min_subpixel = sp;
}

void vg_text_run_cx::free_buffer() {
	if (_buf) {
		hb_buffer_destroy(_buf);
		_buf = nullptr;
	}
}

void vg_text_run_cx::set_font(hb_font_t* font, int fontsize) {
	_primary_font = font;
	_fontsize = fontsize;

	// 从 font user_data 取 cache 指针
	if (font) {
		_cache = (font_cache_cx*)hb_font_get_user_data(font, &g_font_cache_key);
	}
	else {
		_cache = nullptr;
	}
}

void vg_text_run_cx::set_font_families(const font_familys_t* ffs, int fontsize) {
	_ffs = ffs;
	_fontsize = fontsize;
	if (ffs && ffs->count > 0 && ffs->familys[0]) {
		set_font(ffs->familys[0]->font, _fontsize);
	}
}

void vg_text_run_cx::set_text(const void* str8, size_t len) {
	_utf32.clear();
	if (!str8 || len == 0) return;
	if (len == (size_t)-1) len = strlen((const char*)str8);
	vg_utf8_to_utf32(str8, len, &_utf32);
}


void vg_text_run_cx::clear_glyphs() {
	_glyphs.clear();
	_glyph_count = 0;
	_extents = {};
	for (auto& r : _runs) {
		if (r.buf) { hb_buffer_destroy(r.buf); r.buf = nullptr; }
	}
	_runs.clear();
}

void vg_text_run_cx::shape() {
	clear_glyphs();
	if (!_primary_font || _utf32.empty()) return;

	// ── 单字体简单路径（无 fallback）──
	if (!_ffs || _ffs->count == 0) {
		shape_run(0, _utf32.size(), _primary_font, _fontsize);
		return;
	}
	// ── 多字体 fallback：按 script/coverage 切 run ──
	size_t run_start = 0;
	while (run_start < _utf32.size()) {
		uint32_t cp = _utf32[run_start];
		const font_family_t* ff = resolve_family(_ffs, cp);
		if (!ff) ff = _ffs->familys[0];
		size_t run_end = run_start + 1;
		while (run_end < _utf32.size() && resolve_family(_ffs, _utf32[run_end]) == ff)
			run_end++;
		int scale = _fontsize > 0 ? _fontsize : ff->upem;
		shape_run(run_start, run_end, ff->font, scale);
		run_start = run_end;
	}
}

void vg_text_run_cx::shape_run(size_t run_start, size_t run_end, hb_font_t* font, int fontsize)
{
	if (!font || run_start >= run_end) return;
	if (_cache) {
		_cache->min_subpixel = min_subpixel;
	}
	hb_font_set_scale(font, fontsize, fontsize);
	hb_buffer_t* buf = hb_buffer_create();
	hb_buffer_add_utf32(buf, _utf32.data() + run_start, run_end - run_start, 0, -1);
	hb_buffer_guess_segment_properties(buf);
	// 关闭 kerning（UI 场景常见需求）
	hb_feature_t features[] = { {HB_TAG('k','e','r','n'), 0, 0, ~0u} };
	hb_shape(font, buf, features, 1);
	unsigned int glyph_count = 0;
	hb_glyph_info_t* info = hb_buffer_get_glyph_infos(buf, &glyph_count);
	hb_glyph_position_t* pos = hb_buffer_get_glyph_positions(buf, nullptr);
	// 记录 run
	_runs.push_back({ buf, font, fontsize, (uint32_t)run_start, (uint32_t)run_end });
	// 字体度量
	hb_font_extents_t fextents = {};
	hb_font_get_extents_for_direction(font, HB_DIRECTION_LTR, &fextents);
	// 填充 glyph 列表
	float x = 0;
	for (unsigned int i = 0; i < glyph_count; ++i) {
		vg_glyph_info_t g;
		g.glyph_id = info[i].codepoint;
		g.x_offset = (float)pos[i].x_offset;
		g.y_offset = (float)pos[i].y_offset;
		g.x_advance = (float)pos[i].x_advance;
		g.y_advance = (float)pos[i].y_advance;
		// 查缓存
		if (_cache) {
			g.cache_entry = _cache->get_cache_lookup_glyph(font, info[i].codepoint, fontsize);
		}
		else {
			g.cache_entry = nullptr;
		}
		_glyphs.push_back(g);
		// 累加 extents
		_extents.x_advance += g.x_advance;
		_extents.y_advance += g.y_advance;
	}
	_glyph_count = (uint32_t)_glyphs.size();
	// 行高
	_extents.height = (float)(fextents.ascender - fextents.descender + fextents.line_gap);
	// bearing（第一个 glyph）
	if (!_glyphs.empty()) {
		_extents.x_bearing = -_glyphs[0].x_offset;
		_extents.y_bearing = -_glyphs[0].y_offset;
	}
	// width = x_advance 总和
	_extents.width = _extents.x_advance;
}

void vg_text_run_cx::populate_draw_list(text_draw_list& list, float origin_x, float origin_y, uint32_t color, render_mode mode)
{
	float pen_x = origin_x;
	float pen_y = origin_y;
	list.fontsize = _fontsize;
	for (const auto& g : _glyphs) {
		if (!g.cache_entry) { pen_x += g.x_advance; continue; }

		auto* e = g.cache_entry;
		float x = pen_x + g.x_offset;
		float y = pen_y + g.y_offset;
		if (mode == VECTOR_ONLY || !e->atlas_img) {
			if (e->path_data) {
				list.push_vector(e, x, y, color);
			}
		}
		else if (e->atlas_img) {
			float w = (float)e->uv_rect.z;
			float h = (float)e->uv_rect.w;
			float atlas_w = (float)e->atlas_img->width;
			float atlas_h = (float)e->atlas_img->height;
			glm::vec4 uv(
				(float)e->uv_rect.x / atlas_w,
				(float)e->uv_rect.y / atlas_h,
				(float)(e->uv_rect.x + e->uv_rect.z) / atlas_w,
				(float)(e->uv_rect.y + e->uv_rect.w) / atlas_h
			);
			list.push_raster(e, x, y, w, h, uv, color);
		}
		pen_x += g.x_advance;
	}
	list.extents = _extents;
}
