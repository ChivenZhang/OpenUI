#pragma once
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

#if defined( _MSVC_LANG )
#	define OPENVG_CPLUSPLUS _MSVC_LANG
#else
#	define __FUNCTION__ __func__
#	define OPENVG_CPLUSPLUS __cplusplus
#endif
#if 201703L < OPENVG_CPLUSPLUS
#	define OPENVG_CPP_VERSION 20
#elif 201402L < OPENVG_CPLUSPLUS
#	define OPENVG_CPP_VERSION 17
#elif 201103L < OPENVG_CPLUSPLUS
#	define OPENVG_CPP_VERSION 14
#elif 199711L < OPENVG_CPLUSPLUS
#	define OPENVG_CPP_VERSION 11
#else
#	define OPENVG_CPP_VERSION 0
#endif
#if OPENVG_CPP_VERSION < 17
#	error "At least c++ standard version 17"
#endif

// ============================================

#ifdef OPENVG_SHARED_LIBRARY
#	if defined(_WIN32)
#		define OPENVG_API __declspec(dllexport)
#		define OPENVG_C_API extern "C" __declspec(dllexport)
#	else
#		define OPENVG_API __attribute__((visibility("default")))
#		define OPENVG_C_API extern "C" __attribute__((visibility("default")))
#	endif
#else
#	if defined(_WIN32)
#		define OPENVG_API 
#		define OPENVG_C_API extern "C" 
#	else
#		define OPENVG_API 
#		define OPENVG_C_API extern "C"
#	endif
#endif

#ifdef _WIN32
#	ifdef _DEBUG
#		define OPENVG_DEBUG_MODE
#	endif
#endif

// ============================================

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning(error:4150)
#pragma warning(disable:4250)
#pragma warning(disable:4200)
#pragma warning(disable:26812)
#pragma warning(disable:26815)
#pragma warning(disable:26816)

#define _USE_MATH_DEFINES
#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <bitset>
#include <condition_variable>
#include <deque>
#include <exception>
#include <functional>
#include <future>
#include <iostream>
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <span>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// ============================================

#include <assert.h>
#define VGAssert(...) assert(__VA_ARGS__)

// ============================================

