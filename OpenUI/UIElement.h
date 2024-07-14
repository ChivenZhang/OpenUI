#pragma once
#include "UIEvent.h"
#include "UIPainter.h"

class UIElement;
using UIElementRef = UIRef<UIElement>;
using UIElementRaw = UIRaw<UIElement>;

class UIElementPrivate {};
using UIElementPrivateRaw = UIRaw<UIElementPrivate>;

class UIContext;
using UIContextRaw = UIRaw<UIContext>;

#define UINAN (NAN)

template <class T, class E = uint8_t>
struct UIValue
{
	T Value = T();
	E Unit = E();
	UIValue(T const& value = T(), E uint = E(1)) : Value(value), Unit(uint) {}
	UIValue(UIValue&&) = default;
	UIValue(UIValue const&) = default;
	operator T() { return Value; }
	UIValue& operator =(UIValue&& value) { Value = value.Value; Unit = value.Unit; return *this; }
	UIValue& operator =(UIValue const& value) { Value = value.Value; Unit = value.Unit; return *this; }
};
using UIValueF = UIValue<float>;
using UIValue2F = UIArray<UIValueF, 2>;
using UIValue3F = UIArray<UIValueF, 3>;
using UIValue4F = UIArray<UIValueF, 4>;

namespace UI
{
	enum AlignItems
	{
		AlignAuto,
		AlignFlexStart,
		AlignCenter,
		AlignFlexEnd,
		AlignStretch,
		AlignBaseline,
		AlignSpaceBetween,
		AlignSpaceAround,
		AlignSpaceEvenly,
	};
	enum FlexDirection
	{
		FlexDirectionColumn,
		FlexDirectionColumnReverse,
		FlexDirectionRow,
		FlexDirectionRowReverse,
	};
	enum FlexWrap
	{
		FlexNoWrap,
		FlexDoWrap,
		FlexWrapReverse,
	};
	using AlignContent = AlignItems;
	enum JustifyContent
	{
		JustifyFlexStart,
		JustifyCenter,
		JustifyFlexEnd,
		JustifySpaceBetween,
		JustifySpaceAround,
		JustifySpaceEvenly,
	};

	using FlexGrow = UIValueF;
	using FlexBasis = UIValueF;
	using FlexShrink = UIValueF;
	using AlignSelf = AlignItems;

	enum ValueUnit : uint8_t { UnitNone = 0, UnitPoint, UnitPercent, UnitAuto, };
};

/// @brief Base interface of element
class OPENUI_API UIElement : public UIReactor, public UIHandler, public UIFilter
{
public:
	UIElement();
	virtual ~UIElement();
	UIElementRaw getParent() const;
	UIArrayView<const UIElementRef> getChildren() const;
	virtual UIPainterRaw getPainter() const;
	virtual void setPainter(UIPainterRef value);
	virtual UIFilterRaw getEventFilter() const;
	virtual void setEventFilter(UIFilterRaw value);
	virtual UIString getStyle() const;
	virtual void setStyle(UIString value);
	virtual UIString getStyle(UIString name) const;
	virtual void setStyle(UIString name, UIString value);
	virtual UIArrayView<const UIPointUV3> getPrimitive() const;
	virtual bool addElement(UIElementRef value);
	virtual bool removeElement(UIElementRef value);
	virtual void removeElement();
	virtual void layout(UIRect client);
	virtual void paint(UIRect client, UIPainterRaw painter);
	virtual bool filter(UIReactorRaw source, UIEventRaw event) override;
	virtual void handle(UIReactorRaw source, UIEventRaw event) final override;

	// =================================

	virtual bool getEnable() const;
	virtual void setEnable(bool value);
	virtual bool getVisible() const;
	virtual void setVisible(bool value);
	virtual UIRect getBounds() const;
	virtual void setBounds(UIRect value);
	virtual UIRect getViewport() const;
	virtual void setViewport(UIRect value);

