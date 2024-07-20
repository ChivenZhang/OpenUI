#pragma once
#include "OpenUI/UIElement.h"
#include "OpenUI/UIFactory.h"
#include "UIButton.h"

/// @brief Combo
class OPENUI_API UICombo : public UIElement
{
public:
	UICombo();
	~UICombo();
	virtual void arrange(UIRect client) override;
	virtual void layout(UIRect client) override;
	virtual void paint(UIRect client, UIPainterRaw painter) override;

	UIStringList const& getItems() const;
	void setItems(UIStringList const& texts);

	int32_t getMaxCount() const;
	void setMaxCount(int32_t value);

	int32_t getCurrentIndex() const;
	void setCurrentIndex(int32_t index);

	UIString getCurrentText() const;
	void setCurrentText(UIString const& text);

	UIButtonStyle getStyle() const;
	void setStyle(UIButtonStyle value);

	UIButtonStyle getItemStyle() const;
	void setItemStyle(UIButtonStyle value);

protected:
	virtual bool filter(UIReactorRaw source, UIEventRaw event) override;

public:
	UISignalAsRaw<int /*index*/> activated;
	UISignalAsRaw<int /*index*/> currentIndexChanged;
	UISignalAsRaw<UIString /*text*/> currentTextChanged;
	// UISignalAsRaw<UIString /*text*/> editTextChanged;
	UISignalAsRaw<int /*index*/> highlighted;
	UISignalAsRaw<UIString /*text*/> textActivated;
	UISignalAsRaw<UIString /*text*/> textHighlighted;

private:
	UIElementPrivateRaw m_PrivateCombo;
};
using UIComboRef = UIRef<UICombo>;
using UIComboRaw = UIRaw<UICombo>;

/// @brief Combo Factory
class OPENUI_API UIComboFactory : public UIFactory
{
public:
	UIString getTagName() const override;
	UIElementRef getElement(UIString style) const override;
};