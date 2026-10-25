#pragma once
/*=================================================
* Copyright © 2020-2026 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by chivenzhang@gmail.com.
*
* =================================================*/
#include "UI.h"

struct UIPrivate
{
    virtual ~UIPrivate() = default;
};
using UIPrivateRaw = UIRaw<UIPrivate>;

/// Light palette for the classical controls.
static constexpr UIColor UICtrlInk{ 28 / 255.0f, 31 / 255.0f, 36 / 255.0f, 1.0f };
static constexpr UIColor UICtrlMuted{ 106 / 255.0f, 113 / 255.0f, 122 / 255.0f, 1.0f };
static constexpr UIColor UICtrlLine{ 197 / 255.0f, 202 / 255.0f, 211 / 255.0f, 1.0f };
static constexpr UIColor UICtrlLineHover{ 138 / 255.0f, 160 / 255.0f, 184 / 255.0f, 1.0f };
static constexpr UIColor UICtrlPaper{ 1.0f, 1.0f, 1.0f, 1.0f };
static constexpr UIColor UICtrlCanvas{ 244 / 255.0f, 245 / 255.0f, 247 / 255.0f, 1.0f };
static constexpr UIColor UICtrlHover{ 242 / 255.0f, 246 / 255.0f, 251 / 255.0f, 1.0f };
static constexpr UIColor UICtrlPress{ 227 / 255.0f, 237 / 255.0f, 247 / 255.0f, 1.0f };
static constexpr UIColor UICtrlTrack{ 231 / 255.0f, 233 / 255.0f, 238 / 255.0f, 1.0f };
static constexpr UIColor UICtrlAccent{ 15 / 255.0f, 108 / 255.0f, 189 / 255.0f, 1.0f };
static constexpr UIColor UICtrlAccentHover{ 17 / 255.0f, 94 / 255.0f, 163 / 255.0f, 1.0f };
static constexpr UIColor UICtrlAccentPress{ 12 / 255.0f, 74 / 255.0f, 122 / 255.0f, 1.0f };
static constexpr UIColor UICtrlOnAccent{ 1.0f, 1.0f, 1.0f, 1.0f };
static constexpr UIColor UICtrlDisabledLine{ 225 / 255.0f, 228 / 255.0f, 232 / 255.0f, 1.0f };
static constexpr UIColor UICtrlDisabledFill{ 247 / 255.0f, 248 / 255.0f, 250 / 255.0f, 1.0f };
static constexpr UIColor UICtrlDisabledInk{ 163 / 255.0f, 168 / 255.0f, 176 / 255.0f, 1.0f };

static constexpr UIPen UICtrlLinePen{ .Color = UICtrlLine, .Width = 1.0f };
static constexpr UIPen UICtrlLineHoverPen{ .Color = UICtrlLineHover, .Width = 1.0f };
static constexpr UIPen UICtrlMutedPen{ .Color = UICtrlMuted, .Width = 1.0f };
static constexpr UIPen UICtrlAccentPen{ .Color = UICtrlAccent, .Width = 1.0f };
static constexpr UIPen UICtrlAccentHoverPen{ .Color = UICtrlAccentHover, .Width = 1.0f };
static constexpr UIPen UICtrlAccentPressPen{ .Color = UICtrlAccentPress, .Width = 1.0f };
static constexpr UIPen UICtrlDisabledPen{ .Color = UICtrlDisabledLine, .Width = 1.0f };

static constexpr UIBrush UICtrlInkBrush{ .Color = UICtrlInk };
static constexpr UIBrush UICtrlMutedBrush{ .Color = UICtrlMuted };
static constexpr UIBrush UICtrlPaperBrush{ .Color = UICtrlPaper };
static constexpr UIBrush UICtrlCanvasBrush{ .Color = UICtrlCanvas };
static constexpr UIBrush UICtrlHoverBrush{ .Color = UICtrlHover };
static constexpr UIBrush UICtrlPressBrush{ .Color = UICtrlPress };
static constexpr UIBrush UICtrlTrackBrush{ .Color = UICtrlTrack };
static constexpr UIBrush UICtrlAccentBrush{ .Color = UICtrlAccent };
static constexpr UIBrush UICtrlOnAccentBrush{ .Color = UICtrlOnAccent };
static constexpr UIBrush UICtrlDisabledFillBrush{ .Color = UICtrlDisabledFill };
static constexpr UIBrush UICtrlDisabledInkBrush{ .Color = UICtrlDisabledInk };
