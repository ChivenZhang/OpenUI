#pragma once
#include "RmGUI.h"
#include "Utility/HashUtility.h"

/// @brief Base interface of all events.
class RMGUI_API IRmGUIEvent
{
public:
	IRmGUIEvent(uint32_t type) : Type(type), Accept(false) {}

	virtual ~IRmGUIEvent() = default;

	const uint32_t Type;
	bool Accept;
};
using rmevent = RmRaw<IRmGUIEvent>;

/// @brief Base interface of all event reactors.
class RMGUI_API IRmGUIEventReactor
{
public:
	virtual ~IRmGUIEventReactor() = default;
};
using rmreactor = RmRaw<IRmGUIEventReactor>;

/// @brief Base interface of all event handlers.
class RMGUI_API IRmGUIEventHandler
{
public:
	virtual ~IRmGUIEventHandler() = default;

	virtual void handle(rmreactor source, rmevent event) = 0;
};
using rmhandler = RmRaw<IRmGUIEventHandler>;

class IRmGUIShowEvent : public IRmGUIEvent
{
public:
	IRmGUIShowEvent()
		:
		IRmGUIEvent(HashUtility::Encrypt("Show")) {}
};

class IRmGUIHideEvent : public IRmGUIEvent
{
public:
	IRmGUIHideEvent()
		:
		IRmGUIEvent(HashUtility::Encrypt("Hide")) {}
};

class IRmGUICloseEvent : public IRmGUIEvent
{
public:
	IRmGUICloseEvent()
		:
		IRmGUIEvent(HashUtility::Encrypt("Close")) {}
};

class IRmGUIFocusEvent : public IRmGUIEvent
{
public:
	IRmGUIFocusEvent(bool focus)
		:
		IRmGUIEvent(focus ? HashUtility::Encrypt("FocusIn") : HashUtility::Encrypt("FocusOut")) {}

	bool gotFocus() const { return Type == HashUtility::Encrypt("FocusIn"); }
	bool lostFocus() const { return Type == HashUtility::Encrypt("FocusOut"); }
};

class IRmGUIMoveEvent : public IRmGUIEvent
{
public:
	IRmGUIMoveEvent(int32_t x, int32_t y, int32_t oldX = 0, int32_t oldY = 0)
		:
		IRmGUIEvent(HashUtility::Encrypt("Move")),
		X(x), Y(y), OldX(oldX), OldY(oldY) {}

	const int32_t X, Y, OldX, OldY;
};

class IRmGUIResizeEvent : public IRmGUIEvent
{
public:
	IRmGUIResizeEvent(int32_t w, int32_t h, int32_t oldW = 0, int32_t oldH = 0)
		:
		IRmGUIEvent(HashUtility::Encrypt("Resize")),
		W(w), H(h), OldW(oldW), OldH(oldH) {}

	const int32_t W, H, OldW, OldH;
};

class IRmGUIMouseEvent : public IRmGUIEvent
{
protected:
	IRmGUIMouseEvent(uint32_t type, int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIEvent(type),
		X(x), Y(y), GlobalX(globalX), GlobalY(globalY), Button(button), Buttons(buttons), Modifiers(modifiers) {}

public:
	const int32_t X, Y, GlobalX, GlobalY;
	const uint32_t Button, Buttons, Modifiers;
};

class IRmGUIMouseDownEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseDownEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("MouseDown"), x, y, globalX, globalY, button, buttons, modifiers) {}
};

class IRmGUIMouseUpEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseUpEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("MouseUp"), x, y, globalX, globalY, button, buttons, modifiers) {}
};

class IRmGUIMouseDblClickEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseDblClickEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("DoubleClick"), x, y, globalX, globalY, button, buttons, modifiers) {}
};

class IRmGUIMouseMoveEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseMoveEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("MouseMove"), x, y, globalX, globalY, button, buttons, modifiers) {}
};

class IRmGUIMouseWheelEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseWheelEvent(int32_t px, int32_t py, int32_t ax, int32_t ay, int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("MouseWheel"), x, y, globalX, globalY, button, buttons, modifiers),
		PixelX(px), PixelY(py), AngleX(ax), AngleY(ay) {}

	const int32_t PixelX, PixelY, AngleX, AngleY;
};

class IRmGUIMouseEnterEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseEnterEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("MouseEnter"), x, y, globalX, globalY, button, buttons, modifiers) {}
};

class IRmGUIMouseLeaveEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseLeaveEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("MouseLeave"), x, y, globalX, globalY, button, buttons, modifiers) {}
};

class IRmGUIKeyEvent : public IRmGUIEvent
{
protected:
	IRmGUIKeyEvent(uint32_t type, uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, RmString text = RmString(), uint16_t count = 1)
		:
		IRmGUIEvent(type),
		Key(key), Modifiers(modifiers), NativeScanCode(nativeScanCode), NativeVirtualKey(nativeVirtualKey), NativeModifiers(nativeModifiers), Text(text), Count(count) {}

public:
	const uint32_t Key, Modifiers;
	const uint32_t NativeScanCode, NativeVirtualKey, NativeModifiers;
	const RmString Text;
	const uint16_t Count;
};

class IRmGUIKeyDownEvent : public IRmGUIKeyEvent
{
public:
	IRmGUIKeyDownEvent(uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, RmString text = RmString(), uint16_t count = 1)
		:
		IRmGUIKeyEvent(HashUtility::Encrypt("KeyDown"), key, modifiers, nativeScanCode, nativeVirtualKey, nativeModifiers, text, count) {}
};

class IRmGUIKeyUpEvent : public IRmGUIKeyEvent
{
public:
	IRmGUIKeyUpEvent(uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, RmString text = RmString(), uint16_t count = 1)
		:
		IRmGUIKeyEvent(HashUtility::Encrypt("KeyUp"), key, modifiers, nativeScanCode, nativeVirtualKey, nativeModifiers, text, count) {}
};

class IRmGUITextInputEvent : public IRmGUIKeyEvent
{
public:
	IRmGUITextInputEvent(uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, RmString text = RmString(), uint16_t count = 1)
		:
		IRmGUIKeyEvent(HashUtility::Encrypt("TextInput"), key, modifiers, nativeScanCode, nativeVirtualKey, nativeModifiers, text, count) {}
};

class IRmGUIDropEvent : public IRmGUIEvent
{
public:
	IRmGUIDropEvent()
		:
		IRmGUIEvent(HashUtility::Encrypt("Drop")) {}
};

class IRmGUIDragEvent : public IRmGUIEvent
{
public:
	using mime_t = struct {};
	using rmmime = RmRaw<mime_t>;

protected:
	IRmGUIDragEvent(uint32_t type, int32_t x, int32_t y, uint32_t buttons, uint32_t modifiers, rmmime data = nullptr)
		:
		IRmGUIEvent(type),
		X(x), Y(y), Buttons(buttons), Modifiers(modifiers), MimeData(data) {}

public:
	const int32_t X, Y;
	const uint32_t Buttons, Modifiers;
	const rmmime MimeData;
};

class IRmGUIDragMoveEvent : public IRmGUIDragEvent
{
public:
	IRmGUIDragMoveEvent(int32_t x, int32_t y, uint32_t buttons, uint32_t modifiers, rmmime data = nullptr)
		:
		IRmGUIDragEvent(HashUtility::Encrypt("DragMove"), x, y, buttons, modifiers, data) {}
};

class IRmGUIDragEnterEvent : public IRmGUIDragEvent
{
public:
	IRmGUIDragEnterEvent(int32_t x, int32_t y, uint32_t buttons, uint32_t modifiers, rmmime data = nullptr)
		:
		IRmGUIDragEvent(HashUtility::Encrypt("DragEnter"), x, y, buttons, modifiers, data) {}
};

class IRmGUIDragLeaveEvent : public IRmGUIEvent
{
public:
	IRmGUIDragLeaveEvent()
		:
		IRmGUIEvent(HashUtility::Encrypt("DragLeave")) {}
};