#pragma once

#if defined( _MSVC_LANG )
#	define RMGUI_CPLUSPLUS _MSVC_LANG
#else
#	define RMGUI_CPLUSPLUS __cplusplus
#endif
#if 201703L < RMGUI_CPLUSPLUS
#	define RMGUI_CPP_VERSION 20
#elif 201402L < RMGUI_CPLUSPLUS
#	define RMGUI_CPP_VERSION 17
#elif 201103L < RMGUI_CPLUSPLUS
#	define RMGUI_CPP_VERSION 14
#elif 199711L < RMGUI_CPLUSPLUS
#	define RMGUI_CPP_VERSION 11
#else
#	error "At least c++ standard version 11"
#endif

// ============================================

#ifdef RMGUI_SHARED_LIBRARY
#	if defined(_WIN32)
#		define RMGUI_API __declspec(dllexport)
#		define RMGUI_C_API extern "C" __declspec(dllexport)
#	else
#		define RMGUI_API __attribute__((visibility("default")))
#		define RMGUI_C_API extern "C" __attribute__((visibility("default")))
#	endif
#else
#	if defined(_WIN32)
#		define RMGUI_API 
#		define RMGUI_C_API extern "C" 
#	else
#		define RMGUI_API 
#		define RMGUI_C_API 
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
#include <math.h>
#include <assert.h>
#include <iostream>
#include <utility>
#include <algorithm>
#include <sstream>
#include <memory>
#include <string>
#include <array>
#if 20 <= RMGUI_CPP_VERSION
#include <span>
#else
#include "Utility/span.h"
#endif
#include <vector>
#include <deque>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <queue>
#include <stack>
#include <bitset>
#include <exception>
#include <functional>

// ============================================

template<class T>
using RmRaw = T*;
template<class T>
using RmRef = std::shared_ptr<T>;
using RmString = std::string;
using RmCString = const char*;
using RmWString = std::wstring;
#if 20 <= RMGUI_CPP_VERSION
using RmString8 = std::u8string;
#else
using RmString8 = std::string;
#endif
using RmString16 = std::u16string;
using RmString32 = std::u32string;
using RmStringView = std::string_view;
using RmWStringView = std::wstring_view;
#if 20 <= RMGUI_CPP_VERSION
using RmString8View = std::u8string_view;
#else
using RmString8View = std::string_view;
#endif
using RmString16View = std::u16string_view;
using RmString32View = std::u32string_view;
template <class T, size_t N>
using RmArray = std::array<T, N>;
template <class T, size_t N = std::dynamic_extent>
using RmArrayView = std::span<T, N>;
template <class T>
using RmVector = std::vector<T>;
template <class T>
using RmDeque = std::deque<T>;
template <class T>
using RmList = std::list<T>;
template <class T, class L = std::less<T>>
using RmSet = std::set<T, L>;
template <class K, class T, class L = std::less<K>>
using RmMultiSet = std::multiset<K, T, L>;
template <class K, class H = std::hash<K>, class E = std::equal_to<K>>
using RmHashSet = std::unordered_set<K, H, E>;
template <class K, class T, class L = std::less<K>>
using RmMap = std::map<K, T, L>;
template <class K, class T, class L = std::less<K>>
using RmMultiMap = std::multimap<K, T, L>;
template <class K, class T, class H = std::hash<K>, class E = std::equal_to<K>>
using RmHashMap = std::unordered_map<K, T, H, E>;
template <class T>
using RmQueue = std::queue<T>;
template <class T, class C = RmVector<T>, class L = std::less<typename C::value_type>>
using RmPriorityQueue = std::priority_queue<T, C, L>;
template <class T>
using RmStack = std::stack<T>;
template <size_t N>
using RmBitset = std::bitset<N>;
template <class T, class U>
using RmBinary = std::pair<T, U>;
template <class ...TS>
using RmTuple = std::tuple<TS...>;
using RmException = std::exception;
template <class T>
using RmLambda = std::function<T>;
using RmStringList = RmVector<RmString>;
using RmWStringList = RmVector<RmWString>;
using RmString8List = RmVector<RmString8>;
using RmString16List = RmVector<RmString16>;
using RmString32List = RmVector<RmString32>;
template<class T>
using RmStringMap = RmMap<RmString, T>;
template<class T>
using RmWStringMap = RmMap<RmWString, T>;
template<class T>
using RmString8Map = RmMap<RmString8, T>;
template<class T>
using RmString16Map = RmMap<RmString16, T>;
template<class T>
using RmString32Map = RmMap<RmString32, T>;
template<class T>
using RmStringHashMap = RmHashMap<RmString, T>;
template<class T>
using RmWStringHashMap = RmHashMap<RmWString, T>;
template<class T>
using RmString8HashMap = RmHashMap<RmString8, T>;
template<class T>
using RmString16HashMap = RmHashMap<RmString16, T>;
template<class T>
using RmString32HashMap = RmHashMap<RmString32, T>;

