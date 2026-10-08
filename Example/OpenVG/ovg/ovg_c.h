#pragma once
/*
矢量/图片/文本/三角形录制到rvg_t对象
*/
#ifndef OVG_H
#define OVG_H

#include <stdint.h>

#ifdef __cplusplus
#include <cstdint>
using uvec2 = glm::uvec2;
using uvec3 = glm::uvec3;
using vec2 = glm::vec2;
using vec3 = glm::vec3;
using vec4 = glm::vec4;
using ivec2 = glm::ivec2;
using ivec4 = glm::ivec4;
using mat4 = glm::mat4;
using mat3x2 = glm::mat3x2;
class vg_alloc_cx
{
public:
	vg_alloc_cx();
	virtual ~vg_alloc_cx();
	virtual void* alloc(const size_t _Bytes, const size_t align);
	virtual void dealloc(void* t, size_t n);
private:

};
extern "C" {
#else
typedef struct uvec2 {
	uint32_t x;
	uint32_t y;
} uvec2;
typedef struct uvec3 {
	uint32_t x;
	uint32_t y;
	uint32_t z;
} uvec3;
typedef struct vec2 {
	float x;
	float y;
} vec2;
typedef struct vec3 {
	float x;
	float y;
	float z;
} vec3;
typedef struct vec4 {
	float x;
	float y;
	float z;
	float w;
} vec4;
typedef struct ivec2 {
	int x;
	int y;
	int z;
} ivec2;
typedef struct ivec4 {
	int x;
	int y;
	int z;
	int w;
} ivec4;
typedef struct mat4 {
	float m[16];
}mat4;
typedef struct mat3x2 {
	float m[12];
}mat3x2;

#ifndef bool
#define bool uint8_t
#endif

#endif
//!cpp
typedef struct hb_font_t hb_font_t;
typedef struct hb_set_t hb_set_t;

#ifdef __cplusplus 
}
#endif
// 内存资源分配器
typedef struct mem_resource_t {
	void* vf;
	size_t _Align;
	void* ptr;
}mem_resource_t;

typedef struct quadratic_v_t
{
	vec2 p0, p1, p2;
}quadratic_v_t;
typedef struct cubic_v_t
{
	vec2 p0, p1, p2, p3;	// p1 p2是控制点
}cubic_v_t;
/*
用 bezier curve（贝塞尔曲线） 来设置 color stop（颜色渐变规则），
这里使用下面的曲线形式，其中
X轴为 offset（偏移量，取值范围为 0~1，0 代表阴影绘制起点），
Y轴为 alpha（颜 色透明度，取值范围为0~1，0 代表完全透明），
*/
typedef struct rect_shadow_vt
{
	float radius;// = 4;	// 半径
	int segment;// = 6;	// 细分段
	vec4 cfrom;// = { 0,0,0,0.8 }, 
	vec4 cto;// = { 0.5,0.5,0.5,0.5 };// 颜色从cf到ct
	/*	cubic
		X轴为 offset（偏移量，取值范围为 0~1，0 代表阴影绘制起点），
		Y轴为 alpha（颜 色透明度，取值范围为0~1，0 代表完全透明），
	*/
	cubic_v_t cubic;// = { {0.0,0.6},{0.5,0.39},{0.4,0.1},{1.0,0.0 } };
}rect_shadow_vt;

#ifdef __cplusplus
// 线，二阶曲线，三阶曲线
enum class path_type_et :uint32_t
{
	e_vmove = 1,// 移动
	e_vline,	// 直线
	e_vcurve,	// 二次曲线
	e_quadratic = e_vcurve,	// 二次曲线
	e_vcubic,	// 三次曲线
	e_close		// 封闭线段
};

enum vg_line_cap_t :uint8_t {
	VG_LINE_CAP_BUTT,
	VG_LINE_CAP_ROUND,
	VG_LINE_CAP_SQUARE
};

