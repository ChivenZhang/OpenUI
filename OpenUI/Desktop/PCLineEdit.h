#pragma once
#include "PCStyle.h"

class OPENUI_API PCLineEdit : public UIWidget
{
public:
	explicit PCLineEdit(UICanvasRaw canvas);
	~PCLineEdit() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIString getText() const;
	void setText(UIString const& value);
	UIString getPlaceholder() const;
	void setPlaceholder(UIString const& value);

	UISignalAsRaw<UIString const&> textChanged;
	UISignalAsRaw<> editingFinished;
	UISignalAsRaw<> returnPressed;

protected:
	void mousePressEvent(UIMouseEventRaw event) override;
	void keyPressEvent(UIKeyEventRaw event) override;
	void inputEvent(UITextInputEventRaw event) override;
	void focusOutEvent(UIFocusEventRaw event) override;

private:
	UIPrivateRaw m_Private;
};
using PCLineEditRef = UIRef<PCLineEdit>;
using PCLineEditRaw = UIRaw<PCLineEdit>;