// ============================================

template<typename T, typename ... Args>
inline RmRef<T> RmNew(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}
template<typename U, typename T>
inline RmRef<U> RmCast(RmRef<T>&& target)
{
	if (target == nullptr) return nullptr;
	return std::dynamic_pointer_cast<U>(target);
}
template<typename U, typename T>
inline RmRef<U> RmCast(RmRef<T> const& target)
{
	if (target == nullptr) return nullptr;
	return std::dynamic_pointer_cast<U>(target);
}
template<typename U, typename T>
inline RmRaw<U> RmCast(RmRaw<T>&& target)
{
	if (target == nullptr) return nullptr;
	return dynamic_cast<U*>((T*)target);
}
template<typename U, typename T>
inline RmRaw<U> RmCast(RmRaw<T> const& target)
{
	if (target == nullptr) return nullptr;
	return const_cast<U*>(dynamic_cast<const U*>((const T*)target));
}

inline constexpr uint32_t RmHash(RmCString value)
{
	uint32_t hash = 0; // From JDK 8
	if (value == nullptr) return hash;
	while (*value) hash = hash * 31 + (*value++);
	return hash;
}

// ============================================

struct RmRect
{
	float X = 0, Y = 0, W = 0, H = 0;
};
using RmRectRaw = RmRaw<RmRect>;

inline bool operator ==(RmRect const& a, RmRect const& b)
{
	return a.X == b.X && a.Y == b.Y && a.W == b.W && a.H == b.H;
}

struct RmPoint
{
	float X = 0, Y = 0;
};
using RmPointRaw = RmRaw<RmPoint>;

inline bool operator ==(RmPoint const& a, RmPoint const& b)
{
	return a.X == b.X && a.Y == b.Y;
}

struct RmLine
{
	RmPoint P0, P1;
};
using RmLineRaw = RmRaw<RmLine>;

inline bool operator ==(RmLine const& a, RmLine const& b)
{
	return a.P0 == b.P0 && a.P1 == b.P1;
}

struct RmColor
{
	float R = 0, G = 0, B = 0, A = 0;
};
using RmColorRaw = RmRaw<RmColor>;

inline bool operator ==(RmColor const& a, RmColor const& b)
{
	return a.R == b.R && a.G == b.G && a.B == b.B && a.A == b.A;
}

struct RmPen
{
	using style_t = enum
	{
		NoPen = 0,
		SolidLine,
		DashLine,
		DotLine,
		DashDotLine,
		DashDotDotLine,
		CustomDashLine,
	};
	style_t Style = style_t::SolidLine;
	RmColor Color = RmColor{ 0, 0, 0, 1 };
	float Width = 1.0f;
};

