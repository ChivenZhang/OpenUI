/*
矢量渲染

2026/8/31 支持普通三角形渲染
2026/8/13 版本1.0
2026/8/8 创建文件

*/

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

#if (__has_include("ovg.h"))
#include "ovg.h"
#else
#include "../ovg.h"
#endif


#include <array>
#include <map>
#include <unordered_map>
#include <vector>
#include <stack>
#include <memory_resource> 
#if (__has_include(<tesselator.h>))
#include <tesselator.h>
#define VG_FILL_NZ_GLUTESS2
#endif

#include <harfbuzz/hb.h> 
#include <fontconfig/fontconfig.h>
#include "ovg_fonts.h"

void init_ovg_cb(ovg_canvas_cb* cb);
void init_ovg_ctx_cb(ovg_ctx_cb* cb);

#ifndef MEMAC_PMR
template<class _Ty>
using pmalloc_t = std::pmr::polymorphic_allocator<_Ty>;		// 指定类型内存分配
using uspool_t = std::pmr::unsynchronized_pool_resource;	// 线程不安全
using mbpool_t = std::pmr::monotonic_buffer_resource;		// 线程不安全，多次分配，统一释放
using spool_t = std::pmr::synchronized_pool_resource;		// 线程安全的


vg_alloc_cx::vg_alloc_cx()
{}

vg_alloc_cx::~vg_alloc_cx()
{}
void* vg_alloc_cx::alloc(const size_t _Bytes, const size_t align)
{
	return nullptr;
}
void vg_alloc_cx::dealloc(void* t, size_t n)
{}
class usp_ac_cx :public vg_alloc_cx
{
public:
	size_t _Align = 16;
	void* ptr = 0;
	uspool_t _alloc = {};				// pmr内存分配
public:
	usp_ac_cx() {}
	~usp_ac_cx() {}
public:
	void* alloc(const size_t _Bytes, const size_t align = 0) {
		return  _alloc.allocate(_Bytes, align > 0 ? align : _Align);
	}
	void* allocate(const size_t _Bytes, const size_t align = 0) {
		return  _alloc.allocate(_Bytes, align > 0 ? align : _Align);
	}
	void* new_mem(size_t n)
	{
		n = std::max((size_t)1, n);
		auto p = _alloc.allocate(n, _Align);
		memset(p, 0, n);
		return p;
	}
	void* new_mem0(size_t n)
	{
		n = std::max((size_t)1, n);
		auto p = _alloc.allocate(n, _Align);
		return p;
	}
	template<class T>
	T* new_mem(size_t n)
	{
		n = std::max((size_t)1, n);
		auto p = (T*)_alloc.allocate(sizeof(T) * n, _Align);
		auto ptr = p;
		for (int i = 0; i < n; i++)
		{
			p[i] = {};
		}
		return p;
	}
	template<class T >
	T* new_mem(size_t n, T*& p)
	{
		n = std::max((size_t)1, n);
		p = (T*)_alloc.allocate(sizeof(T) * n, _Align);
		auto ptr = p;
		for (int i = 0; i < n; i++)
		{
			p[i] = {};
		}
		return p;
	}
	template<class T >
	T* new_mem_o(size_t n)
	{
		n = std::max((size_t)1, n);
		auto p = (T*)_alloc.allocate(sizeof(T) * n, _Align);
		return p;
	}
	template<class T>
	T* new_mem(T*& p, size_t n)
	{
		return new_mem(n, p);
	}
	template<class T>
	void free_mem(T* t, size_t n)
	{
		auto ptr = t;
		if (t && n > 0)
		{
			_alloc.deallocate(t, sizeof(T) * n, _Align);
		}
	}
	void free_mem0(void* t, size_t n)
	{
		auto ptr = t;
		if (t && n > 0)
		{
			_alloc.deallocate(t, n, _Align);
		}
	}
	void dealloc(void* t, size_t n)
	{
		auto ptr = t;
		if (t && n > 0)
		{
			_alloc.deallocate(t, n, _Align);
		}
	}
	template<class T, class... Ts>
	T* new_obj(Ts &&... args)
	{
		auto p = (T*)new_mem(sizeof(T));
		if (p)
		{
#ifdef _WIN32
			std::uninitialized_construct_using_allocator(p, _alloc, std::forward<Ts>(args)...);
#else
			std::__uninitialized_construct_using_allocator(p, _alloc, std::forward<Ts>(args)...);
#endif
		}
		return p;
	}
	template<class T>
	void free_obj(T* t)
	{
		auto ptr = t;
		if (t)
		{
			std::destroy_at(ptr);
			_alloc.deallocate(t, sizeof(T), _Align);
		}
	}
};

#endif // !MEMAC_PMR

class ovg_canvas_cx :public ovg_canvas_cb
{
public:
	font_cache_cx* font_ctx = 0;
public:
	ovg_canvas_cx();
	~ovg_canvas_cx();

	void add_text(rvg_t* rvg, text_st_t* p, text_style_t* ts, text_box_rt* box);
private:

};

ovg_canvas_cx::ovg_canvas_cx()
{
	auto a = new usp_ac_cx();
	ac = (mem_resource_t*)a;
	ac->ptr = this;
	init_ovg_cb(this);
}

ovg_canvas_cx::~ovg_canvas_cx()
{
	free_font_cache(font_ctx);
	font_ctx = 0;
	if (ac) {
		auto ac1 = (usp_ac_cx*)ac;
		if (ac1)
			delete ac1;
	}
	ac = 0;
}

ovg_canvas_cb* new_canvas_cb(font_cache_cx* fctx)
{
	auto p = new ovg_canvas_cx();
	if (p) {
		if (fctx)fctx->references++;
		p->font_ctx = fctx;
	}
	return p;
}
void free_canvas_cb(ovg_canvas_cb* p) {
	if (p) {
		if (p->ac)delete p->ac;
		delete p;
	}
}

class font_cache_cx;

class ovg_ctx_cx :public ovg_ctx_cb
{
public:
	font_cache_cx* font_ctx = 0;
public:
	ovg_ctx_cx();
	~ovg_ctx_cx();

	void add_text(rvg_t* rvg, text_st_t* p, text_style_t* ts, text_box_rt* box);
private:

};

ovg_ctx_cx::ovg_ctx_cx()
{
	auto ac0 = new usp_ac_cx();
	ac = (mem_resource_t*)ac0;
	init_ovg_ctx_cb(this);
	ac0->ptr = this;
}

ovg_ctx_cx::~ovg_ctx_cx()
{
	auto ac1 = (usp_ac_cx*)ac;
	if (ac1)
		delete ac1;
	free_font_cache(font_ctx);
	font_ctx = 0;
	ac = 0;
}
ovg_ctx_cb* new_ctx_cb(font_cache_cx* fctx)
{
	auto p = new ovg_ctx_cx();
	if (p)
	{
		if (fctx)fctx->references++;
		p->font_ctx = fctx;
	}
	return p;
}
void free_ctx_cb(ovg_ctx_cb* p) {
	auto p1 = (ovg_ctx_cx*)p;
	if (p) {
		delete p1;
	}
}
// vg
#if 1
struct ovg_path_t {
	usp_ac_cx* ac = 0;
	std::pmr::vector<glm::vec2> points;	// 点数组
	std::pmr::vector<uint32_t> pathes;	// 每段大小
	std::pmr::vector<uint32_t> colors;	// 颜色数组，和pathes大小一样
	uint32_t color = 0xffffffff;		// 默认颜色
	uint32_t segmentPtr = 0;   // current segment count in current path having curves
	uint32_t subpathCount = 0; // store count of subpath, not straight forward to retrieve from segmented path array

	uint32_t  pathPtr = 0;		// 路径数组中的指针pointer in the path array  
	vg_state_save_t* t = 0;
	uint32_t curVertOffset = 0;
	bool     simpleConvex = false; // true if path is single rect or concave closed curve.
};

#define PATH_CLOSED_BIT 0x80000000 /* most significant bit of path elmts is closed/open path state */
#define PATH_HAS_CURVES_BIT                                                                                            \
    0x40000000                        /* 2rd most significant bit of path elmts is curved status                       \
                                       * for main path, this indicate that curve datas are present.                    \
                                       * For segments, this indicate that the segment is curved or not */
#define PATH_IS_CONVEX_BIT 0x20000000 /* simple rectangle or circle. */
#define PATH_ELT_MASK      0x1FFFFFFF /* Bit mask for fetching path element value */

#define ROUNDF(f, c)       (((float)((int)((f) * (c))) / (c)))
#define ROUND_DOWN(v, p)   (floorf(v * p) / p)
#define EQUF1(a, b)         (fabsf(a - (b)) <= FLT_EPSILON)
#define EQUF(a, b)         (fabsf(a - (b)) <= 1e-5)
#ifndef M_PI
#define M_PI 3.14159265358979323846
#define M_PI_2 1.57079632679489661923
#define M_2_PI 0.63661977236758134308 // 2/pi
#endif
void _matrix_get_scale(const glm::mat3x2* matrix, float* sx, float* sy) {
	auto c0 = (*matrix)[0];
	auto c1 = (*matrix)[1];
	auto c2 = (*matrix)[2];
	*sx = sqrt(c0.x * c0.x + c1.x * c1.x);
	/*if (matrix->xx < 0)
	 *sx = -*sx;*/
	*sy = sqrt(c0.y * c0.y + c1.y * c1.y);
	/*if (matrix->yy < 0)
	 *sy = -*sy;*/
}

void o_remove_last_point(ovg_path_t* ctx) {
	ctx->points.pop_back();
	ctx->pathes[ctx->pathPtr]--;
	if (ctx->segmentPtr > 0) {                            // if path is segmented
		if (!ctx->pathes[ctx->pathPtr + ctx->segmentPtr]) // if current segment is empty
			ctx->segmentPtr--;
		ctx->pathes[ctx->pathPtr + ctx->segmentPtr]--;                          // decrement last segment point count
		if ((ctx->pathes[ctx->pathPtr + ctx->segmentPtr] & PATH_ELT_MASK) == 0) // if no point left (was only one)
			ctx->pathes[ctx->pathPtr + ctx->segmentPtr] = 0;                    // reset current segment
		else if (ctx->pathes[ctx->pathPtr + ctx->segmentPtr] & PATH_HAS_CURVES_BIT) // if segment is a curve
			ctx->segmentPtr++; // then segPtr has to be forwarded to new segment
	}
	if (ctx->pathes.size() < ctx->segmentPtr + ctx->pathPtr)
		ctx->pathes.resize(ctx->segmentPtr + ctx->pathPtr + 1);
}
// test equality of two single precision vectors
inline bool vec2_equ(const glm::vec2& a, const glm::vec2& b) {
	bool r = (EQUF(a.x, b.x) & EQUF(a.y, b.y));
	return r;
}
inline glm::vec2 vec2_line_norm(const glm::vec2& a, const glm::vec2& b) {
	glm::vec2  d = { b.x - a.x, b.y - a.y };
	float md = sqrtf(d.x * d.x + d.y * d.y);
	d.x /= md;
	d.y /= md;
	return d;
}
// compute sum of two single precision vectors
inline glm::vec2 vec2_add(const glm::vec2& a, const glm::vec2& b) { return glm::vec2{ a.x + b.x, a.y + b.y }; }
// compute subbstraction of two single precision vectors
inline glm::vec2 vec2_sub(const glm::vec2& a, const glm::vec2& b) { return glm::vec2{ a.x - b.x, a.y - b.y }; }
// multiply 2d vector by scalar
inline glm::vec2 vec2_mult_s(const glm::vec2& a, float m) { return glm::vec2{ a.x * m, a.y * m }; }
// devide 2d vector by scalar
inline glm::vec2 vec2_div_s(const glm::vec2& a, float m) { return glm::vec2{ a.x / m, a.y / m }; }
// normalize float vector
inline glm::vec2 vec2_norm(const glm::vec2& a) {
	float m = sqrtf(a.x * a.x + a.y * a.y);
	return glm::vec2{ a.x / m, a.y / m };
}
inline glm::vec2 vec2_perp(const glm::vec2& a) { return glm::vec2{ a.y, -a.x }; }

void matrix_transform_distance(const glm::mat3x2* matrix, float* dx, float* dy) {
	float new_x, new_y;
	auto m = *matrix;
	new_x = (m[0].x * *dx + m[1].x * *dy);
	new_y = (m[0].y * *dx + m[1].y * *dy);

	*dx = new_x;
	*dy = new_y;
}

void matrix_transform_point(const glm::mat3x2* matrix, float* x, float* y) {
	glm::mat3x2 m = *matrix;
	glm::vec3 v = { *x,*y,1.0f };
	auto vv = m * v;
	*x = v.x; *y = v.y;
}

inline float vec2_zcross(const glm::vec2& v1, const glm::vec2& v2) { return v1.x * v2.y - v1.y * v2.x; }

#ifndef VG_COL32_A_MASK
#define VG_COL32_A_MASK     0xFF000000
#endif // !VG_COL32_A_MASK
#ifndef FIXNORMAL2F_MAX_INVLEN2
void normalize2f_over_zero(float& VX, float& VY)
{
	float d2 = VX * VX + VY * VY;
	if (d2 > 0.0f) {
		float inv_len = 1.0f / sqrtf(d2);
		VX *= inv_len; VY *= inv_len;
	}
}
#define FIXNORMAL2F_MAX_INVLEN2          100.0f // 500.0f (see #4053, #3366)
void fixnormal2f(float& VX, float& VY)
{
	float d2 = VX * VX + VY * VY;
	if (d2 > 0.000001f) {
		float inv_len2 = 1.0f / d2;
		if (inv_len2 > FIXNORMAL2F_MAX_INVLEN2)
			inv_len2 = FIXNORMAL2F_MAX_INVLEN2;
		VX *= inv_len2; VY *= inv_len2;
	}
}
#endif
bool o_path_has_curves(uint32_t* pathes, uint32_t ptrPath) { return   pathes[ptrPath] & PATH_HAS_CURVES_BIT; }

void _ovg_path_extents(ovg_path_t* ctx, bool transformed, float* x1, float* y1, float* x2, float* y2) {
	uint32_t ptrPath = 0;
	uint32_t firstPtIdx = 0;

	float xMin = FLT_MAX, yMin = FLT_MAX;
	float xMax = FLT_MIN, yMax = FLT_MIN;

	while (ptrPath < ctx->pathPtr) {
		uint32_t pathPointCount = ctx->pathes[ptrPath] & PATH_ELT_MASK;

		for (uint32_t i = firstPtIdx; i < firstPtIdx + pathPointCount; i++) {
			glm::vec2 p = ctx->points[i];
			if (transformed)
				matrix_transform_point(&ctx->t->pushConsts.mat, &p.x, &p.y);
			if (p.x < xMin)
				xMin = p.x;
			if (p.x > xMax)
				xMax = p.x;
			if (p.y < yMin)
				yMin = p.y;
			if (p.y > yMax)
				yMax = p.y;
		}

		firstPtIdx += pathPointCount;
		if (o_path_has_curves(ctx->pathes.data(), ptrPath)) {
			// skip segments lengths used in stroke
			ptrPath++;
			uint32_t totPts = 0;
			while (totPts < pathPointCount)
				totPts += (ctx->pathes[ptrPath++] & PATH_ELT_MASK);
		}
		else
			ptrPath++;
	}
	*x1 = xMin;
	*x2 = xMax;
	*y1 = yMin;
	*y2 = yMax;
}
bool _current_path_is_empty(ovg_path_t* ctx) {
	return ctx && (ctx->pathes.empty() || ctx->pathes[ctx->pathPtr] == 0);
}
// this function expect that current point exists
glm::vec2 _get_current_position(ovg_path_t* ctx) {
	return ctx->points.empty() ? glm::vec2() : ctx->points.back();
}

glm::vec2 _get_current_point(ovg_path_t* ctx) {
	glm::vec2 cp = {};
	if (_current_path_is_empty(ctx)) {
	}
	else
	{
		cp = _get_current_position(ctx);
	}
	return cp;
}
#define VG_ARRAY_THRESHOLD 8
#define VG_PATHES_SIZE 16
bool _check_pathes_array(ovg_path_t* ctx) {
	if (ctx->pathes.size() - ctx->pathPtr - ctx->segmentPtr > VG_ARRAY_THRESHOLD)
		return false;
	ctx->pathes.resize(ctx->pathes.size() + VG_PATHES_SIZE);
	return false;
}
#if 1
void _set_curve_start(ovg_path_t* ctx) {
	_check_pathes_array(ctx);
	if (ctx->segmentPtr > 0) {
		// check if current segment has points (straight)
		if ((ctx->pathes[ctx->pathPtr + ctx->segmentPtr] & PATH_ELT_MASK) > 0)
			ctx->segmentPtr++;
	}
	else {
		// not yet segmented path, first segment length is copied
		if (ctx->pathes[ctx->pathPtr] > 0) { // create first straight segment first
			ctx->pathes[ctx->pathPtr + 1] = ctx->pathes[ctx->pathPtr];
			ctx->segmentPtr = 2;
		}
		else
			ctx->segmentPtr = 1;
	}
	ctx->pathes[ctx->pathPtr + ctx->segmentPtr] = 0;
}
void _set_curve_end(ovg_path_t* ctx) {
	_check_pathes_array(ctx);
	ctx->pathes[ctx->pathPtr + ctx->segmentPtr] |= PATH_HAS_CURVES_BIT;
	ctx->segmentPtr++;
	ctx->pathes[ctx->pathPtr + ctx->segmentPtr] = 0;
}

void o_finish_path(ovg_path_t* ctx) {
	if (!ctx)return;
	do {
		if (ctx->pathes.empty())
			ctx->pathes.push_back(0);
		if (ctx->pathes[ctx->pathPtr] == 0) // empty
			break;
		if ((ctx->pathes[ctx->pathPtr] & PATH_ELT_MASK) < 2) {
			// only current pos is in path
			auto pointCount = ctx->points.size();
			pointCount -= ctx->pathes[ctx->pathPtr];
			ctx->points.resize(pointCount);
			ctx->pathes[ctx->pathPtr] = 0;
			ctx->segmentPtr = 0;
			break;
		}

		if (ctx->pathPtr == 0 && ctx->simpleConvex)
			ctx->pathes[0] |= PATH_IS_CONVEX_BIT;

		if (ctx->segmentPtr > 0) {
			ctx->pathes[ctx->pathPtr] |= PATH_HAS_CURVES_BIT;
			if ((ctx->pathes[ctx->pathPtr + ctx->segmentPtr] & PATH_HAS_CURVES_BIT) == 0 &&
				(ctx->pathes[ctx->pathPtr + ctx->segmentPtr] & PATH_ELT_MASK) > 0)
				ctx->segmentPtr++;
			ctx->pathPtr += ctx->segmentPtr;
		}
		else
			ctx->pathPtr++;

		if (_check_pathes_array(ctx))
			return;

		ctx->pathes[ctx->pathPtr] = 0;
		ctx->segmentPtr = 0;
		ctx->subpathCount++;
		ctx->simpleConvex = false;
	} while (0);

}
#else
void _set_curve_start(ovg_path_t* ctx) {
	if (ctx->segmentPtr > 0) {
		// check if current segment has points (straight)
		if ((ctx->pathes[ctx->pathPtr + ctx->segmentPtr] & PATH_ELT_MASK) > 0)
			ctx->segmentPtr++;
	}
	else {
		// not yet segmented path, first segment length is copied
		if (ctx->pathes[ctx->pathPtr] > 0) { // create first straight segment first
			ctx->pathes.push_back(ctx->pathes[ctx->pathPtr]);
			ctx->segmentPtr = 2;
		}
		else
			ctx->segmentPtr = 1;
	}
	//_check_pathes_array(ctx); 
	ctx->pathes.push_back(0);
	assert(ctx->pathes.size() > ctx->pathPtr + ctx->segmentPtr + 1);
}
void _set_curve_end(ovg_path_t* ctx) {
	ctx->pathes[ctx->pathPtr + ctx->segmentPtr] |= PATH_HAS_CURVES_BIT;
	ctx->segmentPtr++;
	ctx->pathes.push_back(0);
}
#endif
bool _path_is_closed(ovg_path_t* ctx, uint32_t ptrPath) { return ctx->pathes[ptrPath] & PATH_CLOSED_BIT; }
void _add_point(ovg_path_t* ctx, float x, float y) {
	if (isnan(x) || isnan(y)) {
		return;
	}
	glm::vec2 v = { x, y };
	ctx->points.push_back(v);
	if (ctx->pathes.size() <= ctx->pathPtr + ctx->segmentPtr)
		ctx->pathes.resize(ctx->pathPtr + ctx->segmentPtr + 1);
	ctx->pathes[ctx->pathPtr]++; // total point count in path
	if (ctx->segmentPtr > 0)
		ctx->pathes[ctx->pathPtr + ctx->segmentPtr]++; // total point count in path's segment
}

void _line_to(ovg_path_t* ctx, float x, float y) {
	glm::vec2 p = { x, y };
	if (!_current_path_is_empty(ctx)) {
		// prevent adding the same point
		auto v = _get_current_position(ctx);
		if (vec2_equ(v, p))
			return;
	}
	_add_point(ctx, x, y);
	ctx->simpleConvex = false;
}

float _get_arc_step(ovg_path_t* ctx, float radius) {
	float sx = 1.0, sy = 1.0;
	if (ctx->t)
		_matrix_get_scale(&ctx->t->pushConsts.mat, &sx, &sy);
	float r = radius * fabsf(fmaxf(sx, sy));
	if (r < 30.0f)
		return fminf(M_PI / 3.f, M_PI / r);
	return fminf(M_PI / 3.f, M_PI / (r * 0.4f));
}

void ovg_move_to(ovg_path_t* path, float x, float y);
void ovg_line_to(ovg_path_t* path, float x, float y);
void ovg_quadratic_to(ovg_path_t* path, float x1, float y1, float x2, float y2);
void ovg_curve_to(ovg_path_t* path, float x1, float y1, float x2, float y2, float x3, float y3);
void ovg_elliptic_arc_to(ovg_path_t* path, float x, float y, bool large_arc_flag, bool sweep_flag, float rx, float ry, float phi);

#define M_APPROXIMATION_SCALE         1.0
#define M_ANGLE_TOLERANCE             0.01
#define M_CUSP_LIMIT                  0.01
#define CURVE_RECURSION_LIMIT         100
#define CURVE_COLLINEARITY_EPSILON    1.7
#define CURVE_ANGLE_TOLERANCE_EPSILON 0.001
// no floating point arithmetic operation allowed in macro.
#pragma warning(disable : 4127)
void _recursive_bezier(ovg_path_t* ctx, float distanceTolerance, float x1, float y1, float x2, float y2, float x3,
	float y3, float x4, float y4, unsigned level) {
	if (level > CURVE_RECURSION_LIMIT) {
		return;
	}

	// Calculate all the mid-points of the line segments
	//----------------------
	float x12 = (x1 + x2) / 2;
	float y12 = (y1 + y2) / 2;
	float x23 = (x2 + x3) / 2;
	float y23 = (y2 + y3) / 2;
	float x34 = (x3 + x4) / 2;
	float y34 = (y3 + y4) / 2;
	float x123 = (x12 + x23) / 2;
	float y123 = (y12 + y23) / 2;
	float x234 = (x23 + x34) / 2;
	float y234 = (y23 + y34) / 2;
	float x1234 = (x123 + x234) / 2;
	float y1234 = (y123 + y234) / 2;

	if (level > 0) // Enforce subdivision first time
	{
		// Try to approximate the full cubic curve by a single straight line
		//------------------
		float dx = x4 - x1;
		float dy = y4 - y1;

		float d2 = fabsf(((x2 - x4) * dy - (y2 - y4) * dx));
		float d3 = fabsf(((x3 - x4) * dy - (y3 - y4) * dx));

		float da1, da2;

		if (d2 > CURVE_COLLINEARITY_EPSILON && d3 > CURVE_COLLINEARITY_EPSILON) {
			// Regular care
			//-----------------
			if ((d2 + d3) * (d2 + d3) <= (dx * dx + dy * dy) * distanceTolerance) {
				// If the curvature doesn't exceed the distance_tolerance value
				// we tend to finish subdivisions.
				//----------------------
				if (M_ANGLE_TOLERANCE < CURVE_ANGLE_TOLERANCE_EPSILON) {
					_add_point(ctx, x1234, y1234);
					return;
				}

				// Angle & Cusp Condition
				//----------------------
				float a23 = atan2f(y3 - y2, x3 - x2);
				da1 = fabsf(a23 - atan2f(y2 - y1, x2 - x1));
				da2 = fabsf(atan2f(y4 - y3, x4 - x3) - a23);
				if (da1 >= M_PI)
					da1 = M_2_PI - da1;
				if (da2 >= M_PI)
					da2 = M_2_PI - da2;

				if (da1 + da2 < (float)M_ANGLE_TOLERANCE) {
					// Finally we can stop the recursion
					//----------------------
					_add_point(ctx, x1234, y1234);
					return;
				}

				if (M_CUSP_LIMIT != 0.0) {
					if (da1 > M_CUSP_LIMIT) {
						_add_point(ctx, x2, y2);
						return;
					}

					if (da2 > M_CUSP_LIMIT) {
						_add_point(ctx, x3, y3);
						return;
					}
				}
			}
		}
		else {
			if (d2 > CURVE_COLLINEARITY_EPSILON) {
				// p1,p3,p4 are collinear, p2 is considerable
				//----------------------
				if (d2 * d2 <= distanceTolerance * (dx * dx + dy * dy)) {
					if (M_ANGLE_TOLERANCE < CURVE_ANGLE_TOLERANCE_EPSILON) {
						_add_point(ctx, x1234, y1234);
						return;
					}

					// Angle Condition
					//----------------------
					da1 = fabsf(atan2f(y3 - y2, x3 - x2) - atan2f(y2 - y1, x2 - x1));
					if (da1 >= M_PI)
						da1 = M_2_PI - da1;

					if (da1 < M_ANGLE_TOLERANCE) {
						_add_point(ctx, x2, y2);
						_add_point(ctx, x3, y3);
						return;
					}

					if (M_CUSP_LIMIT != 0.0) {
						if (da1 > M_CUSP_LIMIT) {
							_add_point(ctx, x2, y2);
							return;
						}
					}
				}
			}
			else if (d3 > CURVE_COLLINEARITY_EPSILON) {
				// p1,p2,p4 are collinear, p3 is considerable
				//----------------------
				if (d3 * d3 <= distanceTolerance * (dx * dx + dy * dy)) {
					if (M_ANGLE_TOLERANCE < CURVE_ANGLE_TOLERANCE_EPSILON) {
						_add_point(ctx, x1234, y1234);
						return;
					}

					// Angle Condition
					//----------------------
					da1 = fabsf(atan2f(y4 - y3, x4 - x3) - atan2f(y3 - y2, x3 - x2));
					if (da1 >= M_PI)
						da1 = M_2_PI - da1;

					if (da1 < M_ANGLE_TOLERANCE) {
						_add_point(ctx, x2, y2);
						_add_point(ctx, x3, y3);
						return;
					}

					if (M_CUSP_LIMIT != 0.0) {
						if (da1 > M_CUSP_LIMIT) {
							_add_point(ctx, x3, y3);
							return;
						}
					}
				}
			}
			else {
				// Collinear case
				//-----------------
				dx = x1234 - (x1 + x4) / 2;
				dy = y1234 - (y1 + y4) / 2;
				if (dx * dx + dy * dy <= distanceTolerance) {
					_add_point(ctx, x1234, y1234);
					return;
				}
			}
		}
	}

	// Continue subdivision
	//----------------------
	_recursive_bezier(ctx, distanceTolerance, x1, y1, x12, y12, x123, y123, x1234, y1234, level + 1);
	_recursive_bezier(ctx, distanceTolerance, x1234, y1234, x234, y234, x34, y34, x4, y4, level + 1);
}

