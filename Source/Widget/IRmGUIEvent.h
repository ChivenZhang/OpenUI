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

	virtual bool filter(rmreactor source, rmevent event) = 0;

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
using rmevent_show = RmRaw<IRmGUIShowEvent>;

class IRmGUIHideEvent : public IRmGUIEvent
{
public:
	IRmGUIHideEvent()
		:
		IRmGUIEvent(HashUtility::Encrypt("Hide")) {}
};
using rmevent_hide = RmRaw<IRmGUIHideEvent>;

class IRmGUICloseEvent : public IRmGUIEvent
{
public:
	IRmGUICloseEvent()
		:
		IRmGUIEvent(HashUtility::Encrypt("Close")) {}
};
using rmevent_close = RmRaw<IRmGUICloseEvent>;

class IRmGUIFocusEvent : public IRmGUIEvent
{
public:
	IRmGUIFocusEvent(bool focus)
		:
		IRmGUIEvent(focus ? HashUtility::Encrypt("FocusIn") : HashUtility::Encrypt("FocusOut")) {}

	bool gotFocus() const { return Type == HashUtility::Encrypt("FocusIn"); }
	bool lostFocus() const { return Type == HashUtility::Encrypt("FocusOut"); }
};
using rmevent_focus = RmRaw<IRmGUIFocusEvent>;

class IRmGUIMoveEvent : public IRmGUIEvent
{
public:
	IRmGUIMoveEvent(int32_t x, int32_t y, int32_t oldX = 0, int32_t oldY = 0)
		:
		IRmGUIEvent(HashUtility::Encrypt("Move")),
		X(x), Y(y), OldX(oldX), OldY(oldY) {}

	const int32_t X, Y, OldX, OldY;
};
using rmevent_move = RmRaw<IRmGUIMoveEvent>;

class IRmGUIResizeEvent : public IRmGUIEvent
{
public:
	IRmGUIResizeEvent(int32_t w, int32_t h, int32_t oldW = 0, int32_t oldH = 0)
		:
		IRmGUIEvent(HashUtility::Encrypt("Resize")),
		W(w), H(h), OldW(oldW), OldH(oldH) {}

	const int32_t W, H, OldW, OldH;
};
using rmevent_resize = RmRaw<IRmGUIResizeEvent>;

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
using rmevent_mouse = RmRaw<IRmGUIMouseEvent>;

class IRmGUIMouseDownEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseDownEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("MouseDown"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using rmevent_ms_down = RmRaw<IRmGUIMouseDownEvent>;

class IRmGUIMouseUpEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseUpEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("MouseUp"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using rmevent_ms_up = RmRaw<IRmGUIMouseUpEvent>;

class IRmGUIMouseDblClickEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseDblClickEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("DoubleClick"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using rmevent_double = RmRaw<IRmGUIMouseDblClickEvent>;

class IRmGUIMouseMoveEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseMoveEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("MouseMove"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using rmevent_ms_move = RmRaw<IRmGUIMouseMoveEvent>;

class IRmGUIMouseWheelEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseWheelEvent(int32_t px, int32_t py, int32_t ax, int32_t ay, int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("MouseWheel"), x, y, globalX, globalY, button, buttons, modifiers),
		PixelX(px), PixelY(py), AngleX(ax), AngleY(ay) {}

	const int32_t PixelX, PixelY, AngleX, AngleY;
};
using rmevent_wheel = RmRaw<IRmGUIMouseWheelEvent>;

class IRmGUIMouseEnterEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseEnterEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("MouseEnter"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using rmevent_enter = RmRaw<IRmGUIMouseEnterEvent>;

class IRmGUIMouseLeaveEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseLeaveEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("MouseLeave"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using rmevent_leave = RmRaw<IRmGUIMouseLeaveEvent>;

class IRmGUIMouseTabletEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseTabletEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers, float pressure, float rotation, float tangentialPressure, float xTile, float yTile, float z)
		:
		IRmGUIMouseEvent(HashUtility::Encrypt("Tablet"), x, y, globalX, globalY, button, buttons, modifiers),
		Pressure(pressure), Rotation(rotation), TangentialPressure(tangentialPressure), XTile(xTile), YTile(yTile), Z(z) {}

	const float Pressure, Rotation, TangentialPressure, XTile, YTile, Z;
};
using rmevent_tablet = RmRaw<IRmGUIMouseTabletEvent>;

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
using rmevent_key = RmRaw<IRmGUIKeyEvent>;

class IRmGUIKeyDownEvent : public IRmGUIKeyEvent
{
public:
	IRmGUIKeyDownEvent(uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, RmString text = RmString(), uint16_t count = 1)
		:
		IRmGUIKeyEvent(HashUtility::Encrypt("KeyDown"), key, modifiers, nativeScanCode, nativeVirtualKey, nativeModifiers, text, count) {}
};
using rmevent_key_down = RmRaw<IRmGUIKeyDownEvent>;

class IRmGUIKeyUpEvent : public IRmGUIKeyEvent
{
public:
	IRmGUIKeyUpEvent(uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, RmString text = RmString(), uint16_t count = 1)
		:
		IRmGUIKeyEvent(HashUtility::Encrypt("KeyUp"), key, modifiers, nativeScanCode, nativeVirtualKey, nativeModifiers, text, count) {}
};
using rmevent_key_up = RmRaw<IRmGUIKeyUpEvent>;

class IRmGUIKeyInputEvent : public IRmGUIKeyEvent
{
public:
	IRmGUIKeyInputEvent(uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, RmString text = RmString(), uint16_t count = 1)
		:
		IRmGUIKeyEvent(HashUtility::Encrypt("KeyInput"), key, modifiers, nativeScanCode, nativeVirtualKey, nativeModifiers, text, count) {}
};
using rmevent_input = RmRaw<IRmGUIKeyInputEvent>;

class IRmGUIDropEvent : public IRmGUIEvent
{
public:
	IRmGUIDropEvent()
		:
		IRmGUIEvent(HashUtility::Encrypt("Drop")) {}
};
using rmevent_drop = RmRaw<IRmGUIDropEvent>;

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
using rmevent_drag = RmRaw<IRmGUIDragEvent>;

class IRmGUIDragMoveEvent : public IRmGUIDragEvent
{
public:
	IRmGUIDragMoveEvent(int32_t x, int32_t y, uint32_t buttons, uint32_t modifiers, rmmime data = nullptr)
		:
		IRmGUIDragEvent(HashUtility::Encrypt("DragMove"), x, y, buttons, modifiers, data) {}
};
using rmevent_drag_move = RmRaw<IRmGUIDragMoveEvent>;

class IRmGUIDragEnterEvent : public IRmGUIDragEvent
{
public:
	IRmGUIDragEnterEvent(int32_t x, int32_t y, uint32_t buttons, uint32_t modifiers, rmmime data = nullptr)
		:
		IRmGUIDragEvent(HashUtility::Encrypt("DragEnter"), x, y, buttons, modifiers, data) {}
};
using rmevent_drag_enter = RmRaw<IRmGUIDragEnterEvent>;

class IRmGUIDragLeaveEvent : public IRmGUIEvent
{
public:
	IRmGUIDragLeaveEvent()
		:
		IRmGUIEvent(HashUtility::Encrypt("DragLeave")) {}
};
using rmevent_drag_leave = RmRaw<IRmGUIDragLeaveEvent>;