#pragma once

#if defined( _MSVC_LANG )
#	define OPENUI_CPLUSPLUS _MSVC_LANG
#else
#	define OPENUI_CPLUSPLUS __cplusplus
#endif
#if 201703L < OPENUI_CPLUSPLUS
#	define OPENUI_CPP_VERSION 20
#elif 201402L < OPENUI_CPLUSPLUS
#	define OPENUI_CPP_VERSION 17
#elif 201103L < OPENUI_CPLUSPLUS
#	define OPENUI_CPP_VERSION 14
#elif 199711L < OPENUI_CPLUSPLUS
#	define OPENUI_CPP_VERSION 11
#else
#	error "At least c++ standard version 11"
#endif

// ============================================

#ifdef OPENUI_SHARED_LIBRARY
#	if defined(_WIN32)
#		define OPENUI_API __declspec(dllexport)
#		define OPENUI_C_API extern "C" __declspec(dllexport)
#	else
#		define OPENUI_API __attribute__((visibility("default")))
#		define OPENUI_C_API extern "C" __attribute__((visibility("default")))
#	endif
#else
#	if defined(_WIN32)
#		define OPENUI_API 
#		define OPENUI_C_API extern "C" 
#	else
#		define OPENUI_API 
#		define OPENUI_C_API 
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
#if 20 <= OPENUI_CPP_VERSION
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
#define UINAN (NAN)

// ============================================

template<class T>
using UIRaw = T*;
template<class T>
using UIRef = std::shared_ptr<T>;
using UIString = std::string;
using UICString = const char*;
using UIWString = std::wstring;
#if 20 <= OPENUI_CPP_VERSION
using UIString8 = std::u8string;
#else
using UIString8 = std::string;
#endif
using UIString16 = std::u16string;
using UIString32 = std::u32string;
using UIStringView = std::string_view;
using UIWStringView = std::wstring_view;
#if 20 <= OPENUI_CPP_VERSION
using UIString8View = std::u8string_view;
#else
using UIString8View = std::string_view;
#endif
using UIString16View = std::u16string_view;
using UIString32View = std::u32string_view;
template <class T, size_t N>
using UIArray = std::array<T, N>;
template <class T, size_t N = std::dynamic_extent>
using UIArrayView = std::span<T, N>;
template <class T>
using UIVector = std::vector<T>;
template <class T>
using UIDeque = std::deque<T>;
template <class T>
using UIList = std::list<T>;
template <class T, class L = std::less<T>>
using UISet = std::set<T, L>;
template <class K, class T, class L = std::less<K>>
using UIMultiSet = std::multiset<K, T, L>;
template <class K, class H = std::hash<K>, class E = std::equal_to<K>>
using UIHashSet = std::unordered_set<K, H, E>;
template <class K, class T, class L = std::less<K>>
using UIMap = std::map<K, T, L>;
template <class K, class T, class L = std::less<K>>
using UIMultiMap = std::multimap<K, T, L>;
template <class K, class T, class H = std::hash<K>, class E = std::equal_to<K>>
using UIHashMap = std::unordered_map<K, T, H, E>;
template <class T>
using UIQueue = std::queue<T>;
template <class T, class C = UIVector<T>, class L = std::less<typename C::value_type>>
using UIPriorityQueue = std::priority_queue<T, C, L>;
template <class T>
using UIStack = std::stack<T>;
template <size_t N>
using UIBitset = std::bitset<N>;
template <class T, class U>
using UIBinary = std::pair<T, U>;
template <class ...TS>
using UITuple = std::tuple<TS...>;
using UIException = std::exception;
template <class T>
using UILambda = std::function<T>;
using UIStringList = UIVector<UIString>;
using UIWStringList = UIVector<UIWString>;
using UIString8List = UIVector<UIString8>;
using UIString16List = UIVector<UIString16>;
using UIString32List = UIVector<UIString32>;
template<class T>
using UIStringMap = UIMap<UIString, T>;
template<class T>
using UIWStringMap = UIMap<UIWString, T>;
template<class T>
using UIString8Map = UIMap<UIString8, T>;
template<class T>
using UIString16Map = UIMap<UIString16, T>;
template<class T>
using UIString32Map = UIMap<UIString32, T>;
template<class T>
using UIStringHashMap = UIHashMap<UIString, T>;
template<class T>
using UIWStringHashMap = UIHashMap<UIWString, T>;
template<class T>
using UIString8HashMap = UIHashMap<UIString8, T>;
template<class T>
using UIString16HashMap = UIHashMap<UIString16, T>;
template<class T>
using UIString32HashMap = UIHashMap<UIString32, T>;