static const glm::vec2 _v2_unit_x = { 1.f, 0 };
static const glm::vec2 _v2_unit_y = { 0, 1.f };
void _elliptic_arc(ovg_path_t* ctx, float x1, float y1, float x2, float y2, bool largeArc, bool counterClockWise, float _rx, float _ry, float phi) {
	if (!ctx)
		return;

	if (_rx == 0 || _ry == 0) {
		if (_current_path_is_empty(ctx))
			ovg_move_to(ctx, x1, y1);
		ovg_line_to(ctx, x2, y2);
		return;
	}
	float rx = fabsf(_rx);
	float ry = fabsf(_ry);

	glm::mat2 m = { {cosf(phi), sinf(phi)}, {-sinf(phi), cosf(phi)} };
	glm::vec2 p = { (x1 - x2) / 2, (y1 - y2) / 2 };
	glm::vec2 p1 = m * p;

	// radii corrections
	double lambda = powf(p1.x, 2) / powf(rx, 2) + powf(p1.y, 2) / powf(ry, 2);
	if (lambda > 1) {
		lambda = sqrtf(lambda);
		rx *= lambda;
		ry *= lambda;
	}

	p = glm::vec2{ rx * p1.y / ry, -ry * p1.x / rx };

	glm::vec2 cp = p * sqrtf(fabsf((powf(rx, 2) * powf(ry, 2) - powf(rx, 2) * powf(p1.y, 2) - powf(ry, 2) * powf(p1.x, 2)) /
		(powf(rx, 2) * powf(p1.y, 2) + powf(ry, 2) * powf(p1.x, 2))));

	if (largeArc == counterClockWise)
		cp = -cp;

	m = glm::mat2({ cosf(phi), -sinf(phi) }, { sinf(phi), cosf(phi) });
	p = glm::vec2((x1 + x2) / 2, (y1 + y2) / 2);
	glm::vec2 c = (m * cp) + p;

	glm::vec2   u = _v2_unit_x;
	glm::vec2   v = { (p1.x - cp.x) / rx, (p1.y - cp.y) / ry };
	double sa = acosf(glm::dot(u, v) / (fabsf(glm::length(v)) * fabsf(glm::length(u))));
	if (isnan((float)sa))
		sa = M_PI;
	if (u.x * v.y - u.y * v.x < 0)
		sa = -sa;

	u = v;
	v = glm::vec2{ (-p1.x - cp.x) / rx, (-p1.y - cp.y) / ry };
	double delta_theta = acosf(glm::dot(u, v) / (fabsf(glm::length(v)) * fabsf(glm::length(u))));
	if (isnan((float)delta_theta))
		delta_theta = M_PI;
	if (u.x * v.y - u.y * v.x < 0)
		delta_theta = -delta_theta;

	if (counterClockWise) {
		if (delta_theta < 0)
			delta_theta += M_PI * 2.0;
	}
	else if (delta_theta > 0)
		delta_theta -= M_PI * 2.0;

	m = glm::mat2{ {cosf(phi), -sinf(phi)}, {sinf(phi), cosf(phi)} };

	double theta = sa;
	double ea = sa + delta_theta;

	float step = fmaxf(0.001f, fminf(M_PI, _get_arc_step(ctx, fminf(rx, ry)) * 0.1f));

	p = glm::vec2{ rx * cosf(theta), ry * sinf(theta) };
	glm::vec2 xy = ((m * p) + c);
	if (_current_path_is_empty(ctx)) {
		_set_curve_start(ctx);
		_add_point(ctx, xy.x, xy.y);
		if (!ctx->pathPtr)
			ctx->simpleConvex = true;
		else
			ctx->simpleConvex = false;
	}
	else {
		ovg_line_to(ctx, xy.x, xy.y);
		_set_curve_start(ctx);
		ctx->simpleConvex = false;
	}

	_set_curve_start(ctx);

	if (sa < ea) {
		theta += step;
		while (theta < ea) {
			p = glm::vec2{ rx * cosf(theta), ry * sinf(theta) };
			xy = ((m * p) + c);
			_add_point(ctx, xy.x, xy.y);
			theta += step;
		}
	}
	else {
		theta -= step;
		while (theta > ea) {
			p = glm::vec2{ rx * cosf(theta), ry * sinf(theta) };
			xy = ((m * p) + c);
			_add_point(ctx, xy.x, xy.y);
			theta -= step;
		}
	}
	p = glm::vec2{ rx * cosf(ea), ry * sinf(ea) };
	xy = ((m * p) + c);
	_add_point(ctx, xy.x, xy.y);
	_set_curve_end(ctx);
}

// todo 接口实现开始

void ovg_clear_path(ovg_path_t* path) {
	if (!path)return;
	path->points.clear();
	path->pathes.clear();
	path->pathes.push_back(0);
	path->pathPtr = 0;
	path->segmentPtr = 0;
	path->subpathCount = 0;
	path->curVertOffset = 0;
	path->simpleConvex = 0;
}

void ovg_close_path(ovg_path_t* path)
{
	auto ctx = path;
	if (!ctx)
		return;
	if (ctx->pathes[ctx->pathPtr] & PATH_CLOSED_BIT) // already closed
		return;
	// check if at least 3 points are present
	if (ctx->pathes[ctx->pathPtr] < 3)
		return;
	auto pointCount = ctx->points.size();
	// prevent closing on the same point
	if (vec2_equ(ctx->points[pointCount - 1], ctx->points[pointCount - ctx->pathes[ctx->pathPtr]])) {
		if (ctx->pathes[ctx->pathPtr] < 4) // ensure enough points left for closing
			return;
		o_remove_last_point(ctx);
	}
	//pointCount = ctx->points.size();
	//auto v0 = ctx->points[pointCount - ctx->pathes[ctx->pathPtr]];
	//if (!vec2_equ(ctx->points[pointCount - 1], v0)) {
	//	_add_point(ctx, v0.x, v0.y);
	//}
	ctx->pathes[ctx->pathPtr] |= PATH_CLOSED_BIT;

	o_finish_path(ctx);
}
void ovg_new_sub_path(ovg_path_t* path)
{
	o_finish_path(path);
}
void ovg_path_extents(ovg_path_t* path, float* x1, float* y1, float* x2, float* y2)
{
	if (!path)return;
	o_finish_path(path);
	if (!path->pathPtr) { // no path
		*x1 = *x2 = *y1 = *y2 = 0;
		return;
	}
	_ovg_path_extents(path, false, x1, y1, x2, y2);
}
void ovg_get_current_point(ovg_path_t* path, float* x, float* y)
{
	auto cp = _get_current_point(path);
	if (x)*x = cp.x;
	if (y)*y = cp.y;
}
size_t ovg_get_segment_count(ovg_path_t* path) {
	return path ? path->pathes.size() : 0;
}
void ovg_set_segment_color(ovg_path_t* path, size_t idx, uint32_t color) {
	if (path)
	{
		if (path->colors.size() < path->pathes.size())
			path->colors.resize(path->pathes.size());
		if (idx < path->colors.size())
			path->colors[idx] = color;
	}
}
// 添加数据到当前路径，参考path_type_e
void ovg_add_path(ovg_path_t* path, float* data, size_t count)
{
	if (!path || !data || !count)return;
	auto d = data;
	for (; d - data < count;) {
		auto t = (path_type_et)*d;
		float x = d[1], y = d[2];
		d += 3;
		switch (t) {
		case path_type_et::e_vmove:
			ovg_move_to(path, x, y);//2
			break;
		case path_type_et::e_vline:
			ovg_line_to(path, x, y);//2
			break;
		case path_type_et::e_vcurve:
		{
			ovg_quadratic_to(path, x, y, d[0], d[1]);//5
			d += 2;
		}
		break;
		case path_type_et::e_vcubic:
		{
			ovg_curve_to(path, x, y, d[0], d[1], d[2], d[3]);//6
			d += 4;
		}
		break;
		case path_type_et::e_close:
		{
			ovg_close_path(path); d -= 2; //1
		}
		break;
		}
	}
}
// todo path copy
void ovg_add_path0(ovg_path_t* path, ovg_path_t* src)
{
	if (!path || !src)return;
	if (_current_path_is_empty(src))return;

}
void ovg_move_to(ovg_path_t* path, float x, float y)
{
	if (!path)
		return;
	o_finish_path(path);
	_add_point(path, x, y);

}
void ovg_rel_move_to(ovg_path_t* path, float x, float y)
{
	if (!path)return;
	if (_current_path_is_empty(path))
		_add_point(path, 0, 0);
	auto cp = _get_current_position(path);
	o_finish_path(path);
	_add_point(path, cp.x + x, cp.y + y);
}
void ovg_line_to(ovg_path_t* path, float x, float y)
{
	if (!path)
		return;
	_line_to(path, x, y);
}
void ovg_rel_line_to(ovg_path_t* path, float dx, float dy)
{
	if (!path)
		return;
	auto cp = _get_current_position(path);
	_line_to(path, cp.x + dx, cp.y + dy);
}
void ovg_arc(ovg_path_t* path, float xc, float yc, float radius, float a1, float a2)
{
	if (!path)
		return;
	while (a2 < a1) // positive arc must have a1<a2
		a2 += 2.f * M_PI;
	if (a2 - a1 > 2.f * M_PI) // limit arc to 2PI
		a2 = a1 + 2.f * M_PI;
	glm::vec2 v = { cosf(a1) * radius + xc, sinf(a1) * radius + yc };
	auto v0 = v;
	float step = _get_arc_step(path, radius);
	float a = a1;
	if (_current_path_is_empty(path)) {
		_set_curve_start(path);
		_add_point(path, v.x, v.y);
		if (!path->pathPtr)
			path->simpleConvex = true;
		else
			path->simpleConvex = false;
	}
	else {
		_line_to(path, v.x, v.y);
		_set_curve_start(path);
		path->simpleConvex = false;
	}
	a += step;
	if (EQUF(a2, a1))
		return;
	while (a < a2) {
		v.x = cosf(a) * radius + xc;
		v.y = sinf(a) * radius + yc;
		_add_point(path, v.x, v.y);
		a += step;
	}
	if (EQUF(a2 - a1, M_PI * 2.f)) { // if arc is complete circle, last point is the same as the first one
		_set_curve_end(path);
		ovg_close_path(path);
		return;
	}
	a = a2;
	// vec2 lastP = v;
	v.x = cosf(a) * radius + xc;
	v.y = sinf(a) * radius + yc;
	// if (!vec2_equ (v,lastP))//this test should not be required
	_add_point(path, v.x, v.y);
	_set_curve_end(path);
	// todo 结束精度1000
	auto d = abs(v0 - v);
	d *= 1000.0;
	if (d.x < 1.0 && d.y < 1.0)
		ovg_close_path(path);
}
void ovg_arc_negative(ovg_path_t* path, float xc, float yc, float radius, float a1, float a2)
{
	if (!path)
		return;
	auto ctx = path;

	while (a2 > a1)
		a2 -= 2.f * M_PI;
	if (a1 - a2 > a1 + 2.f * M_PI) // limit arc to 2PI
		a2 = a1 - 2.f * M_PI;

	glm::vec2 v = { cosf(a1) * radius + xc, sinf(a1) * radius + yc };
	auto v0 = v;
	float step = _get_arc_step(ctx, radius);
	float a = a1;

	if (_current_path_is_empty(ctx)) {
		_set_curve_start(ctx);
		_add_point(ctx, v.x, v.y);
		if (!ctx->pathPtr)
			ctx->simpleConvex = true;
		else
			ctx->simpleConvex = false;
	}
	else {
		_line_to(ctx, v.x, v.y);
		_set_curve_start(ctx);
		ctx->simpleConvex = false;
	}

	a -= step;

	if (EQUF(a2, a1))
		return;

	while (a > a2) {
		v.x = cosf(a) * radius + xc;
		v.y = sinf(a) * radius + yc;
		_add_point(ctx, v.x, v.y);
		a -= step;
	}

	if (EQUF(a1 - a2, M_PI * 2.f)) { // if arc is complete circle, last point is the same as the first one
		_set_curve_end(ctx);
		ovg_close_path(ctx);
		return;
	}

	a = a2;
	// vec2 lastP = v;
	v.x = cosf(a) * radius + xc;
	v.y = sinf(a) * radius + yc;
	// if (!vec2_equ (v,lastP))
	_add_point(ctx, v.x, v.y);
	_set_curve_end(ctx);
	auto d = abs(v0 - v);
	d *= 1000.0;
	if (d.x < 1.0 && d.y < 1.0)
		ovg_close_path(path);
}
void ovg_curve_to(ovg_path_t* path, float x1, float y1, float x2, float y2, float x3, float y3)
{
	if (EQUF(x1, x2) && EQUF(x2, x3) && EQUF(y1, y2) && EQUF(y2, y3)) {
		auto cp = _get_current_position(path);
		if (_current_path_is_empty(path) || (EQUF(cp.x, x1) && EQUF(cp.y, y1)))
			return;
	}

	path->simpleConvex = false;
	_set_curve_start(path);
	if (_current_path_is_empty(path))
		_add_point(path, x1, y1);

	glm::vec2 cp = _get_current_position(path);
	float sx = 1, sy = 1;
	if (path->t)
		_matrix_get_scale(&path->t->pushConsts.mat, &sx, &sy);
	float distanceTolerance = fabs(0.25f / fmaxf(sx, sy));
	_recursive_bezier(path, distanceTolerance, cp.x, cp.y, x1, y1, x2, y2, x3, y3, 0);
	_add_point(path, x3, y3);
	_set_curve_end(path);
}
void ovg_rel_curve_to(ovg_path_t* path, float x1, float y1, float x2, float y2, float x3, float y3)
{
	glm::vec2 cp = _get_current_position(path);
	ovg_curve_to(path, cp.x + x1, cp.y + y1, cp.x + x2, cp.y + y2, cp.x + x3, cp.y + y3);
}
const double quadraticFact = 2.0 / 3.0;
void ovg_quadratic_to(ovg_path_t* path, float x1, float y1, float x2, float y2)
{
	float x0, y0;
	if (_current_path_is_empty(path)) {
		x0 = x1;
		y0 = y1;
	}
	else
	{
		glm::vec2 cp = _get_current_position(path);
		x0 = cp.x; y0 = cp.y;
	}
	ovg_curve_to(path, x0 + (x1 - x0) * quadraticFact, y0 + (y1 - y0) * quadraticFact, x2 + (x1 - x2) * quadraticFact,
		y2 + (y1 - y2) * quadraticFact, x2, y2);
}
void ovg_rel_quadratic_to(ovg_path_t* path, float x1, float y1, float x2, float y2)
{
	glm::vec2 cp = _get_current_position(path);
	ovg_quadratic_to(path, cp.x + x1, cp.y + y1, cp.x + x2, cp.y + y2);
}
void ovg_rectangle(ovg_path_t* path, float x, float y, float w, float h)
{
	if (!path)
		return;
	o_finish_path(path);
	if (w <= 0 || h <= 0)
		return;
	ovg_move_to(path, x, y);
	ovg_line_to(path, x + w, y);
	ovg_line_to(path, x + w, y + h);
	_add_point(path, x, y + h);
	assert(path->pathPtr < path->pathes.size());
	path->pathes[path->pathPtr] |= (PATH_CLOSED_BIT | PATH_IS_CONVEX_BIT);
	ovg_close_path(path);
}
void ovg_rounded_rectangle(ovg_path_t* path, float x, float y, float w, float h, float radius)
{
	if (!path)
		return;
	if (w <= 0 || h <= 0)
		return;
	o_finish_path(path);
	if ((radius > w / 2.0f) || (radius > h / 2.0f))
		radius = fmin(w / 2.0f, h / 2.0f);
	ovg_move_to(path, x, y + radius);
	ovg_arc(path, x + radius, y + radius, radius, M_PI, -M_PI_2);
	ovg_line_to(path, x + w - radius, y);
	ovg_arc(path, x + w - radius, y + radius, radius, -M_PI_2, 0);
	ovg_line_to(path, x + w, y + h - radius);
	ovg_arc(path, x + w - radius, y + h - radius, radius, 0, M_PI_2);
	ovg_line_to(path, x + radius, y + h);
	ovg_arc(path, x + radius, y + h - radius, radius, M_PI_2, M_PI);
	ovg_line_to(path, x, y + radius);
	ovg_close_path(path);
}
void ovg_rounded_rectangle2(ovg_path_t* path, float x, float y, float w, float h, float rx, float ry)
{
	if (!path)
		return;
	ovg_move_to(path, x + rx, y);
	ovg_line_to(path, x + w - rx, y);
	ovg_elliptic_arc_to(path, x + w, y + ry, false, true, rx, ry, 0);

	ovg_line_to(path, x + w, y + h - ry);
	ovg_elliptic_arc_to(path, x + w - rx, y + h, false, true, rx, ry, 0);

	ovg_line_to(path, x + rx, y + h);
	ovg_elliptic_arc_to(path, x, y + h - ry, false, true, rx, ry, 0);

	ovg_line_to(path, x, y + ry);
	ovg_elliptic_arc_to(path, x + rx, y, false, true, rx, ry, 0);

	ovg_close_path(path);
}
void ovg_ellipse(ovg_path_t* path, float radiusX, float radiusY, float x, float y, float rotationAngle)
{
	if (!path)
		return;
	float width_two_thirds = radiusX * 4 / 3;

	float dx1 = sinf(rotationAngle) * radiusY;
	float dy1 = cosf(rotationAngle) * radiusY;
	float dx2 = cosf(rotationAngle) * width_two_thirds;
	float dy2 = sinf(rotationAngle) * width_two_thirds;

	float topCenterX = x - dx1;
	float topCenterY = y + dy1;
	float topRightX = topCenterX + dx2;
	float topRightY = topCenterY + dy2;
	float topLeftX = topCenterX - dx2;
	float topLeftY = topCenterY - dy2;

	float bottomCenterX = x + dx1;
	float bottomCenterY = y - dy1;
	float bottomRightX = bottomCenterX + dx2;
	float bottomRightY = bottomCenterY + dy2;
	float bottomLeftX = bottomCenterX - dx2;
	float bottomLeftY = bottomCenterY - dy2;

	o_finish_path(path);
	ovg_move_to(path, bottomCenterX, bottomCenterY);

	ovg_curve_to(path, bottomRightX, bottomRightY, topRightX, topRightY, topCenterX, topCenterY);
	ovg_curve_to(path, topLeftX, topLeftY, bottomLeftX, bottomLeftY, bottomCenterX, bottomCenterY);

	path->pathes[path->pathPtr] |= PATH_CLOSED_BIT;
	o_finish_path(path);
}
void ovg_elliptic_arc_to(ovg_path_t* path, float x, float y, bool large_arc_flag, bool sweep_flag, float rx, float ry, float phi)
{
	if (!path)
		return;
	float x1 = 0.0, y1 = 0.0;
	auto cp = _get_current_point(path);
	_elliptic_arc(path, x1, y1, x, y, large_arc_flag, sweep_flag, rx, ry, phi);
}
void ovg_rel_elliptic_arc_to(ovg_path_t* path, float x, float y, bool large_arc_flag, bool sweep_flag, float rx, float ry, float phi)
{
	if (!path)
		return;
	auto cp = _get_current_point(path);
	_elliptic_arc(path, cp.x, cp.y, x + cp.x, y + cp.y, large_arc_flag, sweep_flag, rx, ry, phi);
}
void ovg_circle(ovg_path_t* path, float x, float y, float radius) {
	//ovg_arc(path, x, y, radius, 0, 2.0 * glm::pi<float>());
	ovg_ellipse(path, radius, radius, x, y, 0.0f);
}

#ifdef CreateRgbaf
#undef CreateRgbaf
#endif
#define CreateRgbaf(r, g, b, a)                                                                                        \
    (((int)(a * 255.0f) << 24) | ((int)(b * 255.0f) << 16) | ((int)(g * 255.0f) << 8) | (int)(r * 255.0f))



struct ss_act :public  vg_state_save_t {
	usp_ac_cx* ac = 0;
};

struct pat_act :public  vg_pattern_t {
	vg_gradient_t g = {};
	usp_ac_cx* ac = 0;
};


vg_pattern_t* ovg_pattern_create_for_surface(usp_ac_cx* ac, void* surf) {
	if (!surf || !ac) {
		return 0;
	}
	pat_act* pat = (pat_act*)ac->new_obj<pat_act>();
	if (!pat) {
		return 0;
	}
	*pat = {};
	pat->ac = ac;
	pat->type = vg_pattern_type_t::VG_PATTERN_TYPE_SURFACE;
	pat->extend = vg_extend_t::VG_EXTEND_NONE;
	pat->data = surf;
	pat->references = 1;

	return pat;
}
// todo vg_state_save_t
void ovg_set_opacity(vg_state_save_t* ctx, float opacity) {
	if (ctx)ctx->pushConsts.opacity = opacity;
}
void ovg_set_source_color(vg_state_save_t* ctx, uint32_t c) {
	if (ctx)
	{
		ctx->color = c; ctx->pattern = 0;
	}
}
void ovg_set_source_rgba(vg_state_save_t* ctx, float r, float g, float b, float a) {
	if (ctx)
	{
		ctx->color = CreateRgbaf(r, g, b, a); ctx->pattern = 0;
	}
}
void ovg_set_source_rgb(vg_state_save_t* ctx, float r, float g, float b) {
	ovg_set_source_rgba(ctx, r, g, b, 1.0f);
}
void ovg_set_line_width(vg_state_save_t* ctx, float width) {
	if (ctx)ctx->lineWidth = width;
}
void ovg_set_miter_limit(vg_state_save_t* ctx, float limit) {
	if (ctx)ctx->miterLimit = limit;
}
void ovg_set_line_cap(vg_state_save_t* ctx, int cap) {
	if (ctx)ctx->lineCap = cap;
}
void ovg_set_line_join(vg_state_save_t* ctx, int join) {
	if (ctx)ctx->lineJoin = join;
}
void ovg_set_source_surface(vg_state_save_t* ctx, vg_image_t* surf, float x, float y) {
	auto p = (ss_act*)ctx;
	p->pushConsts.source.x = x;
	p->pushConsts.source.y = y;
	auto pat = ovg_pattern_create_for_surface(p->ac, surf);
	p->pattern = pat;
}
void ovg_set_source(vg_state_save_t* ctx, vg_pattern_t* pat) {
	if (ctx)ctx->pattern = pat;
}
void ovg_set_operator(vg_state_save_t* ctx, int op) {
	if (ctx)ctx->curOperator = (vg_operator_t)op;
}
void ovg_set_fill_rule(vg_state_save_t* ctx, int fr) {
	if (ctx)ctx->curFillRule = fr;
}
void ovg_set_dash(vg_state_save_t* ctx, const float* dashes, uint32_t num_dashes, float offset) {
	if (!ctx || !dashes)return;
	auto t = (ss_act*)ctx;
	if (!dashes || !num_dashes) {
		t->dashCount = 0;
	}
	if (!t->dashes || t->dashCount != num_dashes)
	{
		if (t->dashes)
			t->ac->free_mem(t->dashes, t->dashCount);
		t->dashes = (float*)t->ac->allocate(sizeof(float) * num_dashes);
	}
	t->dashCount = num_dashes;
	t->dashOffset = offset;
	if (t->dashes)
		memcpy(t->dashes, dashes, sizeof(float) * t->dashCount);
	else
		t->dashCount = 0;
}
void ovg_set_dash8(vg_state_save_t* ctx, uint64_t dashes0, uint32_t num_dashes, float offset) {

	float dashes[64] = {};
	uint64_t x = 1;
	auto t = dashes;
	auto v8 = (uint8_t*)&dashes0;
	if (num_dashes > 64)num_dashes = 64;
	{
		if (num_dashes > 8)num_dashes = 8;
		for (size_t i = 0; i < num_dashes; i++)
		{
			*t = v8[i]; t++;
		}
		if (num_dashes > 0)
			ovg_set_dash(ctx, dashes, num_dashes, offset);
	}
}
void ovg_translate(vg_state_save_t* ctx, float dx, float dy) {
	if (!ctx)return;
	auto m = glm::translate(glm::mat3x3(1.0), glm::vec2(dx, dy));
	glm::mat3x3 inv = ctx->pushConsts.mat;
	ctx->pushConsts.mat = inv * m; inv = ctx->pushConsts.mat;
	ctx->pushConsts.matInv = glm::inverse(inv);
}
void ovg_scale(vg_state_save_t* ctx, float sx, float sy) {
	if (!ctx)return;
	auto m = glm::scale(glm::mat3x3(1.0), glm::vec2(sx, sy));
	glm::mat3x3 inv = ctx->pushConsts.mat;
	ctx->pushConsts.mat = inv * m; inv = ctx->pushConsts.mat;
	ctx->pushConsts.matInv = glm::inverse(inv);
}
void ovg_rotate(vg_state_save_t* ctx, float radians) {
	if (!ctx)return;
	auto m = glm::rotate(glm::mat3x3(1.0), radians);
	glm::mat3x3 inv = ctx->pushConsts.mat;
	ctx->pushConsts.mat = inv * m; inv = ctx->pushConsts.mat;
	ctx->pushConsts.matInv = glm::inverse(inv);
}
void ovg_transform(vg_state_save_t* ctx, const void* matrix) {
	auto m = (glm::mat3x2*)matrix;
	if (!ctx || !m)return;
	glm::mat3x3 inv = ctx->pushConsts.mat;
	glm::mat3x3 m0 = *m;
	ctx->pushConsts.mat = inv * m0; inv = ctx->pushConsts.mat;
	ctx->pushConsts.matInv = glm::inverse(inv);
}
void ovg_set_matrix(vg_state_save_t* ctx, const void* matrix) {
	auto m = (glm::mat3x2*)matrix;
	if (!ctx || !m)return;
	ctx->pushConsts.mat = *m;
	glm::mat3x3 inv = *m;
	ctx->pushConsts.matInv = glm::inverse(inv);
}
void ovg_get_matrix(vg_state_save_t* ctx, void* matrix) {
	auto m = (glm::mat3x2*)matrix;
	if (!ctx || !m)return;
	*m = ctx->pushConsts.mat;
}
void ovg_identity_matrix(vg_state_save_t* ctx) {
	if (!ctx)return;
	ctx->pushConsts.mat = glm::mat3x2(1.0);
	glm::mat3x3 inv = ctx->pushConsts.mat;
	ctx->pushConsts.matInv = glm::inverse(inv);
}

void ovg_matrix_init(void* mat, float xx, float yx, float xy, float yy, float x0, float y0)
{
	if (!mat)return;
	*((glm::mat3x2*)mat) = glm::mat3x2(xx, yx, xy, yy, x0, y0);
}

int  ovg_pattern_add_color_stop(vg_pattern_t* pat, float o, float r, float g, float b, float a) {
	if (pat->type == vg_pattern_type_t::VG_PATTERN_TYPE_SURFACE || pat->type == vg_pattern_type_t::VG_PATTERN_TYPE_SOLID)
		return -1;
	vg_gradient_t* grad = (vg_gradient_t*)pat->data;
	if (grad->count < MAX_STOPS)
	{
		glm::vec4 c = { r, g, b, a };
		grad->colors[grad->count] = c;
#ifndef NOT_VG_ENABLE_VK_SCALAR_BLOCK_LAYOUT
		grad->stops[grad->count] = o;
#else
		grad->stops[grad->count].r = o;
#endif
		grad->count++;
	}
	return 0;
}
int  ovg_pattern_set_color_stop(vg_pattern_t* pat, int idx, float o, float r, float g, float b, float a) {
	if (!pat)return -1;
	if (pat->type == vg_pattern_type_t::VG_PATTERN_TYPE_SURFACE || pat->type == vg_pattern_type_t::VG_PATTERN_TYPE_SOLID)
		return -2;
	vg_gradient_t* grad = (vg_gradient_t*)pat->data;
	if (idx < 0 || idx >= MAX_STOPS)return -3;
	if (idx >= grad->count)
		grad->count = idx + 1;
	glm::vec4 c = { r, g, b, a };
	grad->colors[idx] = c;
	grad->stops[idx] = o;
	return 0;
}
void ovg_pattern_set_matrix(vg_pattern_t* pat, const void* matrix) {
	if (!pat || !matrix)return;
	pat->matrix = *((glm::mat3x2*)matrix);
	pat->hasMatrix = true;
}
void ovg_pattern_get_matrix(vg_pattern_t* pat, void* matrix) {
	if (!pat || !matrix)
		return;
	*((glm::mat3x2*)matrix) = (pat->hasMatrix) ? pat->matrix : glm::mat3x2(1.0);
}
void ovg_pattern_set_extend(vg_pattern_t* pat, int extend) {
	if (pat)pat->extend = (vg_extend_t)extend;
}
void ovg_pattern_set_filter(vg_pattern_t* pat, int filter) {
	if (pat)pat->filter = (vg_filter_t)filter;
}
void ovg_pattern_destroy(vg_pattern_t* pat) {
	if (pat) {
		auto p = (pat_act*)pat;
		if (p->ac) {
			p->ac->free_obj(p);
		}
	}
}