#include <cstdio>
#include <ctime>
#include <thread>
#define VG_FORMAT(TARGET, FORMAT, LEVEL, ...) \
do { \
char __DATETIME__[32]; auto __NOWTIME__ = std::time(nullptr); \
std::strftime(__DATETIME__, sizeof(__DATETIME__), "%Y-%m-%d %H:%M:%S", std::localtime(&__NOWTIME__)); \
auto __THREAD__ = []()->uint32_t { std::stringstream ss; ss << std::this_thread::get_id(); return std::stoul(ss.str()); }(); \
std::fprintf(TARGET, "%s:%d\n" "%s " #LEVEL " %d --- " FORMAT "\n\n", __FILE__, __LINE__, __DATETIME__, __THREAD__, ##__VA_ARGS__); \
} while (0)

#ifndef VG_DEBUG
#ifdef OPENVG_DEBUG_MODE
#	define VG_DEBUG(FORMAT, ...) VG_FORMAT(stdout, FORMAT, DEBUG, ##__VA_ARGS__)
#else
#	define VG_DEBUG(FORMAT, ...)
#endif
#endif

#ifndef VG_WARN
#	define VG_WARN(FORMAT, ...) VG_FORMAT(stdout, FORMAT, WARN, ##__VA_ARGS__)
#endif

#ifndef VG_INFO
#	define VG_INFO(FORMAT, ...) VG_FORMAT(stdout, FORMAT, INFO, ##__VA_ARGS__)
#endif

#ifndef VG_ERROR
#	define VG_ERROR(FORMAT, ...) VG_FORMAT(stderr, FORMAT, ERROR, ##__VA_ARGS__)
#endif

#ifndef VG_FATAL
#	define VG_FATAL(FORMAT, ...) do{ VG_FORMAT(stderr, FORMAT, FATAL, ##__VA_ARGS__); std::abort(); } while(0)
#endif

#ifndef VG_PRINT
#	define VG_PRINT(FORMAT, ...) VG_INFO(FORMAT, ##__VA_ARGS__)
#endif

// ============================================

template<class T>
using VGRaw = T*;
template<class T>
using VGRef = std::shared_ptr<T>;
template<class T>
using VGHnd = std::weak_ptr<T>;
using VGString = std::string;
using VGCString = const char*;
using VGWString = std::wstring;
#if 20 <= OPENVG_CPP_VERSION
using VGString8 = std::u8string;
#endif
using VGString16 = std::u16string;
using VGString32 = std::u32string;
#if 17 <= OPENVG_CPP_VERSION
using VGStringView = std::string_view;
using VGWStringView = std::wstring_view;
using VGString16View = std::u16string_view;
using VGString32View = std::u32string_view;
#endif
#if 20 <= OPENVG_CPP_VERSION
using VGString8View = std::u8string_view;
#endif
template <class T, size_t N>
using VGArray = std::array<T, N>;
#if 20 <= OPENVG_CPP_VERSION
template <class T, size_t N = std::dynamic_extent>
using VGListView = std::span<T, N>;
#endif
template <class T>
using VGList = std::vector<T>;
template <class T>
using VGDeque = std::deque<T>;
template <class T>
using VGLinkedList = std::list<T>;
template <class T, class L = std::less<T>>
using VGSet = std::set<T, L>;
template <class K, class T, class L = std::less<K>>
using VGMultiSet = std::multiset<K, T, L>;
template <class K, class H = std::hash<K>, class E = std::equal_to<K>>
using VGHashSet = std::unordered_set<K, H, E>;
template <class K, class T, class L = std::less<K>>
using VGMap = std::map<K, T, L>;
template <class K, class T, class L = std::less<K>>
using VGMultiMap = std::multimap<K, T, L>;
template <class K, class T, class H = std::hash<K>, class E = std::equal_to<K>>
using VGHashMap = std::unordered_map<K, T, H, E>;
template <class T>
using VGQueue = std::queue<T>;
template <class T, class C = VGList<T>, class L = std::less<typename C::value_type>>
using VGSortedQueue = std::priority_queue<T, C, L>;
template <class T>
using VGStack = std::stack<T>;
template <size_t N>
using VGBitset = std::bitset<N>;
template <class T, class U>
using VGBinary = std::pair<T, U>;
template <class ...TS>
using VGTuple = std::tuple<TS...>;
using VGAny = std::any;
using VGError = std::exception;
template <class T>
using VGLambda = std::function<T>;
using VGThread = std::thread;
template <class T>
using VGFuture = std::future<T>;
template <class T>
using VGPromise = std::promise<T>;
template <class T>
using VGAtomic = std::atomic<T>;
using VGMutex = std::recursive_mutex;
using VGMutexLock = std::lock_guard<VGMutex>;
using VGUniqueLock = std::unique_lock<VGMutex>;
using VGMutexUnlock = std::condition_variable_any;
using VGStringList = VGList<VGString>;
using VGWStringList = VGList<VGWString>;
#if 20 <= OPENVG_CPP_VERSION
using VGString8List = VGList<VGString8>;
#endif
using VGString16List = VGList<VGString16>;
using VGString32List = VGList<VGString32>;
template<class T>
using VGStringMap = VGMap<VGString, T>;
template<class T>
using VGWStringMap = VGMap<VGWString, T>;
#if 20 <= OPENVG_CPP_VERSION
template<class T>
using VGString8Map = VGMap<VGString8, T>;
#endif
template<class T>
using VGString16Map = VGMap<VGString16, T>;
template<class T>
using VGString32Map = VGMap<VGString32, T>;
template<class T>
using VGStringHashMap = VGHashMap<VGString, T>;
template<class T>
using VGWStringHashMap = VGHashMap<VGWString, T>;
#if 20 <= OPENVG_CPP_VERSION
template<class T>
using VGString8HashMap = VGHashMap<VGString8, T>;
#endif
template<class T>
using VGString16HashMap = VGHashMap<VGString16, T>;
template<class T>
using VGString32HashMap = VGHashMap<VGString32, T>;

// ============================================

template<typename T, typename ... Args>
inline VGRef<T> VGNew(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}
template<typename U, typename T>
inline VGRef<U> VGCast(VGRef<T>&& target)
{
	if (target == nullptr) return nullptr;
	return std::dynamic_pointer_cast<U>(target);
}
template<typename U, typename T>
inline VGRef<U> VGCast(VGRef<T> const& target)
{
	if (target == nullptr) return nullptr;
	return std::dynamic_pointer_cast<U>(target);
}
template<typename U, typename T>
inline VGHnd<U> VGCast(VGHnd<T>&& target)
{
	if (target == nullptr) return VGHnd<U>();
	return std::dynamic_pointer_cast<U>(target.lock());
}
template<typename U, typename T>
inline VGHnd<U> VGCast(VGHnd<T> const& target)
{
	if (target == nullptr) return VGHnd<U>();
	return std::dynamic_pointer_cast<U>(target.lock());
}
template<typename U, typename T>
inline VGRaw<U> VGCast(VGRaw<T>&& target)
{
	if (target == nullptr) return nullptr;
	return dynamic_cast<U*>((T*)target);
}
template<typename U, typename T>
inline VGRaw<U> VGCast(VGRaw<T> const& target)
{
	if (target == nullptr) return nullptr;
	return const_cast<U*>(dynamic_cast<const U*>((const T*)target));
}

inline constexpr uint32_t VGHash32(const char* const first, const size_t count) noexcept
{
	// These FNV-1a utility functions are extremely performance sensitive,
	// check examples like that in VSO-653642 before making changes.
	constexpr uint32_t _FNV_offset_basis = 2166136261U;
	constexpr uint32_t _FNV_prime = 16777619U;
	auto result = _FNV_offset_basis;
	// accumulate range [_First, _First + _Count) into partial FNV-1a hash _Val
	for (size_t i = 0; i < count; ++i)
	{
		result ^= (uint32_t)first[i];
		result *= _FNV_prime;
	}
	return result;
}
inline constexpr uint64_t VGHash64(const char* const first, const size_t count) noexcept
{
	// These FNV-1a utility functions are extremely performance sensitive,
	// check examples like that in VSO-653642 before making changes.
	constexpr uint64_t _FNV_offset_basis = 14695981039346656037ULL;
	constexpr uint64_t _FNV_prime = 1099511628211ULL;
	auto result = _FNV_offset_basis;
	// accumulate range [_First, _First + _Count) into partial FNV-1a hash _Val
	for (size_t i = 0; i < count; ++i)
	{
		result ^= (uint64_t)first[i];
		result *= _FNV_prime;
	}
	return result;
}
inline constexpr uint32_t VGHash(const char* const value) noexcept
{
	size_t count = 0; for (size_t i = 0; value[i]; ++i) ++count;
	return VGHash32(value, count);
}
inline const uint32_t VGHash(VGString const& value) noexcept
{
	return VGHash32(value.c_str(), value.size());
}
#if 17 <= OPENVG_CPP_VERSION
inline const uint32_t VGHash(VGStringView value) noexcept
{
	return VGHash32(value.data(), value.size());
}
#endif

// ============================================

#define VGNAN (NAN)

#define VG_MATH_PI  3.14159265358979323846f

static inline float VGDeg2Rad(float degree)
{
	return degree * (VG_MATH_PI / 180.0f);
}

static inline float VGRad2Deg(float radian)
{
	return radian * (180.0f / VG_MATH_PI);
}

struct VGFloat2
{
	union
	{
		float Value[2]{ };
		struct { float X, Y; };
	};

	float& operator [](size_t index)
	{
		return Value[index];
	}

	float const& operator [](size_t index) const
	{
		return Value[index];
	}
};

inline bool operator ==(VGFloat2 const& a, VGFloat2 const& b)
{
	return a.X == b.X && a.Y == b.Y;
}

struct VGFloat3
{
	union
	{
		float Value[3]{ };
		struct { float X, Y, Z; };
	};

	float& operator [](size_t index)
	{
		return Value[index];
	}

	float const& operator [](size_t index) const
	{
		return Value[index];
	}
};

inline bool operator ==(VGFloat3 const& a, VGFloat3 const& b)
{
	return a.X == b.X && a.Y == b.Y && a.Z == b.Z;
}

struct VGFloat4
{
	union
	{
		float Value[4]{ };
		struct { float X, Y, Z, W; };
	};

	float& operator [](size_t index)
	{
		return Value[index];
	}

	float const& operator [](size_t index) const
	{
		return Value[index];
	}
};

inline bool operator ==(VGFloat4 const& a, VGFloat4 const& b)
{
	return a.X == b.X && a.Y == b.Y && a.Z == b.Z && a.W == b.W;
}

struct VGFloat2x2
{
	union
	{
		VGFloat2 Value[2]{ {1,0},{0,1} };
		struct { VGFloat2 X, Y; };
	};

	VGFloat2& operator [](size_t index)
	{
		return Value[index];
	}

	VGFloat2 const& operator [](size_t index) const
	{
		return Value[index];
	}
};

inline bool operator ==(VGFloat2x2 const& a, VGFloat2x2 const& b)
{
	return a.X == b.X && a.Y == b.Y;
}

struct VGFloat3x3
{
	union
	{
		VGFloat4 Value[3]{ {1,0,0,0},{0,1,0,0},{0,0,1,0} };
		struct { VGFloat4 X, Y, Z; };
	};

	VGFloat4& operator [](size_t index)
	{
		return Value[index];
	}

	VGFloat4 const& operator [](size_t index) const
	{
		return Value[index];
	}

	// 创建单位矩阵
	static VGFloat3x3 Identity()
	{
		return VGFloat3x3();
	}

	// 平移变换矩阵
	static VGFloat3x3 Translate(float tx, float ty)
	{
		VGFloat3x3 matrix = Identity();
		matrix.Value[2][0] = tx;
		matrix.Value[2][1] = ty;
		return matrix;
	}

	// 缩放变换矩阵
	static VGFloat3x3 Scaling(float sx, float sy)
	{
		VGFloat3x3 matrix = Identity();
		matrix.Value[0][0] = sx;
		matrix.Value[1][1] = sy;
		return matrix;
	}

	// 旋转变换矩阵
	static VGFloat3x3 Rotation(float angleInDegrees)
	{
		VGFloat3x3 matrix = Identity();
		float angleInRadians = angleInDegrees * VG_MATH_PI / 180.0f;
		matrix.Value[0][0] = cos(angleInRadians);
		matrix.Value[0][1] = -sin(angleInRadians);
		matrix.Value[1][0] = sin(angleInRadians);
		matrix.Value[1][1] = cos(angleInRadians);
		return matrix;
	}

	static VGFloat3x3 Transform(float tx, float ty, float rz, float sx = 1, float sy = 1)
	{
		VGFloat3x3 matrix = VGFloat3x3::Scaling(sx, sy) * VGFloat3x3::Rotation(rz) * VGFloat3x3::Translate(tx, ty);
		return matrix;
	}

	friend VGFloat3x3 operator *(const VGFloat3x3& a, const VGFloat3x3& b);
};

inline VGFloat3x3 operator *(const VGFloat3x3& a, const VGFloat3x3& b)
{
	VGFloat3x3 result = { {{0}} };
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			for (int k = 0; k < 3; ++k) {
				result.Value[i][j] += a.Value[i][k] * b.Value[k][j];
			}
		}
	}
	return result;
}

