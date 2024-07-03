#pragma once
#include "RmGUIControl.h"

struct RmGUITextStyle
{
	RmPen Pen;
	RmFont Font;
	RmBrush Brush;
};

/// @brief Line Edit
class RMGUI_API RmGUIText : public RmGUIControl
{
public:
	using echo_t = enum
	{
		EchoModeNormal = 0,
		EchoModeNoEcho = 1,
		EchoModePassword = 2,
		EchoModePasswordOnEdit = 3,
	};

public:
	RmGUIText(IRmGUIWidgetRaw parent = nullptr);
	~RmGUIText();
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;

	RmGUITextStyle getStyle() const;
	void setStyle(RmGUITextStyle value);

	echo_t getEchoMode() const;
	void setEchoMode(echo_t value);

	int32_t getMaxLength() const;
	void setMaxLength(int32_t value);

	RmString getPlaceholderText() const;
	void setPlaceholderText(RmString const& value);

	bool getReadOnly() const;
	void setReadOnly(bool value);

	RmFontAligns getAlignment() const;
	void setAlignment(RmFontAligns value);

	int32_t getCursorPosition() const;
	int32_t getCursorPosition(int32_t x, int32_t y) const;
	void setCursorPosition(int32_t value);

	RmString getSelectedText() const;
	void getSelection(int32_t& start, int32_t& length) const;
	void setSelection(int32_t start, int32_t& length);

	RmString getText() const;
	void setText(RmString const& value);

	void backspace();
	void del();
	void deselect();
	void insert(RmString const& text);

	void clear();
	void cut();
	void redo();
	void undo();

protected:
	virtual void keyPressEvent(IRmGUIKeyEventRaw event) override;
	virtual void inputEvent(IRmGUITextInputEventRaw event) override;
	virtual void mouseDoubleEvent(IRmGUIMouseEventRaw event) override;
	virtual void mousePressEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseReleaseEvent(IRmGUIMouseEventRaw event) override;
	virtual void mouseMoveEvent(IRmGUIMouseEventRaw event) override;

public:
	IRmGUISignalAsRaw<int32_t /*oldPos*/, int32_t /*newPos*/> cursorPositionChanged;
	IRmGUISignalAsRaw<RmRect /*area*/> editingStarted;
	IRmGUISignalAsRaw<> editingFinished;
	IRmGUISignalAsRaw<> inputRejected;
	IRmGUISignalAsRaw<> returnPressed;
	IRmGUISignalAsRaw<> selectionChanged;
	IRmGUISignalAsRaw<RmString const& /*text*/> textChanged;
	IRmGUISignalAsRaw<RmString const& /*text*/> textEdited;

private:
	RmGUIWidgetPrivateRaw m_PrivateText;
};
using RmGUITextRef = RmRef<RmGUIText>;
using RmGUITextRaw = RmRaw<RmGUIText>;
using RmGUITextEchoMode = RmGUIText::echo_t;