/*=================================================
* Copyright @ 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang at 2025/03/30 14:03:10.
*
* =================================================*/
#include "../UIFactory.h"

UIFactory::UIFactory(UICanvasRaw canvas)
	:
	m_Context(canvas)
{
}

UICanvasRaw UIFactory::getContext() const
{
	return m_Context;
}