int _vg_pattern_edit_linear(vg_pattern_t* pat, float x0, float y0, float x1, float y1) {
	if (!pat)
		return -2;
	if (pat->type != vg_pattern_type_t::VG_PATTERN_TYPE_LINEAR)
		return -1;
	vg_gradient_t* grad = (vg_gradient_t*)pat->data;
	*grad = {};
	grad->cp[0] = glm::vec4{ {x0}, {y0}, {x1}, {y1} };
	grad->m = glm::ivec4(1024, 0, 0, 1024);
	grad->extend = pat->extend;
	grad->scale = glm::vec2{ 1.0,1.0 };
	grad->type = vg_pattern_type_t::VG_PATTERN_TYPE_LINEAR;
	return 0;
}
// 自定义分配
vg_pattern_t* ovg_new_pattern_linear(mem_resource_t* ac0, float x0, float y0, float x1, float y1) {
	auto ac = (usp_ac_cx*)ac0;
	if (!ac) {
		return 0;
	}
	pat_act* pat = (pat_act*)ac->new_obj<pat_act>();
	if (!pat) {
		return 0;
	}
	pat->ac = ac;
	pat->type = vg_pattern_type_t::VG_PATTERN_TYPE_LINEAR;
	pat->extend = vg_extend_t::VG_EXTEND_NONE;
	pat->data = &pat->g;
	_vg_pattern_edit_linear(pat, x0, y0, x1, y1);
	pat->matrix = glm::mat3x2(1.0);
	pat->references = 1;
	return pat;
}
int vg_pattern_edit_radial(pat_act* pat, float cx0, float cy0, float radius0, float cx1, float cy1, float radius1, bool is_ellipse) {
	if (!(pat))
		return -2;
	if (pat->type != vg_pattern_type_t::VG_PATTERN_TYPE_RADIAL)
		return -1;
	vg_gradient_t* grad = (vg_gradient_t*)pat->data;
	*grad = {};
	glm::vec2 c0 = { cx0, cy0 };
	glm::vec2 c1 = { cx1, cy1 };
	if (radius0 > radius1 - 1.0f)
		radius0 = radius1 - 1.0f;
	glm::vec2  u = (c0 - c1);
	float l = glm::length(u);
	if (l + radius0 + 1.0f >= radius1) {
		glm::vec2 v = (u / l);
		c0 = (c1 + (v * (radius1 - radius0 - 1.0f)));
	}
	grad->cp[0] = glm::vec4{ {c0.x}, {c0.y}, {radius0}, {0} };
	grad->cp[1] = glm::vec4{ {c1.x}, {c1.y}, {radius1}, {0} };
	grad->m = glm::ivec4(1024, 0, 0, 1024);
	grad->extend = pat->extend;
	grad->scale = glm::vec2{ 1.0,1.0 };
	grad->type = vg_pattern_type_t::VG_PATTERN_TYPE_RADIAL;
	if (is_ellipse)grad->scale.x *= 2;
	return 0;
}
int vg_pattern_edit_sweep(pat_act* pat, float cx, float cy, float start_angle, float end_angle) {
	if (!(pat))
		return -1;
	if (pat->type != vg_pattern_type_t::VG_PATTERN_TYPE_SWEEP)
		return -2;
	vg_gradient_t* grad = (vg_gradient_t*)pat->data;
	*grad = {};
	grad->cp[0] = glm::vec4{ cx, cy, start_angle, end_angle };
	grad->m = glm::ivec4(1024, 0, 0, 1024);
	grad->extend = pat->extend;
	grad->scale = glm::vec2{ 1.0,1.0 };
	grad->type = vg_pattern_type_t::VG_PATTERN_TYPE_SWEEP;
	return 0;
}
vg_pattern_t* ovg_new_pattern_radial(mem_resource_t* ac0, float cx0, float cy0, float radius0, float cx1, float cy1, float radius1, bool is_ellipse) {
	auto ac = (usp_ac_cx*)ac0;
	if (!ac) {
		return 0;
	}
	pat_act* pat = (pat_act*)ac->new_obj<pat_act>();
	if (!pat) {
		return 0;
	}
	pat->ac = ac;
	pat->type = vg_pattern_type_t::VG_PATTERN_TYPE_RADIAL;
	pat->extend = vg_extend_t::VG_EXTEND_NONE;
	pat->data = &pat->g;
	pat->matrix = glm::mat3x2(1.0);
	vg_pattern_edit_radial(pat, cx0, cy0, radius0, cx1, cy1, radius1, is_ellipse);
	pat->references = 1;
	return pat;
}
vg_pattern_t* ovg_new_pattern_sweep(mem_resource_t* ac0, float cx, float cy, float start_angle, float end_angle) {
	auto ac = (usp_ac_cx*)ac0;
	if (!ac) {
		return 0;
	}
	pat_act* pat = (pat_act*)ac->new_obj<pat_act>();
	if (!pat) {
		return 0;
	}
	pat->ac = ac;
	pat->type = vg_pattern_type_t::VG_PATTERN_TYPE_SWEEP;
	pat->extend = vg_extend_t::VG_EXTEND_NONE;
	pat->data = &pat->g;
	pat->matrix = glm::mat3x2(1.0);
	vg_pattern_edit_sweep(pat, cx, cy, start_angle, end_angle);
	pat->references = 1;
	return pat;
}


ovg_path_t* ovg_new_path(mem_resource_t* ac0) {
	auto ac = (usp_ac_cx*)ac0;
	ovg_path_t* p = 0;
	if (ac) {
		p = ac->new_obj<ovg_path_t>();
		ovg_clear_path(p);
		p->ac = ac;
	}
	return p;
}
void ovg_destroy_path(ovg_path_t* path) {
	if (path && path->ac)
		path->ac->free_obj(path);
}

vg_state_save_t* ovg_new_state(mem_resource_t* ac0) {
	auto ac = (usp_ac_cx*)ac0;
	vg_state_save_t* p = 0;
	if (ac) {
		auto pp = (ss_act*)ac->new_mem(sizeof(ss_act));
		pp->ac = ac;
		p = pp;
		*p = {};
		push_constants_t pc = {};
		pc.source.w = 1;
		pc.size = { (float)100, (float)100 };
		pc.fsq_patternType = VG_PATTERN_TYPE_SOLID;
		pc.opacity = 1.0f;
		pc.mat = pc.matInv = glm::mat3x2(1.0);
		p->lineWidth = 1.f;
		p->miterLimit = 10.f;
		p->curOperator = vg_operator_t::VG_OPERATOR_OVER;
		p->curFillRule = VG_FILL_RULE_NON_ZERO;
		p->pushConsts = pc;
	}
	return p;
}
void ovg_state_destroy(vg_state_save_t* p) {
	auto p0 = (ss_act*)p;
	if (!p0 || !p0->ac)return;
	if (p0->dashes) {
		p0->ac->free_mem(p0->dashes, p0->dashCount);
	}
	p0->ac->free_mem(p, sizeof(ss_act));
}
// 渲染对象
#if 1


struct geom2d_cmd_c
{
	glm::ivec4 clip_rect = {};
	void* texid = 0;
	uint32_t vtxOffset = 0;
	uint32_t idxOffset = 0;
	uint32_t elemCount = 0;
	uint32_t vCount = 0;
	uint16_t blend_mode = 0;		// 混合模式	 
};

struct dash_context_t {
	bool     dashOn;
	uint32_t curDash;       // current dash index
	float    curDashOffset; // cur dash offset between defined path point and last dash segment(on/off) start
	float    totDashLength; // total length of dashes
	glm::vec2 normal;
};

struct stroke_context_t {
	uint32_t iL;
	uint32_t iR;
	uint32_t cp; // current point

	uint32_t firstIdx; // save first point idx for closed path
	float               hw;       // stroke half width, computed once.
	float               lhMax;    // miter limit * line width
	float arcStep; // cached arcStep, prevent compute multiple times for same stroke, 0 if not yet computed
};
struct rvg_cx;

class mesh2d_x
{
public:
	struct vertex_t
	{
		glm::vec2 position = {};		// 坐标	
		glm::vec2 tex_coord = {};		// 纹理uv
		uint32_t color = 0xffffffff;	// 顶点颜色 
	};

	std::vector<geom2d_cmd_c> cmd_data;	// 渲染命令
	std::vector<vertex_t> vtxs;		// 顶点数据
	std::vector<int> idxs;				// 索引
	glm::ivec4 viewport = { 0,0,0,0 };
	glm::ivec4 _clip_rect = { };// 当前裁剪 
public:
	mesh2d_x();
	virtual ~mesh2d_x();
	void set_viewport(const glm::ivec4& vp);
	void set_clip(const glm::ivec4& rc);
	// 清除数据,保留viewport
	void clear_m2d();
	bool nohas_clip(const glm::ivec4&);
	// 添加相同纹理/裁剪区域则自动合批
	void add(void* user_image, std::vector<vertex_t>& vertex, std::vector<int>& vt_index, const glm::ivec4& clip);
	void add(void* user_image, vertex_t* vertex, size_t vcount, int* vt_index, size_t icount, const glm::ivec4& clip);
	// 添加图片渲染，自动生成顶点数据
	void add_image0(void* img, const glm::ivec2& texsize, const glm::ivec4& clip, const glm::ivec4& dst, const glm::ivec4& src, const glm::ivec4& sliced, uint32_t color = 0xffffffff);
	// 添加九宫格图片渲染
	void add_image_sliced(void* user_image, const glm::ivec2& texsize, const glm::ivec4& a, const glm::ivec4& sliced, const glm::ivec4& rect, uint32_t col, const glm::ivec4& clip);
	// 添加旋转图片渲染，angle为旋转角度，center为旋转中心坐标（相对于dst）
	void add_image_angle(void* img, const glm::ivec2& texsize, const glm::ivec4& src, const glm::ivec4& dst, float angle, const glm::vec2* center, uint32_t col, const glm::ivec4& clip, int flip);
private:

};

class geom_primitive :public mesh2d_x {
public:
	struct Vertex1 {
		glm::vec3 pos;
		glm::vec2 uv;
		uint32_t color;
	};
	struct Vertex2 {
		glm::vec3 pos;
		glm::vec2 uv;
		uint32_t color;
		uint32_t color1;
	};
public:
	std::pmr::vector<Vertex1> vd1;	// 单面顶点
	std::pmr::vector<Vertex2> vd2;	// 双面顶点
	std::pmr::vector<uint32_t> ids;	// 索引 
	std::pmr::vector<glm::mat4> instance_mat;	// 实例矩阵 
	glm::mat4 mat = glm::mat4(1.0f);// 当前矩阵
	gem_info_t curState = {};		// 当前状态	 
	std::pmr::vector<gcmd_t>* gt = 0;
	std::pmr::vector<gem_info_t> build_pipes;
	rvg_cx* dc = 0;
	size_t inst_count = 0;			// 当前实例
	size_t inst_idx = 0;			// 当前实例
public:
	geom_primitive();
	~geom_primitive();
public:
	// 清空数据
	void clear();
	void set_state(gem_info_t* info, const glm::mat4* matrix);
	size_t set_instance_mat(const glm::mat4* matrix, size_t count);
	// 添加几何数据到缓冲区，xy顶点坐标，color顶点颜色，uv顶点纹理坐标，indices索引数据，color_type=0表示float4，1表示uint32_t
	bool add_geometry(void* texture, const float* xy, int xy_stride, const void* color, int color_stride
		, const float* uv, int uv_stride, int num_vertices, const void* indices, int num_indices, int size_indices, int color_type);
	// 添加3D几何数据到缓冲区，xyz顶点坐标，color顶点颜色（双面则要双倍），uv顶点纹理坐标，indices索引数据
	bool add_geometry3d(void* texture, const float* xyz, int xyz_stride, const void* color, int color_stride
		, const float* uv, int uv_stride, int num_vertices, const void* indices, int num_indices, int size_indices, int color_type);

	void add_image(ovg_image_r* r);
};

struct rvg_cx :public rvg_t {
	struct Vertex {
		glm::vec2	pos;
		glm::vec2	uv;
		uint32_t	color;
	};
	struct ear_clip_point {
		glm::vec2 pos;
		uint32_t idx;
		struct ear_clip_point* next;
	};
	usp_ac_cx* ac = 0;
	mbpool_t mac;
	std::pmr::vector<gcmd_t> cmdlist;		// 命令列表
	std::pmr::vector<Vertex> _vertex;		// 矢量顶点
	std::pmr::vector<uint32_t> _indices;	// 矢量索引
	// 临时缓冲用
	std::pmr::vector<ear_clip_point> ecpsd;
	std::pmr::vector<glm::vec2> _normals;

	std::pmr::map<vg_image_t*, vg_image_desc_t> _images;
	std::pmr::vector<vg_image_desc_t*> _images_up;

	// 23d
	geom_primitive gps = {};

	size_t gCount = 0;	// ubo数量
	size_t _curVertOffset = 0;
	uint32_t curColor = 0;
	glm::ivec4 curClip = {};
	std::stack<vg_state_save_t*> _cst;	// 保存栈 
public:
	rvg_cx();
	~rvg_cx();
	void clear_all();
	void set_path(ovg_path_t* path, vg_state_save_t* st);
	void stroke_preserve();
	void fill_preserve();
	void clip_preserve();
	void fill();
	void paint();
	void clip();
	void clip0(uint8_t ref);
	void clip(const glm::ivec4* rc);

	void save();
	void restore();

	void image_update(vg_image_t* img, vg_image_desc_t* desc);
	void image_destroy(vg_image_t* img);
public:
	void fill_non_zero_tess2(ovg_path_t* p);
	void fill_non_zero(ovg_path_t* p);

	bool _build_vb_step(ovg_path_t* ctx, stroke_context_t* str, bool isCurve);
	void _draw_stoke_cap(ovg_path_t* ctx, stroke_context_t* str, glm::vec2 p0, glm::vec2 n, bool isStart);
	float _draw_dashed_segment(ovg_path_t* ctx, stroke_context_t* str, dash_context_t* dc, bool isCurve);
	void _draw_segment(ovg_path_t* ctx, stroke_context_t* str, dash_context_t* dc, bool isCurve);

	void _add_triangle_indices(ovg_path_t* ctx, uint32_t i0, uint32_t i1, uint32_t i2);
	void _add_tri_indices_for_rect(uint32_t i);
	void _add_vertexf(ovg_path_t* ctx, float x, float y);
	// 复制状态，自动释放
	void cp_cmdt(vgcmd_t* c, vg_state_save_t* t);

	vg_state_save_t* new_ss(vg_state_save_t* src);
	vg_state_save_t* new_state();
	void free_state(vg_state_save_t* p);
	void swap_state(vg_state_save_t* p, vg_state_save_t* p1);
};

rvg_cx::rvg_cx()
{
	gps.gt = &cmdlist;
	gps.dc = this;
}

rvg_cx::~rvg_cx()
{}
void rvg_cx::clear_all()
{
	gps.clear();
	ovg_clear_path(path);
	_curVertOffset = 0;
	gCount = 0;
	mac.release();
	_vertex.clear();
	_indices.clear();
	cmdlist.clear();
	while (_cst.size())
	{
		auto c = _cst.top();
		free_state(c);
		_cst.pop();
	}
	free_state(st);
	st = new_state();
	for (auto& [k, v] : _images) {
		if (v.is_copy && v.pixels) {
			ac->free_mem0(v.pixels, v.px_size);
		}
	}
	_images.clear();
}
void rvg_cx::set_path(ovg_path_t* path0, vg_state_save_t* st0)
{
	path = path0;
	path->t = st0;
	st = st0;
}
void rvg_cx::stroke_preserve()
{
	o_finish_path(path);
	if (!path || !path->pathPtr || !st)
		return;
	auto p = path;
	p->t = st;
	if (p->t->pattern)
		gCount++;
	auto ctx = p;
	vgcmd_t c = {};
	c.vertex.x = _vertex.size();
	c.index.x = _indices.size();
	c.type = 1;
	cp_cmdt(&c, st);
	ctx->curVertOffset = c.vertex.x;
	stroke_context_t str = { 0 };
	str.hw = p->t->lineWidth * 0.5f;
	str.lhMax = p->t->miterLimit * p->t->lineWidth;
	uint32_t ptrPath = 0;
	curColor = p->color = p->t->color;

	while (ptrPath < ctx->pathPtr) {
		uint32_t ptrSegment = 0, lastSegmentPointIdx = 0;
		uint32_t firstPathPointIdx = str.cp;
		uint32_t pathPointCount = ctx->pathes[ptrPath] & PATH_ELT_MASK;
		uint32_t lastPathPointIdx = str.cp + pathPointCount - 1;

		dash_context_t dc = { 0 };

		if (o_path_has_curves(ctx->pathes.data(), ptrPath)) {
			ptrSegment = 1;
			lastSegmentPointIdx = str.cp + (ctx->pathes[ptrPath + ptrSegment] & PATH_ELT_MASK) - 1;
		}

		str.firstIdx = (uint32_t)_vertex.size() - ctx->curVertOffset;

		if (p->t->dashCount > 0) {
			// init dash stroke
			dc.dashOn = true;
			dc.curDash = 0; // current dash index
			dc.totDashLength = 0; // limit offset to total length of dashes
			for (uint32_t i = 0; i < p->t->dashCount; i++)
				dc.totDashLength += p->t->dashes[i];
			if (dc.totDashLength == 0) {
				break;
				//return;
			}
			dc.curDashOffset = fmodf(fmodf(p->t->dashOffset, dc.totDashLength) + dc.totDashLength, dc.totDashLength);
			// cur dash offset between defined path point and last dash segment(on/off) start
			str.iL = lastPathPointIdx;
		}
		else if (_path_is_closed(ctx, ptrPath)) {
			str.iL = lastPathPointIdx;
		}
		else {
			_draw_stoke_cap(ctx, &str, ctx->points[str.cp],
				vec2_line_norm(ctx->points[str.cp], ctx->points[str.cp + 1]), true);
			str.iL = str.cp++;
		}

		if (o_path_has_curves(ctx->pathes.data(), ptrPath)) {
			while (str.cp < lastPathPointIdx) {

				bool curved = ctx->pathes[ptrPath + ptrSegment] & PATH_HAS_CURVES_BIT;
				if (lastSegmentPointIdx == lastPathPointIdx) // last segment of path, dont draw end point here
					lastSegmentPointIdx--;
				while (str.cp <= lastSegmentPointIdx)
					_draw_segment(ctx, &str, &dc, curved);

				ptrSegment++;
				uint32_t cptSegPts = ctx->pathes[ptrPath + ptrSegment] & PATH_ELT_MASK;
				lastSegmentPointIdx = str.cp + cptSegPts - 1;
				if (lastSegmentPointIdx == lastPathPointIdx && cptSegPts == 1) {
					// single point last segment
					ptrSegment++;
					break;
				}
			}
		}
		else
			while (str.cp < lastPathPointIdx)
				_draw_segment(ctx, &str, &dc, false);

		if (p->t->dashCount > 0) {
			if (_path_is_closed(ctx, ptrPath)) {
				str.iR = firstPathPointIdx;

				_draw_dashed_segment(ctx, &str, &dc, false);

				str.iL++;
				str.cp++;
			}
			//else {
			//	// 强制结束当前 dash
			//	dc.dashOn = false;
			//	dc.curDashOffset = 0.0f;
			//}
			if (!dc.dashOn) {
				// finishing last dash that is already started, draw end caps but not too close to start
				// the default gap is the next void
				int32_t prevDash = (int32_t)dc.curDash - 1;
				if (prevDash < 0)
					dc.curDash = p->t->dashCount - 1;
				float m = fminf(p->t->dashes[prevDash] - dc.curDashOffset, p->t->dashes[dc.curDash]);
				glm::vec2  p2 = vec2_sub(ctx->points[str.iR], vec2_mult_s(dc.normal, m));
				_draw_stoke_cap(ctx, &str, p2, dc.normal, false);
			}
		}
		else if (_path_is_closed(ctx, ptrPath)) {
			str.iR = firstPathPointIdx;
			bool inverse = _build_vb_step(ctx, &str, false);
			uint32_t* inds = &_indices[_indices.size() - 6];
			uint32_t  ii = str.firstIdx;
			if (inverse) {
				inds[1] = ii + 1;
				inds[4] = ii + 1;
				inds[5] = ii;
			}
			else {
				inds[1] = ii;
				inds[4] = ii;
				inds[5] = ii + 1;
			}
			str.cp++;
		}
		else
			_draw_stoke_cap(ctx, &str, ctx->points[str.cp],
				vec2_line_norm(ctx->points[str.cp - 1], ctx->points[str.cp]), false);

		str.cp = firstPathPointIdx + pathPointCount;

		if (ptrSegment > 0)
			ptrPath += ptrSegment;
		else
			ptrPath++;

	}
	c.vertex.y = _vertex.size() - c.vertex.x;
	c.index.y = _indices.size() - c.index.x;
	cmdlist.push_back({ .vg = c });

}

void rvg_cx::fill_preserve()
{
	o_finish_path(path);
	if (!path || !path->pathPtr || !st)
		return;
	auto p = path;
	p->t = st;
	if (p->t->pattern)
		gCount++;
	auto t = p->t;
	uint32_t color = t->color;
	p->color = color;
	vgcmd_t c = {};
	c.vertex.x = _vertex.size();
	c.index.x = _indices.size();
	c.type = 0;
	cp_cmdt(&c, st);
	p->curVertOffset = c.vertex.x;
	fill_non_zero(p);
	c.vertex.y = _vertex.size() - c.vertex.x;
	c.index.y = _indices.size() - c.index.x;
	cmdlist.push_back({ .vg = c });
}

void rvg_cx::clip_preserve()
{
	o_finish_path(path);
	if (!path || !path->pathPtr || !st)
		return;
	path->t = st;
	auto p = path;
	auto t = st;
	vgcmd_t c = {};
	c.type = 2;
	{
		c.vertex.x = _vertex.size();
		c.index.x = _indices.size();
		cp_cmdt(&c, t);
		p->curVertOffset = c.vertex.x;
		fill_non_zero(p);
		c.vertex.y = _vertex.size() - c.vertex.x;
		c.index.y = _indices.size() - c.index.x;
	}
	c.full_screen_quad = _vertex.size();
	cmdlist.push_back({ .vg = c });
	Vertex v = {};
	v.pos = { -1,-1 };
	v.color = t->color;
	_vertex.push_back(v);
	v.pos = { 3,-1 };
	_vertex.push_back(v);
	v.pos = { -1,3 };
	_vertex.push_back(v);
}
void rvg_cx::clip0(uint8_t ref)
{
	vgcmd_t c = {};
	c.type = 2;
	c.ref = ref;
	c.full_screen_quad = _vertex.size();
	Vertex v = {};
	v.pos = { -1,-1 };
	v.color = st->color;
	_vertex.push_back(v);
	v.pos = { 3,-1 };
	_vertex.push_back(v);
	v.pos = { -1,3 };
	_vertex.push_back(v);
	cmdlist.push_back({ .vg = c });
}

void rvg_cx::clip()
{
	clip_preserve();
	ovg_clear_path(path);
}
void rvg_cx::clip(const glm::ivec4* rc)
{
	if (rc)
	{
		auto ct = _cst.size() ? _cst.top() : st;
		curClip = *rc;
		if (ct)
		{
			glm::vec3 ps = { curClip.x ,curClip.y,1.0 };
			auto nps = ct->pushConsts.mat * ps;
			curClip.x = nps.x; curClip.y = nps.y;
		}
		vgcmd_t c = {};
		c.type = 2;
		c.bounds = curClip;
		cmdlist.push_back({ .vg = c });
	}
}


vg_state_save_t* rvg_cx::new_ss(vg_state_save_t* src)
{
	vg_state_save_t* dst = new_state();
	if (!dst)return dst;
	if (src)
	{
		*dst = *src;
		if (src->dashes && src->dashCount > 0) {
			ac->free_mem(dst->dashes, src->dashCount);
			dst->dashes = (float*)ac->allocate(sizeof(float) * src->dashCount);
			if (dst->dashes)
				memcpy(dst->dashes, src->dashes, sizeof(float) * src->dashCount);
			else
				dst->dashCount = 0;
		}
	}
	else
		*dst = {};
	return dst;
}
vg_state_save_t* rvg_cx::new_state()
{
	auto t = (vg_state_save_t*)ac->allocate(sizeof(vg_state_save_t));
	*t = {};
	t->color = -1;
	//VkRect2D b = {};
	//b.extent = { ctx->pSurf->width, ctx->pSurf->height };
	push_constants_t pc = {};
	pc.source.w = 1;
	pc.size = { (float)100, (float)100 };
	pc.fsq_patternType = VG_PATTERN_TYPE_SOLID;
	pc.opacity = 1.0f;
	pc.mat = pc.matInv = glm::mat3x2(1.0);

	t->lineWidth = 1.f;
	t->miterLimit = 10.f;
	t->curOperator = vg_operator_t::VG_OPERATOR_OVER;
	t->curFillRule = VG_FILL_RULE_NON_ZERO;
	//t->bounds = b;
	t->pushConsts = pc;
	return t;
}
void rvg_cx::free_state(vg_state_save_t* p)
{
	if (p) {
		if (p->dashes && p->dashCount > 0)
			ac->free_mem(p->dashes, p->dashCount);
		ac->free_mem(p, 1);
	}
}
void rvg_cx::swap_state(vg_state_save_t* p, vg_state_save_t* p1)
{
	std::swap(*p, *p1);
}
void rvg_cx::save()
{
	auto ss = new_ss(st);
	_cst.push(ss);
}
void rvg_cx::restore()
{
	if (_cst.empty())return;
	auto c = _cst.top();
	swap_state(st, c);
	free_state(c);
	_cst.pop();
}

void rvg_cx::fill()
{
	fill_preserve();
	ovg_clear_path(path);
}

void rvg_cx::paint()
{
	auto ph = path;
	o_finish_path(ph);
	if (path && ph->pathPtr && st) {
		fill();
		return;
	}
	vgcmd_t c = {};
	c.type = 3;
	c.full_screen_quad = _vertex.size();

	Vertex v = {};
	if (st) {
		if (st->pattern)
			gCount++;
		cp_cmdt(&c, st);
	}
	v.pos = { -1,-1 };
	v.color = st ? st->color : -1;
	_vertex.push_back(v);
	v.pos = { 3,-1 };
	_vertex.push_back(v);
	v.pos = { -1,3 };
	_vertex.push_back(v);
	cmdlist.push_back({ .vg = c });
}

#define COLOR_R(c) ((c) & 0xFF)
#define COLOR_G(c) (((c) >> 8) & 0xFF)
#define COLOR_B(c) (((c) >> 16) & 0xFF)
#define COLOR_A(c) (((c) >> 24) & 0xFF)

#define MAKE_RGBA(r,g,b,a) \
    ((uint32_t)((a) << 24) | ((b) << 16) | ((g) << 8) | (r))
static inline uint8_t mul_unorm8(uint8_t a, uint8_t b)
{
	return (uint8_t)((a * b + 127) / 255);
}
// ---------------------------------------------------------------------------
// 工具函数：计算有符号面积，判断绕向
//   area > 0 : CCW (逆时针, 外轮廓)
//   area < 0 : CW  (顺时针, hole)
// ---------------------------------------------------------------------------
static float path_signed_area(const glm::vec2* pts, int n) {
	float area = 0.0f;
	for (int i = 0; i < n; ++i) {
		const glm::vec2& p1 = pts[i];
		const glm::vec2& p2 = pts[(i + 1) % n];
		area += (p2.x - p1.x) * (p2.y + p1.y);
	}
	return area * 0.5f; // CCW > 0
}

// ---------------------------------------------------------------------------
// 用 tess2 做 non-zero fill 的主函数
// 替换原来的 fill_non_zero
// ---------------------------------------------------------------------------

#ifdef VG_FILL_NZ_GLUTESS2

