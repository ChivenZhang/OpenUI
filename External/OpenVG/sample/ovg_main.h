// ovg.h: 标准系统包含文件的包含文件
// 或项目特定的包含文件。

#pragma once

#include <iostream>

// TODO: 在此处引用程序需要的其他标头。

#include <chrono>
#include "pch.h"


class runtime_cx
{
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _begin;
public:
	runtime_cx()
	{
		begin();
	}
	~runtime_cx()
	{}
	void begin() {
		_begin = (std::chrono::high_resolution_clock::now());
	}
	int64_t end() {

		double aa = elapsed_micro();
		aa *= 0.001;
		return aa;
	}
	int64_t get_ms() {
		auto t = std::chrono::high_resolution_clock::now().time_since_epoch();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t).count();
		return ms;
	}
	int64_t elapsed() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - _begin).count();
	}
	//微秒
	int64_t elapsed_micro() const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _begin).count();
	}
private:

};