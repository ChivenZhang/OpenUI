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

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning(error:4150)
#pragma warning(disable:4250)
#pragma warning(disable:4200)
#pragma warning(disable:26812)
#pragma warning(disable:26815)
#pragma warning(disable:26816)

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
#include <thread>
#include <future>
#include <atomic>
#include <mutex>
#include "Utility/shared_recursive_mutex.h"
#include <condition_variable>

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
using RmThread = std::thread;
template <class T>
using RmFuture = std::future<T>;
template <class T>
using RmPromise = std::promise<T>;
template <class T>
using RmAtomic = std::atomic<T>;
using RmMutex = std::recursive_mutex;
using RmMutexLock = std::lock_guard<RmMutex>;
using RmUniqueLock = std::unique_lock<RmMutex>;
using RmSharedMutex = std::shared_recursive_mutex;
using RmMutexReadLock = std::shared_lock<RmSharedMutex>;
using RmMutexWriteLock = std::unique_lock<RmSharedMutex>;
using RmMutexUnlock = std::condition_variable_any;
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