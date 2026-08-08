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
	struct attrib_t
	{
		UIString Name, Value;
	};
	struct style_t
	{
		UIString Name, Value;
		uint32_t Priority;
	};
	struct widget_t
	{
		UIString Type, Class, ID, Text;
		UIList<style_t> Style;
		UIList<attrib_t> Attrib;
		UIList<widget_t> Children;
	};

public:
	virtual ~UIParser() = default;
	virtual bool parse(UIString html, widget_t& result) const;
};
