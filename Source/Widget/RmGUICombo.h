#pragma once
#include "RmGUIWidget.h"
#include "RmGUIButton.h"

/// @brief Combo Box
class RMGUI_API RmGUICombo : public RmGUIWidget
{
public:
	RmGUICombo(IRmGUIWidgetRaw parent = nullptr);
	~RmGUICombo();
	virtual void layout(RmRectRaw client) override;
	virtual void paint(IRmGUIPainterRaw painter, RmRectRaw client) override;
	RmStringList const& getItems() const;
	void setItems(RmStringList const& texts);
	int32_t getMaxCount() const;
	void setMaxCount(int32_t value);
	int32_t getCurrentIndex() const;
	void setCurrentIndex(int32_t index);
	RmString getCurrentText() const;
	void setCurrentText(RmString const& text);
	RmGUIButtonStyle getStyle() const;
	void setStyle(RmGUIButtonStyle value);
	RmGUIButtonStyle getItemStyle() const;
	void setItemStyle(RmGUIButtonStyle value);

protected:
	virtual bool filter(IRmGUIReactorRaw source, IRmGUIEventRaw event) override;

public:
	IRmGUISignalAsRaw<int /*index*/> activated;
	IRmGUISignalAsRaw<int /*index*/> currentIndexChanged;
	IRmGUISignalAsRaw<RmString /*text*/> currentTextChanged;
	// IRmGUISignalAsRaw<RmString /*text*/> editTextChanged;
	IRmGUISignalAsRaw<int /*index*/> highlighted;
	IRmGUISignalAsRaw<RmString /*text*/> textActivated;
	IRmGUISignalAsRaw<RmString /*text*/> textHighlighted;

private:
	RmGUIWidgetPrivateRaw m_PrivateCombo;
};