void rvg_cx::fill_non_zero_tess2(ovg_path_t* ctx)
{
	Vertex v{};
	v.color = ctx->color;
	v.uv = { 0, 0 };
	_curVertOffset = ctx->curVertOffset;
	/* ---------- 凸路径快速路径 ---------- */
	if (ctx->pathPtr == 1 && (ctx->pathes[0] & PATH_IS_CONVEX_BIT)) {
		uint32_t firstVertIdx = (uint32_t)(_vertex.size() - ctx->curVertOffset);
		uint32_t pathPointCount = ctx->pathes[0] & PATH_ELT_MASK;

		uint32_t i = 0;
		while (i < 2) {
			v.pos = ctx->points[i++];
			_vertex.push_back(v);
		}
		while (i < pathPointCount) {
			v.pos = ctx->points[i];
			_vertex.push_back(v);
			uint32_t inds[3] = {
				firstVertIdx,
				firstVertIdx + i - 1,
				firstVertIdx + i
			};
			_indices.push_back(inds[0]);
			_indices.push_back(inds[1]);
			_indices.push_back(inds[2]);
			++i;
		}
		return;
	}
	TESStesselator* tess = tessNewTess(nullptr);
	if (!tess) return;
	uint32_t ptrPath = 0;
	uint32_t firstPtIdx = 0;
	while (ptrPath < ctx->pathPtr) {
		uint32_t pathPointCount = ctx->pathes[ptrPath] & PATH_ELT_MASK;
		if (pathPointCount > 2) {
			std::vector<TESSreal> dpts(pathPointCount * 2);
			for (uint32_t i = 0; i < pathPointCount; ++i) {
				const auto& p = ctx->points[firstPtIdx + i];
				dpts[i * 2 + 0] = p.x;
				dpts[i * 2 + 1] = p.y;
			}
			// 显式闭合（关键）
			if (pathPointCount < 3 ||
				(dpts[0] != dpts[(pathPointCount - 1) * 2 + 0] ||
					dpts[1] != dpts[(pathPointCount - 1) * 2 + 1])) {
				dpts.push_back(dpts[0]);
				dpts.push_back(dpts[1]);
			}
			tessAddContour(tess, 2, dpts.data(), sizeof(TESSreal) * 2, (int)dpts.size() / 2);
		}
		firstPtIdx += pathPointCount;
		if (o_path_has_curves(ctx->pathes.data(), ptrPath)) {
			ptrPath++;
			uint32_t totPts = 0;
			while (totPts < pathPointCount)
				totPts += (ctx->pathes[ptrPath++] & PATH_ELT_MASK);
		}
		else {
			ptrPath++;
		}
	}
	tessTesselate(tess, ctx->t->curFillRule ? TESS_WINDING_NONZERO : TESS_WINDING_ODD, TESS_POLYGONS, 3, 2, nullptr);
	uint32_t base = (uint32_t)_vertex.size() - ctx->curVertOffset;
	const TESSreal* verts = tessGetVertices(tess);
	const TESSindex* elems = tessGetElements(tess);
	int nverts = tessGetVertexCount(tess);
	int nelems = tessGetElementCount(tess);
	for (int i = 0; i < nverts; ++i) {
		v.pos = { (float)verts[i * 2], (float)verts[i * 2 + 1] };
		_vertex.push_back(v);
	}
	for (int i = 0; i < nelems; ++i) {
		TESSindex a = elems[i * 3 + 0];
		TESSindex b = elems[i * 3 + 1];
		TESSindex c = elems[i * 3 + 2];
		if (a == TESS_UNDEF || b == TESS_UNDEF || c == TESS_UNDEF) continue;
		_indices.push_back(base + a);
		_indices.push_back(base + b);
		_indices.push_back(base + c);
	}
	tessDeleteTess(tess);
}
#endif

inline float ecp_zcross(rvg_cx::ear_clip_point* p0, rvg_cx::ear_clip_point* p1, rvg_cx::ear_clip_point* p2) {
	return vec2_zcross(vec2_sub(p1->pos, p0->pos), vec2_sub(p2->pos, p0->pos));
}

bool ptInTriangle(const glm::vec2& p, const glm::vec2& p0, const glm::vec2& p1, const glm::vec2& p2) {
	float dX = p.x - p2.x;
	float dY = p.y - p2.y;
	float dX21 = p2.x - p1.x;
	float dY12 = p1.y - p2.y;
	float D = dY12 * (p0.x - p2.x) + dX21 * (p0.y - p2.y);
	float s = dY12 * dX + dX21 * dY;
	float t = (p2.y - p0.y) * dX + (p0.x - p2.x) * dY;
	if (D < 0)
		return (s <= 0) && (t <= 0) && (s + t >= D);
	return (s >= 0) && (t >= 0) && (s + t <= D);
}

void rvg_cx::fill_non_zero(ovg_path_t* p)
{
	auto t = p->t;
	uint32_t color = t->color;
	p->color = color;
#ifdef VG_FILL_NZ_GLUTESS2
	fill_non_zero_tess2(p);
	return;
#endif
	uint32_t ptrPath = 0;
	uint32_t firstPtIdx = 0;
	const glm::vec3 uv = { 0,0,-1 };
	Vertex v = {}; v.color = color; v.uv = { 0, 0 };
	uint32_t cur_idx = _vertex.size() - p->curVertOffset;
	auto pcolor = p->colors.data();
	auto pcn = p->colors.size();
	if (p->colors.empty())pcolor = 0;
	while (ptrPath < p->pathPtr) {
		uint32_t pathPointCount = p->pathes[ptrPath] & PATH_ELT_MASK;
		auto col = pcolor && ptrPath < pcn ? pcolor[ptrPath] : color;
		v.color = col;
		if (pathPointCount > 2) {
			uint32_t firstVertIdx = (uint32_t)cur_idx;
			ecpsd.resize(pathPointCount);
			auto ecps = ecpsd.data();
			if (!ecps)break;
			uint32_t            ecps_count = pathPointCount;
			uint32_t i = 0;
			auto points = p->points.data() + firstPtIdx;
			while (i < pathPointCount - 1) {
				v.pos = points[i];
				ear_clip_point ecp = { v.pos, firstVertIdx + i, &ecps[i + 1] };
				ecps[i] = ecp;
				_vertex.push_back(v);
				i++;
			}
			v.pos = points[i];
			ear_clip_point ecp = { v.pos, firstVertIdx + i, ecps };
			ecps[i] = ecp;
			_vertex.push_back(v);
			ear_clip_point* ecp_current = ecps;
			uint32_t        tries = 0;
			while (ecps_count > 3) {
				if (tries > ecps_count) { break; }
				ear_clip_point* v0 = ecp_current->next, * v1 = ecp_current, * v2 = ecp_current->next->next;
				if (ecp_zcross(v0, v2, v1) < 0) {
					ecp_current = ecp_current->next;
					tries++;
					continue;
				}
				ear_clip_point* vP = v2->next;
				bool            isEar = true;
				while (vP != v1) {
					if (ptInTriangle(vP->pos, v0->pos, v2->pos, v1->pos)) {
						isEar = false;
						break;
					}
					vP = vP->next;
				}
				if (isEar) {
					uint32_t t3[3] = { v0->idx, v1->idx, v2->idx };
					_indices.insert(_indices.end(), t3, t3 + 3);
					v1->next = v2;
					ecps_count--;
					tries = 0;
				}
				else {
					ecp_current = ecp_current->next;
					tries++;
				}
			}
			if (ecps_count == 3)
			{
				uint32_t t3[3] = { ecp_current->next->idx, ecp_current->idx, ecp_current->next->next->idx };
				_indices.insert(_indices.end(), t3, t3 + 3);
			}
			cur_idx += pathPointCount;
		}
		firstPtIdx += pathPointCount;
		if (o_path_has_curves(p->pathes.data(), ptrPath)) {
			// skip segments lengths used in stroke
			ptrPath++;
			uint32_t totPts = 0;
			while (totPts < pathPointCount)
				totPts += (p->pathes[ptrPath++] & PATH_ELT_MASK);
		}
		else
			ptrPath++;
	}
}



void rvg_cx::_add_triangle_indices(ovg_path_t* ctx, uint32_t i0, uint32_t i1, uint32_t i2) {
	_indices.push_back(i0);
	_indices.push_back(i1);
	_indices.push_back(i2);
}
void rvg_cx::_add_tri_indices_for_rect(uint32_t i) {
	_indices.resize(_indices.size() + 6);
	uint32_t* inds = _indices.data() + _indices.size() - 6;
	inds[0] = i;
	inds[1] = i + 2;
	inds[2] = i + 1;
	inds[3] = i + 1;
	inds[4] = i + 2;
	inds[5] = i + 3;
}
void rvg_cx::_add_vertexf(ovg_path_t* ctx, float x, float y) {
	Vertex v = {};
	v.pos = { x,y };
	v.color = ctx->color;
	_vertex.push_back(v);
}
void rvg_cx::cp_cmdt(vgcmd_t* c, vg_state_save_t* t)
{
	c->state = (vg_state_save_t*)mac.allocate(sizeof(vg_state_save_t) * 1);
	if (!c->state)return;
	*c->state = *t;
	if (t->dashes && t->dashCount > 0) {
		c->state->dashes = (float*)mac.allocate(sizeof(float) * t->dashCount);
		if (c->state->dashes)
			memcpy(c->state->dashes, t->dashes, sizeof(float) * t->dashCount);
		else
			c->state->dashCount = 0;
	}
	if (t->pattern) {
		auto pat = (pat_act*)mac.allocate(sizeof(pat_act));
		memcpy(pat, t->pattern, sizeof(pat_act));
		pat->data = &pat->g;
		c->state->pattern = pat;
	}
}
bool rvg_cx::_build_vb_step(ovg_path_t* ctx, stroke_context_t* str, bool isCurve) {
	Vertex v = {};
	v.color = ctx->color; v.uv = { };
	glm::vec2   p0 = ctx->points[str->cp];
	glm::vec2   v0 = p0 - ctx->points[str->iL];
	glm::vec2   v1 = ctx->points[str->iR] - p0;
	float  length_v0 = glm::length(v0);
	float  length_v1 = glm::length(v1);
	if (length_v0 < FLT_EPSILON || length_v1 < FLT_EPSILON) {
		return false;
	}
	glm::vec2  v0n = (v0 / length_v0);
	glm::vec2  v1n = (v1 / length_v1);
	float dot = glm::dot(v0n, v1n);
	float det = v0n.x * v1n.y - v0n.y * v1n.x;
	if (EQUF(dot, 1.0f)) { // colinear

		return false;
	}
	if (EQUF(dot, -1.0f)) { // cusp (could draw line butt?)
		glm::vec2 vPerp = (vec2_perp(v0n) * str->hw);
		uint32_t idx = (uint32_t)(_vertex.size() - ctx->curVertOffset);
		v.pos = (p0 + vPerp);
		_vertex.push_back(v);
		v.pos = (p0 - vPerp);
		_vertex.push_back(v);
		_add_triangle_indices(ctx, idx, idx + 1, idx + 2);
		_add_triangle_indices(ctx, idx, idx + 2, idx + 3);
		return true;
	}
	glm::vec2  bisec_n = glm::normalize(v0n + v1n); // bisec/bisec_perp are inverted names
	float alpha = acosf(dot);

	if (det < 0)
		alpha = -alpha;
	float halfAlpha = alpha / 2.f;
	float cosHalfAlpha = cosf(halfAlpha);
	float lh = str->hw / cosHalfAlpha;
	glm::vec2  bisec_n_perp = vec2_perp(bisec_n);
	// limit bisectrice length
	float rlh = lh; // rlh is for inside pos tweeks
	if (dot < 0.f)
		rlh = fminf(rlh, fminf(length_v0, length_v1));
	//---

	glm::vec2 bisec = (bisec_n_perp * rlh);

	uint32_t idx = (uint32_t)(_vertex.size() - ctx->curVertOffset);

	glm::vec2 rlh_inside_pos, rlh_outside_pos;
	if (rlh < lh) {
		glm::vec2 vnPerp;
		if (length_v0 < length_v1)
			vnPerp = vec2_perp(v1n);
		else
			vnPerp = vec2_perp(v0n);
		glm::vec2 vHwPerp = (vnPerp * str->hw);

		double lbc = cosHalfAlpha * rlh;
		if (det < 0.f) {
			rlh_inside_pos = ((vnPerp * glm::vec2(-lbc) + (p0 + bisec)) + vHwPerp);
			rlh_outside_pos = (p0 - (bisec_n_perp * lh));
		}
		else {
			rlh_inside_pos = vec2_sub(vec2_add(vec2_mult_s(vnPerp, lbc), vec2_sub(p0, bisec)), vHwPerp);
			rlh_outside_pos = vec2_add(p0, vec2_mult_s(bisec_n_perp, lh));
		}
	}
	else {
		if (det < 0.0) {
			rlh_inside_pos = vec2_add(p0, bisec);
			rlh_outside_pos = vec2_sub(p0, bisec);
		}
		else {
			rlh_inside_pos = vec2_sub(p0, bisec);
			rlh_outside_pos = vec2_add(p0, bisec);
		}
	}

	auto join = (vg_line_join_t)ctx->t->lineJoin;

	if (isCurve) {
		if (dot < 0.8f)
			join = VG_LINE_JOIN_ROUND;
		else
			join = VG_LINE_JOIN_MITER;
	}

	if (join == VG_LINE_JOIN_MITER) {
		if (lh > str->lhMax) { // miter limit
			double x = (lh - str->lhMax) * cosHalfAlpha;
			glm::vec2   bisecPerp = vec2_mult_s(bisec_n, x);
			bisec = vec2_mult_s(bisec_n_perp, str->lhMax);
			if (det < 0) {
				v.pos = rlh_inside_pos;
				_vertex.push_back(v);

				glm::vec2 p = vec2_sub(p0, bisec);

				v.pos = vec2_sub(p, bisecPerp);
				_vertex.push_back(v);
				v.pos = vec2_add(p, bisecPerp);
				_vertex.push_back(v);

				_add_triangle_indices(ctx, idx, idx + 2, idx + 1);
				_add_triangle_indices(ctx, idx + 2, idx + 4, idx);
				_add_triangle_indices(ctx, idx, idx + 3, idx + 4);
				return true;
			}
			else {
				glm::vec2 p = vec2_add(p0, bisec);
				v.pos = vec2_sub(p, bisecPerp);
				_vertex.push_back(v);

				v.pos = rlh_inside_pos;
				_vertex.push_back(v);

				v.pos = vec2_add(p, bisecPerp);
				_vertex.push_back(v);

				_add_triangle_indices(ctx, idx, idx + 2, idx + 1);
				_add_triangle_indices(ctx, idx + 2, idx + 3, idx + 1);
				_add_triangle_indices(ctx, idx + 1, idx + 3, idx + 4);
				return false;
			}

		}
		else { // normal miter
			if (det < 0) {
				v.pos = rlh_inside_pos;
				_vertex.push_back(v);
				v.pos = rlh_outside_pos;
				_vertex.push_back(v);
			}
			else {
				v.pos = rlh_outside_pos;
				_vertex.push_back(v);
				v.pos = rlh_inside_pos;
				_vertex.push_back(v);
			}

			_add_tri_indices_for_rect(idx);
			return false;
		}
	}
	else {
		glm::vec2 vp = vec2_perp(v0n);

		if (det < 0) {
			if (dot < 0 && rlh < lh)
				v.pos = rlh_inside_pos;
			else
				v.pos = vec2_add(p0, bisec);
			_vertex.push_back(v);
			v.pos = vec2_sub(p0, vec2_mult_s(vp, str->hw));
		}
		else {
			v.pos = vec2_add(p0, vec2_mult_s(vp, str->hw));
			_vertex.push_back(v);
			if (dot < 0 && rlh < lh)
				v.pos = rlh_inside_pos;
			else
				v.pos = vec2_sub(p0, bisec);
		}
		_vertex.push_back(v);

		if (join == VG_LINE_JOIN_BEVEL) {
			if (det < 0) {
				_add_triangle_indices(ctx, idx, idx + 2, idx + 1);
				_add_triangle_indices(ctx, idx + 2, idx + 4, idx + 0);
				_add_triangle_indices(ctx, idx, idx + 3, idx + 4);
			}
			else {
				_add_triangle_indices(ctx, idx, idx + 2, idx + 1);
				_add_triangle_indices(ctx, idx + 2, idx + 3, idx + 1);
				_add_triangle_indices(ctx, idx + 1, idx + 3, idx + 4);
			}
		}
		else if (join == VG_LINE_JOIN_ROUND) {
			if (!str->arcStep)
				str->arcStep = _get_arc_step(ctx, str->hw);
			float a = acosf(vp.x);
			if (vp.y < 0)
				a = -a;

			if (det < 0) {
				a += M_PI;
				float a1 = a + alpha;
				a -= str->arcStep;
				while (a > a1) {
					_add_vertexf(ctx, cosf(a) * str->hw + p0.x, sinf(a) * str->hw + p0.y);
					a -= str->arcStep;
				}
			}
			else {
				float a1 = a + alpha;
				a += str->arcStep;
				while (a < a1) {
					_add_vertexf(ctx, cosf(a) * str->hw + p0.x, sinf(a) * str->hw + p0.y);
					a += str->arcStep;
				}
			}
			uint32_t p0Idx = (uint32_t)(_vertex.size() - ctx->curVertOffset);
			_add_triangle_indices(ctx, idx, idx + 2, idx + 1);
			if (det < 0) {
				for (uint32_t p = idx + 2; p < p0Idx; p++)
					_add_triangle_indices(ctx, p, p + 1, idx);
				_add_triangle_indices(ctx, p0Idx, p0Idx + 2, idx);
				_add_triangle_indices(ctx, idx, p0Idx + 1, p0Idx + 2);
			}
			else {
				for (uint32_t p = idx + 2; p < p0Idx; p++)
					_add_triangle_indices(ctx, p, p + 1, idx + 1);
				_add_triangle_indices(ctx, p0Idx, p0Idx + 1, idx + 1);
				_add_triangle_indices(ctx, idx + 1, p0Idx + 1, p0Idx + 2);
			}
		}

		vp = vec2_mult_s(vec2_perp(v1n), str->hw);
		if (det < 0)
			v.pos = vec2_sub(p0, vp);
		else
			v.pos = vec2_add(p0, vp);
		_vertex.push_back(v);
	}

	return (det < 0);
}

void rvg_cx::_draw_stoke_cap(ovg_path_t* ctx, stroke_context_t* str, glm::vec2 p0, glm::vec2 n, bool isStart) {
	Vertex v = {}; v.color = ctx->color; v.uv = { };

	uint32_t firstIdx = (uint32_t)(_vertex.size() - ctx->curVertOffset);

	if (isStart) {
		glm::vec2 vhw = vec2_mult_s(n, str->hw);

		if (ctx->t->lineCap == VG_LINE_CAP_SQUARE)
			p0 = vec2_sub(p0, vhw);

		vhw = vec2_perp(vhw);

		if (ctx->t->lineCap == VG_LINE_CAP_ROUND) {
			if (!str->arcStep)
				str->arcStep = _get_arc_step(ctx, str->hw);

			float a = acosf(n.x) + M_PI_2;
			if (n.y < 0)
				a = M_PI - a;
			float a1 = a + M_PI;

			a += str->arcStep;
			while (a < a1) {
				_add_vertexf(ctx, cosf(a) * str->hw + p0.x, sinf(a) * str->hw + p0.y);
				a += str->arcStep;
			}
			uint32_t p0Idx = (uint32_t)(_vertex.size() - ctx->curVertOffset);
			for (uint32_t p = firstIdx; p < p0Idx; p++)
				_add_triangle_indices(ctx, p0Idx + 1, p, p + 1);
			firstIdx = p0Idx;
		}

		v.pos = vec2_add(p0, vhw);
		_vertex.push_back(v);
		v.pos = vec2_sub(p0, vhw);
		_vertex.push_back(v);

		_add_tri_indices_for_rect(firstIdx);
	}
	else {
		glm::vec2 vhw = vec2_mult_s(n, str->hw);

		if (ctx->t->lineCap == VG_LINE_CAP_SQUARE)
			p0 = vec2_add(p0, vhw);

		vhw = vec2_perp(vhw);

		v.pos = vec2_add(p0, vhw);
		_vertex.push_back(v);
		v.pos = vec2_sub(p0, vhw);
		_vertex.push_back(v);

		firstIdx = (uint32_t)(_vertex.size() - ctx->curVertOffset);

		if (ctx->t->lineCap == VG_LINE_CAP_ROUND) {
			if (!str->arcStep)
				str->arcStep = _get_arc_step(ctx, str->hw);

			float a = acosf(n.x) + M_PI_2;
			if (n.y < 0)
				a = M_PI - a;
			float a1 = a - M_PI;

			a -= str->arcStep;
			while (a > a1) {
				_add_vertexf(ctx, cosf(a) * str->hw + p0.x, sinf(a) * str->hw + p0.y);
				a -= str->arcStep;
			}

			uint32_t p0Idx = (uint32_t)(_vertex.size() - ctx->curVertOffset);
			for (uint32_t p = firstIdx - 1; p < p0Idx; p++)
				_add_triangle_indices(ctx, p + 1, p, firstIdx - 2);
		}
	}
}
float rvg_cx::_draw_dashed_segment(ovg_path_t* ctx, stroke_context_t* str, dash_context_t* dc, bool isCurve) {
	// vec2 pL = ctx->points[str->iL];
	glm::vec2 p = ctx->points[str->cp];
	glm::vec2 pR = ctx->points[str->iR];

	if (!dc->dashOn) // we test in fact the next dash start, if dashOn = true => next segment is a void.
		_build_vb_step(ctx, str, isCurve);

	glm::vec2 d = vec2_sub(pR, p);
	dc->normal = vec2_norm(d);
	float segmentLength = glm::length(d);

	while (dc->curDashOffset < segmentLength) {
		glm::vec2 p0 = vec2_add(p, vec2_mult_s(dc->normal, dc->curDashOffset));

		_draw_stoke_cap(ctx, str, p0, dc->normal, dc->dashOn);
		dc->dashOn ^= true;
		dc->curDashOffset += ctx->t->dashes[dc->curDash];
		if (++dc->curDash == ctx->t->dashCount)
			dc->curDash = 0;
	}
	dc->curDashOffset -= segmentLength;
	dc->curDashOffset = fmodf(dc->curDashOffset, dc->totDashLength);
	return segmentLength;
}
void rvg_cx::_draw_segment(ovg_path_t* ctx, stroke_context_t* str, dash_context_t* dc, bool isCurve) {
	str->iR = str->cp + 1;
	if (ctx->t->dashCount > 0)
		_draw_dashed_segment(ctx, str, dc, isCurve);
	else
		_build_vb_step(ctx, str, isCurve);
	str->iL = str->cp++;
}

void rvg_cx::image_update(vg_image_t* img, vg_image_desc_t* desc)
{
	if (!img)img = desc->img;
	auto& dst = _images[img]; dst = *desc;
	img->width = desc->width;
	img->height = desc->height;
	if (desc->is_copy) {
		dst.px_size = dst.height * dst.stride;
		dst.pixels = ac->new_mem(dst.px_size);
		memcpy((void*)dst.pixels, desc->pixels, dst.px_size);
	}
}

void rvg_cx::image_destroy(vg_image_t* img)
{
	_images[img].is_destroy = true;
}


// todo 渲染操作，rvg_cx可以多次执行fill或stroke/clip
rvg_t* ovg_new_rvg(mem_resource_t* ac0)
{
	auto ac = (usp_ac_cx*)ac0;
	if (!ac) {
		return 0;
	}
	auto p = ac->new_obj<rvg_cx>();
	p->ac = ac;
	return p;
}
void ovg_destroy_rvg(rvg_t* p0) {
	auto p = (rvg_cx*)p0;
	if (p && p->ac) {
		p->ac->free_obj(p);
	}
}
void ovg_clear(rvg_t* v0)
{
	auto v = (rvg_cx*)v0;
	if (v)v->clear_all();
}
void ovg_set_path(rvg_t* v0, ovg_path_t* path, vg_state_save_t* st)
{
	auto v = (rvg_cx*)v0;
	if (!v)return;
	v->set_path(path, st);
}
void ovg_reset_clip(rvg_t* v0, uint8_t ref)
{
	auto v = (rvg_cx*)v0;
	if (v)v->clip0(ref);
}
void ovg_clip(rvg_t* v0)
{
	auto v = (rvg_cx*)v0;
	if (v)v->clip();
}
void ovg_clip_preserve(rvg_t* v0)
{
	auto v = (rvg_cx*)v0;
	if (v)v->clip_preserve();
}
void ovg_clip_rect(rvg_t* v0, int x, int y, int width, int height)
{
	auto v = (rvg_cx*)v0;
	glm::ivec4 c[1] = { {x,y,width,height} };
	if (v)v->clip(c);
}
void ovg_set_clip_rect(rvg_t* v0, void* rc) {
	auto v = (rvg_cx*)v0;
	if (v && rc) {
		if (v)v->clip((glm::ivec4*)rc);
	}
}
void ovg_get_clip_rect(rvg_t* v0, void* rc) {
	auto v = (rvg_cx*)v0;
	if (v && rc) {
		*((glm::ivec4*)rc) = v->curClip;
	}
}
void ovg_stroke(rvg_t* v0)
{
	auto v = (rvg_cx*)v0;
	if (!v)return;
	v->stroke_preserve();
	ovg_clear_path(v->path);
}
void ovg_stroke_preserve(rvg_t* v0) {
	auto v = (rvg_cx*)v0;
	if (!v)return;
	v->stroke_preserve();
}
void ovg_fill(rvg_t* v0)
{
	auto v = (rvg_cx*)v0;
	if (!v)return;
	v->fill_preserve();
	ovg_clear_path(v->path);
}
void ovg_fill_preserve(rvg_t* v0)
{
	auto v = (rvg_cx*)v0;
	if (!v)return;
	v->fill_preserve();
}
void ovg_paint(rvg_t* v0)
{
	auto v = (rvg_cx*)v0;
	if (v)v->paint();
}

// 添加文本，风格，渲染区可选
void  ovg_add_text(rvg_t* v0, text_st_t* p, text_style_t* ts, text_box_rt* box)
{
	auto dc = (rvg_cx*)v0;
	if (dc) {
		auto cb = (ovg_canvas_cx*)dc->ac->ptr;
		if (cb)
			cb->add_text(v0, p, ts, box);
	}
}
// 普通图片，支持九宫格、混合颜色
void  ovg_add_image(rvg_t* v0, ovg_image_r* r)
{
	auto dc = (rvg_cx*)v0;
	if (dc) {
		dc->gps.set_viewport({ 0,0,dc->width,dc->height });
		dc->gps.add_image(r);
	}
}
// 原始三角形，输入0则不修改
void  ovg_set_geom_state(rvg_t* v0, gem_info_t* info, const glm::mat4* matrix)
{
	auto dc = (rvg_cx*)v0;
	if (dc)dc->gps.set_state(info, matrix);
}
size_t ovg_set_instance_mat(rvg_t* v0, const glm::mat4* matrix, size_t count)
{
	auto dc = (rvg_cx*)v0;
	return (dc) ? dc->gps.set_instance_mat(matrix, count) : 0;
}
// 添加几何数据到缓冲区，xy顶点坐标，color顶点颜色，uv顶点纹理坐标，indices索引数据，color_type=0表示float4，1表示uint32_t
void  ovg_add_geometry(rvg_t* v0, vg_image_t* texture, const float* xy, int xy_stride, const void* color, int color_stride, const float* uv, int uv_stride, int num_vertices, const void* indices, int num_indices, int size_indices, int color_type)
{
	auto dc = (rvg_cx*)v0;
	if (dc)dc->gps.add_geometry(texture, xy, xy_stride, color, color_stride, uv, uv_stride, num_vertices, indices, num_indices, size_indices, color_type);
}
// 添加3D几何数据到缓冲区，xyz顶点坐标，color顶点颜色（双面则要双倍），uv顶点纹理坐标，indices索引数据
void  ovg_add_geometry3d(rvg_t* v0, vg_image_t* texture, const float* xyz, int xyz_stride, const void* color, int color_stride, const float* uv, int uv_stride, int num_vertices, const void* indices, int num_indices, int size_indices, int color_type)
{
	auto dc = (rvg_cx*)v0;
	if (dc)dc->gps.add_geometry3d(texture, xyz, xyz_stride, color, color_stride, uv, uv_stride, num_vertices, indices, num_indices, size_indices, color_type);
}

void ovg_image_update(rvg_t* p, vg_image_t* img, vg_image_desc_t* desc) {
	if (!p || !desc)return;
	auto dc = (rvg_cx*)p;
	dc->image_update(img, desc);
}
//标记图片不再使用（后端延迟释放 GPU 纹理）
void ovg_image_destroy(rvg_t* p, vg_image_t* img) {
	if (!p || !img)return;
	auto dc = (rvg_cx*)p;
	dc->image_destroy(img);
}
#endif // 1

