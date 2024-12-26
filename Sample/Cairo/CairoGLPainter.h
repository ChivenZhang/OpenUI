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
#include "CairoUIPainter.h"

class CairoGLPainter : public CairoUIPainter
{
public:
	CairoGLPainter(uint32_t width, uint32_t height);
	~CairoGLPainter();
	virtual void resize(uint32_t width, uint32_t height) override;
	uint32_t getTexture() const;

protected:
	uint32_t m_NativeTexture;
};