// ============================================

template<typename T, typename ... Args>
inline UIRef<T> UINew(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}
template<typename U, typename T>
inline UIRef<U> UICast(UIRef<T>&& target)
{
	if (target == nullptr) return nullptr;
	return std::dynamic_pointer_cast<U>(target);
}
template<typename U, typename T>
inline UIRef<U> UICast(UIRef<T> const& target)
{
	if (target == nullptr) return nullptr;
	return std::dynamic_pointer_cast<U>(target);
}
template<typename U, typename T>
inline UIRaw<U> UICast(UIRaw<T>&& target)
{
	if (target == nullptr) return nullptr;
	return dynamic_cast<U*>((T*)target);
}
template<typename U, typename T>
inline UIRaw<U> UICast(UIRaw<T> const& target)
{
	if (target == nullptr) return nullptr;
	return const_cast<U*>(dynamic_cast<const U*>((const T*)target));
}

inline constexpr uint32_t UIHash(UICString value)
{
	uint32_t hash = 0; // From JDK 8
	if (value == nullptr) return hash;
	while (*value) hash = hash * 31 + (*value++);
	return hash;
}

// ============================================

struct UIRect
{
	float X = 0, Y = 0, W = 0, H = 0;
};
using UIRectRaw = UIRaw<UIRect>;

inline bool operator ==(UIRect const& a, UIRect const& b)
{
	return a.X == b.X && a.Y == b.Y && a.W == b.W && a.H == b.H;
}

struct UIPoint
{
	float X = 0, Y = 0;
};
using UIPointRaw = UIRaw<UIPoint>;

inline bool operator ==(UIPoint const& a, UIPoint const& b)
{
	return a.X == b.X && a.Y == b.Y;
}

struct UILine
{
	UIPoint P0, P1;
};
using UILineRaw = UIRaw<UILine>;

inline bool operator ==(UILine const& a, UILine const& b)
{
	return a.P0 == b.P0 && a.P1 == b.P1;
}

struct UIColor
{
	float R = 0, G = 0, B = 0, A = 0;
};
using UIColorRaw = UIRaw<UIColor>;

inline bool operator ==(UIColor const& a, UIColor const& b)
{
	return a.R == b.R && a.G == b.G && a.B == b.B && a.A == b.A;
}

struct UIPointUV
{
	float X = 0, Y = 0, U = 0, V = 0;
};

struct UIPointUV3
{
	UIPointUV P0, P1, P2;
};

class UIPainter;
using UIPainterRaw = UIRaw<UIPainter>;
struct UIPrimitive
{
	UIPainterRaw Painter;
	UIArrayView<const UIPointUV3> Primitive;
};

struct UIPen
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
	UIColor Color = UIColor{ 0, 0, 0, 1 };
	float Width = 1.0f;
};

struct UIBrush
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
	UIColor Color = UIColor{ 1, 1, 1, 1 };
};

