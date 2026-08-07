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
#include "UI.h"

/// @brief 
class OPENUI_API UIParser
{
public:
	struct style_t
	{
		UIString Name, Value;
	};
	struct attrib_t
	{
		UIString Name, Value;
	};
	struct elem_t
	{
		UIString Type, Class, ID;
		UIList<style_t> Style;
		UIList<attrib_t> Attrib;
		UIList<UIRef<elem_t>> Child;
	};

public:
	virtual ~UIParser() = default;
	virtual UIRef<elem_t> parse(UIString html) const;
};
