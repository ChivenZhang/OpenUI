#include "RmGUIGrid.h"
#include <taitank.h>
namespace flex = taitank;

class RmGUIGridPrivate : public RmGUIWidgetPrivate
{
public:

};
#define PRIVATE() ((RmGUIGridPrivate*) m_PrivateGrid)

RmGUIGrid::RmGUIGrid(IRmGUIWidgetRaw parent)
	:
	RmGUILayout(parent),
	m_PrivateGrid(nullptr)
{
	m_PrivateGrid = new RmGUIGridPrivate;
}

RmGUIGrid::~RmGUIGrid()
{
	delete m_PrivateGrid; m_PrivateGrid = nullptr;
}
