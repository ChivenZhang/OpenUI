#pragma once
#include "PCStyle.h"

class OPENUI_API PCComboBox : public UIWidget
{
public:
	explicit PCComboBox(UICanvasRaw canvas);
	~PCComboBox() override;
	void paint(UIRect client, UIPainterRaw painter) override;
	bool filter(UIReactorRaw source, UIEventRaw event) override;

	UIStringList const& getItems() const;
	void setItems(UIStringList const& items);
	int32_t getCurrentIndex() const;
	void setCurrentIndex(int32_t value);
	UIString getCurrentText() const;

	UISignalAsRaw<int32_t> currentIndexChanged;
	UISignalAsRaw<UIString const&> currentTextChanged;
	UISignalAsRaw<int32_t> activated;

protected:
	void mousePressEvent(UIMouseEventRaw event) override;
	void mouseMoveEvent(UIMouseEventRaw event) override;
	void leaveEvent(UIMouseEventRaw event) override;

private:
	void openPopup();
	void closePopup();
	UIPrivateRaw m_Private;
};
using PCComboBoxRef = UIRef<PCComboBox>;
using PCComboBoxRaw = UIRaw<PCComboBox>;