// todo init cb
void init_ovg_cb(ovg_canvas_cb* cb) {
	if (!cb)return;
	cb->new_path = ovg_new_path;		// 可自定义分配
	cb->destroy_path = ovg_destroy_path;
	cb->clear_path = ovg_clear_path;
	cb->close_path = ovg_close_path;
	cb->new_sub_path = ovg_new_sub_path;
	cb->path_extents = ovg_path_extents;
	cb->get_current_point = ovg_get_current_point;
	cb->get_segment_count = ovg_get_segment_count;
	cb->set_segment_color = ovg_set_segment_color;
	cb->add_path = ovg_add_path;
	//cb->add_path0 = ovg_add_path0;
	cb->move_to = ovg_move_to;
	cb->rel_move_to = ovg_rel_move_to;
	cb->line_to = ovg_line_to;
	cb->rel_line_to = ovg_rel_line_to;
	cb->arc = ovg_arc;
	cb->arc_negative = ovg_arc_negative;
	cb->curve_to = ovg_curve_to;
	cb->rel_curve_to = ovg_rel_curve_to;
	cb->quadratic_to = ovg_quadratic_to;
	cb->rel_quadratic_to = ovg_rel_quadratic_to;
	cb->rectangle = ovg_rectangle;
	cb->rounded_rectangle = ovg_rounded_rectangle;
	cb->rounded_rectangle2 = ovg_rounded_rectangle2;
	cb->ellipse = ovg_ellipse;
	cb->elliptic_arc_to = ovg_elliptic_arc_to;
	cb->rel_elliptic_arc_to = ovg_rel_elliptic_arc_to;
	cb->circle = ovg_circle;

	cb->new_state = ovg_new_state;
	cb->state_destroy = ovg_state_destroy;
	cb->set_opacity = ovg_set_opacity;
	cb->set_source_color = ovg_set_source_color;
	cb->set_source_rgba = ovg_set_source_rgba;
	cb->set_source_rgb = ovg_set_source_rgb;
	cb->set_line_width = ovg_set_line_width;
	cb->set_miter_limit = ovg_set_miter_limit;
	cb->set_line_cap = ovg_set_line_cap;
	cb->set_line_join = ovg_set_line_join;
	cb->set_source_surface = ovg_set_source_surface;
	cb->set_source = ovg_set_source;
	cb->set_operator = ovg_set_operator;
	cb->set_fill_rule = ovg_set_fill_rule;
	cb->set_dash = ovg_set_dash;
	cb->set_dash8 = ovg_set_dash8;
	cb->translate = ovg_translate;
	cb->scale = ovg_scale;
	cb->rotate = ovg_rotate;
	cb->transform = ovg_transform;
	cb->set_matrix = ovg_set_matrix;
	cb->get_matrix = ovg_get_matrix;
	cb->identity_matrix = ovg_identity_matrix;
	cb->matrix_init = ovg_matrix_init;

	cb->new_pattern_linear = ovg_new_pattern_linear;
	cb->new_pattern_radial = ovg_new_pattern_radial;
	cb->new_pattern_sweep = ovg_new_pattern_sweep;
	cb->pattern_add_color_stop = ovg_pattern_add_color_stop;
	cb->pattern_set_color_stop = ovg_pattern_set_color_stop;
	cb->pattern_set_matrix = ovg_pattern_set_matrix;
	cb->pattern_set_extend = ovg_pattern_set_extend;
	cb->pattern_set_filter = ovg_pattern_set_filter;
	cb->pattern_destroy = ovg_pattern_destroy;

	cb->image_update = ovg_image_update;
	cb->image_destroy = ovg_image_destroy;
	// 渲染操作，rvg_cx可以多次执行fill或stroke/clip
	cb->new_rvg = ovg_new_rvg;
	cb->destroy_rvg = ovg_destroy_rvg;
	cb->set_path = ovg_set_path;
	cb->stroke = ovg_stroke;
	cb->stroke_preserve = ovg_stroke_preserve;
	cb->fill = ovg_fill;
	cb->fill_preserve = ovg_fill_preserve;
	cb->paint = ovg_paint;
	cb->clear = ovg_clear;
	cb->reset_clip = ovg_reset_clip;
	cb->clip = ovg_clip;
	cb->clip_preserve = ovg_clip_preserve;
	cb->clip_rect = ovg_clip_rect;
	cb->set_clip_rect = ovg_set_clip_rect;
	cb->get_clip_rect = ovg_get_clip_rect;
	cb->add_text = ovg_add_text;
	cb->add_image = ovg_add_image;
	cb->set_geom_state = (void (*)(rvg_t*, gem_info_t*, const void*)) ovg_set_geom_state;
	cb->set_instance_mat = (size_t(*)(rvg_t*, const void*, size_t)) ovg_set_instance_mat;
	cb->add_geometry = ovg_add_geometry;
	cb->add_geometry3d = ovg_add_geometry3d;

}

#endif // 1


geom_primitive::geom_primitive()
{}

geom_primitive::~geom_primitive()
{
	clear();
}

void geom_primitive::clear()
{
	vd1.clear();
	vd2.clear();
	ids.clear();
	instance_mat.clear();
	build_pipes.clear();
	mat = glm::mat4(1.0f);
	curState = {};
	inst_idx = 0;
	inst_count = 0;
}

void geom_primitive::set_state(gem_info_t* info, const glm::mat4* matrix)
{
	if (info) {
		curState = *info;
		build_pipes.push_back(curState);
	}
	if (matrix) { mat = *matrix; }
}

size_t geom_primitive::set_instance_mat(const glm::mat4* matrix, size_t count)
{
	auto ps = instance_mat.size();
	if (matrix && count > 0)
	{
		instance_mat.resize(ps + count);
		memcpy(instance_mat.data() + ps, matrix, count * sizeof(glm::mat4));
		inst_idx = ps;
	}
	inst_count = count;
	return ps;
}
float get_has_multiply(const gem_info_t& state) {
	return ((blendMode_e)state.blendMode == blendMode_e::additive_prem || (blendMode_e)state.blendMode == blendMode_e::normal_prem) ? 1.0f : 0.0f;
}
bool geom_primitive::add_geometry(void* texture, const float* xy, int xy_stride, const void* color, int color_stride, const float* uv, int uv_stride, int num_vertices, const void* indices, int num_indices, int size_indices, int color_type)
{
	if (!xy || num_vertices < 1)return false;
	geom_cmd_t c = {};
	c.state = curState;
	c.texture = texture;
	c.mat = mat;
	c.multiply = get_has_multiply(c.state);
	c.instance_count = inst_count;
	if (inst_count > 0)
		c.instance_ssbo_pos = inst_idx;
	float scale_x = 1.0, scale_y = 1.0;
	float u_scale = 1.0, v_scale = 1.0;
	size_indices = indices ? size_indices : 0;
	ids.reserve(ids.size() + num_indices);
	c.firstIndex = ids.size();
	c.elemCount = num_indices;
	if (num_indices < 1 || size_indices < 1)
	{
		c.elemCount = num_vertices;
	}
	if (curState.shader == ST_DOUBLESIDED) {
		c.vertexOffset = vd2.size();
		c.v_offset = 1;
		vd2.resize(vd2.size() + num_vertices);
		auto mem = vd2.data() + c.vertexOffset;	// 双面顶点
		auto verts = mem;
		for (size_t i = 0; i < num_indices; i++) {
			int j;
			float* xy_;
			if (size_indices == 4) {
				j = ((const uint32_t*)indices)[i];
			}
			else if (size_indices == 2) {
				j = ((const uint16_t*)indices)[i];
			}
			else if (size_indices == 1) {
				j = ((const uint8_t*)indices)[i];
			}
			else {
				j = i;
			}
			ids.push_back(j);
		}
		for (size_t i = 0; i < num_vertices; i++) {
			float* xy_;
			xy_ = (float*)((char*)xy + i * xy_stride);
			verts->pos.x = xy_[0] * scale_x;
			verts->pos.y = xy_[1] * scale_y;
			if (color_type == 1) {
				auto c8 = (uint32_t*)((char*)color + i * color_stride);
				verts->color = *c8; c8++;
				verts->color1 = *c8;
			}
			else
			{
				auto c4 = (glm::vec4*)((char*)color + i * color_stride);
				verts->color = CreateRgbaf(c4->x, c4->y, c4->z, c4->w); c4++;
				verts->color1 = CreateRgbaf(c4->x, c4->y, c4->z, c4->w);
			}
			if (texture && uv) {
				float* uv_ = (float*)((char*)uv + i * uv_stride);
				verts->uv.x = uv_[0] * u_scale;
				verts->uv.y = uv_[1] * v_scale;
			}
			else {
				verts->uv = { 0.0f, 0.0f };
			}
			verts += 1;
		}
	}
	else
	{
		c.vertexOffset = vd1.size();
		vd1.resize(vd1.size() + num_vertices);
		auto mem = vd1.data() + c.vertexOffset;	// 单面顶点
		auto verts = mem;
		for (size_t i = 0; i < num_indices; i++) {
			int j;
			float* xy_;
			if (size_indices == 4) {
				j = ((const uint32_t*)indices)[i];
			}
			else if (size_indices == 2) {
				j = ((const uint16_t*)indices)[i];
			}
			else if (size_indices == 1) {
				j = ((const uint8_t*)indices)[i];
			}
			else {
				j = i;
			}
			ids.push_back(j);
		}
		for (size_t i = 0; i < num_vertices; i++) {
			float* xy_;
			xy_ = (float*)((char*)xy + i * xy_stride);
			verts->pos.x = xy_[0] * scale_x;
			verts->pos.y = xy_[1] * scale_y;
			if (color_type == 1) {
				auto c8 = (uint32_t*)((char*)color + i * color_stride);
				verts->color = *c8; c8++;
			}
			else
			{
				auto c4 = (glm::vec4*)((char*)color + i * color_stride);
				verts->color = CreateRgbaf(c4->x, c4->y, c4->z, c4->w);
			}
			if (texture && uv) {
				float* uv_ = (float*)((char*)uv + i * uv_stride);
				verts->uv.x = uv_[0] * u_scale;
				verts->uv.y = uv_[1] * v_scale;
			}
			else {
				verts->uv = { 0.0f, 0.0f };
			}
			verts += 1;
		}
	}

	gt->push_back({ .g = c });
	return true;
}

bool geom_primitive::add_geometry3d(void* texture, const float* xyz, int xyz_stride, const void* color, int color_stride, const float* uv, int uv_stride, int num_vertices, const void* indices, int num_indices, int size_indices, int color_type)
{
	if (!xyz || num_vertices < 1)return false;
	geom_cmd_t c = {};
	c.state = curState;
	c.texture = texture;
	c.mat = mat;
	c.multiply = get_has_multiply(c.state);
	c.instance_count = inst_count;
	if (inst_count > 0)
		c.instance_ssbo_pos = inst_idx;
	float scale_x = 1.0, scale_y = 1.0, scale_z = 1.0;
	float u_scale = 1.0, v_scale = 1.0;
	size_indices = indices ? size_indices : 0;
	ids.reserve(ids.size() + num_indices);
	c.firstIndex = ids.size();
	c.elemCount = num_indices;
	if (num_indices < 1 || size_indices < 1)
	{
		c.elemCount = num_vertices;
	}
	if (curState.shader == ST_DOUBLESIDED) {
		c.vertexOffset = vd2.size();
		c.v_offset = 1;
		vd2.resize(vd2.size() + num_vertices);
		auto mem = vd2.data() + c.vertexOffset;	// 双面顶点
		auto verts = mem;
		for (size_t i = 0; i < num_indices; i++) {
			int j;
			if (size_indices == 4) {
				j = ((const uint32_t*)indices)[i];
			}
			else if (size_indices == 2) {
				j = ((const uint16_t*)indices)[i];
			}
			else if (size_indices == 1) {
				j = ((const uint8_t*)indices)[i];
			}
			else {
				j = i;
			}
			ids.push_back(j);
		}
		for (size_t i = 0; i < num_vertices; i++) {
			float* xyz_;
			xyz_ = (float*)((char*)xyz + i * xyz_stride);
			verts->pos.x = xyz_[0] * scale_x;
			verts->pos.y = xyz_[1] * scale_y;
			verts->pos.z = xyz_[2] * scale_z;
			if (color_type == 1) {
				auto c8 = (uint32_t*)((char*)color + i * color_stride);
				verts->color = *c8; c8++;
				verts->color1 = *c8;
			}
			else
			{
				auto c4 = (glm::vec4*)((char*)color + i * color_stride);
				verts->color = CreateRgbaf(c4->x, c4->y, c4->z, c4->w); c4++;
				verts->color1 = CreateRgbaf(c4->x, c4->y, c4->z, c4->w);
			}
			if (texture && uv) {
				float* uv_ = (float*)((char*)uv + i * uv_stride);
				verts->uv.x = uv_[0] * u_scale;
				verts->uv.y = uv_[1] * v_scale;
			}
			else {
				verts->uv = { 0.0f, 0.0f };
			}
			verts += 1;
		}
	}
	else
	{
		c.vertexOffset = vd1.size();
		vd1.resize(vd1.size() + num_vertices);
		auto mem = vd1.data() + c.vertexOffset;	// 单面顶点
		auto verts = mem;
		for (size_t i = 0; i < num_indices; i++) {
			int j;
			if (size_indices == 4) {
				j = ((const uint32_t*)indices)[i];
			}
			else if (size_indices == 2) {
				j = ((const uint16_t*)indices)[i];
			}
			else if (size_indices == 1) {
				j = ((const uint8_t*)indices)[i];
			}
			else {
				j = i;
			}
			ids.push_back(j);
		}
		for (size_t i = 0; i < num_vertices; i++) {
			float* xyz_;
			xyz_ = (float*)((char*)xyz + i * xyz_stride);
			verts->pos.x = xyz_[0] * scale_x;
			verts->pos.y = xyz_[1] * scale_y;
			verts->pos.z = xyz_[2] * scale_z;
			if (color_type == 1) {
				auto c8 = (uint32_t*)((char*)color + i * color_stride);
				verts->color = *c8; c8++;
			}
			else
			{
				auto c4 = (glm::vec4*)((char*)color + i * color_stride);
				verts->color = CreateRgbaf(c4->x, c4->y, c4->z, c4->w);
			}
			if (texture && uv) {
				float* uv_ = (float*)((char*)uv + i * uv_stride);
				verts->uv.x = uv_[0] * u_scale;
				verts->uv.y = uv_[1] * v_scale;
			}
			else {
				verts->uv = { 0.0f, 0.0f };
			}
			verts += 1;
		}
	}
	gt->push_back({ .g = c });
	return true;
}
#if 0
void build_text()
{
	auto& git = vt.f;
	if (git._image) {
		auto& tp = _vt[git._image];
		if (tex != tp)
		{
			submit_data(tex);
			tex = tp;
		}
		auto ps = git._dwpos + git._apos;
		ps += pos0;
		auto tstyle = tbp[git.tb_idx].style;
		color = tstyle.color;
		auto img = git._image;
		glm::ivec2 tex_size = { img->width,  img->height };
		if (!git.color || tstyle.mcolor_effect)
		{
			if (tstyle.color_shadow)
			{
				auto ps1 = ps;
				ps1 += tstyle.shadow_pos;	// 生成阴影数据
				gen3data(tex_size, ps1, git._rect, {}, tstyle.color_shadow, &opt, &idx);
			}
			if (tstyle.stroke && tstyle.color_stroke)
			{
				int pxx[4] = { -tstyle.stroke, 0, tstyle.stroke, 0 };
				int pyy[4] = { 0, -tstyle.stroke, 0, tstyle.stroke };
				for (int e = 0; e < 4; e++)
				{
					auto ps1 = ps;
					ps1.x += pxx[e];
					ps1.y += pyy[e];	// 生成描边数据
					gen3data(tex_size, ps1, git._rect, {}, tstyle.color_stroke, &opt, &idx);
				}
			}
		}
		gen3data(tex_size, ps, git._rect, {}, git.color ? git.color : color, &opt, &idx);
	}
}
#endif 

glm::mat4 ovg_ortho(float width, float height, float znear, float zfar, bool is_top)
{
	return is_top ? glm::ortho(0.0f, width, height, 0.0f, znear, zfar) : glm::ortho(0.0f, width, 0.0f, height, znear, zfar);
}
void draw_mesh2d_x(rvg_cx* ctx, geom_primitive* gp, const glm::vec2& render_scale)
{
	mesh2d_x* dc = gp;
	glm::vec2 clip_off = {};
	glm::vec2 clip_scale = render_scale;
	glm::ivec4 vp = { 0,0,-1,-1 };
	if (dc->viewport.z > 0 && dc->viewport.w > 0)
	{
		vp.x = dc->viewport.x;
		vp.y = dc->viewport.y;
		vp.z = dc->viewport.z;
		vp.w = dc->viewport.w;
	}
	auto av = dc;
	auto vd = av->vtxs.data();
	auto vdt = av->vtxs.data();
	auto idv = av->idxs.data();
	auto vbs = av->vtxs.size();
	auto ibs = av->idxs.size();
	std::vector<int> idxs;
	struct { void* texture; int blendMode; } states = {};
	glm::ivec4 oldclip = {};
	ovg_get_clip_rect(ctx, (int*)&oldclip);
	size_t cclip = 0;
	gem_info_t info = {};
	info.blendMode = (int8_t)blendMode_e::normal;
	info.topology = 0;
	//info.doubleSided = false;
	//info.depthTestEnable = false;
	//info.depthWriteEnable = false;
	//info.stencilTestEnable = true;
	info.flags = (uint8_t)depth_stencil_State::d_stenciltest_enable;
	info.frontFace = 0;
	info.cullMode = 0;
	glm::mat4 mat = ovg_ortho(dc->viewport.z, dc->viewport.w, -1.0f, 1.0f, 0);
	ovg_set_clip_rect(ctx, &vp);
	gp->set_state(&info, &mat);
	for (auto& pcmd : av->cmd_data)
	{
		glm::vec2 clip_min((pcmd.clip_rect.x - clip_off.x) * clip_scale.x, (pcmd.clip_rect.y - clip_off.y) * clip_scale.y);
		glm::vec2 clip_max((pcmd.clip_rect.z - clip_off.x) * clip_scale.x, (pcmd.clip_rect.w - clip_off.y) * clip_scale.y);
		if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
		{
			ovg_set_clip_rect(ctx, &vp); cclip++;
		}
		else
		{
			glm::ivec4 r = { (int)(clip_min.x), (int)(clip_min.y), (int)(clip_max.x - clip_min.x), (int)(clip_max.y - clip_min.y) };
			ovg_set_clip_rect(ctx, &r); cclip++;
		}
		auto texture = pcmd.texid;
		auto vertices = vdt + pcmd.vtxOffset;
		const float* xy = &vertices->position.x;
		int stride = sizeof(mesh2d_x::vertex_t);
		auto color = &vertices->color;
		const float* uv = &vertices->tex_coord.x;
		int size_indices = 4;
		auto indices = ibs ? idv + pcmd.idxOffset : nullptr;
		auto num_indices = pcmd.elemCount;
		int8_t blend = pcmd.blend_mode;
		if (states.blendMode != blend) {
			states.blendMode = blend;
			info.blendMode = states.blendMode;
			gp->set_state(&info, &mat);
		}
		gp->add_geometry(texture, xy, stride, color, stride, uv, stride, pcmd.vCount, indices, num_indices, size_indices, 1);
	}
	dc->clear_m2d();
	if (cclip > 0) {
		ovg_set_clip_rect(ctx, &oldclip);
	}
}

void geom_primitive::add_image(ovg_image_r* r)
{
	if (!r || !r->img || (r->dst.z * r->dst.w <= 0) || (r->rc.z < 1 || r->rc.w < 1))return;
	add_image0(r->img, { r->img->width,r->img->height }, {}, r->dst, r->rc, r->sliced, r->color);
	draw_mesh2d_x(dc, this, { 1.0,1.0 });
}
#if 1

mesh2d_x::mesh2d_x()
{}

mesh2d_x::~mesh2d_x()
{}

void mesh2d_x::set_viewport(const glm::ivec4& vp)
{
	viewport = vp;
}

void mesh2d_x::set_clip(const glm::ivec4& rc)
{
	_clip_rect = rc;
}

void mesh2d_x::clear_m2d()
{
	vtxs.clear();
	idxs.clear();
	cmd_data.clear();
	_clip_rect = viewport;
	_clip_rect.x = _clip_rect.y = 0;
}

inline uint8_t is_rect_intersect0(int x01, int x02, int y01, int y02,
	int x11, int x12, int y11, int y12)
{
	int zx = abs(x01 + x02 - x11 - x12);
	int x = abs(x01 - x02) + abs(x11 - x12);
	int zy = abs(y01 + y02 - y11 - y12);
	int y = abs(y01 - y02) + abs(y11 - y12);
	if (zx <= x && zy <= y)
		return 1;
	else
		return 0;
}
inline bool is_rect_intersect(glm::vec4 r1, glm::vec4 r2)
{
	//第一种情况：如果b.x > a.x + a.w，则a和b一定不相交，
	//第二种情况：如果a.y > b.y + b.h，则a和b一定不相交，
	//第三种情况：如果b.y > a.y + a.h，则a和b一定不相交，
	//第四种情况：如果a.x > b.x + b.w，则a和b一定不相交
	auto& a = r1; auto& b = r2;
	if (a.x > b.x + b.z || b.x > a.x + a.z || a.y > b.y + b.w || b.y > a.y + a.w) {
		return false;
	}
	else {
		return true;
	}
	return is_rect_intersect0(r1.x, r1.y, r1.z, r1.w, r2.x, r2.y, r2.z, r2.w);
}
bool mesh2d_x::nohas_clip(const glm::ivec4& a)
{
	auto clip = _clip_rect;
	if (clip.z == 0 || clip.w == 0 || viewport.z == 0 || viewport.w == 0)return false;
	if (clip.z > viewport.z || clip.z < 0)clip.z = viewport.z;
	if (clip.w > viewport.w || clip.w < 0)clip.w = viewport.w;
	if (clip.z < 0 || clip.w < 0)
	{
		return false;
	}
	return (!is_rect_intersect(clip, a));
}
void mesh2d_x::add(void* user_image, std::vector<vertex_t>& vertex, std::vector<int>& vt_index, const glm::ivec4& clip)
{
	add(user_image, vertex.data(), vertex.size(), vt_index.data(), vt_index.size(), clip);
}

void mesh2d_x::add(void* user_image, vertex_t* vertex, size_t vcount, int* vt_index, size_t icount, const glm::ivec4& clip)
{
	auto ps0 = vcount;
	auto ps = vtxs.size();
	auto ix = idxs.size();
	auto ic = icount;
	vtxs.resize(ps + vcount);
	idxs.resize(ix + icount);
	auto& cd = cmd_data;
	if (cd.empty())
	{
		cd.push_back({});
	}
	auto dt = &cd.back();
	auto pidx = idxs.data() + ix;
	if (dt->texid != user_image || dt->clip_rect != clip)
	{
		if (dt->elemCount > 0)
			cd.push_back({});
		dt = &cd.back();
		dt->texid = user_image;
		dt->clip_rect = clip;
		dt->vtxOffset = ps;
		dt->idxOffset = ix;
		dt->elemCount = ic;
		dt->vCount = ps0;
	}
	else
	{
		// 合批
		dt->elemCount += ic;
		dt->vCount += ps0;
		auto idt = vt_index;
		for (size_t i = 0; i < ic; i++)
		{
			idt[i] += ix;
		}
	}
	memcpy(vtxs.data() + ps, vertex, vcount * sizeof(vertex[0]));
	memcpy(pidx, vt_index, icount * sizeof(vt_index[0]));
}


void mesh2d_x::add_image0(void* img, const glm::ivec2& texsize, const glm::ivec4& clip, const glm::ivec4& dst, const glm::ivec4& src, const glm::ivec4& sliced, uint32_t color)
{
	auto a = glm::vec4(dst);
	glm::ivec2 pos = { a.x, a.y }, size = { a.z, a.w };
	glm::vec4 v4 = { 0, 0, 1, 1 };
	glm::vec4 uv = v4;
	glm::vec2 s = size;
	if (a.z < 0)
		a.z *= -std::min(src.z, texsize.x);
	if (a.w < 0)
		a.w *= -std::min(src.w, texsize.y);
	if (nohas_clip(a))
		return;

	if (sliced.x > 0)
	{
		add_image_sliced(img, texsize, a, sliced, src, color, clip);// 生成九宫格到mesh
	}
	else
	{
		if (!(src.x < 0))
		{
			v4 = src;
			v4.z += v4.x; v4.w += v4.y;//加上原点坐标
			v4.z = glm::min(v4.z, (float)texsize.x);
			v4.w = glm::min(v4.w, (float)texsize.y);
			uv = { v4.x / texsize.x, v4.y / texsize.y, v4.z / texsize.x, v4.w / texsize.y };
			if (uv.x < 0) { uv.x = 0; }
			if (uv.y < 0) { uv.y = 0; }
		}
		glm::vec2 av = pos, cv = { pos.x + s.x, pos.y + s.y }, uv_a = { uv.x, uv.y }, uv_c{ uv.z, uv.w };
		auto& col = color;
		glm::vec2 bv(cv.x, av.y), dv(av.x, cv.y), uv_b(uv_c.x, uv_a.y), uv_d(uv_a.x, uv_c.y);

		vertex_t vertex[] = {
		   {av, uv_a, col},
		   {bv, uv_b, col},
		   {cv, uv_c, col},
		   {dv, uv_d, col},
		};
		int rect_index_order[] = { 0, 1, 2, 0, 2, 3 };
		add(img, vertex, 4, rect_index_order, 6, clip);// 添加矩形(两个三角形)到mesh
	}
}