enum vg_line_join_t :uint8_t {
	VG_LINE_JOIN_MITER,
	VG_LINE_JOIN_ROUND,
	VG_LINE_JOIN_BEVEL
};
enum vg_fill_rule_t {
	VG_FILL_RULE_EVEN_ODD,
	VG_FILL_RULE_NON_ZERO
};
enum vg_extend_t :uint8_t {
	VG_EXTEND_NONE,
	VG_EXTEND_REPEAT,
	VG_EXTEND_REFLECT,
	VG_EXTEND_PAD
};

enum vg_filter_t :uint8_t {
	VG_FILTER_FAST,
	VG_FILTER_GOOD,
	VG_FILTER_BEST,
	VG_FILTER_NEAREST,
	VG_FILTER_BILINEAR,
	VG_FILTER_GAUSSIAN,
};
enum vg_pattern_type_t :uint8_t {
	VG_PATTERN_TYPE_SOLID,        // 单色
	VG_PATTERN_TYPE_SURFACE,      // 纹理填充
	VG_PATTERN_TYPE_LINEAR,       // 线性渐变 /*!< linear gradient pattern */
	VG_PATTERN_TYPE_RADIAL,       // 径向渐变 /*!< radial gradient pattern */
	VG_PATTERN_TYPE_MESH,         // 网格渐变 /*!< not implemented */
	VG_PATTERN_TYPE_RASTER_SOURCE, //
	VG_PATTERN_TYPE_SWEEP,			// 锥形渐变 
};
enum vg_clip_state_t :uint8_t {
	vg_clip_state_none = 0x00,
	vg_clip_state_clear = 0x01,
	vg_clip_state_clip = 0x02,
	vg_clip_state_clip_saved = 0x06,
};

enum class vg_operator_t :uint8_t {
	VG_OPERATOR_CLEAR,

	VG_OPERATOR_SOURCE,
	VG_OPERATOR_OVER,
	VG_OPERATOR_DIFFERENCE,
	VG_OPERATOR_MAX,
};
enum vg_pipe_t :uint8_t {
	VG_PIPE_OVER,
	VG_PIPE_CLEAR,
	VG_PIPE_SUB,
	VG_PIPE_CLIPPING
};
#else
typedef uint32_t path_type_et;

typedef uint8_t vg_line_cap_t;
typedef uint8_t vg_line_join_t;
typedef uint8_t vg_fill_rule_t;
typedef uint8_t vg_extend_t;

typedef uint8_t vg_filter_t;
typedef uint8_t vg_pattern_type_t;
typedef uint8_t vg_clip_state_t;

typedef uint8_t vg_operator_t;
typedef uint8_t vg_pipe_t;
#endif
// 布局相关
#if 1
/*
	根元素要求
	assert(parent == NULL);
	assert(!isnan(width));
	assert(!isnan(height));
	assert(self_sizing == NULL);

	FLEX_ALIGN_SPACE_BETWEEN,	//两端对齐，两端间隔0，中间间隔1
	FLEX_ALIGN_SPACE_AROUND,	//分散居中,两端间隔0.5，中间间隔1
	FLEX_ALIGN_SPACE_EVENLY,	//分散居中,每个间隔1
*/
#ifdef __cplusplus
enum class flex_align :uint8_t {
	ALIGN_AUTO = 0,
	ALIGN_STRETCH,
	ALIGN_CENTER,
	ALIGN_START,
	ALIGN_END,
	ALIGN_SPACE_BETWEEN,
	ALIGN_SPACE_AROUND,
	ALIGN_SPACE_EVENLY,
	ALIGN_BASELINE
};

enum class flex_position :uint8_t {
	POS_RELATIVE = 0,
	POS_ABSOLUTE
};
// row行，reverse反向，column列
enum flex_direction :uint8_t {
	ROW = 0,
	ROW_REVERSE,
	COLUMN,
	COLUMN_REVERSE
};

