#pragma once
#include "PCStyle.h"

class OPENUI_API PCListBox : public UIWidget
{
public:
	explicit PCListBox(UICanvasRaw canvas);
	~PCListBox() override;
	void paint(UIRect client, UIPainterRaw painter) override;

	UIStringList const& getItems() const;
	void setItems(UIStringList const& items);
	int32_t getCurrentRow() const;
	void setCurrentRow(int32_t value);

	UISignalAsRaw<int32_t> currentRowChanged;
	UISignalAsRaw<int32_t> itemClicked;
	UISignalAsRaw<int32_t> itemActivated;

protected:
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseDoubleEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;
	void leaveEvent(UIMouseEventRaw event) override;
	void wheelEvent(UIMouseWheelEventRaw event) override;
	void keyPressEvent(UIKeyEventRaw event) override;

private:
	int32_t rowAt(float y) const;
	UIPrivateRaw m_Private;
};
using PCListBoxRef = UIRef<PCListBox>;
using PCListBoxRaw = UIRaw<PCListBox>;