/*


九宫格渲染:
+--+---------------+--+
|0 |       1       |2 |
+--+---------------+--+
|  |               |  |
|  |               |  |
|3 |    center     |4 |
|  |               |  |
+--+---------------+--+
|5 |       6       |7 |
+--+---------------+--+

九宫格:索引
0  12                     14  2
8  4                      6   10

9  5                      7   11
1  13                     15  3
+--+-------------------------+--+
|  |                         |  |
+--+-------------------------+--+
|  |                         |  |
|  |                         |  |
+--+-------------------------+--+
|  |                         |  |
+--+-------------------------+--+
sliced.x=左宽，y上高，z右宽，w下高

*/
void mesh2d_x::add_image_sliced(void* user_image, const glm::ivec2& texsize, const glm::ivec4& a, const glm::ivec4& sliced, const glm::ivec4& rect, uint32_t col, const glm::ivec4& clip)
{
	static std::vector<int> vt_index =// { 0,8,12,4,14,6,2,10,11,6,7,4,5,8,9,1,5,13,7,15,11,3 };//E_TRIANGLE_STRIP
	{ 0, 8, 12, 8, 12, 4, 12, 4, 14, 4, 14, 6, 14, 6, 2, 6, 2, 10,
		6, 7, 10, 7, 10, 11, 4, 5, 6, 5, 6, 7, 8, 9, 4, 9, 4, 5,
		9, 1, 5, 1, 5, 13, 5, 13, 7, 13, 7, 15, 7, 15, 11, 15, 11, 3 };//E_TRIANGLE_LIST

	glm::ivec2 pos = { a.x, a.y }, size = { a.z, a.w };
	glm::vec4 uv = { 0, 0, 1, 1 };
	glm::vec4 v4 = { 0, 0, texsize.x, texsize.y };
	if (!(rect.x < 0))
	{
		v4 = rect;
		v4.z += v4.x; v4.w += v4.y;//加上原点坐标
		uv = { v4.x / texsize.x, v4.y / texsize.y, v4.z / texsize.x, v4.w / texsize.y, };
	}
	float left = sliced.x,
		top = sliced.y,
		right = sliced.z,
		bottom = sliced.w;
	float x = pos.x, y = pos.y, width = size.x, height = size.y;
	glm::vec4 suv = { (left + v4.x) / texsize.x, (top + v4.y) / texsize.y,
		(v4.z - right) / texsize.x, (v4.w - bottom) / texsize.y };

	vertex_t vertex[] = {
		//0
		{{x, y}, {uv.x, uv.y}, col},
		//1
		{{x, y + height}, {uv.x, uv.w}, col},
		//2
		{{x + width, y}, {uv.z, uv.y}, col},
		//3
		{{x + width, y + height}, {uv.z, uv.w}, col},
		//4
		{{x + left, y + top}, {suv.x, suv.y}, col},
		//5
		{{x + left, y + height - bottom}, {suv.x, suv.w}, col},
		//6
		{{x + width - right, y + top}, {suv.z, suv.y}, col},
		//7
		{{x + width - right, y + height - bottom}, {suv.z, suv.w}, col},
		//8
		{{x, y + top}, {uv.x, suv.y}, col},
		//9
		{{x, y + height - bottom}, {uv.x, suv.w}, col},
		//10
		{{x + width, y + top}, {uv.z, suv.y}, col},
		//11
		{{x + width, y + height - bottom}, {uv.z, suv.w}, col},
		//12
		{{x + left, y}, {suv.x, uv.y}, col},
		//13
		{{x + left, y + height}, {suv.x, uv.w}, col},
		//14
		{{x + width - right, y}, {suv.z, uv.y}, col},
		//15
		{{x + width - right, y + height}, {suv.z, uv.w}, col}
	};

	add(user_image, vertex, 16, vt_index.data(), vt_index.size(), clip);

	return;
}
void mesh2d_x::add_image_angle(void* img, const glm::ivec2& texsize, const glm::ivec4& srcrect, const glm::ivec4& dstrect, float angle, const glm::vec2* center, uint32_t col, const glm::ivec4& clip, int flip)
{
	int rect_index_order[] = { 0, 1, 2, 0, 2, 3 };
	glm::ivec4 real_srcrect = {};
	glm::vec2 real_center = {};
	if (flip == FLIP_NONE && (int)(angle / 360) == angle / 360) { // fast path when we don't need rotation or flipping
		add_image0(img, texsize, clip, srcrect, dstrect, {}, col);
		return;
	}
	real_srcrect.x = 0.0f;
	real_srcrect.y = 0.0f;
	real_srcrect.z = (float)texsize.x;
	real_srcrect.w = (float)texsize.y;
	if (center) {
		real_center = *center;
	}
	else {
		real_center.x = dstrect.z / 2.0f;
		real_center.y = dstrect.w / 2.0f;
	}
	vertex_t v[4];
	//float xy[8];
	const int xy_stride = 2 * sizeof(float);
	//float uv[8];
	const int uv_stride = 2 * sizeof(float);
	const int num_vertices = 4;
	const int* indices = rect_index_order;
	const int num_indices = 6;
	const int size_indices = 4;
	glm::vec2 minuv, maxuv;
	glm::vec2 minxy, maxxy;
	float centerx, centery;

	float s_minx, s_miny, s_maxx, s_maxy;
	float c_minx, c_miny, c_maxx, c_maxy;

	const float radian_angle = glm::radians(angle);
	const float s = glm::sin(radian_angle);
	const float c = glm::cos(radian_angle);

	minuv.x = real_srcrect.x / texsize.x;
	minuv.y = real_srcrect.y / texsize.y;
	maxuv.x = (real_srcrect.x + real_srcrect.z) / texsize.x;
	maxuv.y = (real_srcrect.y + real_srcrect.w) / texsize.y;

	centerx = real_center.x + dstrect.x;
	centery = real_center.y + dstrect.y;

	if (flip & FLIP_HORIZONTAL) {
		minxy.x = dstrect.x + dstrect.z;
		maxxy.x = dstrect.x;
	}
	else {
		minxy.x = dstrect.x;
		maxxy.x = dstrect.x + dstrect.z;
	}

	if (flip & FLIP_VERTICAL) {
		minxy.y = dstrect.y + dstrect.w;
		maxxy.y = dstrect.y;
	}
	else {
		minxy.y = dstrect.y;
		maxxy.y = dstrect.y + dstrect.w;
	}

	v[0].tex_coord = minuv;
	v[1].tex_coord = maxuv;
	v[2].tex_coord = maxuv;
	v[3].tex_coord = minuv;

	/* apply rotation with 2x2 matrix ( c -s )
	 *                                ( s  c ) */
	s_minx = s * (minxy.x - centerx);
	s_miny = s * (minxy.y - centery);
	s_maxx = s * (maxxy.x - centerx);
	s_maxy = s * (maxxy.y - centery);
	c_minx = c * (minxy.x - centerx);
	c_miny = c * (minxy.y - centery);
	c_maxx = c * (maxxy.x - centerx);
	c_maxy = c * (maxxy.y - centery);

	// (minx, miny)
	v[0].position = glm::vec2((c_minx - s_miny) + centerx, (s_minx + c_miny) + centery);
	// (maxx, miny)
	v[1].position = glm::vec2((c_maxx - s_miny) + centerx, (s_maxx + c_miny) + centery);
	// (maxx, maxy)
	v[2].position = glm::vec2((c_maxx - s_maxy) + centerx, (s_maxx + c_maxy) + centery);
	// (minx, maxy)
	v[3].position = glm::vec2((c_minx - s_maxy) + centerx, (s_minx + c_maxy) + centery);
	auto c4 = (col);
	v[0].color = c4;
	v[1].color = c4;
	v[2].color = c4;
	v[3].color = c4;
	add(img, v, 4, rect_index_order, 6, clip);
}


#endif // 1

ovg_draw_data_t get_draw_list(rvg_t* p0)
{
	rvg_cx* p = (rvg_cx*)p0;
	ovg_draw_data_t ret = {};
	if (p)
	{
		ret.d = p->cmdlist.data(); ret.count = p->cmdlist.size();
		ret.vg_vertex = (ovgVertex*)p->_vertex.data();
		ret.v_count = p->_vertex.size();
		ret.vg_indices = p->_indices.data();
		ret.i_count = p->_indices.size();
		ret.uboCount = p->gCount;
		ret.vertex1 = (geomVertex1*)p->gps.vd1.data();
		ret.v1_count = p->gps.vd1.size();
		ret.vertex2 = (geomVertex2*)p->gps.vd2.data();
		ret.v2_count = p->gps.vd2.size();
		ret.geom_indices = p->gps.ids.data();
		ret.ig_count = p->gps.ids.size();
		ret.instance_count = p->gps.instance_mat.size();
		ret.instance_data = p->gps.instance_mat.data();
		p->_images_up.clear();
		for (auto& [k, v] : p->_images) {
			v.img = k;
			p->_images_up.push_back(&v);
		}
		ret.image_desc = p->_images_up.data();
		ret.image_desc_count = p->_images_up.size();
		ret.pipeinfo = p->gps.build_pipes.data();
		ret.pipeinfo_count = p->gps.build_pipes.size();
	}
	return ret;
}

// cmd ctx
#if 1

rvg_t* vctx_new_rvg(mem_resource_t* ac);
void  vctx_destroy_rvg(rvg_t* p);
void vctx_clear(rvg_t* v);			// 清空画布 
// 路径操作
ovg_path_t* vctx_get_path(rvg_t* ctx);
void  vctx_new_path(rvg_t* ctx);
void vctx_clear_path(rvg_t* ctx);
void vctx_close_path(rvg_t* ctx);
void vctx_new_sub_path(rvg_t* ctx);
void vctx_path_extents(rvg_t* ctx, float* x1, float* y1, float* x2, float* y2);
void vctx_get_current_point(rvg_t* ctx, float* x, float* y);
size_t vctx_get_segment_count(rvg_t* ctx);
void vctx_set_segment_color(rvg_t* ctx, size_t idx, uint32_t color);
// 添加数据到当前路径，参考path_type_e
void vctx_add_path(rvg_t* ctx, float* data, size_t count);
void vctx_move_to(rvg_t* ctx, float x, float y);
void vctx_rel_move_to(rvg_t* ctx, float x, float y);
void vctx_line_to(rvg_t* ctx, float x, float y);
void vctx_rel_line_to(rvg_t* ctx, float dx, float dy);
void vctx_arc(rvg_t* ctx, float xc, float yc, float radius, float a1, float a2);
void vctx_arc_negative(rvg_t* ctx, float xc, float yc, float radius, float a1, float a2);
// 有缩放时，先执行set_path一次再执行curve_to
void vctx_curve_to(rvg_t* ctx, float x1, float y1, float x2, float y2, float x3, float y3);
void vctx_rel_curve_to(rvg_t* ctx, float x1, float y1, float x2, float y2, float x3, float y3);
void vctx_quadratic_to(rvg_t* ctx, float x1, float y1, float x2, float y2);
void vctx_rel_quadratic_to(rvg_t* ctx, float x1, float y1, float x2, float y2);
void vctx_rectangle(rvg_t* ctx, float x, float y, float w, float h);
void vctx_rounded_rectangle(rvg_t* ctx, float x, float y, float w, float h, float radius);
void vctx_rounded_rectangle2(rvg_t* ctx, float x, float y, float w, float h, float rx, float ry);
void vctx_ellipse(rvg_t* ctx, float radiusX, float radiusY, float x, float y, float rotationAngle);
void vctx_elliptic_arc_to(rvg_t* ctx, float x, float y, bool large_arc_flag, bool sweep_flag, float rx, float ry, float phi);
void vctx_rel_elliptic_arc_to(rvg_t* ctx, float x, float y, bool large_arc_flag, bool sweep_flag, float rx, float ry, float phi);
void vctx_circle(rvg_t* ctx, float x, float y, float radius);
// 配置
void vctx_set_opacity(rvg_t* ctx, float opacity);
void vctx_set_source_color(rvg_t* ctx, uint32_t c);
void vctx_set_source_rgba(rvg_t* ctx, float r, float g, float b, float a);
void vctx_set_source_rgb(rvg_t* ctx, float r, float g, float b);
void vctx_set_line_width(rvg_t* ctx, float width);
void vctx_set_miter_limit(rvg_t* ctx, float limit);
void vctx_set_line_cap(rvg_t* ctx, int cap);
void vctx_set_line_join(rvg_t* ctx, int join);
void vctx_set_source_surface(rvg_t* ctx, vg_image_t* surf, float x, float y);
void vctx_set_source(rvg_t* ctx, vg_pattern_t* pat);
void vctx_set_operator(rvg_t* ctx, int op);
void vctx_set_fill_rule(rvg_t* ctx, int fr);
void vctx_set_dash(rvg_t* ctx, const float* dashes, uint32_t num_dashes, float offset);		// 虚线
void vctx_set_dash8(rvg_t* ctx, uint64_t dashes, uint32_t num_dashes, float offset);								// 虚线,用uint8_t v8[8]表示
void vctx_translate(rvg_t* ctx, float dx, float dy);
void vctx_scale(rvg_t* ctx, float sx, float sy);
void vctx_rotate(rvg_t* ctx, float radians);
void vctx_transform(rvg_t* ctx, const void* matrix);
void vctx_set_matrix(rvg_t* ctx, const void* matrix);
void vctx_get_matrix(rvg_t* ctx, void* matrix);
void vctx_identity_matrix(rvg_t* ctx);
void vctx_matrix_init(void* mat, float xx, float yx, float xy, float yy, float x0, float y0);

// 图案：渐变/图片 
vg_pattern_t* vctx_new_pattern_linear(rvg_t* ctx, float x0, float y0, float x1, float y1);
vg_pattern_t* vctx_new_pattern_radial(rvg_t* ctx, float cx0, float cy0, float radius0, float cx1, float cy1, float radius1, bool is_ellipse);
vg_pattern_t* vctx_new_pattern_sweep(rvg_t* ctx, float cx, float cy, float start_angle, float end_angle);
int  vctx_pattern_add_color_stop(vg_pattern_t* pat, float o, float r, float g, float b, float a);
int  vctx_pattern_set_color_stop(vg_pattern_t* pat, int idx, float o, float r, float g, float b, float a);
void vctx_pattern_set_matrix(vg_pattern_t* pat, const void* matrix);	// mat3x2
void vctx_pattern_set_extend(vg_pattern_t* pat, int extend);
void vctx_pattern_set_filter(vg_pattern_t* pat, int filter);

void vctx_save(rvg_t* v);
void vctx_restore(rvg_t* v);
void vctx_stroke(rvg_t* v);
void vctx_stroke_preserve(rvg_t* v);
void vctx_fill(rvg_t* v);
void vctx_fill_preserve(rvg_t* v);
void vctx_paint(rvg_t* v);			// 全屏渲染
void vctx_reset_clip(rvg_t* v, uint8_t ref);	// 重置裁剪
void vctx_clip(rvg_t* v);			// 路径裁剪，清空当前路径
void vctx_clip_preserve(rvg_t* v);	// 路径裁剪
void vctx_clip_rect(rvg_t* v, int x, int y, int width, int height);	// 矩形裁剪
void vctx_set_clip_rect(rvg_t* v, void* rc);	// 矩形裁剪,int[4]
void vctx_get_clip_rect(rvg_t* v, void* rc);	// 获取矩形裁剪

// 添加文本，风格，渲染区可选
void  vctx_add_text(rvg_t* dc, text_st_t* p, text_style_t* ts, text_box_rt* box);
// 普通图片，支持九宫格、混合颜色
void  vctx_add_image(rvg_t* dc, ovg_image_r* r);
// 原始三角形，输入0则不修改
void  vctx_set_geom_state(rvg_t* dc, gem_info_t* info, const void* matrix4x4);
// 添加几何数据到缓冲区，xy顶点坐标，color顶点颜色，uv顶点纹理坐标，indices索引数据，color_type=0表示float4，1表示uint32_t
void  vctx_add_geometry(rvg_t* dc, vg_image_t* texture, const float* xy, int xy_stride, const void* color, int color_stride, const float* uv, int uv_stride, int num_vertices, const void* indices, int num_indices, int size_indices, int color_type);
// 添加3D几何数据到缓冲区，xyz顶点坐标，color顶点颜色（双面则要双倍），uv顶点纹理坐标，indices索引数据
void  vctx_add_geometry3d(rvg_t* dc, vg_image_t* texture, const float* xyz, int xyz_stride, const void* color, int color_stride, const float* uv, int uv_stride, int num_vertices, const void* indices, int num_indices, int size_indices, int color_type);

void vctx_start_recording(rvg_t* ctx);
ovg_recording_t* vctx_stop_recording(rvg_t* ctx);
void vctx_replay(rvg_t* ctx, ovg_recording_t* rec);
void vctx_replay_command(rvg_t* ctx, ovg_recording_t* rec, uint32_t cmdIndex);
uint32_t vctx_recording_get_count(ovg_recording_t* rec);
void* vctx_recording_get_data(ovg_recording_t* rec);
void  vctx_recording_destroy(ovg_recording_t* rec);
// familys多个字体时用小写逗号分隔，style逗号分隔字体的风格(可空) 比如(char*)u8"Consolas,新宋体,Segoe UI Emoji


// TODO 命令模式实现
#define PRI2CTX(ac) 
rvg_t* vctx_new_rvg(mem_resource_t* ac) {
	auto r = (rvg_cx*)ovg_new_rvg(ac);
	if (r)
	{
		r->path = ovg_new_path(ac);
		r->st = r->new_state();
		assert(r->path && r->st);
	}
	return r;
}
void  vctx_destroy_rvg(rvg_t* p) {
	ovg_destroy_rvg(p);
}
void vctx_clear(rvg_t* v) {
	ovg_clear(v);
}
ovg_path_t* vctx_get_path(rvg_t* ctx) {
	return ctx->path;
}
void  vctx_new_path(rvg_t* ctx) {
	if (ctx)
		ovg_clear_path(ctx->path);
}
void vctx_clear_path(rvg_t* ctx) {
	vctx_new_path(ctx);
}
void vctx_close_path(rvg_t* ctx) {
	if (ctx)ovg_close_path(ctx->path);
}
void vctx_new_sub_path(rvg_t* ctx) {
	if (ctx)ovg_new_sub_path(ctx->path);
}
void vctx_path_extents(rvg_t* ctx, float* x1, float* y1, float* x2, float* y2) {
	if (ctx)ovg_path_extents(ctx->path, x1, y1, x2, y2);
}
void vctx_get_current_point(rvg_t* ctx, float* x, float* y) {
	if (ctx)ovg_get_current_point(ctx->path, x, y);
}
size_t vctx_get_segment_count(rvg_t* ctx) {
	return ctx ? ovg_get_segment_count(ctx->path) : 0;
}
void vctx_set_segment_color(rvg_t* ctx, size_t idx, uint32_t color) {
	if (ctx)ovg_set_segment_color(ctx->path, idx, color);
}
// 添加数据到当前路径，参考path_type_e
void vctx_add_path(rvg_t* ctx, float* data, size_t count) {
	if (ctx)ovg_add_path(ctx->path, data, count);
}
void vctx_move_to(rvg_t* ctx, float x, float y) {
	if (ctx)ovg_move_to(ctx->path, x, y);
}
void vctx_rel_move_to(rvg_t* ctx, float x, float y) {
	if (!ctx) return;
	float cx, cy;
	ovg_get_current_point(ctx->path, &cx, &cy);
	ovg_move_to(ctx->path, cx + x, cy + y);
}

void vctx_line_to(rvg_t* ctx, float x, float y) {
	if (ctx) ovg_line_to(ctx->path, x, y);
}

void vctx_rel_line_to(rvg_t* ctx, float dx, float dy) {
	if (!ctx) return;
	float cx, cy;
	ovg_get_current_point(ctx->path, &cx, &cy);
	ovg_line_to(ctx->path, cx + dx, cy + dy);
}

void vctx_arc(rvg_t* ctx, float xc, float yc, float radius, float a1, float a2) {
	if (ctx) ovg_arc(ctx->path, xc, yc, radius, a1, a2);
}

void vctx_arc_negative(rvg_t* ctx, float xc, float yc, float radius, float a1, float a2) {
	if (ctx) ovg_arc_negative(ctx->path, xc, yc, radius, a1, a2);
}

void vctx_curve_to(rvg_t* ctx, float x1, float y1,
	float x2, float y2,
	float x3, float y3) {
	if (ctx) ovg_curve_to(ctx->path, x1, y1, x2, y2, x3, y3);
}

void vctx_rel_curve_to(rvg_t* ctx, float x1, float y1,
	float x2, float y2,
	float x3, float y3) {
	if (!ctx) return;
	float cx, cy;
	ovg_get_current_point(ctx->path, &cx, &cy);
	ovg_curve_to(ctx->path,
		cx + x1, cy + y1,
		cx + x2, cy + y2,
		cx + x3, cy + y3);
}

void vctx_quadratic_to(rvg_t* ctx, float x1, float y1, float x2, float y2) {
	if (ctx) ovg_quadratic_to(ctx->path, x1, y1, x2, y2);
}

void vctx_rel_quadratic_to(rvg_t* ctx, float x1, float y1, float x2, float y2) {
	if (!ctx) return;
	float cx, cy;
	ovg_get_current_point(ctx->path, &cx, &cy);
	ovg_quadratic_to(ctx->path,
		cx + x1, cy + y1,
		cx + x2, cy + y2);
}

void vctx_rectangle(rvg_t* ctx, float x, float y, float w, float h) {
	if (ctx) ovg_rectangle(ctx->path, x, y, w, h);
}

void vctx_rounded_rectangle(rvg_t* ctx, float x, float y,
	float w, float h, float radius) {
	if (ctx) ovg_rounded_rectangle(ctx->path, x, y, w, h, radius);
}

void vctx_rounded_rectangle2(rvg_t* ctx, float x, float y,
	float w, float h, float rx, float ry) {
	if (ctx) ovg_rounded_rectangle2(ctx->path, x, y, w, h, rx, ry);
}

void vctx_ellipse(rvg_t* ctx, float radiusX, float radiusY,
	float x, float y, float rotationAngle) {
	if (ctx) ovg_ellipse(ctx->path, radiusX, radiusY, x, y, rotationAngle);
}

void vctx_elliptic_arc_to(rvg_t* ctx, float x, float y,
	bool large_arc_flag, bool sweep_flag,
	float rx, float ry, float phi) {
	if (ctx) ovg_elliptic_arc_to(ctx->path, x, y,
		large_arc_flag, sweep_flag,
		rx, ry, phi);
}

void vctx_rel_elliptic_arc_to(rvg_t* ctx, float x, float y,
	bool large_arc_flag, bool sweep_flag,
	float rx, float ry, float phi) {
	if (!ctx) return;
	float cx, cy;
	ovg_get_current_point(ctx->path, &cx, &cy);
	ovg_elliptic_arc_to(ctx->path,
		cx + x, cy + y,
		large_arc_flag, sweep_flag,
		rx, ry, phi);
}

void vctx_circle(rvg_t* ctx, float x, float y, float radius) {
	if (ctx) ovg_circle(ctx->path, x, y, radius);
}
// 配置 
void vctx_set_opacity(rvg_t* ctx, float opacity) {
	if (ctx) ovg_set_opacity(ctx->st, opacity);
}

void vctx_set_source_color(rvg_t* ctx, uint32_t c) {
	if (ctx) ovg_set_source_color(ctx->st, c);
}

void vctx_set_source_rgba(rvg_t* ctx, float r, float g, float b, float a) {
	if (ctx) ovg_set_source_rgba(ctx->st, r, g, b, a);
}

void vctx_set_source_rgb(rvg_t* ctx, float r, float g, float b) {
	if (ctx) ovg_set_source_rgba(ctx->st, r, g, b, 1.0f);
}

void vctx_set_line_width(rvg_t* ctx, float width) {
	if (ctx) ovg_set_line_width(ctx->st, width);
}

void vctx_set_miter_limit(rvg_t* ctx, float limit) {
	if (ctx) ovg_set_miter_limit(ctx->st, limit);
}

void vctx_set_line_cap(rvg_t* ctx, int cap) {
	if (ctx) ovg_set_line_cap(ctx->st, cap);
}

void vctx_set_line_join(rvg_t* ctx, int join) {
	if (ctx) ovg_set_line_join(ctx->st, join);
}

void vctx_set_source_surface(rvg_t* ctx, vg_image_t* surf, float x, float y) {
	if (ctx) ovg_set_source_surface(ctx->st, surf, x, y);
}

void vctx_set_source(rvg_t* ctx, vg_pattern_t* pat) {
	if (ctx) ovg_set_source(ctx->st, pat);
}

void vctx_set_operator(rvg_t* ctx, int op) {
	if (ctx) ovg_set_operator(ctx->st, op);
}

void vctx_set_fill_rule(rvg_t* ctx, int fr) {
	if (ctx) ovg_set_fill_rule(ctx->st, fr);
}

void vctx_set_dash(rvg_t* v0, const float* dashes, uint32_t num_dashes, float offset) {
	auto ctx = (rvg_cx*)v0;
	if (!ctx || !dashes)return;
	auto t = ctx->st;
	if (!dashes || !num_dashes) {
		t->dashCount = 0;
	}
	if (!t->dashes || t->dashCount != num_dashes)
	{
		if (t->dashes)
			ctx->ac->free_mem(t->dashes, t->dashCount);
		t->dashes = (float*)ctx->ac->allocate(sizeof(float) * num_dashes);
	}
	t->dashCount = num_dashes;
	t->dashOffset = offset;
	if (t->dashes)
		memcpy(t->dashes, dashes, sizeof(float) * t->dashCount);
	else
		t->dashCount = 0;
}

void vctx_set_dash8(rvg_t* ctx, uint64_t dashes0, uint32_t num_dashes, float offset) {
	if (ctx) {
		float dashes[64] = {};
		uint64_t x = 1;
		auto t = dashes;
		auto v8 = (uint8_t*)&dashes0;
		if (num_dashes > 64)num_dashes = 64;
		{
			if (num_dashes > 8)num_dashes = 8;
			for (size_t i = 0; i < num_dashes; i++)
			{
				*t = v8[i]; t++;
			}
			if (num_dashes > 0)
				vctx_set_dash(ctx, dashes, num_dashes, offset);
		}
	}
}

void vctx_translate(rvg_t* ctx, float dx, float dy) {
	if (ctx) ovg_translate(ctx->st, dx, dy);
}

void vctx_scale(rvg_t* ctx, float sx, float sy) {
	if (ctx) ovg_scale(ctx->st, sx, sy);
}

void vctx_rotate(rvg_t* ctx, float radians) {
	if (ctx) ovg_rotate(ctx->st, radians);
}

void vctx_transform(rvg_t* ctx, const void* matrix) {
	if (ctx) ovg_transform(ctx->st, matrix);
}

void vctx_set_matrix(rvg_t* ctx, const void* matrix) {
	if (ctx) ovg_set_matrix(ctx->st, matrix);
}

void vctx_get_matrix(rvg_t* ctx, void* matrix) {
	if (ctx) ovg_get_matrix(ctx->st, matrix);
}

void vctx_identity_matrix(rvg_t* ctx) {
	if (ctx) ovg_identity_matrix(ctx->st);
}
void vctx_matrix_init(void* mat, float xx, float yx, float xy, float yy, float x0, float y0) {
	if (mat) { ovg_matrix_init(mat, xx, yx, xy, yy, x0, y0); }
}
typedef glm::mat3x2 ovg_matrix_t;
// 图案：渐变/图片 
vg_pattern_t* vctx_new_pattern_linear(rvg_t* v0, float x0, float y0, float x1, float y1) {
	auto ctx = (rvg_cx*)v0;
	if (!ctx)return 0;
	auto pat = (pat_act*)ctx->mac.allocate(sizeof(pat_act));
	if (!pat) {
		return 0;
	}
	*pat = {};
	pat->type = vg_pattern_type_t::VG_PATTERN_TYPE_LINEAR;
	pat->extend = vg_extend_t::VG_EXTEND_NONE;
	pat->data = &pat->g;
	_vg_pattern_edit_linear(pat, x0, y0, x1, y1);
	pat->matrix = glm::mat3x2(1.0);
	pat->references = 1;
	return pat;
}
vg_pattern_t* vctx_new_pattern_radial(rvg_t* v0, float cx0, float cy0, float radius0, float cx1, float cy1, float radius1, bool is_ellipse) {
	auto ctx = (rvg_cx*)v0;
	if (!ctx)return 0;
	auto pat = (pat_act*)ctx->mac.allocate(sizeof(pat_act));
	if (!pat) {
		return 0;
	}
	*pat = {};
	pat->type = vg_pattern_type_t::VG_PATTERN_TYPE_RADIAL;
	pat->extend = vg_extend_t::VG_EXTEND_NONE;
	pat->data = &pat->g;
	pat->matrix = glm::mat3x2(1.0);
	vg_pattern_edit_radial(pat, cx0, cy0, radius0, cx1, cy1, radius1, is_ellipse);
	pat->references = 1;
	return pat;
}
vg_pattern_t* vctx_new_pattern_sweep(rvg_t* v0, float cx, float cy, float start_angle, float end_angle) {
	auto ctx = (rvg_cx*)v0;
	if (!ctx)return 0;
	auto pat = (pat_act*)ctx->mac.allocate(sizeof(pat_act));
	if (!pat) {
		return 0;
	}
	*pat = {};
	pat->type = vg_pattern_type_t::VG_PATTERN_TYPE_SWEEP;
	pat->extend = vg_extend_t::VG_EXTEND_NONE;
	pat->data = &pat->g;
	pat->matrix = glm::mat3x2(1.0);
	vg_pattern_edit_sweep(pat, cx, cy, start_angle, end_angle);
	pat->references = 1;
	return pat;
}
int  vctx_pattern_add_color_stop(vg_pattern_t* pat, float o, float r, float g, float b, float a) {
	if (!pat) return -1;
	return ovg_pattern_add_color_stop(pat, o, r, g, b, a);
}
int  vctx_pattern_set_color_stop(vg_pattern_t* pat, int idx, float o, float r, float g, float b, float a) {
	if (!pat || idx < 0) return -1;
	return ovg_pattern_set_color_stop(pat, idx, o, r, g, b, a);
}
void vctx_pattern_set_matrix(vg_pattern_t* pat, const void* matrix) {
	if (pat) ovg_pattern_set_matrix(pat, (const ovg_matrix_t*)matrix);
}
void vctx_pattern_set_extend(vg_pattern_t* pat, int extend) {
	if (pat) ovg_pattern_set_extend(pat, extend);
}
void vctx_pattern_set_filter(vg_pattern_t* pat, int filter) {
	if (pat) ovg_pattern_set_filter(pat, filter);
}

void vctx_save(rvg_t* v0) {
	auto v = (rvg_cx*)v0;
	if (v)v->save();
}
void vctx_restore(rvg_t* v0) {
	auto v = (rvg_cx*)v0;
	if (v)v->restore();
}
void vctx_stroke(rvg_t* v) {
	if (v) ovg_stroke(v);
}
void vctx_stroke_preserve(rvg_t* v) {
	if (v) ovg_stroke_preserve(v);
}

void vctx_fill(rvg_t* v) {
	if (v) ovg_fill(v);
}

void vctx_fill_preserve(rvg_t* v) {
	if (v) ovg_fill_preserve(v);
}

void vctx_paint(rvg_t* v) {
	if (v) ovg_paint(v);
}

void vctx_reset_clip(rvg_t* v, uint8_t ref) {
	if (v) ovg_reset_clip(v, ref);
}

void vctx_clip(rvg_t* v) {
	if (v) ovg_clip(v);
}

void vctx_clip_preserve(rvg_t* v) {
	if (v) ovg_clip_preserve(v);
}

void vctx_clip_rect(rvg_t* v, int x, int y, int width, int height) {
	if (v) ovg_clip_rect(v, x, y, width, height);
}

