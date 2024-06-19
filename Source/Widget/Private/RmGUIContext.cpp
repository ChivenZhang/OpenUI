#include "RmGUIContext.h"

RmRef<IRmGUIContext> IRmGUIContext::GetInstance()
{
	return RmNew<RmGUIContext>();
}

void RmGUIContext::sendEvent(rmreactor source, rmevent event)
{
	switch (event->Type)
	{
	case HashUtility::Encrypt("Show"):
	{
		printf("show\n");
	} break;
	case HashUtility::Encrypt("Hide"):
	{
		printf("hide\n");
	} break;
	case HashUtility::Encrypt("Close"):
	{
		printf("close\n");
	} break;
	case HashUtility::Encrypt("Move"):
	{
		printf("move\n");
	} break;
	case HashUtility::Encrypt("Resize"):
	{
		printf("resize\n");
	} break;
	case HashUtility::Encrypt("MouseDown"):
	{
		printf("mouse down\n");
	} break;
	case HashUtility::Encrypt("MouseUp"):
	{
		printf("mouse up\n");
	} break;
	case HashUtility::Encrypt("DoubleClick"):
	{
		printf("mouse dbl\n");
	} break;
	case HashUtility::Encrypt("MouseMove"):
	{
		// printf("mouse move\n");
	} break;
	case HashUtility::Encrypt("MouseWheel"):
	{
		printf("mouse wheel\n");
	} break;
	case HashUtility::Encrypt("MouseEnter"):
	{
		printf("mouse enter\n");
	} break;
	case HashUtility::Encrypt("MouseLeave"):
	{
		printf("mouse leave\n");
	} break;
	case HashUtility::Encrypt("KeyDown"):
	{
		printf("key down\n");
	} break;
	case HashUtility::Encrypt("KeyUp"):
	{
		printf("key up\n");
	} break;
	case HashUtility::Encrypt("TextInput"):
	{
		auto real = (IRmGUITextInputEvent*)event;
		printf("input %s\n", real->Text.c_str());
	} break;
	case HashUtility::Encrypt("Drop"):
	{
		printf("drop\n");
	} break;
	case HashUtility::Encrypt("DragMove"):
	{
		printf("drag move\n");
	} break;
	case HashUtility::Encrypt("DragEnter"):
	{
		printf("drag enter\n");
	} break;
	case HashUtility::Encrypt("DragLeave"):
	{
		printf("drag leave\n");
	} break;
	case HashUtility::Encrypt("FocusIn"):
	{
		printf("focus in\n");
	} break;
	case HashUtility::Encrypt("FocusOut"):
	{
		printf("focus out\n");
	} break;
	}
}

void RmGUIContext::postEvent(rmreactor source, rmevent event)
{
}
