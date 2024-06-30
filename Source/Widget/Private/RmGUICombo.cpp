#include "RmGUICombo.h"
#include "RmGUIButton.h"
#include "RmGUIScroll.h"
#include "RmGUIVBox.h"
#include "RmGUIContext.h"

class RmGUIComboPrivate : public RmGUIWidgetPrivate
{
public:
	int32_t Index = -1;
	int32_t MaxCount = -1;
	RmStringList Items;
	RmGUIButtonRef Button;
	RmGUIScrollRef Popup;
	RmGUIButtonStyle ItemStyle;

	RmGUISignalAs<int /*index*/> OnActivated;
	RmGUISignalAs<int /*index*/> OnCurrentIndexChanged;
	RmGUISignalAs<RmString /*text*/> OnCurrentTextChanged;
	// RmGUISignalAs<RmString /*text*/> editTextChanged;
	RmGUISignalAs<int /*index*/> OnHighlighted;
	RmGUISignalAs<RmString /*text*/> OnTextActivated;
	RmGUISignalAs<RmString /*text*/> OnTextHighlighted;
};
#define PRIVATE() ((RmGUIComboPrivate*)m_PrivateCombo)

RmGUICombo::RmGUICombo(IRmGUIWidgetRaw parent)
	:
	RmGUIWidget(parent),
	m_PrivateCombo(nullptr),
	activated(nullptr),
	currentIndexChanged(nullptr),
	currentTextChanged(nullptr),
	highlighted(nullptr),
	textActivated(nullptr),
	textHighlighted(nullptr)
{
	m_PrivateCombo = new RmGUIComboPrivate;

	activated = &PRIVATE()->OnActivated;
	currentIndexChanged = &PRIVATE()->OnCurrentIndexChanged;
	currentTextChanged = &PRIVATE()->OnCurrentTextChanged;
	highlighted = &PRIVATE()->OnHighlighted;
	textActivated = &PRIVATE()->OnTextActivated;
	textHighlighted = &PRIVATE()->OnTextHighlighted;

	PRIVATE()->ItemStyle.Normal.Pen = { .Style = RmPen::NoPen, .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
	PRIVATE()->ItemStyle.Normal.Brush = { .Color = {253 / 255.0f, 253 / 255.0f, 253 / 255.0f, 1.0f} };
	PRIVATE()->ItemStyle.Hover.Pen = { .Style = RmPen::NoPen, .Color = {0 / 255.0f, 120 / 255.0f, 212 / 255.0f, 1.0f} };
	PRIVATE()->ItemStyle.Hover.Brush = { .Color = {224 / 255.0f, 238 / 255.0f, 249 / 255.0f, 1.0f} };
	PRIVATE()->ItemStyle.Press.Pen = { .Style = RmPen::NoPen,.Color = {0 / 255.0f, 84 / 255.0f, 153 / 255.0f, 1.0f} };
	PRIVATE()->ItemStyle.Press.Brush = { .Color = {204 / 255.0f, 228 / 255.0f, 247 / 255.0f, 1.0f} };

	PRIVATE()->Button = RmNew<RmGUIButton>();
	addWidget(PRIVATE()->Button);
	PRIVATE()->Button->clicked->connect(this, [=](bool checked) {
		auto posX = PRIVATE()->Button->getPositionX();
		auto posY = PRIVATE()->Button->getPositionY() + PRIVATE()->Button->getHeight();
		PRIVATE()->Popup->setPosition(posX, posY);
		PRIVATE()->Popup->getHorizontalBar()->setValue(0);
		PRIVATE()->Popup->getVerticalBar()->setValue(0);
		getContext()->addWidget(PRIVATE()->Popup, 1);
		});

	PRIVATE()->Popup = RmNew<RmGUIScroll>();
	PRIVATE()->Popup->setEventFilter(this);
}

RmGUICombo::~RmGUICombo()
{
	delete m_PrivateCombo; m_PrivateCombo = nullptr;
}

void RmGUICombo::layout(RmRectRaw client)
{
	PRIVATE()->Button->setRect(*client);
	PRIVATE()->Button->setViewport(getViewport());

	auto painter = getPainter();
	if (painter == nullptr) painter = getContext()->getPainter();
	if (painter == nullptr) return;

	auto itemsWidget = PRIVATE()->Popup->getChildren()[2];
	itemsWidget->setFixedHeight(0);
	for (size_t i = 0; i < PRIVATE()->Items.size(); ++i)
	{
		auto text = PRIVATE()->Items[i];
		auto button = itemsWidget->getChildren()[i];
		auto textRect = painter->boundingRect(0, 0, INT_MAX, INT_MAX, text);

		button->setFixedHeight(textRect.H);
		itemsWidget->setFixedHeight(itemsWidget->getFixedHeight() + textRect.H);
		if (PRIVATE()->MaxCount == -1 || i < PRIVATE()->MaxCount) PRIVATE()->Popup->setFixedHeight(itemsWidget->getFixedHeight());
		PRIVATE()->Popup->setFixedWidth(std::max(PRIVATE()->Popup->getFixedWidth(), textRect.W));
	}
}

void RmGUICombo::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
	RmGUIWidget::paint(painter, client);
}

