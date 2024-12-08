/*=================================================
* Copyright © 2020-2025 ChivenZhang.
* All Rights Reserved.
* =====================Note=========================
*
*
* ====================History=======================
* Created by ChivenZhang@gmail.com.
*
* =================================================*/
#include "../UICombo.h"
#include "../UIVBox.h"
#include "../UIScroll.h"
#include "../UIContext.h"

class UIComboPrivate : public UIElementPrivate
{
public:
	int32_t Index = -1;
	int32_t MaxCount = -1;
	UIStringList Items;
	UIButtonRef Button;
	UIScrollRef Popup;
	UIComboStyle Style;

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

	PRIVATE()->Button = UINew<UIButton>();
	addElement(PRIVATE()->Button);
	PRIVATE()->Popup = UINew<UIScroll>();
	PRIVATE()->Popup->setEventFilter(this);
	setStyle(PRIVATE()->Style);

	PRIVATE()->Button->clicked->connect(this, [=](bool checked) {
		auto posX = PRIVATE()->Button->getPosX();
		auto posY = PRIVATE()->Button->getPosY() + PRIVATE()->Button->getHeight() + 2;
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
	setMinWidth(30);
	setJustifyContent(UI::JustifySpaceEvenly);
	PRIVATE()->Button->setFlexGrow(1);
	PRIVATE()->Button->getLabel()->setMargin({ 8, 0, 20, 0 });

	PRIVATE()->Popup->setAlignItems(UI::AlignStretch);
	PRIVATE()->Popup->setJustifyContent(UI::JustifySpaceEvenly);
	PRIVATE()->Popup->setPositionType(UI::PositionAbsolute);
	PRIVATE()->Popup->setFixedHeight(0);

	auto painter = getPainter();
	if (painter == nullptr) painter = getContext()->getPainter();
	if (painter == nullptr) return;
	painter->setFont(PRIVATE()->Style.Button.Label.Normal.Foreground.Font);

	auto content = PRIVATE()->Popup->getContentView();
	if (content)
	{
		content->setFixedWidth(0);
		content->setFixedHeight(0);
		for (size_t i = 0; content && i < content->getChildren().size(); ++i)
		{
			auto itemWidget = UICast<UIButton>(content->getChildren()[i]);
			auto textRect = painter->boundingRect(0, 0, FLT_MAX, FLT_MAX, itemWidget->getText(), 0);

			content->getChildren()[i]->setFlexGrow(1);
			content->getChildren()[i]->setMinWidth(textRect.W);
			content->getChildren()[i]->setMinHeight(textRect.H);
			content->setFixedWidth(std::max(content->getFixedWidth().Value, textRect.W + 8 + 20));
			content->setFixedHeight(content->getFixedHeight() + textRect.H);

			if (std::isnan(getFixedWidth()) == false)
			{
				content->setFixedWidth(std::max<float>(getFixedWidth(), content->getFixedWidth()));
				PRIVATE()->Popup->setFixedWidth(std::max<float>(getFixedWidth(), content->getFixedWidth()));
			}
			PRIVATE()->Popup->setFixedWidth(std::max<float>(PRIVATE()->Popup->getFixedWidth(), content->getFixedWidth()));
			if (PRIVATE()->MaxCount == -1 || i < PRIVATE()->MaxCount) PRIVATE()->Popup->setFixedHeight(content->getFixedHeight());
		}
	}
	else
	{
		if (std::isnan(getFixedWidth()) == false)
		{
			PRIVATE()->Popup->setFixedWidth(std::max<float>(getFixedWidth(), PRIVATE()->Popup->getFixedWidth()));
		}
	}
	PRIVATE()->Popup->setFixedHeight(std::max<float>(painter->getFont().Size, PRIVATE()->Popup->getFixedHeight()));
}

void UICombo::layout(UIRect client)
{
}

void UICombo::paint(UIRect client, UIPainterRaw painter)
{
	UIElement::paint(client, painter);
}

void UICombo::repaint(UIRect client, UIPainterRaw painter)
{
	UIElement::repaint(client, painter);

	painter->setPen(PRIVATE()->Style.Button.Normal.Pen);
	painter->drawLine(client.X + client.W - 20, client.Y + (client.H - 5) * 0.5f, client.X + client.W - 15, client.Y + (client.H + 5) * 0.5f);
	painter->drawLine(client.X + client.W - 15, client.Y + (client.H + 5) * 0.5f, client.X + client.W - 10, client.Y + (client.H - 5) * 0.5f);
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

	for (size_t i = 0; i < PRIVATE()->Items.size(); ++i)
	{
		auto text = PRIVATE()->Items[i];
		auto button = UINew<UIButton>();
		itemsWidget->addElement(button);
		button->setText(text);
		button->setStyle(PRIVATE()->Style.Items);
		button->getLabel()->setMargin({ 8, 0, 20, 0 });

		button->hovered->connect(this, [=]() {
			PRIVATE()->OnHighlighted.signal(i);
			PRIVATE()->OnTextHighlighted.signal(text);
			});

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

UIComboStyle UICombo::getStyle() const
{
	return PRIVATE()->Style;
}

void UICombo::setStyle(UIComboStyle value)
{
	PRIVATE()->Style = value;
	PRIVATE()->Button->setStyle(PRIVATE()->Style.Button);

	auto contentWidget = PRIVATE()->Popup->getContentView();
	for (size_t i = 0; contentWidget && i < contentWidget->getChildren().size(); ++i)
	{
		auto itemWidget = UICast<UIButton>(contentWidget->getChildren()[i]);
		if (itemWidget == nullptr) continue;
		itemWidget->setStyle(PRIVATE()->Style.Items);
		itemWidget->getLabel()->setStyle(PRIVATE()->Style.Items.Label);
	}
}

bool UICombo::filter(UIReactorRaw source, UIEventRaw event)
{
	switch (event->Type)
	{
	case UIHash("MouseDown"):
	{
		auto event2 = UICast<UIMouseEvent>(event);
		auto client = PRIVATE()->Popup->getBounds();
		auto viewport = PRIVATE()->Popup->getViewport();
		if (UIBounds(UIOverlap(client, viewport), event2->X, event2->Y) == false)
		{
			getContext()->removeElement(PRIVATE()->Popup);
		}
	} break;
	case UIHash("DoubleClick"):
	{
		auto event2 = UICast<UIMouseEvent>(event);
		auto client = PRIVATE()->Popup->getBounds();
		auto viewport = PRIVATE()->Popup->getViewport();
		if (UIBounds(UIOverlap(client, viewport), event2->X, event2->Y) == false)
		{
			getContext()->removeElement(PRIVATE()->Popup);
		}
	} break;
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
