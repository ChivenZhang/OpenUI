#include "UICombo.h"
#include "UIVBox.h"
#include "UIScroll.h"
#include "OpenUI/UIContext.h"

class UIComboPrivate : public UIElementPrivate
{
public:
	int32_t Index = -1;
	int32_t MaxCount = -1;
	UIStringList Items;
	UIButtonRef Button;
	UIScrollRef Popup;
	UIButtonStyle ItemStyle;

	UISignalAs<int /*index*/> OnActivated;
	UISignalAs<int /*index*/> OnCurrentIndexChanged;
	UISignalAs<UIString /*text*/> OnCurrentTextChanged;
	// UISignalAs<UIString /*text*/> editTextChanged;
	UISignalAs<int /*index*/> OnHighlighted;
	UISignalAs<UIString /*text*/> OnTextActivated;
	UISignalAs<UIString /*text*/> OnTextHighlighted;
};
#define PRIVATE() ((UIComboPrivate*)m_PrivateCombo)

UICombo::UICombo()
{
	m_PrivateCombo = new UIComboPrivate;

	activated = &PRIVATE()->OnActivated;
	currentIndexChanged = &PRIVATE()->OnCurrentIndexChanged;
	currentTextChanged = &PRIVATE()->OnCurrentTextChanged;
	highlighted = &PRIVATE()->OnHighlighted;
	textActivated = &PRIVATE()->OnTextActivated;
	textHighlighted = &PRIVATE()->OnTextHighlighted;

	PRIVATE()->ItemStyle.Normal.Pen = { .Style = UIPen::NoPen, .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
	PRIVATE()->ItemStyle.Normal.Brush = { .Color = {253 / 255.0f, 253 / 255.0f, 253 / 255.0f, 1.0f} };
	PRIVATE()->ItemStyle.Hover.Pen = { .Style = UIPen::NoPen, .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
	PRIVATE()->ItemStyle.Hover.Brush = { .Color = {224 / 255.0f, 238 / 255.0f, 249 / 255.0f, 1.0f} };
	PRIVATE()->ItemStyle.Press.Pen = { .Style = UIPen::NoPen,.Color = {0 / 255.0f, 84 / 255.0f, 153 / 255.0f, 1.0f} };
	PRIVATE()->ItemStyle.Press.Brush = { .Color = {204 / 255.0f, 228 / 255.0f, 247 / 255.0f, 1.0f} };

	PRIVATE()->Button = UINew<UIButton>();
	addElement(PRIVATE()->Button);
	PRIVATE()->Popup = UINew<UIScroll>();
	PRIVATE()->Popup->setEventFilter(this);

	PRIVATE()->Button->clicked->connect(this, [=](bool checked) {
		auto posX = PRIVATE()->Button->getPosX();
		auto posY = PRIVATE()->Button->getPosY() + PRIVATE()->Button->getHeight();
		PRIVATE()->Popup->setFixedPos(posX, posY);
		PRIVATE()->Popup->getHorizontalBar()->setValue(0);
		PRIVATE()->Popup->getVerticalBar()->setValue(0);
		getContext()->addElement(PRIVATE()->Popup, 1);
		});
}

UICombo::~UICombo()
{
	delete m_PrivateCombo; m_PrivateCombo = nullptr;
}

void UICombo::arrange(UIRect client)
{
	setJustifyContent(UI::JustifySpaceEvenly);
	PRIVATE()->Button->setFlexGrow(1);
}

void UICombo::layout(UIRect client)
{
}

void UICombo::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
}

UIStringList const& UICombo::getItems() const
{
	return PRIVATE()->Items;
}

void UICombo::setItems(UIStringList const& texts)
{
	PRIVATE()->Items = texts;
	PRIVATE()->Popup->removeElement();
	auto itemsWidget = UINew<UIVBox>();
	PRIVATE()->Popup->addElement(itemsWidget);
	PRIVATE()->Popup->setFixedWidth(getFixedWidth() * 1.2f);
	itemsWidget->setFixedHeight(200);
	PRIVATE()->Popup->setFixedHeight(200);

	for (size_t i = 0; i < PRIVATE()->Items.size(); ++i)
	{
		auto text = PRIVATE()->Items[i];
		auto button = UINew<UIButton>();
		itemsWidget->addElement(button);
		button->setText(text);
		button->setStyle(PRIVATE()->ItemStyle);
		button->setFixedHeight(35);

		button->clicked->connect(this, [=](bool clicked) {
			auto oldIndex = PRIVATE()->Index;
			PRIVATE()->Index = (int32_t)i;
			PRIVATE()->Button->setText(text);
			getContext()->removeElement(PRIVATE()->Popup);
			PRIVATE()->OnTextActivated.signal(text);
			if (PRIVATE()->Index != oldIndex)
			{
				PRIVATE()->OnCurrentIndexChanged.signal(i);
				PRIVATE()->OnCurrentTextChanged.signal(text);
			}
			});

		button->hovered->connect(this, [=]() {
			PRIVATE()->OnHighlighted.signal(i);
			PRIVATE()->OnTextHighlighted.signal(text);
			});
	}
}

int32_t UICombo::getMaxCount() const
{
	return PRIVATE()->MaxCount;
}

void UICombo::setMaxCount(int32_t value)
{
	PRIVATE()->MaxCount = value;
	setItems(PRIVATE()->Items);
}

int32_t UICombo::getCurrentIndex() const
{
	return PRIVATE()->Index;
}

void UICombo::setCurrentIndex(int32_t index)
{
	PRIVATE()->Index = index;
	if (0 <= PRIVATE()->Index && PRIVATE()->Index < PRIVATE()->Items.size())
	{
		PRIVATE()->Button->setText(getCurrentText());
	}
	else
	{
		PRIVATE()->Index = -1;
		PRIVATE()->Button->setText(UIString());
	}
}

UIString UICombo::getCurrentText() const
{
	if (0 <= PRIVATE()->Index && PRIVATE()->Index < PRIVATE()->Items.size())
	{
		return PRIVATE()->Items[PRIVATE()->Index];
	}
	return UIString();
}

void UICombo::setCurrentText(UIString const& text)
{
	PRIVATE()->Index = -1;
	for (size_t i = 0; i < PRIVATE()->Items.size(); ++i)
	{
		if (PRIVATE()->Items[i] == text)
		{
			setCurrentIndex(i);
			break;
		}
	}
}

UIButtonStyle UICombo::getStyle() const
{
	return PRIVATE()->Button->getStyle();
}

void UICombo::setStyle(UIButtonStyle value)
{
	PRIVATE()->Button->setStyle(value);
}

UIButtonStyle UICombo::getItemStyle() const
{
	return PRIVATE()->ItemStyle;
}

void UICombo::setItemStyle(UIButtonStyle value)
{
	PRIVATE()->ItemStyle = value;
	auto contentWidget = PRIVATE()->Popup->getChildren()[0];
	for (size_t i = 0; contentWidget && i < contentWidget->getChildren().size(); ++i)
	{
		auto itemWidget = UICast<UIButton>(contentWidget->getChildren()[i]);
		if (itemWidget == nullptr) continue;
		itemWidget->setStyle(value);
	}
}

bool UICombo::filter(UIReactorRaw source, UIEventRaw event)
{
	if (source == UICast<UIReactor>(PRIVATE()->Popup.get()))
	{
		switch (event->Type)
		{
		case UIHash("MouseDown"):
		{
			auto event2 = UICast<UIMouseEvent>(event);
			auto client = PRIVATE()->Popup->getBounds();
			auto viewport = PRIVATE()->Popup->getViewport();
			if (client.X <= event2->X && event2->X <= client.X + client.W
				&& client.Y <= event2->Y && event2->Y <= client.Y + client.H
				&& viewport.X <= event2->X && event2->X <= viewport.X + viewport.W
				&& viewport.Y <= event2->Y && event2->Y <= viewport.Y + viewport.H)
			{
				;
			}
			else
			{
				getContext()->removeElement(PRIVATE()->Popup);
			}

		} break;
		}
	}
	return false;
}

UIString UIComboFactory::getTagName() const
{
	return "combo";
}

UIElementRef UIComboFactory::getElement(UIString style) const
{
	auto result = UINew<UICombo>();
	result->setStyleText(style);
	return result;
}
