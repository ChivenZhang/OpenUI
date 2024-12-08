#pragma once
/*=================================================
* Copyright © 2020-2024 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/

#if defined( _MSVC_LANG )
#	define OPENUI_CPLUSPLUS _MSVC_LANG
#else
#	define __FUNCTION__ __func__
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
#	define OPENUI_CPP_VERSION 0
#endif
#if OPENUI_CPP_VERSION < 17
#	error "At least c++ standard version 17"
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
#		define OPENUI_C_API extern "C"
#	endif
#endif

#ifdef _WIN32
#	ifdef _DEBUG
#		define OPENUI_DEBUG_MODE
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
#define UIAssert(...) assert(__VA_ARGS__)

// ============================================

#include <cstdio>
#include <ctime>
#include <thread>
#define UI_FORMAT(TARGET, FORMAT, LEVEL, ...) \
do { \
char __DATETIME__[32]; auto __NOWTIME__ = std::time(nullptr); \
std::strftime(__DATETIME__, sizeof(__DATETIME__), "%Y-%m-%d %H:%M:%S", std::localtime(&__NOWTIME__)); \
auto __THREAD__ = []()->uint32_t { std::stringstream ss; ss << std::this_thread::get_id(); return std::stoul(ss.str()); }(); \
std::fprintf(TARGET, "%s:%d\n" "%s " #LEVEL " %d --- " FORMAT "\n\n", __FILE__, __LINE__, __DATETIME__, __THREAD__, ##__VA_ARGS__); \
} while (0)

#ifndef UI_DEBUG
#ifdef OPENUI_DEBUG_MODE
#	define UI_DEBUG(FORMAT, ...) UI_FORMAT(stdout, FORMAT, DEBUG, ##__VA_ARGS__)
#else
#	define UI_DEBUG(FORMAT, ...)
#endif
#endif

#ifndef UI_WARN
#	define UI_WARN(FORMAT, ...) UI_FORMAT(stdout, FORMAT, WARN, ##__VA_ARGS__)
#endif

#ifndef UI_INFO
#	define UI_INFO(FORMAT, ...) UI_FORMAT(stdout, FORMAT, INFO, ##__VA_ARGS__)
#endif

#ifndef UI_ERROR
#	define UI_ERROR(FORMAT, ...) UI_FORMAT(stderr, FORMAT, ERROR, ##__VA_ARGS__)
#endif

#ifndef UI_FATAL
#	define UI_FATAL(FORMAT, ...) do{ UI_FORMAT(stderr, FORMAT, FATAL, ##__VA_ARGS__); std::abort(); } while(0)
#endif

#ifndef UI_PRINT
#	define UI_PRINT(FORMAT, ...) UI_INFO(FORMAT, ##__VA_ARGS__)
#endif

// ============================================

template<class T>
using UIRaw = T*;
template<class T>
using UIRef = std::shared_ptr<T>;
template<class T>
using UIHnd = std::weak_ptr<T>;
using UIString = std::string;
using UICString = const char*;
using UIWString = std::wstring;
#if 20 <= OPENUI_CPP_VERSION
using UIString8 = std::u8string;
#endif
using UIString16 = std::u16string;
using UIString32 = std::u32string;
#if 17 <= OPENUI_CPP_VERSION
using UIStringView = std::string_view;
using UIWStringView = std::wstring_view;
using UIString16View = std::u16string_view;
using UIString32View = std::u32string_view;
#endif
#if 20 <= OPENUI_CPP_VERSION
using UIString8View = std::u8string_view;
#endif
template <class T, size_t N>
using UIArray = std::array<T, N>;
#if 20 <= OPENUI_CPP_VERSION
template <class T, size_t N = std::dynamic_extent>
using UIArrayView = std::span<T, N>;
#endif
template <class T>
using UIList = std::vector<T>;
template <class T>
using UIDeque = std::deque<T>;
template <class T>
using UILinkedList = std::list<T>;
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
template <class T, class C = UIList<T>, class L = std::less<typename C::value_type>>
using UISortedQueue = std::priority_queue<T, C, L>;
template <class T>
using UIStack = std::stack<T>;
template <size_t N>
using UIBitset = std::bitset<N>;
template <class T, class U>
using UIBinary = std::pair<T, U>;
template <class ...TS>
using UITuple = std::tuple<TS...>;
using UIAny = std::any;
using UIError = std::exception;
template <class T>
using UILambda = std::function<T>;
using UIThread = std::thread;
template <class T>
using UIFuture = std::future<T>;
template <class T>
using UIPromise = std::promise<T>;
template <class T>
using UIAtomic = std::atomic<T>;
using UIMutex = std::recursive_mutex;
using UIMutexLock = std::lock_guard<UIMutex>;
using UIUniqueLock = std::unique_lock<UIMutex>;
using UIMutexUnlock = std::condition_variable_any;
using UIStringList = UIList<UIString>;
using UIWStringList = UIList<UIWString>;
#if 20 <= OPENUI_CPP_VERSION
using UIString8List = UIList<UIString8>;
#endif
using UIString16List = UIList<UIString16>;
using UIString32List = UIList<UIString32>;
template<class T>
using UIStringMap = UIMap<UIString, T>;
template<class T>
using UIWStringMap = UIMap<UIWString, T>;
#if 20 <= OPENUI_CPP_VERSION
template<class T>
using UIString8Map = UIMap<UIString8, T>;
#endif
template<class T>
using UIString16Map = UIMap<UIString16, T>;
template<class T>
using UIString32Map = UIMap<UIString32, T>;
template<class T>
using UIStringHashMap = UIHashMap<UIString, T>;
template<class T>
using UIWStringHashMap = UIHashMap<UIWString, T>;
#if 20 <= OPENUI_CPP_VERSION
template<class T>
using UIString8HashMap = UIHashMap<UIString8, T>;
#endif
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
inline UIHnd<U> UICast(UIHnd<T>&& target)
{
	if (target == nullptr) return THnd<U>();
	return std::dynamic_pointer_cast<U>(target.lock());
}
template<typename U, typename T>
inline UIHnd<U> UICast(UIHnd<T> const& target)
{
	if (target == nullptr) return THnd<U>();
	return std::dynamic_pointer_cast<U>(target.lock());
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

inline constexpr uint32_t UIHash32(const char* const first, const size_t count) noexcept
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
inline constexpr uint64_t UIHash64(const char* const first, const size_t count) noexcept
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
inline constexpr uint32_t UIHash(const char* const value) noexcept
{
	size_t count = 0; for (size_t i = 0; value[i]; ++i) ++count;
	return UIHash32(value, count);
}
inline const uint32_t UIHash(UIString const& value) noexcept
{
	return UIHash32(value.c_str(), value.size());
}
#if 17 <= OPENUI_CPP_VERSION
inline const uint32_t UIHash(UIStringView value) noexcept
{
	return UIHash32(value.data(), value.size());
}
#endif

// ============================================

#define UINAN (NAN)

#define UI_INPUT_FOREACH(XX) \
XX(NONE, NONE, NONE), \
XX(KEY_BACKSPACE, KEY_BACKSPACE, KEY_BACKSPACE), \
XX(KEY_TAB, KEY_TAB, KEY_TAB), \
XX(KEY_ENTER, KEY_ENTER, KEY_ENTER), \
XX(KEY_ESCAPE, KEY_ESCAPE, KEY_ESCAPE), \
XX(KEY_SPACE, KKEY_SPACE, EY_SPACE), \
XX(KEY_QUOTE, KEY_QUOTE, KEY_QUOTE), \
XX(KEY_COMMA, KEY_COMMA, KEY_COMMA), \
XX(KEY_MINUS, KEY_MINUS, KEY_MINUS), \
XX(KEY_PERIOD, KEY_PERIOD, KEY_PERIOD), \
XX(KEY_SLASH, KKEY_SLASH, EY_SLASH), \
XX(KEY_0, KEY_0, KEY_0), \
XX(KEY_1, KEY_1, KEY_1), \
XX(KEY_2, KEY_2, KEY_2), \
XX(KEY_3, KEY_3, KEY_3), \
XX(KEY_4, KEY_4, KEY_4), \
XX(KEY_5, KEY_5, KEY_5), \
XX(KEY_6, KEY_6, KEY_6), \
XX(KEY_7, KEY_7, KEY_7), \
XX(KEY_8, KEY_8, KEY_8), \
XX(KEY_9, KEY_9, KEY_9), \
XX(KEY_SEMICOLON, KEY_SEMICOLON, KEY_SEMICOLON), \
XX(KEY_EQUAL, KEY_EQUAL, KEY_EQUAL), \
XX(KEY_A, KEY_A, KEY_A), \
XX(KEY_B, KEY_B, KEY_B), \
XX(KEY_C, KEY_C, KEY_C), \
XX(KEY_D, KEY_D, KEY_D), \
XX(KEY_E, KEY_E, KEY_E), \
XX(KEY_F, KEY_F, KEY_F), \
XX(KEY_G, KEY_G, KEY_G), \
XX(KEY_H, KEY_H, KEY_H), \
XX(KEY_I, KEY_I, KEY_I), \
XX(KEY_J, KEY_J, KEY_J), \
XX(KEY_K, KEY_K, KEY_K), \
XX(KEY_L, KEY_L, KEY_L), \
XX(KEY_M, KEY_M, KEY_M), \
XX(KEY_N, KEY_N, KEY_N), \
XX(KEY_O, KEY_O, KEY_O), \
XX(KEY_P, KEY_P, KEY_P), \
XX(KEY_Q, KEY_Q, KEY_Q), \
XX(KEY_R, KEY_R, KEY_R), \
XX(KEY_S, KEY_S, KEY_S), \
XX(KEY_T, KEY_T, KEY_T), \
XX(KEY_U, KEY_U, KEY_U), \
XX(KEY_V, KEY_V, KEY_V), \
XX(KEY_W, KEY_W, KEY_W), \
XX(KEY_X, KEY_X, KEY_X), \
XX(KEY_Y, KEY_Y, KEY_Y), \
XX(KEY_Z, KEY_Z, KEY_Z), \
XX(KEY_LEFT_BRACKET, KEY_LEFT_BRACKET, KEY_LEFT_BRACKET), \
XX(KEY_BACK_SLASH, KEY_BACK_SLASH, KEY_BACK_SLASH), \
XX(KEY_RIGHT_BRACKET, KEY_RIGHT_BRACKET, KEY_RIGHT_BRACKET), \
XX(KEY_BACK_QUOTE, KEY_BACK_QUOTE, KEY_BACK_QUOTE), \
XX(KEY_WORLD_1, KEY_WORLD_1, KEY_WORLD_1), \
XX(KEY_WORLD_2, KEY_WORLD_2, KEY_WORLD_2), \
XX(KEY_INSERT, KEY_INSERT, KEY_INSERT), \
XX(KEY_DELETE, KEY_DELETE, KEY_DELETE), \
XX(KEY_RIGHT, KEY_RIGHT, KEY_RIGHT), \
XX(KEY_LEFT, KEY_LEFT, KEY_LEFT), \
XX(KEY_DOWN, KEY_DOWN, KEY_DOWN), \
XX(KEY_UP, KEY_UP, KEY_UP), \
XX(KEY_PAGE_UP, KEY_PAGE_UP, KEY_PAGE_UP), \
XX(KEY_PAGE_DOWN, KEY_PAGE_DOWN, KEY_PAGE_DOWN), \
XX(KEY_HOME, KEY_HOME, KEY_HOME), \
XX(KEY_END, KEY_END, KEY_END), \
XX(KEY_CAPS_LOCK, KEY_CAPS_LOCK, KEY_CAPS_LOCK), \
XX(KEY_SCROLL_LOCK, KEY_SCROLL_LOCK, KEY_SCROLL_LOCK), \
XX(KEY_NUM_LOCK, KEY_NUM_LOCK, KEY_NUM_LOCK), \
XX(KEY_PRINT_SCREEN, KEY_PRINT_SCREEN, KEY_PRINT_SCREEN), \
XX(KEY_PAUSE, KEY_PAUSE, KEY_PAUSE), \
XX(KEY_F1, KEY_F1, KEY_F1), \
XX(KEY_F2, KEY_F2, KEY_F2), \
XX(KEY_F3, KEY_F3, KEY_F3), \
XX(KEY_F4, KEY_F4, KEY_F4), \
XX(KEY_F5, KEY_F5, KEY_F5), \
XX(KEY_F6, KEY_F6, KEY_F6), \
XX(KEY_F7, KEY_F7, KEY_F7), \
XX(KEY_F8, KEY_F8, KEY_F8), \
XX(KEY_F9, KEY_F9, KEY_F9), \
XX(KEY_F10, KEY_F10, KEY_F10), \
XX(KEY_F11, KEY_F11, KEY_F11), \
XX(KEY_F12, KEY_F12, KEY_F12), \
XX(KEY_F13, KEY_F13, KEY_F13), \
XX(KEY_F14, KEY_F14, KEY_F14), \
XX(KEY_F15, KEY_F15, KEY_F15), \
XX(KEY_F16, KEY_F16, KEY_F16), \
XX(KEY_F17, KEY_F17, KEY_F17), \
XX(KEY_F18, KEY_F18, KEY_F18), \
XX(KEY_F19, KEY_F19, KEY_F19), \
XX(KEY_F20, KEY_F20, KEY_F20), \
XX(KEY_F21, KEY_F21, KEY_F21), \
XX(KEY_F22, KEY_F22, KEY_F22), \
XX(KEY_F23, KEY_F23, KEY_F23), \
XX(KEY_F24, KEY_F24, KEY_F24), \
XX(KEY_F25, KEY_F25, KEY_F25), \
XX(KEY_F26, KEY_F26, KEY_F26), \
XX(KEY_F27, KEY_F27, KEY_F27), \
XX(KEY_F28, KEY_F28, KEY_F28), \
XX(KEY_F29, KEY_F29, KEY_F29), \
XX(KEY_F30, KEY_F30, KEY_F30), \
XX(KEY_F31, KEY_F31, KEY_F31), \
XX(KEY_F32, KEY_F32, KEY_F32), \
XX(KEY_F33, KEY_F33, KEY_F33), \
XX(KEY_F34, KEY_F34, KEY_F34), \
XX(KEY_F35, KEY_F35, KEY_F35), \
XX(KEY_KP_0, KEY_KP_0, KEY_KP_0), \
XX(KEY_KP_1, KEY_KP_1, KEY_KP_1), \
XX(KEY_KP_2, KEY_KP_2, KEY_KP_2), \
XX(KEY_KP_3, KEY_KP_3, KEY_KP_3), \
XX(KEY_KP_4, KEY_KP_4, KEY_KP_4), \
XX(KEY_KP_5, KEY_KP_5, KEY_KP_5), \
XX(KEY_KP_6, KEY_KP_6, KEY_KP_6), \
XX(KEY_KP_7, KEY_KP_7, KEY_KP_7), \
XX(KEY_KP_8, KEY_KP_8, KEY_KP_8), \
XX(KEY_KP_9, KEY_KP_9, KEY_KP_9), \
XX(KEY_KP_DECIMAL, KEY_KP_DECIMAL, KEY_KP_DECIMAL), \
XX(KEY_KP_DIVIDE, KEY_KP_DIVIDE, KEY_KP_DIVIDE), \
XX(KEY_KP_MULTIPLY, KEY_KP_MULTIPLY, KEY_KP_MULTIPLY), \
XX(KEY_KP_MINUS, KEY_KP_MINUS, KEY_KP_MINUS), \
XX(KEY_KP_PLUS, KEY_KP_PLUS, KEY_KP_PLUS), \
XX(KEY_KP_ENTER, KEY_KP_ENTER, KEY_KP_ENTER), \
XX(KEY_KP_EQUAL, KEY_KP_EQUAL, KEY_KP_EQUAL), \
XX(KEY_SHIFT, KEY_SHIFT, KEY_SHIFT), \
XX(KEY_CTRL, KEY_CTRL, KEY_CTRL), \
XX(KEY_ALT, KEY_ALT, KEY_ALT), \
XX(KEY_SUPER, KEY_SUPER, KEY_SUPER), \
XX(KEY_MENU, KEY_MENU, KEY_MENU), \
XX(KEY_LAST, KEY_LAST, KEY_LAST), \
XX(KEY_MOD_ALT, KEY_MOD_ALT, KEY_MOD_ALT), \
XX(KEY_MOD_CTRL, KEY_MOD_CTRL, KEY_MOD_CTRL), \
XX(KEY_MOD_SHIFT, KEY_MOD_SHIFT, KEY_MOD_SHIFT), \
XX(KEY_MOD_SUPER, KEY_MOD_SUPER, KEY_MOD_SUPER), \
XX(MOUSE_BUTTON_LEFT, MOUSE_BUTTON_LEFT, MOUSE_BUTTON_LEFT), \
XX(MOUSE_BUTTON_RIGHT, MOUSE_BUTTON_RIGHT, MOUSE_BUTTON_RIGHT), \
XX(MOUSE_BUTTON_MIDDLE, MOUSE_BUTTON_MIDDLE, MOUSE_BUTTON_MIDDLE), \
XX(MOUSE_BUTTON_1, MOUSE_BUTTON_1, MOUSE_BUTTON_1), \
XX(MOUSE_BUTTON_2, MOUSE_BUTTON_2, MOUSE_BUTTON_2), \
XX(MOUSE_BUTTON_3, MOUSE_BUTTON_3, MOUSE_BUTTON_3), \
XX(MOUSE_BUTTON_4, MOUSE_BUTTON_4, MOUSE_BUTTON_4), \
XX(MOUSE_BUTTON_5, MOUSE_BUTTON_5, MOUSE_BUTTON_5), \
XX(MOUSE_BUTTON_6, MOUSE_BUTTON_6, MOUSE_BUTTON_6), \
XX(MOUSE_BUTTON_7, MOUSE_BUTTON_7, MOUSE_BUTTON_7), \
XX(MOUSE_BUTTON_8, MOUSE_BUTTON_8, MOUSE_BUTTON_8), \
XX(MOUSE_BUTTON_9, MOUSE_BUTTON_9, MOUSE_BUTTON_9), \
XX(MOUSE_BUTTON_10, MOUSE_BUTTON_10, MOUSE_BUTTON_10), \
XX(MOUSE_BUTTON_11, MOUSE_BUTTON_11, MOUSE_BUTTON_11), \
XX(MOUSE_BUTTON_12, MOUSE_BUTTON_12, MOUSE_BUTTON_12), \
XX(MOUSE_BUTTON_13, MOUSE_BUTTON_13, MOUSE_BUTTON_13), \
XX(MOUSE_BUTTON_14, MOUSE_BUTTON_14, MOUSE_BUTTON_14), \
XX(MOUSE_BUTTON_15, MOUSE_BUTTON_15, MOUSE_BUTTON_15), \
XX(MOUSE_BUTTON_16, MOUSE_BUTTON_16, MOUSE_BUTTON_16), \
XX(MOUSE_BUTTON_17, MOUSE_BUTTON_17, MOUSE_BUTTON_17), \
XX(MOUSE_BUTTON_18, MOUSE_BUTTON_18, MOUSE_BUTTON_18), \
XX(MOUSE_BUTTON_19, MOUSE_BUTTON_19, MOUSE_BUTTON_19), \
XX(MOUSE_BUTTON_20, MOUSE_BUTTON_20, MOUSE_BUTTON_20), \
XX(MOUSE_BUTTON_21, MOUSE_BUTTON_21, MOUSE_BUTTON_21), \
XX(MOUSE_BUTTON_22, MOUSE_BUTTON_22, MOUSE_BUTTON_22), \
XX(MOUSE_BUTTON_23, MOUSE_BUTTON_23, MOUSE_BUTTON_23), \
XX(MOUSE_BUTTON_24, MOUSE_BUTTON_24, MOUSE_BUTTON_24), \
XX(MOUSE_WHEEL_X, MOUSE_WHEEL_X, MOUSE_WHEEL_X), \
XX(MOUSE_WHEEL_Y, MOUSE_WHEEL_Y, MOUSE_WHEEL_Y), \
XX(GAMEPAD_START, GAMEPAD_START, GAMEPAD_START), \
XX(GAMEPAD_BACK, GAMEPAD_BACK, GAMEPAD_BACK), \
XX(GAMEPAD_FACELEFT, GAMEPAD_FACELEFT, GAMEPAD_FACELEFT), \
XX(GAMEPAD_FACERIGHT, GAMEPAD_FACERIGHT, GAMEPAD_FACERIGHT), \
XX(GAMEPAD_FACEUP, GAMEPAD_FACEUP, GAMEPAD_FACEUP), \
XX(GAMEPAD_FACEDOWN, GAMEPAD_FACEDOWN, GAMEPAD_FACEDOWN), \
XX(GAMEPAD_DPADLEFT, GAMEPAD_DPADLEFT, GAMEPAD_DPADLEFT), \
XX(GAMEPAD_DPADRIGHT, GAMEPAD_DPADRIGHT, GAMEPAD_DPADRIGHT), \
XX(GAMEPAD_DPADUP, GAMEPAD_DPADUP, GAMEPAD_DPADUP), \
XX(GAMEPAD_DPADDOWN, GAMEPAD_DPADDOWN, GAMEPAD_DPADDOWN), \
XX(GAMEPAD_L1, GAMEPAD_L1, GAMEPAD_L1), \
XX(GAMEPAD_R1, GAMEPAD_R1, GAMEPAD_R1), \
XX(GAMEPAD_L2, GAMEPAD_L2, GAMEPAD_L2), \
XX(GAMEPAD_R2, GAMEPAD_R2, GAMEPAD_R2), \
XX(GAMEPAD_L3, GAMEPAD_L3, GAMEPAD_L3), \
XX(GAMEPAD_R3, GAMEPAD_R3, GAMEPAD_R3), \
XX(GAMEPAD_LSTICKLEFT, GAMEPAD_LSTICKLEFT, GAMEPAD_LSTICKLEFT), \
XX(GAMEPAD_LSTICKRIGHT, GAMEPAD_LSTICKRIGHT, GAMEPAD_LSTICKRIGHT), \
XX(GAMEPAD_LSTICKUP, GAMEPAD_LSTICKUP, GAMEPAD_LSTICKUP), \
XX(GAMEPAD_LSTICKDOWN, GAMEPAD_LSTICKDOWN, GAMEPAD_LSTICKDOWN), \
XX(GAMEPAD_RSTICKLEFT, GAMEPAD_RSTICKLEFT, GAMEPAD_RSTICKLEFT), \
XX(GAMEPAD_RSTICKRIGHT, GAMEPAD_RSTICKRIGHT, GAMEPAD_RSTICKRIGHT), \
XX(GAMEPAD_RSTICKUP, GAMEPAD_RSTICKUP, GAMEPAD_RSTICKUP), \
XX(GAMEPAD_RSTICKDOWN, GAMEPAD_RSTICKDOWN, GAMEPAD_RSTICKDOWN), \
XX(INPUT_COUNT, INPUT_COUNT, INPUT_COUNT), \

/// @brief 输入枚举
enum UIInputEnum
{
#define XX(type, meta, text) type
	UI_INPUT_FOREACH(XX)
#undef XX
};
using UIInputEnums = uint32_t;

/// @brief 输入名称
const char* const UIInputName[]
{
#define XX(type, meta, text) #text
	UI_INPUT_FOREACH(XX)
#undef XX
};

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
	float Width = 2.0f;
};
static constexpr UIPen UINoPen
{
	.Style = UIPen::NoPen,
};
static constexpr UIPen UIBlackPen
{
	.Color { 0,0,0,1 },
};
static constexpr UIPen UIWhitePen
{
	.Color { 1,1,1,1 },
};
static constexpr UIPen UIRedPen
{
	.Color { 1,0,0,1 },
};
static constexpr UIPen UIGreenPen
{
	.Color { 0,1,0,1 },
};
static constexpr UIPen UIBluePen
{
	.Color { 0,0,1,1 },
};
static constexpr UIPen UIFramePen
{
	.Color { 108 / 255.0f, 110 / 255.0f, 111 / 255.0f, 1.0f },
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
static constexpr UIBrush UINoBrush
{
	.Style = UIBrush::NoBrush,
};
static constexpr UIBrush UIBlackBrush
{
	.Color { 0,0,0,1 },
};
static constexpr UIBrush UIWhiteBrush
{
	.Color { 1,1,1,1 },
};
static constexpr UIBrush UIRedBrush
{
	.Color { 1,0,0,1 },
};
static constexpr UIBrush UIGreenBrush
{
	.Color { 0,1,0,1 },
};
static constexpr UIBrush UIBlueBrush
{
	.Color { 0,0,1,1 },
};
static constexpr UIBrush UIFrameBrush
{
	.Color { 238 / 255.0f, 238 / 255.0f, 242 / 255.0f, 1.0f },
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
	bool LineWrap = false;
	UIString Family = "Segoe UI";
	uint32_t Align = align_t::AlignLeft;
	style_t Style = style_t::StyleNormal;
	weight_t Weight = weight_t::WeightNormal;
	ellipsize_t Ellipsize = ellipsize_t::EllipsizeEnd;
	direction_t Direction = direction_t::DirectionAutoLayout;
	float LineSpacing = 0;
	uint32_t Size = 14, Spacing = 0;
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
	uint32_t Width = 0, Height = 0, Stride = 0, Channel = 0;
	union { void* Pixel; size_t Data = 0; };
	enum { Byte = 0, HWByte, Float, HWFloat, } Type = Byte;
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
	if ((prefix & 0x80) == 0) return 1; // 0xxxxxxx
	if ((prefix & 0xE0) == 0xC0) return 2; // 110xxxxx
	if ((prefix & 0xF0) == 0xE0) return 3; // 1110xxxx
	if ((prefix & 0xF8) == 0xF0) return 4; // 11110xxx
	if ((prefix & 0xFC) == 0xF8) return 5; // 111110xx
	if ((prefix & 0xFE) == 0xFC) return 6; // 1111110x
	return 0;
}