inline bool operator ==(VGFloat3x3 const& a, VGFloat3x3 const& b)
{
	return a.X == b.X && a.Y == b.Y && a.Z == b.Z;
}

struct VGFloat4x4
{
	union
	{
		VGFloat4 Value[4]{ {1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1} };
		struct { VGFloat4 X, Y, Z, W; };
	};

	VGFloat4& operator [](size_t index)
	{
		return Value[index];
	}

	VGFloat4 const& operator [](size_t index) const
	{
		return Value[index];
	}
};

inline bool operator ==(VGFloat4x4 const& a, VGFloat4x4 const& b)
{
	return a.X == b.X && a.Y == b.Y && a.Z == b.Z && a.W == b.W;
}
struct VGRect
{
	float X = 0, Y = 0, W = 0, H = 0;
};

struct VGPoint
{
	float X = 0, Y = 0;
};
static const VGPoint VGNonePoint{ FLT_MAX, FLT_MAX };

enum class VGPointType : uint8_t
{
	Close = 0, ///< Ends the current sub-path and connects it with its initial point. This command doesn't expect any points.
	MoveTo,    ///< Sets a new initial point of the sub-path and a new current point. This command expects 1 point: the starting position.
	LineTo,    ///< Draws a line from the current point to the given point and sets a new value of the current point. This command expects 1 point: the end-position of the line.
	CurveTo,
	CubicTo,   ///< Draws a cubic Bezier curve from the current point to the given point using two given control points and sets a new value of the current point. This command expects 3 points: the 1st control-point, the 2nd control-point, the end-point of the curve.
	ArcTo,
	PieTo,
	ChordTo,
};