enum class flex_wrap :uint8_t {
	NO_WRAP = 0,
	WRAP,
	WRAP_REVERSE
};
#else
typedef uint8_t flex_align;
typedef uint8_t flex_position;
typedef uint8_t flex_direction;
typedef uint8_t flex_wrap;
#endif // __cplusplus
typedef struct flex_data {
	float width, height;	// 大小NAN
	float left, right, top, bottom;	// 偏移
	float padding_left;		// 本元素内边距
	float padding_right;
	float padding_top;
	float padding_bottom;
	float margin_left;		// 本元素外边距
	float margin_right;
	float margin_top;
	float margin_bottom;
	float grow;		// 子元素:自身放大比例，默认为0不放大
	float shrink;	// 子元素:空间不足时自身缩小比例，默认为1自动缩小，0不缩小
	int	  order;	// 子元素:自身排列顺序。数值越小，越靠前
	float basis;// = -1;	// 子元素:定义最小空间NAN
	float baseline; // 基线位置
	flex_align justify_content;// = flex_align::ALIGN_START;	// 父元素:主轴上的元素的排列方式 start\end\center\space-between\space-around\space-evenly
	flex_align align_content;// = flex_align::ALIGN_STRETCH;	// 父元素:适用多行的flex容器 start\end\center\space-between\space-around\space-evenly\stretch 
	flex_align align_items;// = flex_align::ALIGN_STRETCH;		// 父元素:副轴上的元素的排列方式 start\end\center\stretch\baseline
	flex_align align_self;// = flex_align::ALIGN_AUTO;			// 子元素:覆盖父容器align-items的设置
	flex_position position;// = flex_position::POS_RELATIVE;	// 子元素:
	flex_direction direction;// = flex_direction::ROW;			// 父元素:
	flex_wrap wrap;// = flex_wrap::NO_WRAP;					// 父元素:是否换行，超出宽度自动换行
	bool should_order_children;// = false;
}flex_data;
typedef struct flex_data1 {
	float width, height;		// 大小NAN
	float offset[4];// = { 0, 0, 0, 0 };	// 偏移left, right, top, bottom
	float margin[4];//= { 0, 0, 0, 0 };		// 本元素内边距
	float padding[4];//= { 0, 0, 0, 0 };		// 本元素外边距
	float grow;		// 子元素:自身放大比例，默认为0不放大
	float shrink;	// 子元素:空间不足时自身缩小比例，默认为1自动缩小，0不缩小
	int	  order;	// 子元素:自身排列顺序。数值越小，越靠前
	float basis;//= -1;	// 子元素:定义最小空间NAN
	float baseline; // 基线位置
	flex_align justify_content;// = flex_align::ALIGN_START;	// 父元素:主轴上的元素的排列方式 start\end\center\space-between\space-around\space-evenly
	flex_align align_content;// = flex_align::ALIGN_STRETCH;	// 父元素:适用多行的flex容器 start\end\center\space-between\space-around\space-evenly\stretch 
	flex_align align_items;// = flex_align::ALIGN_STRETCH;		// 父元素:副轴上的元素的排列方式 start\end\center\stretch\baseline
	flex_align align_self;// = flex_align::ALIGN_AUTO;			// 子元素:覆盖父容器align-items的设置
	flex_position position;// = flex_position::POS_RELATIVE;	// 子元素:
	flex_direction direction;//= flex_direction::ROW;			// 父元素:
	flex_wrap wrap;// = flex_wrap::NO_WRAP;					// 父元素:是否换行，超出宽度自动换行
	bool should_order_children;// = false;
}flex_data1;

typedef struct node_dt node_dt;
struct node_dt
{
	vec2 size;	// in 原大小
	vec4 offset;	// in 偏移位置
	vec4 frame;	// out 输出位置大小
	size_t index;		// in 样式序号
	float baseline;	// in 基线位置
	int position;		// in 位置,0=relative，1=absolute
	node_dt* child;		// in 子元素指针
	size_t child_count;
	size_t tidx;		// out 自动计算节点索引
	size_t parent;		// out 自动计算父节点索引
	size_t line_count;	// out 行数量
};

