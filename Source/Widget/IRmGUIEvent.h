#pragma once
#include "RmGUI.h"

/// @brief Base interface of all events.
class RMGUI_API IRmGUIEvent
{
protected:
	IRmGUIEvent(uint32_t type) : Type(type), Accept(false) {}
	virtual ~IRmGUIEvent() = default;

public:
	const uint32_t Type;
	bool Accept;
};
using IRmGUIEventRef = RmRef<IRmGUIEvent>;
using IRmGUIEventRaw = RmRaw<IRmGUIEvent>;

/// @brief Base interface of all event reactors.
class RMGUI_API IRmGUIReactor
{
public:
	virtual ~IRmGUIReactor() = default;
};
using IRmGUIReactorRef = RmRef<IRmGUIReactor>;
using IRmGUIReactorRaw = RmRaw<IRmGUIReactor>;

/// @brief Base interface of all event handlers.
class RMGUI_API IRmGUIHandler
{
public:
	virtual ~IRmGUIHandler() = default;

	virtual bool filter(IRmGUIReactorRaw source, IRmGUIEventRaw event) = 0;

	virtual void handle(IRmGUIReactorRaw source, IRmGUIEventRaw event) = 0;
};
using IRmGUIHandlerRef = RmRef<IRmGUIHandler>;
using IRmGUIHandlerRaw = RmRaw<IRmGUIHandler>;

class IRmGUIShowEvent : public IRmGUIEvent
{
public:
	IRmGUIShowEvent()
		:
		IRmGUIEvent(RmHash("Show")) {}
};
using IRmGUIShowEventRaw = RmRaw<IRmGUIShowEvent>;

class IRmGUIHideEvent : public IRmGUIEvent
{
public:
	IRmGUIHideEvent()
		:
		IRmGUIEvent(RmHash("Hide")) {}
};
using IRmGUIHideEventRaw = RmRaw<IRmGUIHideEvent>;

class IRmGUICloseEvent : public IRmGUIEvent
{
public:
	IRmGUICloseEvent()
		:
		IRmGUIEvent(RmHash("Close")) {}
};
using IRmGUICloseEventRaw = RmRaw<IRmGUICloseEvent>;

class IRmGUIFocusEvent : public IRmGUIEvent
{
public:
	IRmGUIFocusEvent(bool focus)
		:
		IRmGUIEvent(focus ? RmHash("FocusIn") : RmHash("FocusOut")) {}

	bool gotFocus() const { return Type == RmHash("FocusIn"); }
	bool lostFocus() const { return Type == RmHash("FocusOut"); }
};
using IRmGUIFocusEventRaw = RmRaw<IRmGUIFocusEvent>;

class IRmGUIMoveEvent : public IRmGUIEvent
{
public:
	IRmGUIMoveEvent(int32_t x, int32_t y, int32_t oldX = 0, int32_t oldY = 0)
		:
		IRmGUIEvent(RmHash("Move")),
		X(x), Y(y), OldX(oldX), OldY(oldY) {}

	const int32_t X, Y, OldX, OldY;
};
using IRmGUIMoveEventRaw = RmRaw<IRmGUIMoveEvent>;

class IRmGUIResizeEvent : public IRmGUIEvent
{
public:
	IRmGUIResizeEvent(int32_t w, int32_t h, int32_t oldW = 0, int32_t oldH = 0)
		:
		IRmGUIEvent(RmHash("Resize")),
		W(w), H(h), OldW(oldW), OldH(oldH) {}

	const int32_t W, H, OldW, OldH;
};
using IRmGUIResizeEventRaw = RmRaw<IRmGUIResizeEvent>;

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
using IRmGUIMouseEventRaw = RmRaw<IRmGUIMouseEvent>;

class IRmGUIMouseDownEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseDownEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(RmHash("MouseDown"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using IRmGUIMouseDownEventRaw = RmRaw<IRmGUIMouseDownEvent>;

class IRmGUIMouseUpEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseUpEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(RmHash("MouseUp"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using IRmGUIMouseUpEventRaw = RmRaw<IRmGUIMouseUpEvent>;

class IRmGUIMouseDblClickEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseDblClickEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(RmHash("DoubleClick"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using IRmGUIMouseDblClickEventRaw = RmRaw<IRmGUIMouseDblClickEvent>;

class IRmGUIMouseMoveEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseMoveEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(RmHash("MouseMove"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using IRmGUIMouseMoveEventRaw = RmRaw<IRmGUIMouseMoveEvent>;

class IRmGUIMouseWheelEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseWheelEvent(int32_t px, int32_t py, int32_t ax, int32_t ay, int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(RmHash("MouseWheel"), x, y, globalX, globalY, button, buttons, modifiers),
		PixelX(px), PixelY(py), AngleX(ax), AngleY(ay) {}

	const int32_t PixelX, PixelY, AngleX, AngleY;
};
using IRmGUIMouseWheelEventRaw = RmRaw<IRmGUIMouseWheelEvent>;

class IRmGUIMouseEnterEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseEnterEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(RmHash("MouseEnter"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using IRmGUIMouseEnterEventRaw = RmRaw<IRmGUIMouseEnterEvent>;

class IRmGUIMouseLeaveEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseLeaveEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		IRmGUIMouseEvent(RmHash("MouseLeave"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using IRmGUIMouseLeaveEventRaw = RmRaw<IRmGUIMouseLeaveEvent>;

class IRmGUIMouseTabletEvent : public IRmGUIMouseEvent
{
public:
	IRmGUIMouseTabletEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers, float pressure, float rotation, float tangentialPressure, float xTile, float yTile, float z)
		:
		IRmGUIMouseEvent(RmHash("Tablet"), x, y, globalX, globalY, button, buttons, modifiers),
		Pressure(pressure), Rotation(rotation), TangentialPressure(tangentialPressure), XTile(xTile), YTile(yTile), Z(z) {}

	const float Pressure, Rotation, TangentialPressure, XTile, YTile, Z;
};
using IRmGUIMouseTabletEventRaw = RmRaw<IRmGUIMouseTabletEvent>;

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
using IRmGUIKeyEventRaw = RmRaw<IRmGUIKeyEvent>;

class IRmGUIKeyDownEvent : public IRmGUIKeyEvent
{
public:
	IRmGUIKeyDownEvent(uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, RmString text = RmString(), uint16_t count = 1)
		:
		IRmGUIKeyEvent(RmHash("KeyDown"), key, modifiers, nativeScanCode, nativeVirtualKey, nativeModifiers, text, count) {}
};
using IRmGUIKeyDownEventRaw = RmRaw<IRmGUIKeyDownEvent>;

class IRmGUIKeyUpEvent : public IRmGUIKeyEvent
{
public:
	IRmGUIKeyUpEvent(uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, RmString text = RmString(), uint16_t count = 1)
		:
		IRmGUIKeyEvent(RmHash("KeyUp"), key, modifiers, nativeScanCode, nativeVirtualKey, nativeModifiers, text, count) {}
};
using IRmGUIKeyUpEventRaw = RmRaw<IRmGUIKeyUpEvent>;

class IRmGUIKeyInputEvent : public IRmGUIKeyEvent
{
public:
	IRmGUIKeyInputEvent(uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, RmString text = RmString(), uint16_t count = 1)
		:
		IRmGUIKeyEvent(RmHash("KeyInput"), key, modifiers, nativeScanCode, nativeVirtualKey, nativeModifiers, text, count) {}
};
using IRmGUIKeyInputEventRaw = RmRaw<IRmGUIKeyInputEvent>;

class IRmGUIDropEvent : public IRmGUIEvent
{
public:
	IRmGUIDropEvent()
		:
		IRmGUIEvent(RmHash("Drop")) {}
};
using IRmGUIDropEventRaw = RmRaw<IRmGUIDropEvent>;

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
using IRmGUIDragEventRaw = RmRaw<IRmGUIDragEvent>;

class IRmGUIDragMoveEvent : public IRmGUIDragEvent
{
public:
	IRmGUIDragMoveEvent(int32_t x, int32_t y, uint32_t buttons, uint32_t modifiers, rmmime data = nullptr)
		:
		IRmGUIDragEvent(RmHash("DragMove"), x, y, buttons, modifiers, data) {}
};
using IRmGUIDragMoveEventRaw = RmRaw<IRmGUIDragMoveEvent>;

class IRmGUIDragEnterEvent : public IRmGUIDragEvent
{
public:
	IRmGUIDragEnterEvent(int32_t x, int32_t y, uint32_t buttons, uint32_t modifiers, rmmime data = nullptr)
		:
		IRmGUIDragEvent(RmHash("DragEnter"), x, y, buttons, modifiers, data) {}
};
using IRmGUIDragEnterEventRaw = RmRaw<IRmGUIDragEnterEvent>;

class IRmGUIDragLeaveEvent : public IRmGUIEvent
{
public:
	IRmGUIDragLeaveEvent()
		:
		IRmGUIEvent(RmHash("DragLeave")) {}
};
using IRmGUIDragLeaveEventRaw = RmRaw<IRmGUIDragLeaveEvent>;