struct VGColor
{
	float R = 0, G = 0, B = 0, A = 0;
};

struct VGColorStop
{
	float Offset = 0;
	float R = 0, G = 0, B = 0, A = 0;
};

struct VGImage
{
	uint32_t Width = 0, Height = 0, Stride = 0, Channel = 0;
	union { void* Pixels; size_t Handle = 0; };
	enum : uint8_t { Byte = 0, HWByte, Float, HWFloat, } Format = Byte;
};
using VGImageRaw = VGRaw<VGImage>;

inline bool operator ==(VGImage const& a, VGImage const& b)
{
	return a.Width == b.Width
		&& a.Height == b.Height
		&& a.Stride == b.Stride
		&& a.Channel == b.Channel
		&& a.Handle == b.Handle
		&& a.Format == b.Format;
}
inline bool operator <(VGImage const& a, VGImage const& b)
{
	if (a.Width != b.Width) return a.Width < b.Width;
	if (a.Height != b.Height) return a.Height < b.Height;
	if (a.Stride != b.Stride) return a.Stride < b.Stride;
	if (a.Channel != b.Channel) return a.Channel < b.Channel;
	if (a.Handle != b.Handle) return a.Handle < b.Handle;
	return a.Format < b.Format;
}

enum class VGStrokeCap : uint8_t
{
	Butt = 0,	///< The stroke ends exactly at each of the two end-points of a sub-path. For zero length sub-paths no stroke is rendered.
	Round,      ///< The stroke is extended in both end-points of a sub-path by a half circle, with a radius equal to the half of a stroke width. For zero length sub-paths a full circle is rendered.
	Square,		///< The stroke is extended in both end-points of a sub-path by a rectangle, with the width equal to the stroke width and the length equal to the half of the stroke width. For zero length sub-paths the square is rendered with the size of the stroke width.
};

