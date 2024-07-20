#pragma once
#include "OpenUI/UIElement.h"
#include "OpenUI/UIFactory.h"

/// @brief 
struct UIGridStyle
{
	UIPen Pen;
	UIBrush Brush;
};

/// @brief Grid
class OPENUI_API UIGrid : public UIElement
{
public:
	UIGrid();
	~UIGrid();
	virtual void arrange(UIRect client) override;
	virtual void paint(UIRect client, UIPainterRaw painter) override;

	virtual bool addElement(UIElementRef value) override;
	virtual bool addElement(UIElementRef value, uint32_t row, uint32_t column, uint32_t rowSpan = 1, uint32_t columnSpan = 1);
	virtual bool removeElement(UIElementRef value) override;
	virtual void removeElement() override;

	UIGridStyle getStyle() const;
	void setStyle(UIGridStyle value);

	size_t getRowCount() const;
	uint32_t getRowStretch(size_t index) const;
	void setRowStretch(size_t index, uint32_t stretch);
	void setRowStretch(UIVector<uint32_t> stretch);

	size_t getColumnCount() const;
	uint32_t getColumnStretch(size_t index) const;
	void setColumnStretch(size_t index, uint32_t stretch);
	void setColumnStretch(UIVector<uint32_t> stretch);

private:
	UIElementPrivateRaw m_PrivateGrid;
};

/// @brief Grid factory
class OPENUI_API UIGridFactory : public UIFactory
{
public:
	virtual UIString getTagName() const override;
	virtual UIElementRef getElement(UIString style) const override;
};