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
#include "UIElement.h"
#include "UIFactory.h"

/// @brief 
struct UIInputStyle
{
	struct
	{
		UIPen Pen{ .Style = UIPen::NoPen, };
		UIFont Font{ .LineWrap = false, .Align = UIFont::AlignLeft | UIFont::AlignVCenter, .Ellipsize = UIFont::EllipsizeNone, };
		UIBrush Brush{ .Color {0,0,0,1}, };
	} Foreground;
	struct
	{
		UIPen Pen = UIFramePen;
		UIBrush Brush = UIWhiteBrush;
	} Background;
};

/// @brief Input
class OPENUI_API UIInput : public UIElement
{
public:
	enum echo_t
	{
		EchoModeNormal = 0,
		EchoModeNoEcho = 1,
		EchoModePassword = 2,
		EchoModePasswordOnEdit = 3,
	};

public:
	explicit UIInput(UIContextRaw context);
	~UIInput() override;
	void arrange(UIRect client) override;
	void layout(UIRect client) override;
	void paint(UIRect client, UIPainterRaw painter) override;
	void repaint(UIRect client, UIPainterRaw painter) override;

	UIInputStyle getStyle() const;
	void setStyle(UIInputStyle value);

	echo_t getEchoMode() const;
	void setEchoMode(echo_t value);

	int32_t getMaxLength() const;
	void setMaxLength(int32_t value);

	UIString getPlaceholderText() const;
	void setPlaceholderText(UIString const& value);

	bool getReadOnly() const;
	void setReadOnly(bool value);

	UIFontAligns getAlignment() const;
	void setAlignment(UIFontAligns value);

	int32_t getCursorPosition() const;
	int32_t getCursorPosition(int32_t x, int32_t y) const;
	void setCursorPosition(int32_t value);

	UIString getSelectedText() const;
	void getSelection(int32_t& start, int32_t& length) const;
	void setSelection(int32_t start, int32_t length);

	UIString getText() const;
	void setText(UIString const& value);

	void backspace();
	void del();
	void deselect();
	void insert(UIString const& text);

	void clear();
	UIString copy();
	void cut();
	void paste(UIString const& text);
	void redo();
	void undo();

protected:
	void keyPressEvent(UIKeyEventRaw event) override;
	void inputEvent(UITextInputEventRaw event) override;
	void mouseDoubleEvent(UIMouseEventRaw event) override;
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseReleaseEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;

public:
	UISignalAsRaw<int32_t /*oldPos*/, int32_t /*newPos*/> cursorPositionChanged;
	UISignalAsRaw<UIRect /*area*/> editingStarted;
	UISignalAsRaw<> editingFinished;
	UISignalAsRaw<> inputRejected;
	UISignalAsRaw<> returnPressed;
	UISignalAsRaw<> selectionChanged;
	UISignalAsRaw<UIString const& /*text*/> textChanged;
	UISignalAsRaw<UIString const& /*text*/> textEdited;
	UISignalAsRaw<UIString& /*text*/> textPasted;
	UISignalAsRaw<UIString const& /*text*/> textCopied;

private:
	UIElementPrivateRaw m_PrivateInput;
};

/// @brief Input factory
class OPENUI_API UIInputFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};