enum class VGStrokeJoin : uint8_t
{
	Miter = 0,	///< The outer corner of the joined path segments is spiked. The spike is created by extension beyond the join point of the outer edges of the stroke until they intersect. In case the extension goes beyond the limit, the join style is converted to the Bevel style.
	Round,		///< The outer corner of the joined path segments is rounded. The circular region is centered at the join point.
	Bevel,		///< The outer corner of the joined path segments is bevelled at the join point. The triangular region of the corner is enclosed by a straight line between the outer corners of each stroke.
};

#define VG_FLAGS_IMAGE_COLOR 0x0001
#define VG_FLAGS_IMAGE_GLYPH 0x0002
#define VG_FLAGS_IMAGE_HWGPU 0x0004
#define VG_FLAGS_STYLE_LINEAR 0x0010
#define VG_FLAGS_STYLE_RADIAL 0x0020

struct VGPrimitive
{
	static constexpr int MAX_STOP_COUNT = 16;
	struct point_t
	{
		float X = 0, Y = 0;
		float U = 0, V = 0;
		int32_t Style = -1, Matrix = -1;
	};
	struct style_t
	{
		VGColor Color;
		int32_t Flags = 0; // [fill|stroke, linear|radial, color|image, absolute|relative uv]
		int32_t Image = -1;
		int32_t Linear = -1;
		int32_t Radial = -1;
	};
	struct linear_t
	{
		VGFloat4  NumStops;	// [Count, ?, Spread, ?]
		VGFloat2  GradStartPos;
		VGFloat2  GradEndPos;
		VGFloat4  StopPoints[MAX_STOP_COUNT / 4];
		VGColor   StopColors[MAX_STOP_COUNT];
	};
	struct radial_t
	{
		VGFloat4  NumStops;	// [Count, ?, Spread, ?]
		VGFloat2  CenterPos;
		VGFloat2  Radius;
		VGFloat4  StopPoints[MAX_STOP_COUNT / 4];
		VGColor   StopColors[MAX_STOP_COUNT];
	};
	struct matrix_t
	{
		VGFloat4 Scissor;
		VGFloat3x3 Transform;
	};
	using image_t = VGImage;
	using scissor_t = VGRect;

