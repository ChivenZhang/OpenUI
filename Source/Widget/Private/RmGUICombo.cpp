#include "RmGUICombo.h"

class RmGUIComboPrivate : public RmGUIWidgetPrivate
{
public:

};
#define PRIVATE() ((RmGUIComboPrivate*)m_PrivateCombo)

RmGUICombo::RmGUICombo(IRmGUIWidgetRaw parent)
	:
	RmGUIWidget(parent),
	m_PrivateCombo(nullptr)
{
	m_PrivateCombo = new RmGUIComboPrivate;

}

RmGUICombo::~RmGUICombo()
{
	delete m_PrivateCombo; m_PrivateCombo = nullptr;
}
