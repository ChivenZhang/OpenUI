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
	RmGUITextStyle getStyle() const;
	void setStyle(RmGUITextStyle value);
	int32_t getCursorPosition() const;
	void setCursorPosition(int32_t value);
	RmString getPlaceholderText() const;
	void setPlaceholderText(RmString const& value);
	echo_t getEchoMode() const;
	void setEchoMode(echo_t value);
	int32_t getMaxLength() const;
	void setMaxLength(int32_t value);
	bool getReadOnly() const;
	void setReadOnly(bool value);
	RmString getSelectedText() const;
	void setSelectedText(RmString const value);
	RmString getText() const;
	void setText(RmString const& value);

private:
	RmGUIWidgetPrivateRaw m_PrivateInput;
};