typedef struct flex_run flex_run;
#endif // !1

// 渲染命令
#if 1
typedef struct push_constants_t {
	mat3x2 mat;
	mat3x2 matInv;
	vec4 source;
	vec2 size;
	uint32_t fsq_patternType;
	float opacity;
}push_constants_t;
#define MAX_STOPS 32
typedef struct vg_gradient_t {
	vec4 colors[MAX_STOPS];
	float stops[MAX_STOPS];
	vec4 cp[2];
	ivec4 m;
	vec2 scale;	// 缩放目标
	uint32_t count;
	int extend;
	int type;
}vg_gradient_t;

typedef struct font_family_t {
	hb_font_t* font;
	hb_set_t* coverage;  // hb_face_collect_unicodes
	float upem;
	// 从 hb_font_extents
	float ascender;
	float descender;
	float line_gap;
}font_family_t;
typedef struct font_familys_t {
	font_family_t** familys;
	int count;
}font_familys_t;

typedef struct vg_pattern_t {
	int					status;
	uint32_t            references;
	vg_extend_t       extend;
	vg_filter_t       filter;
	vg_pattern_type_t	type;
	bool                hasMatrix;
	mat3x2			matrix;
	void* data;	// Surface指针或vg_gradient_t
}vg_pattern_t;

typedef struct vg_state_save_t {
	float		lineWidth;
	float		miterLimit;
	uint32_t	dashCount;  // value count in dash array, 0 if dash not set.
	float		dashOffset; // an offset for dash
	float* dashes;     // an array of alternate lengths of on and off stroke.
	vg_operator_t curOperator;
	uint8_t		lineCap;
	uint8_t		lineJoin;
	uint8_t		curFillRule;
	push_constants_t	pushConsts;
	uint32_t			color;
	vg_pattern_t* pattern;
	vg_clip_state_t		clippingState;
	uint32_t			references;
	bool aa;
}vg_state_save_t;
enum depth_stencil_State {
	d_depthtest_enable = 0x01,
	d_depthwrite_enable = 0x02,
	d_stenciltest_enable = 0x04
};
// 混合模式 
enum blendMode_e {
	none = -1,	// 不混合
	normal = 0,	// 普通混合
	additive,
	multiply,
	modulate,
	screen,
	normal_prem,	// 预乘alpha
	additive_prem,
};
enum shader_type_e {
	ST_NONE,
	ST_MASK,
	ST_DOUBLESIDED,
	ST_INSTANCE,
	ST_INSTANCE_DOUBLESIDED,
};
enum cmd_type_e {
	DRAW_VG,
	DRAW_GEOM,
};
// 0矢量图管线						2d
// 0普通三角形(纹理)					2d/3d		tex0
// 1普通三角形+遮罩纹理				2d/3d		tex0、tex1
// 2双面三角形(两种颜色/纹理)			doubleSided	tex0
// 3三角形(纹理)实例化							ubo0、tex1
// 4双面三角形(两种颜色/纹理)实例化				ubo0、tex1
typedef struct gem_info_t {
	int8_t blendMode;
	uint8_t topology;
	uint8_t polygon;
	uint8_t frontFace;	// COUNTER_CLOCKWISE = 0, CLOCKWISE = 1,
	uint8_t cullMode;	// NONE=0, FRONT=1, BACK=2, FRONT_AND_BACK=3
	uint8_t flags;		// depthTestEnable, depthWriteEnable, stencilTestEnable
	uint8_t lineWidth;
	uint8_t shader;		// shader_type_e
}gem_info_t;