struct UIFont
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
	bool NoWrap = false;
	UIString Family = "Segoe UI";
	uint32_t Align = align_t::AlignLeft;
	style_t Style = style_t::StyleNormal;
	weight_t Weight = weight_t::WeightNormal;
	ellipsize_t Ellipsize = ellipsize_t::EllipsizeEnd;
	direction_t Direction = direction_t::DirectionAutoLayout;
	uint32_t Size = 12, Spacing = 0, LineSpacing = 0;
};
using UIFontAlign = UIFont::align_t;
using UIFontAligns = uint32_t;
using UIFontDirection = UIFont::direction_t;
using UIFontEllipsize = UIFont::ellipsize_t;
using UIFontStyle = UIFont::style_t;
using UIFontWeight = UIFont::weight_t;

struct UIFloat2
{
	float X = 0, Y = 0;
};

inline bool operator ==(UIFloat2 const& a, UIFloat2 const& b)
{
	return a.X == b.X && a.Y == b.Y;
}

struct UIFloat3
{
	float X = 0, Y = 0, Z = 0;
};

inline bool operator ==(UIFloat3 const& a, UIFloat3 const& b)
{
	return a.X == b.X && a.Y == b.Y && a.Z == b.Z;
}

struct UIFloat4
{
	float X = 0, Y = 0, Z = 0, W = 0;
};

inline bool operator ==(UIFloat4 const& a, UIFloat4 const& b)
{
	return a.X == b.X && a.Y == b.Y && a.Z == b.Z && a.W == b.W;
}

struct UIFloat2x2
{
	UIFloat2 X, Y;
};

inline bool operator ==(UIFloat2x2 const& a, UIFloat2x2 const& b)
{
	return a.X == b.X && a.Y == b.Y;
}

struct UIFloat3x3
{
	UIFloat3 X, Y, Z;
};

inline bool operator ==(UIFloat3x3 const& a, UIFloat3x3 const& b)
{
	return a.X == b.X && a.Y == b.Y && a.Z == b.Z;
}

struct UIFloat4x4
{
	UIFloat4 X, Y, Z, W;
};

inline bool operator ==(UIFloat4x4 const& a, UIFloat4x4 const& b)
{
	return a.X == b.X && a.Y == b.Y && a.Z == b.Z && a.W == b.W;
}

struct UIImage
{
	uint32_t Width = 0, Height = 0, Stride = 0;
	UIArrayView<const uint8_t> Data;
};
using UIImageRaw = UIRaw<UIImage>;

template <class T, class E = uint8_t>
struct UIValue
{
	T Value = T();
	E Unit = E();
	UIValue(T const& value = T(), E uint = E(1)) : Value(value), Unit(uint) {}
	UIValue(UIValue&&) = default;
	UIValue(UIValue const&) = default;
	operator T() { return Value; }
	UIValue& operator =(UIValue&& value) { Value = value.Value; Unit = value.Unit; return *this; }
	UIValue& operator =(UIValue const& value) { Value = value.Value; Unit = value.Unit; return *this; }
};
using UIValueF = UIValue<float>;
using UIValue2F = UIArray<UIValueF, 2>;
using UIValue3F = UIArray<UIValueF, 3>;
using UIValue4F = UIArray<UIValueF, 4>;

inline UIRect UIOverlap(UIRect const& viewport, UIRect const& client)
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
	if (xA2 <= xB1 || xB2 <= xA1 || yA2 <= yB1 || yB2 <= yA1) return UIRect{};

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
	return UIRect{ overlapX1, overlapY1, overlapWidth, overlapHeight };
}

inline bool UIBounds(UIRect const& client, float x, float y)
{
	return client.X <= x && x <= client.X + client.W
		&& client.Y <= y && y <= client.Y + client.H;
}

inline int32_t UIUTF8Num(char prefix)
{
	if (0 <= uint8_t(prefix) && uint8_t(prefix) < 0x80) return 1;
	if (0xC2 <= uint8_t(prefix) && uint8_t(prefix) < 0xE0) return 2;
	if (0xE0 <= uint8_t(prefix) && uint8_t(prefix) < 0xF0) return 3;
	if (0xF0 <= uint8_t(prefix) && uint8_t(prefix) < 0xF8) return 4;
	return 0;
}