void vctx_set_clip_rect(rvg_t* v, void* rc) {
	if (v && rc) ovg_set_clip_rect(v, rc);
}

void vctx_get_clip_rect(rvg_t* v, void* rc) {
	if (v && rc) ovg_get_clip_rect(v, rc);
}

/* ================= 高层绘制命令 ================= */

// 添加文本，风格，渲染区可选
void vctx_add_text(rvg_t* v0, text_st_t* p, text_style_t* ts, text_box_rt* box) {

	auto dc = (rvg_cx*)v0;
	if (dc) {
		auto cb = (ovg_ctx_cx*)dc->ac->ptr;
		if (cb)
			cb->add_text(v0, p, ts, box);
	}
}

// 普通图片，支持九宫格、混合颜色
void vctx_add_image(rvg_t* dc, ovg_image_r* r) {
	if (dc) ovg_add_image(dc, r);
}

// 原始三角形，输入0则不修改
void vctx_set_geom_state(rvg_t* dc, gem_info_t* info, const void* matrix4x4) {
	if (dc) ovg_set_geom_state(dc, info, (glm::mat4*)matrix4x4);
}

// 添加几何数据到缓冲区
void vctx_add_geometry(rvg_t* dc,
	vg_image_t* texture,
	const float* xy, int xy_stride,
	const void* color, int color_stride,
	const float* uv, int uv_stride,
	int num_vertices,
	const void* indices, int num_indices,
	int size_indices, int color_type) {
	if (dc) {
		ovg_add_geometry(dc,
			texture,
			xy, xy_stride,
			color, color_stride,
			uv, uv_stride,
			num_vertices,
			indices, num_indices,
			size_indices, color_type);
	}
}

// 添加3D几何数据到缓冲区
void vctx_add_geometry3d(rvg_t* dc,
	vg_image_t* texture,
	const float* xyz, int xyz_stride,
	const void* color, int color_stride,
	const float* uv, int uv_stride,
	int num_vertices,
	const void* indices, int num_indices,
	int size_indices, int color_type) {
	if (dc) {
		ovg_add_geometry3d(dc,
			texture,
			xyz, xyz_stride,
			color, color_stride,
			uv, uv_stride,
			num_vertices,
			indices, num_indices,
			size_indices, color_type);
	}
}


void vctx_start_recording(rvg_t* ctx) {

}
ovg_recording_t* vctx_stop_recording(rvg_t* ctx) {
	return 0;
}
void vctx_replay(rvg_t* ctx, ovg_recording_t* rec) {

}
void vctx_replay_command(rvg_t* ctx, ovg_recording_t* rec, uint32_t cmdIndex) {

}
uint32_t vctx_recording_get_count(ovg_recording_t* rec) {
	return 0;
}
void* vctx_recording_get_data(ovg_recording_t* rec) {
	return 0;
}
void  vctx_recording_destroy(ovg_recording_t* rec) {

}



void init_ovg_ctx_cb(ovg_ctx_cb* cb)
{
	if (!cb)return;
	cb->new_rvg = vctx_new_rvg;
	cb->destroy_rvg = vctx_destroy_rvg;
	cb->clear = vctx_clear;
	cb->get_path = vctx_get_path;
	cb->new_path = vctx_new_path;
	cb->clear_path = vctx_clear_path;
	cb->close_path = vctx_close_path;
	cb->new_sub_path = vctx_new_sub_path;
	cb->path_extents = vctx_path_extents;
	cb->get_current_point = vctx_get_current_point;
	cb->get_segment_count = vctx_get_segment_count;
	cb->set_segment_color = vctx_set_segment_color;
	cb->add_path = vctx_add_path;
	cb->move_to = vctx_move_to;
	cb->rel_move_to = vctx_rel_move_to;
	cb->line_to = vctx_line_to;
	cb->rel_line_to = vctx_rel_line_to;
	cb->arc = vctx_arc;
	cb->arc_negative = vctx_arc_negative;
	cb->curve_to = vctx_curve_to;
	cb->rel_curve_to = vctx_rel_curve_to;
	cb->quadratic_to = vctx_quadratic_to;
	cb->rel_quadratic_to = vctx_rel_quadratic_to;
	cb->rectangle = vctx_rectangle;
	cb->rounded_rectangle = vctx_rounded_rectangle;
	cb->rounded_rectangle2 = vctx_rounded_rectangle2;
	cb->ellipse = vctx_ellipse;
	cb->elliptic_arc_to = vctx_elliptic_arc_to;
	cb->rel_elliptic_arc_to = vctx_rel_elliptic_arc_to;
	cb->circle = vctx_circle;
	cb->set_opacity = vctx_set_opacity;
	cb->set_source_color = vctx_set_source_color;
	cb->set_source_rgba = vctx_set_source_rgba;
	cb->set_source_rgb = vctx_set_source_rgb;
	cb->set_line_width = vctx_set_line_width;
	cb->set_miter_limit = vctx_set_miter_limit;
	cb->set_line_cap = vctx_set_line_cap;
	cb->set_line_join = vctx_set_line_join;
	cb->set_source_surface = vctx_set_source_surface;
	cb->set_source = vctx_set_source;
	cb->set_operator = vctx_set_operator;
	cb->set_fill_rule = vctx_set_fill_rule;
	cb->set_dash = vctx_set_dash;
	cb->set_dash8 = vctx_set_dash8;
	cb->translate = vctx_translate;
	cb->scale = vctx_scale;
	cb->rotate = vctx_rotate;
	cb->transform = vctx_transform;
	cb->set_matrix = vctx_set_matrix;
	cb->get_matrix = vctx_get_matrix;
	cb->identity_matrix = vctx_identity_matrix;
	cb->matrix_init = vctx_matrix_init;
	cb->new_pattern_linear = vctx_new_pattern_linear;
	cb->new_pattern_radial = vctx_new_pattern_radial;
	cb->new_pattern_sweep = vctx_new_pattern_sweep;
	cb->pattern_add_color_stop = vctx_pattern_add_color_stop;
	cb->pattern_set_color_stop = vctx_pattern_set_color_stop;
	cb->pattern_set_matrix = vctx_pattern_set_matrix;
	cb->pattern_set_extend = vctx_pattern_set_extend;
	cb->pattern_set_filter = vctx_pattern_set_filter;

	cb->image_update = ovg_image_update;
	cb->image_destroy = ovg_image_destroy;

	cb->save = vctx_save;
	cb->restore = vctx_restore;
	cb->stroke = vctx_stroke;
	cb->stroke_preserve = vctx_stroke_preserve;
	cb->fill = vctx_fill;
	cb->fill_preserve = vctx_fill_preserve;
	cb->paint = vctx_paint;
	cb->reset_clip = vctx_reset_clip;
	cb->clip = vctx_clip;
	cb->clip_preserve = vctx_clip_preserve;
	cb->clip_rect = vctx_clip_rect;
	cb->set_clip_rect = vctx_set_clip_rect;
	cb->get_clip_rect = vctx_get_clip_rect;
	cb->add_text = vctx_add_text;
	cb->add_image = vctx_add_image;
	cb->set_geom_state = vctx_set_geom_state;
	cb->set_instance_mat = (size_t(*)(rvg_t*, const void*, size_t)) ovg_set_instance_mat;
	cb->add_geometry = vctx_add_geometry;
	cb->add_geometry3d = vctx_add_geometry3d;
	cb->start_recording = vctx_start_recording;
	cb->stop_recording = vctx_stop_recording;
	cb->replay = vctx_replay;
	cb->replay_command = vctx_replay_command;
	cb->recording_get_count = vctx_recording_get_count;
	cb->recording_get_data = vctx_recording_get_data;
	cb->recording_destroy = vctx_recording_destroy;

}
#endif // 1


void draw_grid_fill(rvg_t* vg, glm::vec2 size, glm::ivec2 cols, int width)
{
	int x = fmod(size.x, width);
	int y = fmod(size.y, width);
	int xn = size.x / width;
	int yn = size.y / width;
	if (x > 0)xn++;
	if (y > 0)yn++;

	vctx_rectangle(vg, 0, 0, size.x, size.y);
	vctx_clip(vg);
	auto c = cols[0];
	vctx_set_source_color(vg, c);
	for (size_t i = 0; i < yn; i++)
	{
		auto iw = i * width;
		for (size_t j = 0; j < xn; j++)
		{
			bool k0 = (j & 1);
			bool k1 = !(j & 1);
			auto k = !(i & 1) ? k0 : k1;
			if (k)
			{
				vctx_rectangle(vg, j * width, iw, width, width);
			}
		}
	}
	vctx_fill(vg);
	c = cols[1];
	vctx_set_source_color(vg, c);
	for (size_t i = 0; i < yn; i++)
	{
		auto iw = i * width;
		for (size_t j = 0; j < xn; j++)
		{
			bool k0 = (j & 1);
			bool k1 = !(j & 1);
			auto k = (i & 1) ? k0 : k1;
			if (k)
			{
				vctx_rectangle(vg, j * width, iw, width, width);
			}
		}
	}
	vctx_fill(vg);
}


// flex.c


#ifndef NO_FLEX_IMP


class flex_item :public flex_data
{
public:
	// size[0] == width, size[1] == height
	typedef void (*flex_self_sizing)(flex_item* item, float* size);
	void* managed_ptr = NULL;	// 用户数据指针
	flex_self_sizing self_sizing = NULL; // 运行时计算大小
	float frame[4] = {};	// 输出坐标、大小
	flex_item* parent = 0;	// 父级
	size_t line_count = 0;
	std::pmr::vector<flex_item*> children;	// 子级  
	std::pmr::vector<char> temp_layout;
public:
	flex_item();
	~flex_item();

	void init();
	void setdata(flex_data* d);
	void update_should_order_children();	// 子元素属性改变时执行

	void item_add(flex_item* child);
	void item_insert(uint32_t index, flex_item* child);
	flex_item* item_delete(uint32_t index);
	flex_item* detach(flex_item* child);
	// 清空子元素
	void clear();
	// 执行布局计算
	void layout();
private:
	void layout_items(uint32_t child_begin, uint32_t child_end, uint32_t children_count, struct flex_layout* layout, uint32_t last_count);
	void layout_item(float width, float height);
};

flex_item::flex_item()
{}

flex_item::~flex_item()
{}



void flex_item::update_should_order_children()
{
	if (order != 0 && parent != NULL) {
		parent->should_order_children = true;
	}
}

void flex_item::init()
{
	children.clear();
	temp_layout.clear();
	parent = NULL;
	should_order_children = true;
	return;
}

void flex_item::setdata(flex_data* d)
{
	*((flex_data*)this) = *d;
}

void flex_item::item_add(flex_item* child)
{
	flex_item* item = this;
	children.push_back(child);
	child->parent = item;
	should_order_children = true;
	child->update_should_order_children();
}


void flex_item::item_insert(uint32_t index, flex_item* child)
{
	flex_item* item = this;
	children.insert(children.begin() + index, child);
	child->parent = item;
	child->update_should_order_children();
}


flex_item* flex_item::item_delete(uint32_t index)
{
	flex_item* child = children.data()[index];
	children.erase(children.begin() + index);
	if (child)
		child->parent = 0;
	return child;
}

flex_item* flex_item::detach(flex_item* c)
{
	size_t i = 0;
	flex_item* child = 0;
	auto p = children.data();
	for (i = 0; i < children.size(); i++)
	{
		if (p[i] == c)
		{
			child = c;
			break;
		}
	}
	children.erase(children.begin() + i);
	if (child)
		child->parent = 0;
	return child;
}

void flex_item::clear()
{
	children.clear();
}


flex_item* flex_item_root(flex_item* item)
{
	while (item->parent != NULL) {
		item = item->parent;
	}
	return item;
}
//
//#define FRAME_GETTER(name, index) \
//     float flex_item_get_frame_##name(flex_item *item) { \
//        return item->frame[index]; \
//    }
//
//FRAME_GETTER(x, 0)
//FRAME_GETTER(y, 1)
//FRAME_GETTER(width, 2)
//FRAME_GETTER(height, 3)
//
//#undef FRAME_GETTER

struct flex_layout {
	// Set during init.
	bool wrap;
	bool reverse;               // whether main axis is reversed
	bool reverse2;              // whether cross axis is reversed (wrap only)
	bool vertical;
	float size_dim;             // main axis parent size
	float align_dim;            // cross axis parent size
	uint32_t frame_pos_i;   // main axis position
	uint32_t frame_pos2_i;  // cross axis position
	uint32_t frame_size_i;  // main axis size
	uint32_t frame_size2_i; // cross axis size
	uint32_t* ordered_indices;
	size_t ordered_count;
	// Set for each line layout.
	float line_dim;             // the cross axis size
	float flex_dim;             // the flexible part of the main axis size
	float extra_flex_dim;       // sizes of flexible items
	float flex_grows;
	float flex_shrinks;
	float pos2;                 // cross axis position
	float baseline;
	// Calculated layout lines - only tracked when needed:
	//   - if the root's align_content property isn't set to FLEX_ALIGN_START
	//   - or if any child item doesn't have a cross-axis size set
	bool need_lines;
	struct flex_layout_line {
		uint32_t child_begin;
		uint32_t child_end;
		float size;
	};
	flex_layout_line* lines;
	size_t lines_idx;
	//uint32_t lines_count;
	float lines_sizes;
	//uint32_t lines_cap;
	size_t lines_idx0;
};

flex_align child_align(flex_item* child, flex_item* parent)
{
	auto align = child->align_self;
	if (align == flex_align::ALIGN_AUTO && parent) {
		align = parent->align_items;
	}
	return align;
}

void layout_init(flex_item* item, float width, float height, struct flex_layout* layout)
{
	assert(item->padding_left >= 0);
	assert(item->padding_right >= 0);
	assert(item->padding_top >= 0);
	assert(item->padding_bottom >= 0);
	width -= item->padding_left + item->padding_right;
	height -= item->padding_top + item->padding_bottom;
	assert(width >= 0);
	assert(height >= 0);

	layout->reverse = false;
	layout->vertical = true;
	switch (item->direction) {
	case flex_direction::ROW_REVERSE:
		layout->reverse = true;
	case flex_direction::ROW:
		layout->vertical = false;
		layout->size_dim = width;
		layout->align_dim = height;
		layout->frame_pos_i = 0;
		layout->frame_pos2_i = 1;
		layout->frame_size_i = 2;
		layout->frame_size2_i = 3;
		break;

	case flex_direction::COLUMN_REVERSE:
		layout->reverse = true;
	case flex_direction::COLUMN:
		layout->size_dim = height;
		layout->align_dim = width;
		layout->frame_pos_i = 1;
		layout->frame_pos2_i = 0;
		layout->frame_size_i = 3;
		layout->frame_size2_i = 2;
		break;

	default:
		assert(false && "incorrect direction");
	}

	//layout->ordered_indices.clear();
	if (item->should_order_children && item->children.size() > 0) {
		item->temp_layout.resize(item->children.size() * (sizeof(flex_layout::flex_layout_line) + sizeof(uint32_t)));
		layout->ordered_indices = (uint32_t*)item->temp_layout.data();
		auto indices = layout->ordered_indices;
		assert(indices != NULL);
		// Creating a list of item indices sorted using the children's `order'
		// attribute values. We are using a simple insertion sort as we need
		// stability (insertion order must be preserved) and cross-platform
		// support. We should eventually switch to merge sort (or something
		// else) if the number of items becomes significant enough.
		auto icv = item->children.data();
		for (uint32_t i = 0; i < item->children.size(); i++) {
			indices[i] = i;
			for (uint32_t j = i; j > 0; j--) {
				uint32_t prev = indices[j - 1];
				uint32_t curr = indices[j];
				if (icv[prev]->order <= icv[curr]->order) {
					break;
				}
				indices[j - 1] = curr;
				indices[j] = prev;
			}
		}
	}

	layout->flex_dim = 0;
	layout->flex_grows = 0;
	layout->flex_shrinks = 0;

	layout->reverse2 = false;
	layout->wrap = item->wrap != flex_wrap::NO_WRAP;
	if (layout->wrap) {
		if (item->wrap == flex_wrap::WRAP_REVERSE) {
			layout->reverse2 = true;
			layout->pos2 = layout->align_dim;
		}
	}
	else {
		layout->pos2 = layout->vertical
			? item->padding_left : item->padding_top;
	}

	layout->need_lines = layout->wrap && item->align_content != flex_align::ALIGN_START;
	layout->lines = (flex_layout::flex_layout_line*)(item->temp_layout.data() + sizeof(uint32_t) * item->children.size());
	layout->lines_idx = 0;
	layout->lines_idx0 = 1;
	layout->lines_sizes = 0;
	auto align_items = child_align(item, item->parent);
	if (align_items == flex_align::ALIGN_BASELINE)
	{
		layout->baseline = 0;
		for (auto& it : item->children) {
			layout->baseline = std::max(layout->baseline, it->baseline);
		}
	}
}

void layout_cleanup(struct flex_layout* layout)
{
	if (layout)
	{
		layout->ordered_indices = 0;
		layout->lines = 0;
	}
}

#define LAYOUT_RESET() \
    do { \
        layout->line_dim = layout->wrap ? 0 : layout->align_dim; \
        layout->flex_dim = layout->size_dim; \
        layout->extra_flex_dim = 0; \
        layout->flex_grows = 0; \
        layout->flex_shrinks = 0; \
    } \
    while (0)

#define LAYOUT_CHILD_AT(item, i) (item->children[(layout->ordered_count ? layout->ordered_indices[i] : i)])
//#define LAYOUT_CHILD_AT(item, i) ((*item->children)[(layout->ordered_count ? layout->ordered_indices[i] : i)])
//#define LAYOUT_CHILD_AT(item, i) ((*item->children)[(layout->ordered_indices.size() ? layout->ordered_indices[i] : i)])
//#define LAYOUT_CHILD_AT(item, i) (item->children.ary[(layout->ordered_indices != NULL ? layout->ordered_indices[i] : i)])  

#define _LAYOUT_FRAME(child, name) child->frame[layout->frame_##name##_i]

#define CHILD_POS(child) _LAYOUT_FRAME(child, pos)
#define CHILD_POS2(child) _LAYOUT_FRAME(child, pos2)
#define CHILD_SIZE(child) _LAYOUT_FRAME(child, size)
#define CHILD_SIZE2(child) _LAYOUT_FRAME(child, size2)