// 普通三角形命令
typedef struct geom_cmd_t {
	int stype;// = 1;
	gem_info_t state;
	void* texture;
	void* texture_mask;
	mat4 mat;// = mat4(1.0f);	// 矩阵
	float mask_time;// = 1.0;				// 遮罩时间
	float multiply;				// 预乘1.0
	uint32_t elemCount;				// 元素计数，索引数量或顶点数量
	uint32_t firstIndex;			// -1则非索引渲染
	int32_t  vertexOffset;
	size_t v_offset;				// vbo绑定偏移：0单面，1双面
	mat4* instance_mat;		// 实例矩阵
	uint32_t instance_count;		// 实例数量
	uint32_t instance_ssbo_pos;		// 实例数据在gpu偏移,由vg计算
}geom_cmd_t;

// 矢量命令
typedef struct vgcmd_t {
	int stype;// = 0;
	int full_screen_quad;// = -1;
	ivec2 vertex;			// 顶点开始、数量
	ivec2 index;			// 索引开始、数量
	vg_state_save_t* state;	// 渲染参数
	vec4 bounds;			// 全屏填充,odd/clip专用
	int8_t type;				// 类型：填充0、描边1、裁剪2、全屏3、清屏4
	int8_t ref;					// 参考值
}vgcmd_t;
typedef union gcmd_t {
	vgcmd_t vg;
	geom_cmd_t g;
}gcmd_t;
typedef struct ovgVertex {
	vec2	pos;
	vec2	uv;
	uint32_t	color;
}ovgVertex;
typedef struct geomVertex1 {
	vec3 pos;
	vec2 uv;
	uint32_t color;
}geomVertex1;
typedef struct geomVertex2 {
	vec3 pos;
	vec2 uv;
	uint32_t color;
	uint32_t color1;
}geomVertex2;
enum vg_format_t {
	VG_FORMAT_RGBA8 = 0,	// 对应SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM
	VG_FORMAT_BGRA8,
	VG_FORMAT_RGBA8_SRGB,	// SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM_SRGB
	VG_FORMAT_BGRA8_SRGB,
	VG_FORMAT_RGBA16F,
	VG_FORMAT_RGBA32F,
};

// 图片
typedef struct vg_image_t {
	uint32_t id;		// 由设备分配
	int width, height;		// 更新纹理时自动更新大小
	bool valid;	// 是否要更新到纹理
	bool copy_status;	// 复制状态
}vg_image_t;
/**
 * 更新图片数据（不重建句柄）：
 *   - 尺寸不变 → 覆盖上传（partial=true 时只传脏矩形）
 *   - 尺寸变化 → 后端重建纹理，旧纹理延迟释放
 */
typedef struct vg_image_desc_t {
	vg_image_t* img;		// 需要更新的纹理
	uint32_t	width;
	uint32_t	height;
	uint32_t	format;
	uint32_t	stride;
	void* pixels;			// CPU 像素数据。is_copy=false时vg_image_t.copy_status等于true时才能释放
	uint32_t x, y, w, h;	// 更新矩形区域
	uint32_t idx;			// 纹理数组层号
	uint32_t px_size;		// 像素字节大小
	bool is_destroy;		// true = 删除纹理；
	bool is_copy;			// 是否要复制内存；
}vg_image_desc_t;

#endif

enum ImageFlipMode
{
	FLIP_NONE,			// 不翻转
	FLIP_HORIZONTAL,	// 水平翻转 
	FLIP_VERTICAL,		// 垂直翻转
	FLIP_HORIZONTAL_AND_VERTICAL = (FLIP_HORIZONTAL | FLIP_VERTICAL)    // 水平和垂直翻转（不是对角翻转）
};

typedef struct image_ptr_t
{
	int width, height;
	int type;				// 0=rgba，1=bgra
	int stride;
	uint32_t* data;			// 像素数据 
	void* ptr;				// 用户数据
	int comp;				// 通道数0单色位图，1灰度图，4rgba/bgra
	int  blendmode;			// 混合模式
	bool static_tex;	// 静态纹理
	bool multiply;		// 预乘的纹理
	bool valid;			// 是否更新到纹理
}image_ptr_t;