struct RmBrush
{
	using style_t = enum
	{
		NoBrush = 0,
		SolidPattern,
		Dense1Pattern,
		Dense2Pattern,
		Dense3Pattern,
		Dense4Pattern,
		Dense5Pattern,
		Dense6Pattern,
		Dense7Pattern,
		HorPattern,
		VerPattern,
		CrossPattern,
		BDiagPattern,
		FDiagPattern,
		DiagCrossPattern,
		LinearGradientPattern,
		ConicalGradientPattern,
		RadialGradientPattern,
		TexturePattern,
	};
	style_t Style = style_t::SolidPattern;
	RmColor Color = RmColor{ 0, 0, 0, 1 };
};

struct RmFont
{
	using align_t = enum
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
	using direction_t = enum
	{
		DirectionAutoLayout = 0,
		DirectionLeftToRight = 1,
		DirectionRightToLeft = 2,
	};
	using ellipsize_t = enum
	{
		EllipsizeNone = 0,
		EllipsizeStart = 1,
		EllipsizeMiddle = 2,
		EllipsizeEnd = 3,
	};
	using style_t = enum
	{
		StyleNormal = 0,
		StyleOblique = 1,
		StyleItalic = 2,
	};
	using weight_t = enum
	{
		WeightThin = 100,
		WeightExtraLight = 200,
		WeightLight = 300,
		WeightNormal = 400,
		WeightMedium = 500,
		WeightDemiBold = 600,
		WeightBold = 700,
		WeightExtraBold = 800,
		WeightBlack = 900,
	};
	RmString Family = "Sans";
	uint32_t Align = align_t::AlignLeft;
	style_t Style = style_t::StyleNormal;
	weight_t Weight = weight_t::WeightNormal;
	ellipsize_t Ellipsize = ellipsize_t::EllipsizeEnd;
	direction_t Direction = direction_t::DirectionAutoLayout;
	uint32_t Size = 12, Spacing = 0, LineSpacing = 0;
};
using RmFontAlign = RmFont::align_t;
using RmFontAligns = uint32_t;
using RmFontDirection = RmFont::direction_t;
using RmFontEllipsize = RmFont::ellipsize_t;
using RmFontStyle = RmFont::style_t;
using RmFontWeight = RmFont::weight_t;

struct RmFloat2
{
	float X = 0, Y = 0;
};

inline bool operator ==(RmFloat2 const& a, RmFloat2 const& b)
{
	return a.X == b.X && a.Y == b.Y;
}

struct RmFloat3
{
	float X = 0, Y = 0, Z = 0;
};

inline bool operator ==(RmFloat3 const& a, RmFloat3 const& b)
{
	return a.X == b.X && a.Y == b.Y && a.Z == b.Z;
}

struct RmFloat4
{
	float X = 0, Y = 0, Z = 0, W = 0;
};

inline bool operator ==(RmFloat4 const& a, RmFloat4 const& b)
{
	return a.X == b.X && a.Y == b.Y && a.Z == b.Z && a.W == b.W;
}

struct RmFloat2x2
{
	RmFloat2 X, Y;
};

inline bool operator ==(RmFloat2x2 const& a, RmFloat2x2 const& b)
{
	return a.X == b.X && a.Y == b.Y;
}

struct RmFloat3x3
{
	RmFloat3 X, Y, Z;
};

inline bool operator ==(RmFloat3x3 const& a, RmFloat3x3 const& b)
{
	return a.X == b.X && a.Y == b.Y && a.Z == b.Z;
}

struct RmFloat4x4
{
	RmFloat4 X, Y, Z, W;
};

inline bool operator ==(RmFloat4x4 const& a, RmFloat4x4 const& b)
{
	return a.X == b.X && a.Y == b.Y && a.Z == b.Z && a.W == b.W;
}

class RmImage;
using RmImageRaw = RmRaw<RmImage>;

inline RmRect RmOverlap(RmRect const& viewport, RmRect const& client)
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
	if (xA2 <= xB1 || xB2 <= xA1 || yA2 <= yB1 || yB2 <= yA1) return viewport;

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
	return RmRect{ overlapX1, overlapY1, overlapWidth, overlapHeight };
}