#define CHILD_MARGIN(child, if_vertical, if_horizontal) \
    (layout->vertical \
     ? child->margin_##if_vertical \
     : child->margin_##if_horizontal)


bool layout_align(flex_align align, float flex_dim, uint32_t children_count, float* pos_p, float* spacing_p, bool stretch_allowed)
{
	assert(flex_dim > 0);

	float pos = 0;
	float spacing = 0;
	switch (align) {
	case flex_align::ALIGN_START:
		break;

	case flex_align::ALIGN_END:
		pos = flex_dim;
		break;

	case flex_align::ALIGN_CENTER:
		pos = flex_dim / 2;
		break;

	case flex_align::ALIGN_SPACE_BETWEEN:
		if (children_count > 0) {
			spacing = flex_dim / (children_count - 1);
		}
		break;

	case flex_align::ALIGN_SPACE_AROUND:
		if (children_count > 0) {
			spacing = flex_dim / children_count;
			pos = spacing / 2;
		}
		break;

	case flex_align::ALIGN_SPACE_EVENLY:
		if (children_count > 0) {
			spacing = flex_dim / (children_count + 1);
			pos = spacing;
		}
		break;

	case flex_align::ALIGN_AUTO:
	case flex_align::ALIGN_STRETCH:
		if (stretch_allowed) {
			spacing = flex_dim / children_count;
			break;
		}
		// fall through
		break;
	default:
		return false;
	}

	*pos_p = pos;
	*spacing_p = spacing;
	return true;
}

void flex_item::layout_items(uint32_t child_begin, uint32_t child_end, uint32_t children_count, struct flex_layout* layout, uint32_t last_count)
{
	flex_item* item = this;
	assert(children_count <= (child_end - child_begin));
	if (children_count <= 0) {
		return;
	}
	if (last_count > 0 && last_count > children_count)
	{
		//children_count = last_count;
	}
	if (layout->flex_dim > 0 && layout->extra_flex_dim > 0) {
		// If the container has a positive flexible space, let's add to it
		// the sizes of all flexible children->
		layout->flex_dim += layout->extra_flex_dim;
	}

	// Determine the main axis initial position and optional spacing.
	float pos = 0;
	float spacing = 0;
	if (layout->flex_grows == 0 && layout->flex_dim > 0) {
		if (!layout_align(item->justify_content, layout->flex_dim,
			children_count, &pos, &spacing, false))
		{
			assert(0 && "incorrect justify_content");
		}
		if (layout->reverse) {
			pos = layout->size_dim - pos;
		}
	}

	if (layout->reverse) {
		pos -= layout->vertical ? item->padding_bottom : item->padding_right;
	}
	else {
		pos += layout->vertical ? item->padding_top : item->padding_left;
	}
	if (layout->wrap && layout->reverse2) {
		layout->pos2 -= layout->line_dim;
	}

	for (uint32_t i = child_begin; i < child_end; i++) {
		flex_item* child = LAYOUT_CHILD_AT(item, i);
		if (child->position == flex_position::POS_ABSOLUTE) {
			// Already positioned.
			continue;
		}

		// Grow or shrink the main axis item size if needed.
		float flex_size = 0;
		if (layout->flex_dim > 0) {
			if (child->grow != 0) {
				CHILD_SIZE(child) = 0; // Ignore previous size when growing.
				flex_size = (layout->flex_dim / layout->flex_grows)
					* child->grow;
			}
		}
		else if (layout->flex_dim < 0) {
			if (child->shrink != 0) {
				flex_size = (layout->flex_dim / layout->flex_shrinks)
					* child->shrink;
			}
		}
		CHILD_SIZE(child) += flex_size;

		// Set the cross axis position (and stretch the cross axis size if
		// needed).
		float align_size = CHILD_SIZE2(child);
		float align_pos = layout->pos2 + 0;
		switch (child_align(child, item)) {
		case flex_align::ALIGN_END:
			align_pos += layout->line_dim - align_size
				- CHILD_MARGIN(child, right, bottom);
			break;

		case flex_align::ALIGN_CENTER:
			align_pos += (layout->line_dim / 2) - (align_size / 2)
				+ (CHILD_MARGIN(child, left, top)
					- CHILD_MARGIN(child, right, bottom));
			break;

		case flex_align::ALIGN_STRETCH:
			if (align_size == 0) {
				CHILD_SIZE2(child) = layout->line_dim
					- (CHILD_MARGIN(child, left, top)
						+ CHILD_MARGIN(child, right, bottom));
			}
			// fall through
			align_pos += CHILD_MARGIN(child, left, top);
			break;
		case flex_align::ALIGN_START:
			align_pos += CHILD_MARGIN(child, left, top);
			break;
		case flex_align::ALIGN_BASELINE:
			align_pos += CHILD_MARGIN(child, left, top);
			if (child->baseline > 0) {
				align_pos += layout->baseline - child->baseline;
			}
			break;
		default:
			assert(false && "incorrect align_self");
		}
		CHILD_POS2(child) = align_pos;

		// Set the main axis position.
		if (layout->reverse) {
			pos -= CHILD_MARGIN(child, bottom, right);
			pos -= CHILD_SIZE(child);
			CHILD_POS(child) = pos;
			pos -= spacing;
			pos -= CHILD_MARGIN(child, top, left);
		}
		else {
			pos += CHILD_MARGIN(child, top, left);
			CHILD_POS(child) = pos;
			pos += CHILD_SIZE(child);
			pos += spacing;
			pos += CHILD_MARGIN(child, bottom, right);
		}

		// Now that the item has a frame, we can layout its children.
		child->layout_item(child->frame[2], child->frame[3]);
	}

	if (layout->wrap && !layout->reverse2) {
		layout->pos2 += layout->line_dim;
	}

	if (layout->need_lines) {
		flex_layout::flex_layout_line line[1] = {};
		line->child_begin = child_begin;
		line->child_end = child_end;
		line->size = layout->line_dim;
		layout->lines[layout->lines_idx] = (line[0]);
		layout->lines_idx++;
		//layout->lines.push_back(line[0]);
		layout->lines_sizes += line->size;
	}
}

void flex_item::layout_item(float width, float height)
{
	flex_item* item = this;
	if (item->children.size() == 0) {
		return;
	}

	struct flex_layout layout_s = { 0 }, * layout = &layout_s;
	layout_init(item, width, height, &layout_s);

	LAYOUT_RESET();
	uint32_t last_count = 0;
	uint32_t last_layout_child = 0;
	uint32_t relative_children_count = 0;
	for (uint32_t i = 0; i < item->children.size(); i++) {
		flex_item* child = LAYOUT_CHILD_AT(item, i);

		// Items with an absolute position have their frames determined
		// directly and are skipped during layout.
		if (child->position == flex_position::POS_ABSOLUTE) {
#define ABSOLUTE_SIZE(val, pos1, pos2, dim) \
            (!isnan(val) \
             ? val \
             : (!isnan(pos1) && !isnan(pos2) \
                 ? dim - pos2 - pos1 \
                 : 0))

#define ABSOLUTE_POS(pos1, pos2, size, dim) \
            (!isnan(pos1) \
             ? pos1 \
             : (!isnan(pos2) \
                 ? dim - size - pos2 \
                 : 0))

			float child_width = ABSOLUTE_SIZE(child->width, child->left,
				child->right, width);

			float child_height = ABSOLUTE_SIZE(child->height, child->top,
				child->bottom, height);

			float child_x = ABSOLUTE_POS(child->left, child->right,
				child_width, width);

			float child_y = ABSOLUTE_POS(child->top, child->bottom,
				child_height, height);

			child->frame[0] = child_x;
			child->frame[1] = child_y;
			child->frame[2] = child_width;
			child->frame[3] = child_height;

			// Now that the item has a frame, we can layout its children.
			child->layout_item(child->frame[2], child->frame[3]);

#undef ABSOLUTE_POS
#undef ABSOLUTE_SIZE
			continue;
		}

		// Initialize frame.
		child->frame[0] = 0;
		child->frame[1] = 0;
		child->frame[2] = child->width;
		child->frame[3] = child->height;

		// Main axis size defaults to 0.
		if (isnan(CHILD_SIZE(child))) {
			CHILD_SIZE(child) = 0;
		}

		// Cross axis size defaults to the parent's size (or line size in wrap
		// mode, which is calculated later on).
		if (isnan(CHILD_SIZE2(child))) {
			if (layout->wrap) {
				layout->need_lines = true;
			}
			else {
				CHILD_SIZE2(child) = (layout->vertical ? width : height)
					- CHILD_MARGIN(child, left, top)
					- CHILD_MARGIN(child, right, bottom);
			}
		}

		// Call the self_sizing callback if provided. Only non-NAN values
		// are taken into account. If the item's cross-axis align property
		// is set to stretch, ignore the value returned by the callback.
		if (child->self_sizing != NULL) {
			float size[2] = { child->frame[2], child->frame[3] };

			child->self_sizing(child, size);

			for (uint32_t j = 0; j < 2; j++) {
				uint32_t size_off = j + 2;
				if (size_off == layout->frame_size2_i
					&& child_align(child, item) == flex_align::ALIGN_STRETCH) {
					continue;
				}
				float val = size[j];
				if (!isnan(val)) {
					child->frame[size_off] = val;
				}
			}
		}

		// Honor the `basis' property which overrides the main-axis size.
		if (!(isnan(child->basis) || child->basis < 0)) {
			assert(child->basis >= 0);
			CHILD_SIZE(child) = child->basis;
		}

		float child_size = CHILD_SIZE(child);
		if (layout->wrap) {
			if (layout->flex_dim < child_size) {
				// Not enough space for this child on this line, layout the
				// remaining items and move it to a new line.
				item->layout_items(last_layout_child, i, relative_children_count, layout, last_count);

				LAYOUT_RESET();
				last_layout_child = i;
				if (last_count < relative_children_count)
					last_count = relative_children_count;
				relative_children_count = 0;
				layout->lines_idx0++;
			}

			float child_size2 = CHILD_SIZE2(child);
			if (!isnan(child_size2) && child_size2 > layout->line_dim) {
				layout->line_dim = child_size2;
			}
		}

		assert(child->grow >= 0);
		assert(child->shrink >= 0);

		layout->flex_grows += child->grow;
		layout->flex_shrinks += child->shrink;

		layout->flex_dim -= child_size
			+ (CHILD_MARGIN(child, top, left)
				+ CHILD_MARGIN(child, bottom, right));

		relative_children_count++;

		if (child_size > 0 && child->grow > 0) {
			layout->extra_flex_dim += child_size;
		}
	}

	// Layout remaining items in wrap mode, or everything otherwise.
	item->layout_items(last_layout_child, item->children.size(), relative_children_count, layout, last_count);

	// In wrap mode we may need to tweak the position of each line according to
	// the align_content property as well as the cross-axis size of items that
	// haven't been set yet.
	if (layout->need_lines && layout->lines_idx > 0) {
		float pos = 0;
		float spacing = 0;
		float flex_dim = layout->align_dim - layout->lines_sizes;
		if (flex_dim > 0) {
			if (!layout_align(item->align_content, flex_dim, layout->lines_idx, &pos, &spacing, true))
			{
				assert(0 && "incorrect align_content");
			}
		}

		float old_pos = 0;
		if (layout->reverse2) {
			pos = layout->align_dim - pos;
			old_pos = layout->align_dim;
		}

		for (uint32_t i = 0; i < layout->lines_idx; i++) {
			auto line = &layout->lines[i];

			if (layout->reverse2) {
				pos -= line->size;
				pos -= spacing;
				old_pos -= line->size;
			}

			// Re-position the children of this line, honoring any child
			// alignment previously set within the line.
			for (uint32_t j = line->child_begin; j < line->child_end;
				j++) {
				flex_item* child = LAYOUT_CHILD_AT(item, j);
				if (child->position == flex_position::POS_ABSOLUTE) {
					// Should not be re-positioned.
					continue;
				}
				if (isnan(CHILD_SIZE2(child))) {
					// If the child's cross axis size hasn't been set it, it
					// defaults to the line size.
					CHILD_SIZE2(child) = line->size
						+ (item->align_content == flex_align::ALIGN_STRETCH
							? spacing : 0);
				}
				CHILD_POS2(child) = pos + (CHILD_POS2(child) - old_pos);
			}

			if (!layout->reverse2) {
				pos += line->size;
				pos += spacing;
				old_pos += line->size;
			}
		}
	}
	line_count = layout->lines_idx > 0 ? layout->lines_idx : layout->lines_idx0;
	layout_cleanup(layout);
}

#undef CHILD_MARGIN
#undef CHILD_POS
#undef CHILD_POS2
#undef CHILD_SIZE
#undef CHILD_SIZE2
#undef _LAYOUT_FRAME
#undef LAYOUT_CHILD_AT
#undef LAYOUT_RESET


void flex_item::layout()
{
	assert(parent == NULL);
	assert(!isnan(width));
	assert(!isnan(height));
	assert(self_sizing == NULL);
	layout_item(width, height);
}
struct flex_run {
	std::pmr::vector<flex_item> items;
	std::stack<node_dt*> q;
	usp_ac_cx* ac = 0;
	bool meac = false;
};
flex_run* new_flex_run(mem_resource_t* a) {
	usp_ac_cx* ac = (usp_ac_cx*)a;
	if (!ac) {
		ac = new usp_ac_cx();
	}
	if (ac)return 0;
	auto p = ac->new_obj<flex_run>();
	if (p) {
		p->ac = ac; p->meac = !a;
	}
	return p;
}
void free_flex_run(flex_run* p) {
	if (p && p->ac) { p->ac->free_obj(p); }
}
mem_resource_t* flex_run_ac(flex_run* p) {
	return (mem_resource_t*)(p ? p->ac : nullptr);
}
// 输入样式数据，根节点指针，所有节点数量 
glm::vec4 flex_run_layout(flex_run* ctx, flex_data* fd, size_t count, node_dt* p, size_t node_count) {
	glm::vec4 rect = {};
	if (!fd || count == 0 || !p || !node_count || !p->child || !p->child_count || !ctx)
		return rect;
	auto& items = ctx->items;
	items.resize(node_count);
	auto fitem = items.data();
	if (!fitem) return rect;
	for (size_t i = 0; i < node_count; i++) {
		fitem[i].init();
	}
	size_t idx = 0;
	auto q = ctx->q;  // 队列存储待处理坐标 
	while (q.size()) {
		q.pop();
	}
	q.push(p);
	p->tidx = idx;
	while (q.size()) {
		auto it = q.top(); q.pop();
		if (it && it->tidx < node_count)
		{
			auto& k = fitem[it->tidx];
			auto pidx = it->tidx;
			idx++;
			if (it->index < count)
			{
				k.setdata(fd + it->index);
			}
			else {
				k.setdata(fd);
			}
			k.position = it->position ? flex_position::POS_ABSOLUTE : flex_position::POS_RELATIVE;
			k.managed_ptr = it;
			k.baseline = it->baseline;
			k.width = it->size.x; k.height = it->size.y;
			k.left = it->offset.x; k.top = it->offset.y;
			k.right = it->offset.z; k.bottom = it->offset.w;
			for (size_t i = 0; i < it->child_count; i++) {
				it->child[i].parent = pidx;
				it->child[i].tidx = idx;
				q.push(it->child + i);
				k.item_add(fitem + idx);
				idx++;
			}
		}
	}
	fitem->layout();
	for (size_t i = 0; i < node_count; i++)
	{
		auto& it = fitem[i];
		auto pt = (node_dt*)it.managed_ptr;
		pt->frame = glm::vec4(it.frame[0], it.frame[1], it.frame[2], it.frame[3]);
		rect.x = std::min(rect.x, it.frame[0]);
		rect.y = std::min(rect.y, it.frame[1]);
		rect.z = std::max(rect.z, it.frame[2] + it.frame[0]);
		rect.w = std::max(rect.w, it.frame[3] + it.frame[1]);

	}
	p->line_count = fitem->line_count;
	return rect;
}
#endif // !NO_FLEX_IMP 

struct font_familys_cx :public font_familys_t {
	std::pmr::vector<font_family_t*> v;
	usp_ac_cx* ac = 0;
};
void vg_split(std::string str, const std::string& pattern, std::vector<std::string>& result)
{
	std::string::size_type pos;
	str += pattern;//扩展字符串以方便操作
	int size = str.size();
	result.clear();
	int ct = 0;
	for (int i = 0; i < size; i++)
	{
		pos = str.find(pattern, i);
		if (pos < size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
			ct++;
		}
	}
}
font_cache_cx* new_font_cache()
{
	auto p = new font_cache_cx();
	if (p)
	{
		auto ac = new usp_ac_cx();
		if (!ac)
		{
			delete p; p = 0;
		}
		else
		{
			p->set_alloc_ptr(ac);
		}
	}
	return p;
}
void free_font_cache(font_cache_cx* p)
{
	if (p) {
		p->references--;
		if (p->references > 0) return;
		if (p->ac)delete p->ac;
		delete p;
	}
}
font_familys_t* new_font_family(font_cache_cx* ctx, const char* familys, const char* styles) {
	if (!ctx || !familys || !*familys)return nullptr;
	font_familys_cx* p = 0;
	auto ac = (usp_ac_cx*)ctx->ac;
	do {
		std::vector<std::string> v, st;
		vg_split(familys, ",", v);
		if (styles && *styles)
			vg_split(styles, ",", st);
		if (v.empty() || !ctx || !ac)
		{
			break;
		}
		const char* style = st.size() ? st[0].c_str() : nullptr;
		size_t ix = 1;
		p = (font_familys_cx*)ac->new_obj<font_familys_cx>();
		if (!p) {
			return 0;
		}
		p->count = 0;
		for (auto& it : v)
		{
			auto font = ctx->get_font(it.c_str(), style, 0, 0);
			if (ix < st.size())
			{
				style = st.size() ? st[ix].c_str() : nullptr;
			}
			ix++;
			if (font)
			{
				auto ff = (font_family_t*)font;
				p->v.push_back(ff);
			}
		}
		p->familys = p->v.data();
		p->count = p->v.size();
		p->ac = ac;
	} while (0);
	return p;
}
void delete_font_family(font_familys_t* p) {
	auto p1 = (font_familys_cx*)p;
	if (p && p1->ac) {
		p1->ac->free_obj(p1);
	}
}

void submit_vector_cmd(ovg_canvas_cb* cb, rvg_t* rvg, const glyph_draw_cmd& cmd)
{
	ovg_path_t* path = cb->new_path(cb->ac);
	vg_state_save_t* st = cb->new_state(cb->ac);

	// 一次性灌入 upem 空间路径
	cb->add_path(path, cmd.entry->path_data, cmd.entry->path_size);

	// upem → 像素
	float scale = (float)cmd.fontsize / (float)cmd.entry->em_units;

	cb->translate(st, cmd.pos.x, cmd.pos.y);
	cb->scale(st, scale, scale);
	cb->set_source_color(st, cmd.color);
	cb->set_path(rvg, path, st);
	cb->fill(rvg);

	cb->destroy_path(path);
	cb->state_destroy(st);
}
void submit_vector_cmd_ctx(ovg_ctx_cb* cb, rvg_t* rvg, const glyph_draw_cmd& cmd)
{
	cb->save(rvg);
	cb->new_path(rvg);

	cb->add_path(rvg, cmd.entry->path_data, cmd.entry->path_size);

	float scale = (float)cmd.fontsize / (float)cmd.entry->em_units;
	cb->translate(rvg, cmd.pos.x, cmd.pos.y);
	cb->scale(rvg, scale, scale);
	cb->set_source_color(rvg, cmd.color);
	cb->fill(rvg);

	cb->restore(rvg);
}
// 公共代码，两套都能用
void submit_raster_glyphs(
	const text_draw_list& list,
	ovg_canvas_cb* cb, rvg_t* rvg)
{
	for (const auto& cmd : list.cmds) {
		if (cmd.type != glyph_draw_cmd::RASTER) continue;

		float x0 = cmd.pos.x;
		float y0 = cmd.pos.y;
		float w = cmd.size.x;
		float h = cmd.size.y;

		float verts[8] = {
			x0,     y0,
			x0 + w, y0,
			x0 + w, y0 + h,
			x0,     y0 + h,
		};
		float uvs[8] = {
			cmd.uv_rect.x, cmd.uv_rect.y,
			cmd.uv_rect.z, cmd.uv_rect.y,
			cmd.uv_rect.z, cmd.uv_rect.w,
			cmd.uv_rect.x, cmd.uv_rect.w,
		};
		uint16_t idx[6] = { 0, 1, 2, 0, 2, 3 };
		uint32_t color = cmd.color;

		cb->add_geometry(rvg, cmd.entry->atlas_img,
			verts, sizeof(float) * 2,
			&color, 0,          // color_stride=0 表示所有顶点同色
			uvs, sizeof(float) * 2,
			4, idx, 6, 2, 1);   // color_type=1 (uint32_t)
	}
}
void submit_vector_glyphs_object_mode(
	const text_draw_list& list,
	ovg_canvas_cb* cb, rvg_t* rvg,
	float origin_x, float origin_y)
{
	for (const auto& cmd : list.cmds) {
		if (cmd.type != glyph_draw_cmd::VECTOR) continue;

		// 创建 path 和 state
		ovg_path_t* path = cb->new_path(cb->ac);
		vg_state_save_t* st = cb->new_state(cb->ac);

		// 回放路径命令
		cb->add_path(path, cmd.entry->path_data, cmd.entry->path_size);

		// 设置状态
		cb->set_source_color(st, cmd.color);
		float scale = (float)cmd.fontsize / (float)cmd.entry->em_units;
		cb->translate(st, cmd.pos.x, cmd.pos.y);
		cb->scale(st, scale, scale);

		// 绑定并提交
		cb->set_path(rvg, path, st);
		cb->fill(rvg);

		// 清理
		cb->destroy_path(path);
		cb->state_destroy(st);
	}
}
void submit_vector_glyphs_ctx_mode(
	const text_draw_list& list,
	ovg_ctx_cb* cb, rvg_t* rvg)
{
	for (const auto& cmd : list.cmds) {
		if (cmd.type != glyph_draw_cmd::VECTOR) continue;

		cb->save(rvg);
		cb->new_path(rvg);

		// 回放路径命令到 ctx 内部路径
		cb->add_path(rvg, cmd.entry->path_data, cmd.entry->path_size);

		// 设置状态
		cb->set_source_color(rvg, cmd.color);
		float scale = (float)cmd.fontsize / (float)cmd.entry->em_units;
		cb->translate(rvg, cmd.pos.x, cmd.pos.y);
		cb->scale(rvg, scale, scale);

		// 提交
		cb->fill(rvg);

		cb->restore(rvg);
	}
}
// ovg_text.cpp 
void submit_vector_glyphs_stroked(ovg_canvas_cb* cb, rvg_t* rvg, const text_draw_list& list, float stroke_width)
{
	for (const auto& cmd : list.cmds) {
		if (cmd.type != glyph_draw_cmd::VECTOR) continue;
		ovg_path_t* path = cb->new_path(cb->ac);
		vg_state_save_t* st = cb->new_state(cb->ac);
		cb->add_path(path, cmd.entry->path_data, cmd.entry->path_size);
		cb->set_source_color(st, cmd.color);
		float scale = (float)cmd.fontsize / (float)cmd.entry->em_units;
		cb->translate(st, cmd.pos.x, cmd.pos.y);
		cb->scale(st, scale, scale);
		cb->set_line_width(st, (float)stroke_width / scale);
		cb->set_path(rvg, path, st);
		cb->stroke(rvg);  // ← stroke 而非 fill
		cb->destroy_path(path);
		cb->state_destroy(st);
	}
}
void submit_draw_list(ovg_canvas_cb* cb, rvg_t* rvg, const text_draw_list& list)
{
	gem_info_t info2d = {};
	info2d.blendMode = (uint8_t)blendMode_e::normal_prem;
	info2d.topology = 0;
	info2d.flags = (uint8_t)depth_stencil_State::d_stenciltest_enable;
	info2d.frontFace = 0;
	info2d.cullMode = 0;
	info2d.shader = ST_NONE;
	auto mat = ovg_ortho(rvg->width, rvg->height, -1.0f, 1.0f, 0);
	cb->set_geom_state(rvg, &info2d, &mat);

	struct raster_batch {
		std::vector<float>    verts;
		std::vector<float>    uvs;
		std::vector<uint32_t> colors;
		std::vector<uint16_t> idx;
	};
	std::unordered_map<vg_image_t*, raster_batch> batches;

	for (const auto& cmd : list.cmds) {
		if (cmd.type != glyph_draw_cmd::RASTER) continue;

		auto& b = batches[cmd.entry->atlas_img];
		uint16_t base = (uint16_t)(b.verts.size() / 2);

		float x0 = cmd.pos.x + cmd.entry->offset.x;
		float y0 = cmd.pos.y - cmd.entry->offset.y;
		float w = cmd.size.x;
		float h = cmd.size.y;

		b.verts.push_back(x0);     b.verts.push_back(y0);
		b.verts.push_back(x0 + w); b.verts.push_back(y0);
		b.verts.push_back(x0 + w); b.verts.push_back(y0 + h);
		b.verts.push_back(x0);     b.verts.push_back(y0 + h);

		b.uvs.push_back(cmd.uv_rect.x); b.uvs.push_back(cmd.uv_rect.y);
		b.uvs.push_back(cmd.uv_rect.z); b.uvs.push_back(cmd.uv_rect.y);
		b.uvs.push_back(cmd.uv_rect.z); b.uvs.push_back(cmd.uv_rect.w);
		b.uvs.push_back(cmd.uv_rect.x); b.uvs.push_back(cmd.uv_rect.w);
		auto color = cmd.color;
		if (list.has_color && cmd.entry->has_color)color = -1;
		b.colors.push_back(color);
		b.colors.push_back(color);
		b.colors.push_back(color);
		b.colors.push_back(color);

		b.idx.push_back(base + 0);
		b.idx.push_back(base + 1);
		b.idx.push_back(base + 2);
		b.idx.push_back(base + 0);
		b.idx.push_back(base + 2);
		b.idx.push_back(base + 3);
	}

	// 每组一次 add_geometry
	for (auto& [atlas_img, b] : batches) {
		if (b.verts.empty()) continue;
		auto img = (ovg_image_data*)atlas_img;
		vg_image_desc_t desc = {};
		if (img->valid)
		{
			desc.width = img->width;
			desc.height = img->height;
			desc.format = VG_FORMAT_BGRA8;
			desc.stride = desc.width * sizeof(int);
			desc.pixels = img->data;
			desc.x = 0, desc.y = 0, desc.w = img->width, desc.h = img->height;		// 更新矩形区域
			desc.is_copy = false;
			img->valid = false;
			std::string fn = "temp/font_pack_ovg.png";
			write_png_bgra(fn.c_str(), (uint8_t*)img->data, img->width, img->height);
			cb->image_update(rvg, img, &desc);
		}
		cb->add_geometry(
			rvg, img,
			b.verts.data(), sizeof(float) * 2,
			b.colors.data(), sizeof(uint32_t),
			b.uvs.data(), sizeof(float) * 2,
			(int)b.verts.size() / 2,
			b.idx.data(), (int)b.idx.size(),
			2, 1
		);
	}

	// ── 2. 矢量 glyph：逐个提交（每个独立 path） ──
	for (const auto& cmd : list.cmds) {
		if (cmd.type != glyph_draw_cmd::VECTOR) continue;
		submit_vector_cmd(cb, rvg, cmd);
	}
}
void submit_vector_glyphs_stroked_ctx(ovg_ctx_cb* cb, rvg_t* rvg, const text_draw_list& list, float stroke_width)
{
	for (const auto& cmd : list.cmds) {
		if (cmd.type != glyph_draw_cmd::VECTOR) continue;
		cb->save(rvg);
		cb->add_path(rvg, cmd.entry->path_data, cmd.entry->path_size);
		cb->set_source_color(rvg, cmd.color);
		float scale = (float)cmd.fontsize / (float)cmd.entry->em_units;
		cb->translate(rvg, cmd.pos.x, cmd.pos.y);
		cb->scale(rvg, scale, scale);
		cb->set_line_width(rvg, (float)stroke_width / scale);
		cb->stroke(rvg);  // ← stroke 而非 fill	 
		cb->restore(rvg);
	}
}
void submit_draw_list_ctx(ovg_ctx_cb* cb, rvg_t* rvg, const text_draw_list& list)
{
	gem_info_t info2d = {};
	info2d.blendMode = (uint8_t)blendMode_e::normal_prem;
	info2d.topology = 0;
	info2d.flags = (uint8_t)depth_stencil_State::d_stenciltest_enable;
	info2d.frontFace = 0;
	info2d.cullMode = 0;
	info2d.shader = ST_NONE;
	auto mat = ovg_ortho(rvg->width, rvg->height, -1.0f, 1.0f, 0);
	cb->set_geom_state(rvg, &info2d, &mat);

	// 位图 batch（同上，只是调用 ctx 的 add_geometry）
	struct raster_batch {
		std::vector<float>    verts;
		std::vector<float>    uvs;
		std::vector<uint32_t> colors;
		std::vector<uint16_t> idx;
	};
	std::unordered_map<vg_image_t*, raster_batch> batches;

	for (const auto& cmd : list.cmds) {
		if (cmd.type != glyph_draw_cmd::RASTER) continue;

		auto& b = batches[cmd.entry->atlas_img];
		uint16_t base = (uint16_t)(b.verts.size() / 2);

		float x0 = cmd.pos.x + cmd.entry->offset.x;
		float y0 = cmd.pos.y - cmd.entry->offset.y;
		float w = cmd.size.x, h = cmd.size.y;

		b.verts.insert(b.verts.end(), { x0,y0, x0 + w,y0, x0 + w,y0 + h, x0,y0 + h });
		b.uvs.insert(b.uvs.end(), {
			cmd.uv_rect.x, cmd.uv_rect.y,
			cmd.uv_rect.z, cmd.uv_rect.y,
			cmd.uv_rect.z, cmd.uv_rect.w,
			cmd.uv_rect.x, cmd.uv_rect.w,
			});
		auto color = cmd.color;
		if (list.has_color && cmd.entry->has_color)color = -1;
		for (int i = 0; i < 4; i++) b.colors.push_back(color);
		b.idx.push_back(base + 0); b.idx.push_back(base + 1); b.idx.push_back(base + 2);
		b.idx.push_back(base + 0); b.idx.push_back(base + 2); b.idx.push_back(base + 3);
	}

	for (auto& [atlas_img, b] : batches) {
		if (b.verts.empty()) continue;
		auto img = (ovg_image_data*)atlas_img;
		vg_image_desc_t desc = {};
		if (img->valid)
		{
			desc.width = img->width;
			desc.height = img->height;
			desc.format = VG_FORMAT_BGRA8;
			desc.stride = desc.width * sizeof(int);
			desc.pixels = img->data;
			desc.x = 0, desc.y = 0, desc.w = img->width, desc.h = img->height;		// 更新矩形区域
			desc.is_copy = false;
			img->valid = false;
			std::string fn = "temp/font_pack_ovg.png";
			write_png_bgra(fn.c_str(), (uint8_t*)img->data, img->width, img->height);
			cb->image_update(rvg, img, &desc);
		}
		cb->add_geometry(
			rvg, img,
			b.verts.data(), sizeof(float) * 2,
			b.colors.data(), sizeof(uint32_t),
			b.uvs.data(), sizeof(float) * 2,
			(int)b.verts.size() / 2,
			b.idx.data(), (int)b.idx.size(),
			2, 1
		);
	}

	// 矢量逐个
	for (const auto& cmd : list.cmds) {
		if (cmd.type != glyph_draw_cmd::VECTOR) continue;
		submit_vector_cmd_ctx(cb, rvg, cmd);
	}
}

void ovg_canvas_cx::add_text(rvg_t* rvg, text_st_t* p, text_style_t* ts, text_box_rt* box)
{
	if (!p || !p->text || !ts || !ts->family) return;

	int fontsize = ts->fontsize > 0 ? (int)ts->fontsize : 16;

	// ── 1. shape ──
	vg_text_run_cx run;
	run.set_min_subpixel(ts->min_subpixel);
	run.set_font_families(ts->family, fontsize);
	run.set_text(p->text, p->text_len);
	run.shape();  // 内部按 fallback 切 run，lookup 缓存

	if (run.glyph_count() == 0) return;

	// ── 2. 布局计算 ──
	const auto& extents = run.extents();

	// 对齐偏移
	float box_w = box && box->rc.z > 0 ? (float)box->rc.z : extents.width;
	float box_h = box && box->rc.w > 0 ? (float)box->rc.w : extents.height;

	float align_x = ts->align.x;  // 0=左, 0.5=中, 1=右
	float align_y = ts->align.y;

	float offset_x = p->pos.x + (box_w - extents.width) * align_x;
	float offset_y = p->pos.y + (box_h - extents.height) * align_y;

	// 基线位置 = offset_y + ascender
	hb_font_extents_t fextents = {};
	// 用主字体取 ascender
	hb_font_t* primary = ts->family->familys[0]->font;
	hb_font_set_scale(primary, fontsize, fontsize);
	hb_font_get_extents_for_direction(primary, HB_DIRECTION_LTR, &fextents);
	float baseline_y = offset_y;// +(float)fextents.ascender;

	// ── 3. 裁剪 ──
	if (box && box->rc.z > 0 && box->rc.w > 0) {
		this->clip_rect(rvg, box->rc.x, box->rc.y, box->rc.z, box->rc.w);
	}

	// ── 4. 阴影 ──
	if (ts->color_shadow & 0xFF000000) {
		text_draw_list shadow_list;
		run.populate_draw_list(shadow_list, offset_x + ts->shadow_pos.x, baseline_y + ts->shadow_pos.y, ts->color_shadow, vg_text_run_cx::RASTER_FIRST);
		submit_draw_list(this, rvg, shadow_list);
	}

	// ── 5. 描边 ──
	if (ts->stroke != 0) {
		text_draw_list stroke_list;
		float stroke = abs(ts->stroke);
		if (ts->stroke > 0) {
			stroke *= 2;
			run.populate_draw_list(stroke_list, offset_x, baseline_y, ts->color_stroke, vg_text_run_cx::VECTOR_ONLY);
			submit_vector_glyphs_stroked(this, rvg, stroke_list, stroke);
		}
		else {
			int pxx[4] = { -stroke, 0, stroke, 0 };
			int pyy[4] = { 0, -stroke, 0, stroke };
			for (int e = 0; e < 4; e++)
			{
				glm::vec2 ps1 = { offset_x, baseline_y };
				ps1.x += pxx[e];
				ps1.y += pyy[e];
				run.populate_draw_list(stroke_list, ps1.x, ps1.y, ts->color_stroke, vg_text_run_cx::RASTER_FIRST);
			}
			submit_draw_list(this, rvg, stroke_list);
		}
	}

	// ── 6. 主文本 ──
	text_draw_list main_list;
	main_list.has_color = true;
	run.populate_draw_list(main_list, offset_x, baseline_y, ts->color, vg_text_run_cx::RASTER_FIRST);
	submit_draw_list(this, rvg, main_list);

	// ── 7. 恢复裁剪 ──
	if (box && box->rc.z > 0 && box->rc.w > 0) {
		this->reset_clip(rvg, 1);
	}
}

void ovg_ctx_cx::add_text(rvg_t* rvg, text_st_t* p, text_style_t* ts, text_box_rt* box)
{
	if (!p || !p->text || !ts || !ts->family) return;

	int fontsize = ts->fontsize > 0 ? (int)ts->fontsize : 16;

	// ── 1. shape ──
	vg_text_run_cx run;
	run.set_min_subpixel(ts->min_subpixel);
	run.set_font_families(ts->family, fontsize);
	run.set_text(p->text, p->text_len);
	run.shape();  // 内部按 fallback 切 run，lookup 缓存

	if (run.glyph_count() == 0) return;

	// ── 2. 布局计算 ──
	const auto& extents = run.extents();

	// 对齐偏移
	float box_w = box && box->rc.z > 0 ? (float)box->rc.z : extents.width;
	float box_h = box && box->rc.w > 0 ? (float)box->rc.w : extents.height;

	float align_x = ts->align.x;  // 0=左, 0.5=中, 1=右
	float align_y = ts->align.y;

	float offset_x = p->pos.x + (box_w - extents.width) * align_x;
	float offset_y = p->pos.y + (box_h - extents.height) * align_y;

	// 基线位置 = offset_y + ascender
	hb_font_extents_t fextents = {};
	// 用主字体取 ascender
	hb_font_t* primary = ts->family->familys[0]->font;
	hb_font_set_scale(primary, fontsize, fontsize);
	hb_font_get_extents_for_direction(primary, HB_DIRECTION_LTR, &fextents);
	float baseline_y = offset_y;// +(float)fextents.ascender;

	// ── 3. 裁剪 ──
	if (box && box->rc.z > 0 && box->rc.w > 0) {
		this->clip_rect(rvg, box->rc.x, box->rc.y, box->rc.z, box->rc.w);
	}

	// ── 4. 阴影 ──
	if (ts->color_shadow & 0xFF000000) {
		text_draw_list shadow_list;
		run.populate_draw_list(shadow_list, offset_x + ts->shadow_pos.x, baseline_y + ts->shadow_pos.y, ts->color_shadow, vg_text_run_cx::RASTER_FIRST);
		submit_draw_list_ctx(this, rvg, shadow_list);
	}

	// ── 5. 描边 ──
	if (ts->stroke != 0) {
		text_draw_list stroke_list;
		float stroke = abs(ts->stroke);
		if (ts->stroke > 0) {
			stroke *= 2;
			run.populate_draw_list(stroke_list, offset_x, baseline_y, ts->color_stroke, vg_text_run_cx::VECTOR_ONLY);
			submit_vector_glyphs_stroked_ctx(this, rvg, stroke_list, stroke);
		}
		else {
			int pxx[4] = { -stroke, 0, stroke, 0 };
			int pyy[4] = { 0, -stroke, 0, stroke };
			for (int e = 0; e < 4; e++)
			{
				glm::vec2 ps1 = { offset_x, baseline_y };
				ps1.x += pxx[e];
				ps1.y += pyy[e];
				run.populate_draw_list(stroke_list, ps1.x, ps1.y, ts->color_stroke, vg_text_run_cx::RASTER_FIRST);
			}
			submit_draw_list_ctx(this, rvg, stroke_list);
		}
	}

	// ── 6. 主文本 ──
	text_draw_list main_list;
	main_list.has_color = true;
	run.populate_draw_list(main_list, offset_x, baseline_y, ts->color, vg_text_run_cx::RASTER_FIRST);
	submit_draw_list_ctx(this, rvg, main_list);

	// ── 7. 恢复裁剪 ──
	if (box && box->rc.z > 0 && box->rc.w > 0) {
		this->reset_clip(rvg, 1);
	}
}