#ifdef __cplusplus
struct ovg_image_data :public vg_image_t {
	int format;			// 0 rgba, 1 bgra
	int stride;			// 像素宽度
	uint32_t* data;
	bool multiply;		// 是否预乘  
};
#else
typedef struct ovg_image_data {
	vg_image_t v;
	int format;			// 0 rgba, 1 bgra
	int stride;			// 像素宽度
	uint32_t* data;
	bool multiply;		// 是否预乘  
}ovg_image_data;
#endif

typedef struct ovg_image_r
{
	vg_image_t* img;
	ivec4 rc;		// 所在纹理区域
	ivec4 sliced;	// 九宫格 
	ivec4 dst;		// 渲染坐标大小 
	uint32_t color;		// 混合颜色
	int8_t type;		// img的类型
}ovg_image_r;
typedef struct text_st_t {
	vec2 pos;
	vec2 size;
	vec4 clip;		// 裁剪区域
	const char* text;
	int text_len;
}text_st_t;

// 文本样式
typedef struct text_style_t
{
	font_familys_t* family;
	float fontsize;
	float lineheight;
	vec2 align;// = { 0.50,0.50 };	// 文本对齐
	vec2 shadow_pos;// = { 1.0,1.0 };
	float stroke;					// 描边宽度，正数使用矢量路径实现、负数用偏移4方向实现描边
	uint32_t color;// = 0xffc2c2c2;		// 文本颜色
	uint32_t color_stroke;// = 0xff000000;	// 描边颜色
	uint32_t color_shadow;			// 阴影颜色	0xcc121212;
	int min_subpixel;
}text_style_t;
// 文本区域
typedef struct text_box_rt {
	ivec4 rc;		// 设置文本渲染区域，偏移/大小
	vec2 text_align;// 文本对齐
	int8_t auto_break;	// 是否自动换行
	int8_t word_wrap;// = 2;	// 0字符换行，1单词换行，2换行点，3句子断开，4标题大小写断点
	int8_t ellipsis;	// 省略号
}text_box_rt;

// Stencil 位平面
#ifndef STENCIL_CLIP_BIT
#define STENCIL_CLIP_BIT    0x1   // bit1: 裁剪掩码（REPLACE 写入）
#endif
// 路径对象
typedef struct ovg_path_t ovg_path_t;
// 矢量对象 
typedef struct rvg_t {
	int width, height;
	ovg_path_t* path;
	vg_state_save_t* st;
}rvg_t;
// 打包数据引用
typedef struct ovg_draw_data_t {
	gcmd_t* d;				// 渲染命令列表
	size_t count;
	ovgVertex* vg_vertex;	// 矢量顶点
	size_t v_count;
	uint32_t* vg_indices;	// 矢量索引
	size_t i_count;
	size_t uboCount;		// 渐变ubo结构数量
	geomVertex1* vertex1;	// 单面顶点
	size_t v1_count;
	geomVertex2* vertex2;	// 双面顶点
	size_t v2_count;
	uint32_t* geom_indices;	// 索引 
	size_t ig_count;		// 索引数量
	size_t instance_count;	// 实例矩阵总数量
	void* instance_data;	// 实例所有数据
	vg_image_desc_t** image_desc;
	size_t image_desc_count;	// 更新纹理数量
	gem_info_t* pipeinfo;
	size_t pipeinfo_count;	// 需要build的管线数量
	uvec2 vg_offset;	// 渲染自动计算用
	uvec3 geom_offset;
}ovg_draw_data_t;

// 录制
typedef struct ovg_recording_t ovg_recording_t;

