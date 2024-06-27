#include "RmGUICombo.h"
#include "RmGUIButton.h"
#include "RmGUIScroll.h"
#include "RmGUIVBox.h"
#include "RmGUIContext.h"

class RmGUIComboPrivate : public RmGUIWidgetPrivate
{
public:
	int32_t Index = -1;
	RmStringList Items;
	RmGUIButtonRef Button;
	RmGUIWidgetRef Popup;

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
	PRIVATE()->Button = RmNew<RmGUIButton>();
	addWidget(PRIVATE()->Button);
	PRIVATE()->Popup = RmNew<RmGUIScroll>();
	PRIVATE()->Popup->setFixedSize(200, 100);
	PRIVATE()->Popup->setEventFilter(this);

	PRIVATE()->Button->clicked->connect(this, [=](bool checked) {
		auto posX = PRIVATE()->Button->getPositionX();
		auto posY = PRIVATE()->Button->getPositionY() + PRIVATE()->Button->getHeight();
		PRIVATE()->Popup->setPosition(posX, posY);
		getContext()->addWidget(PRIVATE()->Popup, 1);
		});
}

RmGUICombo::~RmGUICombo()
{
	delete m_PrivateCombo; m_PrivateCombo = nullptr;
}

void RmGUICombo::layout(RmRectRaw client)
{
	PRIVATE()->Button->setRect(*client);
	PRIVATE()->Button->setViewport(getViewport());
}

void RmGUICombo::paint(IRmGUIPainterRaw painter, RmRectRaw client)
{
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
	itemsWidget->setFixedSize(RmNAN, 500);
	PRIVATE()->Popup->addWidget(itemsWidget);

	for (size_t i = 0; i < PRIVATE()->Items.size(); ++i)
	{
		auto text = PRIVATE()->Items[i];
		auto button = RmNew<RmGUIButton>();
		itemsWidget->addWidget(button);
		button->setText(text);
		auto textRect = getContext()->getSurface()->getPainter()->boundingRect(0, 0, INT_MAX, INT_MAX, text);
		button->setFixedHeight(textRect.H);
	}
}

int32_t RmGUICombo::getCurrentIndex() const
{
	return PRIVATE()->Index;
}

void RmGUICombo::setCurrentIndex(int32_t index)
{
	PRIVATE()->Index = index;
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
			PRIVATE()->Index = (int32_t)i;
			break;
		}
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