	virtual UIValueF getFixedPosX() const;
	virtual void setFixedPosX(UIValueF value);
	virtual UIValueF getFixedPosY() const;
	virtual void setFixedPosY(UIValueF value);
	virtual UIValue2F getFixedPos() const;
	virtual void setFixedPos(UIValueF left, UIValueF top);
	virtual UIValueF getMinWidth() const;
	virtual void setMinWidth(UIValueF value);
	virtual UIValueF getMaxWidth() const;
	virtual void setMaxWidth(UIValueF value);
	virtual UIValueF getFixedWidth() const;
	virtual void setFixedWidth(UIValueF value);
	virtual UIValueF getMinHeight() const;
	virtual void setMinHeight(UIValueF value);
	virtual UIValueF getMaxHeight() const;
	virtual void setMaxHeight(UIValueF value);
	virtual UIValueF getFixedHeight() const;
	virtual void setFixedHeight(UIValueF value);
	virtual UIValue2F getMinSize() const;
	virtual void setMinSize(UIValueF width, UIValueF height);
	virtual UIValue2F getMaxSize() const;
	virtual void setMaxSize(UIValueF width, UIValueF height);
	virtual UIValue2F getFixedSize() const;
	virtual void setFixedSize(UIValueF width, UIValueF height);

	virtual UIValue4F getBorder() const;
	virtual void setBorder(UIValue4F value);
	virtual UIValue4F getMargin() const;
	virtual void setMargin(UIValue4F value);
	virtual UIValue4F getPadding() const;
	virtual void setPadding(UIValue4F value);

	virtual UI::FlexDirection getFlexDirection() const;
	virtual void setFlexDirection(UI::FlexDirection value);
	virtual UI::FlexWrap getFlexWrap() const;
	virtual void setFlexWrap(UI::FlexWrap value);
	virtual UI::JustifyContent getJustifyContent() const;
	virtual void setJustifyContent(UI::JustifyContent value);
	virtual UI::AlignItems getAlignItems() const;
	virtual void setAlignItems(UI::AlignItems value);
	virtual UI::AlignContent getAlignContent() const;
	virtual void setAlignContent(UI::AlignContent value);
	virtual void setFlexFlow(UI::FlexDirection direction, UI::FlexWrap wrap);

	virtual UI::FlexGrow getFlexGrow() const;
	virtual void setFlexGrow(UI::FlexGrow value);
	virtual UI::FlexShrink getFlexShrink() const;
	virtual void setFlexShrink(UI::FlexShrink value);
	virtual UI::FlexBasis getFlexBasis() const;
	virtual void setFlexBasis(UI::FlexBasis value);
	virtual UI::AlignSelf getAlignSelf() const;
	virtual void setAlignSelf(UI::AlignSelf value);
	virtual void setFlex(UI::FlexGrow grow, UI::FlexShrink shrink, UI::FlexBasis basis);

protected:
	virtual void closeEvent(UICloseEventRaw event);
	virtual void dragEnterEvent(UIDragEnterEventRaw event);
	virtual void dragLeaveEvent(UIDragLeaveEventRaw event);
	virtual void dragMoveEvent(UIDragMoveEventRaw event);
	virtual void dropEvent(UIDropEventRaw event);
	virtual void enterEvent(UIMouseEventRaw event);
	virtual void focusInEvent(UIFocusEventRaw event);
	virtual void focusOutEvent(UIFocusEventRaw event);
	virtual void hideEvent(UIHideEventRaw event);
	virtual void inputEvent(UITextInputEventRaw event);
	virtual void keyPressEvent(UIKeyEventRaw event);
	virtual void keyReleaseEvent(UIKeyEventRaw event);
	virtual void leaveEvent(UIMouseEventRaw event);
	virtual void mouseDoubleEvent(UIMouseEventRaw event);
	virtual void mouseMoveEvent(UIMouseEventRaw event);
	virtual void mousePressEvent(UIMouseEventRaw event);
	virtual void mouseReleaseEvent(UIMouseEventRaw event);
	virtual void moveEvent(UIMoveEventRaw event);
	virtual void resizeEvent(UIResizeEventRaw event);
	virtual void showEvent(UIShowEventRaw event);
	virtual void tabletEvent(UIMouseTabletEventRaw event);
	virtual void wheelEvent(UIMouseWheelEventRaw event);

protected:
	virtual UIContextRaw getContext() const;
	virtual void setContext(UIContextRaw value);
	virtual void setParent(UIElementRaw value);

private:
	friend class UIContext;
	UIElementPrivateRaw m_Private;
};