// 快捷对象命令模式，没有destroy函数的对象都不需要手动释放，rvg_t可创建多个做缓存
typedef struct ovg_ctx_cb {
	mem_resource_t* ac;	// 内存分配器，由new_ctx_cb自己创建 	
	// 渲染操作，rvg_t可以多次执行fill或stroke/clip
	rvg_t* (*new_rvg)(mem_resource_t* ac);
	void (*destroy_rvg)(rvg_t* p);
	void(*clear)(rvg_t* v);			// 清空画布 
	// 路径操作
	ovg_path_t* (*get_path)(rvg_t* ctx);
	void (*new_path)(rvg_t* ctx);
	void(*clear_path)(rvg_t* ctx);
	void(*close_path)(rvg_t* ctx);
	void(*new_sub_path)(rvg_t* ctx);
	void(*path_extents)(rvg_t* ctx, float* x1, float* y1, float* x2, float* y2);
	void(*get_current_point)(rvg_t* ctx, float* x, float* y);
	size_t(*get_segment_count)(rvg_t* ctx);
	void(*set_segment_color)(rvg_t* ctx, size_t idx, uint32_t color);
	// 添加数据到当前路径，参考path_type_e
	void(*add_path)(rvg_t* ctx, float* data, size_t count);
	void(*move_to)(rvg_t* ctx, float x, float y);
	void(*rel_move_to)(rvg_t* ctx, float x, float y);
	void(*line_to)(rvg_t* ctx, float x, float y);
	void(*rel_line_to)(rvg_t* ctx, float dx, float dy);
	void(*arc)(rvg_t* ctx, float xc, float yc, float radius, float a1, float a2);
	void(*arc_negative)(rvg_t* ctx, float xc, float yc, float radius, float a1, float a2);
	// 有缩放时，先执行set_path一次再执行curve_to
	void(*curve_to)(rvg_t* ctx, float x1, float y1, float x2, float y2, float x3, float y3);
	void(*rel_curve_to)(rvg_t* ctx, float x1, float y1, float x2, float y2, float x3, float y3);
	void(*quadratic_to)(rvg_t* ctx, float x1, float y1, float x2, float y2);
	void(*rel_quadratic_to)(rvg_t* ctx, float x1, float y1, float x2, float y2);
	void(*rectangle)(rvg_t* ctx, float x, float y, float w, float h);
	void(*rounded_rectangle)(rvg_t* ctx, float x, float y, float w, float h, float radius);
	void(*rounded_rectangle2)(rvg_t* ctx, float x, float y, float w, float h, float rx, float ry);
	void(*ellipse)(rvg_t* ctx, float radiusX, float radiusY, float x, float y, float rotationAngle);
	void(*elliptic_arc_to)(rvg_t* ctx, float x, float y, bool large_arc_flag, bool sweep_flag, float rx, float ry, float phi);
	void(*rel_elliptic_arc_to)(rvg_t* ctx, float x, float y, bool large_arc_flag, bool sweep_flag, float rx, float ry, float phi);
	void(*circle)(rvg_t* ctx, float x, float y, float radius);
	// 配置状态
	void(*set_opacity)(rvg_t* ctx, float opacity);
	void(*set_source_color)(rvg_t* ctx, uint32_t c);
	void(*set_source_rgba)(rvg_t* ctx, float r, float g, float b, float a);
	void(*set_source_rgb)(rvg_t* ctx, float r, float g, float b);
	void(*set_line_width)(rvg_t* ctx, float width);
	void(*set_miter_limit)(rvg_t* ctx, float limit);
	void(*set_line_cap)(rvg_t* ctx, int cap);
	void(*set_line_join)(rvg_t* ctx, int join);
	void(*set_source_surface)(rvg_t* ctx, vg_image_t* surf, float x, float y);
	void(*set_source)(rvg_t* ctx, vg_pattern_t* pat);
	void(*set_operator)(rvg_t* ctx, int op);
	void(*set_fill_rule)(rvg_t* ctx, int fr);
	void(*set_dash)(rvg_t* ctx, const float* dashes, uint32_t num_dashes, float offset);		// 虚线
	void(*set_dash8)(rvg_t* ctx, uint64_t dashes, uint32_t num_dashes, float offset);								// 虚线,用uint8_t v8[8]表示
	void(*translate)(rvg_t* ctx, float dx, float dy);
	void(*scale)(rvg_t* ctx, float sx, float sy);
	void(*rotate)(rvg_t* ctx, float radians);
	void(*transform)(rvg_t* ctx, const void* matrix);
	void(*set_matrix)(rvg_t* ctx, const void* matrix);
	void(*get_matrix)(rvg_t* ctx, void* matrix);
	void(*identity_matrix)(rvg_t* ctx);
	void(*matrix_init)(void* mat, float xx, float yx, float xy, float yy, float x0, float y0);

	// 图案：渐变/图片 
	vg_pattern_t* (*new_pattern_linear)(rvg_t* ctx, float x0, float y0, float x1, float y1);
	vg_pattern_t* (*new_pattern_radial)(rvg_t* ctx, float cx0, float cy0, float radius0, float cx1, float cy1, float radius1, bool is_ellipse);
	vg_pattern_t* (*new_pattern_sweep)(rvg_t* ctx, float cx, float cy, float start_angle, float end_angle);
	int (*pattern_add_color_stop)(vg_pattern_t* pat, float o, float r, float g, float b, float a);
	int (*pattern_set_color_stop)(vg_pattern_t* pat, int idx, float o, float r, float g, float b, float a);
	void(*pattern_set_matrix)(vg_pattern_t* pat, const void* matrix);	// mat3x2
	void(*pattern_set_extend)(vg_pattern_t* pat, int extend);
	void(*pattern_set_filter)(vg_pattern_t* pat, int filter);

	// 更新纹理，vg_image_t指针由用户创建管理
	void (*image_update)(rvg_t* p, vg_image_t* img, vg_image_desc_t* desc);
	//标记图片不再使用（后端延迟释放 GPU 纹理）
	void (*image_destroy)(rvg_t* p, vg_image_t* img);

	void(*save)(rvg_t* v);		// 保存状态，（裁剪状态暂不实现）
	void(*restore)(rvg_t* v);	// 恢复状态
	void(*stroke)(rvg_t* v);
	void(*stroke_preserve)(rvg_t* v);
	void(*fill)(rvg_t* v);
	void(*fill_preserve)(rvg_t* v);
	void(*paint)(rvg_t* v);			// 全屏渲染
	void(*reset_clip)(rvg_t* v, uint8_t ref);	// 重置裁剪，0清空，1全部通过
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
	// todo 录制
	void (*start_recording)(rvg_t* ctx);
	ovg_recording_t* (*stop_recording)(rvg_t* ctx);
	void (*replay)(rvg_t* ctx, ovg_recording_t* rec);
	void (*replay_command)(rvg_t* ctx, ovg_recording_t* rec, uint32_t cmdIndex);
	uint32_t(*recording_get_count)(ovg_recording_t* rec);
	void* (*recording_get_data)(ovg_recording_t* rec);
	void  (*recording_destroy)(ovg_recording_t* rec);

}ovg_ctx_cb;
// 工具函数：渲染两色交替的格子
void draw_grid_fill(rvg_t* vg, vec2 size, ivec2 cols, int width);
// tiny flex
flex_run* new_flex_run(mem_resource_t* a);
void free_flex_run(flex_run* p);
mem_resource_t* flex_run_ac(flex_run* p);
vec4 flex_run_layout(flex_run* ctx, flex_data* fd, size_t count, node_dt* p, size_t node_count);

// 字体相关
#ifdef __cplusplus 
class font_cache_cx;
#else
typedef void* font_cache_cx;
#endif
font_cache_cx* new_font_cache();
void free_font_cache(font_cache_cx* p);
font_familys_t* new_font_family(font_cache_cx* p, const char* familys, const char* style);
void delete_font_family(font_familys_t* p);
vec3 get_font_extents(hb_font_t* font, int height, bool vert);
// 创建接口实例
ovg_ctx_cb* new_ctx_cb();
void free_ctx_cb(ovg_ctx_cb*);
// 获取渲染数据
ovg_draw_data_t get_draw_list(rvg_t* p);

#endif // !OVG_H
