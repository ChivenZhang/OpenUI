#include "UIContext.h"

class UIContextPrivateData : public UIContextPrivate
{
public:

};
#define PRIVATE() ((UIContextPrivateData*) m_Private)

UIContext::UIContext()
{
	m_Private = new UIContextPrivateData;
}

UIContext::~UIContext()
{
	delete m_Private; m_Private = nullptr;
}