RmStringList const& RmGUICombo::getItems() const
{
	return PRIVATE()->Items;
}

void RmGUICombo::setItems(RmStringList const& texts)
{
	PRIVATE()->Items = texts;
	PRIVATE()->Popup->removeWidget();
	auto itemsWidget = RmNew<RmGUIVBox>();
	PRIVATE()->Popup->addWidget(itemsWidget);
	PRIVATE()->Popup->setFixedWidth(getFixedWidth() * 1.2f);
	itemsWidget->setFixedHeight(200);
	PRIVATE()->Popup->setFixedHeight(200);

	for (size_t i = 0; i < PRIVATE()->Items.size(); ++i)
	{
		auto text = PRIVATE()->Items[i];
		auto button = RmNew<RmGUIButton>();
		itemsWidget->addWidget(button);
		button->setText(text);
		button->setStyle(PRIVATE()->ItemStyle);
		button->setFixedHeight(35);

		button->hovered->connect(this, [=]() {
			PRIVATE()->OnHighlighted.emit(i);
			PRIVATE()->OnTextHighlighted.emit(text);
			});

		button->clicked->connect(this, [=](bool clicked) {
			auto oldIndex = PRIVATE()->Index;
			PRIVATE()->Index = (int32_t)i;
			PRIVATE()->Button->setText(text);
			getContext()->removeWidget(PRIVATE()->Popup);
			PRIVATE()->OnTextActivated.emit(text);
			if (PRIVATE()->Index != oldIndex)
			{
				PRIVATE()->OnCurrentIndexChanged.emit(i);
				PRIVATE()->OnCurrentTextChanged.emit(text);
			}
			});
	}
}

int32_t RmGUICombo::getMaxCount() const
{
	return PRIVATE()->MaxCount;
}

void RmGUICombo::setMaxCount(int32_t value)
{
	PRIVATE()->MaxCount = value;
	setItems(PRIVATE()->Items);
}

int32_t RmGUICombo::getCurrentIndex() const
{
	return PRIVATE()->Index;
}

void RmGUICombo::setCurrentIndex(int32_t index)
{
	PRIVATE()->Index = index;
	if (0 <= PRIVATE()->Index && PRIVATE()->Index < PRIVATE()->Items.size())
	{
		PRIVATE()->Button->setText(getCurrentText());
	}
	else
	{
		PRIVATE()->Index = -1;
		PRIVATE()->Button->setText(RmString());
	}
}

RmString RmGUICombo::getCurrentText() const
{
	if (0 <= PRIVATE()->Index && PRIVATE()->Index < PRIVATE()->Items.size())
	{
		return PRIVATE()->Items[PRIVATE()->Index];
	}
	return RmString();
}

void RmGUICombo::setCurrentText(RmString const& text)
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

RmGUIButtonStyle RmGUICombo::getStyle() const
{
	return PRIVATE()->Button->getStyle();
}

void RmGUICombo::setStyle(RmGUIButtonStyle value)
{
	PRIVATE()->Button->setStyle(value);
}

RmGUIButtonStyle RmGUICombo::getItemStyle() const
{
	return PRIVATE()->ItemStyle;
}

void RmGUICombo::setItemStyle(RmGUIButtonStyle value)
{
	PRIVATE()->ItemStyle = value;
	auto contentWidget = PRIVATE()->Popup->getChildren()[0];
	for (size_t i = 0; contentWidget && i < contentWidget->getChildren().size(); ++i)
	{
		auto itemWidget = RmCast<RmGUIButton>(contentWidget->getChildren()[i]);
		if (itemWidget == nullptr) continue;
		itemWidget->setStyle(value);
	}
}

bool RmGUICombo::filter(IRmGUIReactorRaw source, IRmGUIEventRaw event)
{
	if (source == RmCast<IRmGUIReactor>(PRIVATE()->Popup.get()))
	{
		switch (event->Type)
		{
		case RmHash("MouseDown"):
		{
			auto event2 = RmCast<IRmGUIMouseEvent>(event);
			auto client = PRIVATE()->Popup->getRect();
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
				getContext()->removeWidget(PRIVATE()->Popup);
			}

		} break;
		}
	}
	return false;
}