	VGList<point_t> PointList;
	VGList<style_t> StyleList;
	VGList<image_t> ImageList;
	VGList<linear_t> LinearList;
	VGList<radial_t> RadialList;
	VGList<matrix_t> MatrixList;
	VGList<scissor_t> ScissorList;
};
using VGPrimitiveRaw = VGRaw<VGPrimitive>;
using VGPrimitiveRef = VGRef<VGPrimitive>;

enum VGTextStyle
{
	StyleNormal,
	StyleOblique,
	StyleItalic,
};

enum VGTextWeight
{
	WeightThin = 100,
	WeightUltralight = 200,
	WeightLight = 300,
	WeightSemilight = 350,
	WeightBook = 380,
	WeightNormal = 400,
	WeightMedium = 500,
	WeightSemibold = 600,
	WeightBold = 700,
	WeightUltrabold = 800,
	WeightHeavy = 900,
	WeightUltraheavy = 1000,
};

enum VGTextAlgin
{
	AlignLeft = 0x0001,
	AlignRight = 0x0002,
	AlignCenter = 0x0004,
	AlignJustify = 0x0008,
	AlignTop = 0x0020,
	AlignBottom = 0x0040,
	AlignVCenter = 0x0080,
	AlignBaseline = 0x0100,
};
using VGTextAlgins = uint32_t;

enum VGTextDirection
{
	DirectionAutoLayout = 0,
	DirectionLeftToRight = 1,
	DirectionRightToLeft = 2,
};

enum VGTextEllipsize
{
	EllipsizeNone = 0,
	EllipsizeStart = 1,
	EllipsizeMiddle = 2,
	EllipsizeEnd = 3,
};

inline VGRect VGOverlap(VGRect const& viewport, VGRect const& client)
{
	// 计算两个矩形的右下角坐标  
	float xA1 = viewport.X;
	float yA1 = viewport.Y;
	float xA2 = viewport.X + viewport.W;
	float yA2 = viewport.Y + viewport.H;
	float xB1 = client.X;
	float yB1 = client.Y;
	float xB2 = client.X + client.W;
	float yB2 = client.Y + client.H;

	// 检查是否有重叠  
	if (xA2 <= xB1 || xB2 <= xA1 || yA2 <= yB1 || yB2 <= yA1) return VGRect{};

	// 计算重叠区域的左上角坐标  
	float overlapX1 = std::max(xA1, xB1);
	float overlapY1 = std::max(yA1, yB1);

	// 计算重叠区域的右下角坐标  
	float overlapX2 = std::min(xA2, xB2);
	float overlapY2 = std::min(yA2, yB2);

	// 计算重叠区域的宽度和高度  
	float overlapWidth = overlapX2 - overlapX1;
	float overlapHeight = overlapY2 - overlapY1;

	// 创建一个表示重叠区域的矩形（如果有的话）  
	return VGRect{ overlapX1, overlapY1, overlapWidth, overlapHeight };
}