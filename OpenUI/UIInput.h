#pragma once
#include "UIElement.h"
#include "UIFactory.h"

/// @brief 
struct UIInputStyle
{
	struct
	{
		UIPen Pen{ .Style = UIPen::NoPen, };
		UIFont Font{ .LineWrap = true, .Align = UIFont::AlignLeft | UIFont::AlignVCenter, .Ellipsize = UIFont::EllipsizeNone, };
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
	UIInput();
	~UIInput();
	virtual void arrange(UIRect client) override;
	virtual void layout(UIRect client) override;
	virtual void paint(UIRect client, UIPainterRaw painter) override;
	virtual void repaint(UIRect client, UIPainterRaw painter) override;

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
	virtual void keyPressEvent(UIKeyEventRaw event) override;
	virtual void inputEvent(UITextInputEventRaw event) override;
	virtual void mouseDoubleEvent(UIMouseEventRaw event) override;
	virtual void mousePressEvent(UIMouseEventRaw event) override;
	virtual void mouseReleaseEvent(UIMouseEventRaw event) override;
	virtual void mouseMoveEvent(UIMouseEventRaw event) override;

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
	virtual UIString getTagName() const override;
	virtual UIElementRef getElement(UIString style) const override;
};