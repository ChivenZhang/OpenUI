#pragma once
#include "UI.h"

/// @brief Base interface of event.
class OPENUI_API UIEvent
{
public:
	virtual ~UIEvent() = default;

protected:
	UIEvent(uint32_t type) : Type(type), Accept(false) {}

public:
	const uint32_t Type;
	bool Accept;
};
using UIEventRef = UIRef<UIEvent>;
using UIEventRaw = UIRaw<UIEvent>;

/// @brief Base interface of event reactor.
class OPENUI_API UIReactor
{
public:
	virtual ~UIReactor() = default;
};
using UIReactorRef = UIRef<UIReactor>;
using UIReactorRaw = UIRaw<UIReactor>;

/// @brief Base interface of event filter.
class OPENUI_API UIFilter
{
public:
	virtual ~UIFilter() = default;
	virtual bool filter(UIReactorRaw source, UIEventRaw event) = 0;
};
using UIFilterRef = UIRef<UIFilter>;
using UIFilterRaw = UIRaw<UIFilter>;

/// @brief Base interface of event handler.
class OPENUI_API UIHandler
{
public:
	virtual ~UIHandler() = default;
	virtual void handle(UIReactorRaw source, UIEventRaw event) = 0;
};
using UIHandlerRef = UIRef<UIHandler>;
using UIHandlerRaw = UIRaw<UIHandler>;

class UIShowEvent : public UIEvent
{
public:
	UIShowEvent()
		:
		UIEvent(UIHash("Show")) {}
};
using UIShowEventRaw = UIRaw<UIShowEvent>;

class UIHideEvent : public UIEvent
{
public:
	UIHideEvent()
		:
		UIEvent(UIHash("Hide")) {}
};
using UIHideEventRaw = UIRaw<UIHideEvent>;

class UICloseEvent : public UIEvent
{
public:
	UICloseEvent()
		:
		UIEvent(UIHash("Close")) {}
};
using UICloseEventRaw = UIRaw<UICloseEvent>;

class UIFocusEvent : public UIEvent
{
public:
	UIFocusEvent(bool focus)
		:
		UIEvent(focus ? UIHash("FocusIn") : UIHash("FocusOut")) {}

	bool gotFocus() const { return Type == UIHash("FocusIn"); }
	bool lostFocus() const { return Type == UIHash("FocusOut"); }
};
using UIFocusEventRaw = UIRaw<UIFocusEvent>;

class UIMoveEvent : public UIEvent
{
public:
	UIMoveEvent(int32_t x, int32_t y, int32_t oldX = 0, int32_t oldY = 0)
		:
		UIEvent(UIHash("Move")),
		X(x), Y(y), OldX(oldX), OldY(oldY) {}

	const int32_t X, Y, OldX, OldY;
};
using UIMoveEventRaw = UIRaw<UIMoveEvent>;

class UIResizeEvent : public UIEvent
{
public:
	UIResizeEvent(int32_t w, int32_t h, int32_t oldW = 0, int32_t oldH = 0)
		:
		UIEvent(UIHash("Resize")),
		W(w), H(h), OldW(oldW), OldH(oldH) {}

	const int32_t W, H, OldW, OldH;
};
using UIResizeEventRaw = UIRaw<UIResizeEvent>;

class UIMouseEvent : public UIEvent
{
protected:
	UIMouseEvent(uint32_t type, int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers, uint32_t clicks = 0)
		:
		UIEvent(type),
		X(x), Y(y), GlobalX(globalX), GlobalY(globalY), Button(button), Buttons(buttons), Modifiers(modifiers), Clicks(clicks) {}

public:
	const int32_t X, Y, GlobalX, GlobalY;
	const uint32_t Button, Buttons, Modifiers, Clicks;
};
using UIMouseEventRaw = UIRaw<UIMouseEvent>;

class UIMouseDownEvent : public UIMouseEvent
{
public:
	UIMouseDownEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers, uint32_t clicks)
		:
		UIMouseEvent(UIHash("MouseDown"), x, y, globalX, globalY, button, buttons, modifiers, clicks) {}
};
using UIMouseDownEventRaw = UIRaw<UIMouseDownEvent>;

class UIMouseUpEvent : public UIMouseEvent
{
public:
	UIMouseUpEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		UIMouseEvent(UIHash("MouseUp"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using UIMouseUpEventRaw = UIRaw<UIMouseUpEvent>;

class UIMouseDblClickEvent : public UIMouseEvent
{
public:
	UIMouseDblClickEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers, uint32_t clicks)
		:
		UIMouseEvent(UIHash("DoubleClick"), x, y, globalX, globalY, button, buttons, modifiers, clicks) {}
};
using UIMouseDblClickEventRaw = UIRaw<UIMouseDblClickEvent>;

class UIMouseMoveEvent : public UIMouseEvent
{
public:
	UIMouseMoveEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		UIMouseEvent(UIHash("MouseMove"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using UIMouseMoveEventRaw = UIRaw<UIMouseMoveEvent>;

class UIMouseWheelEvent : public UIMouseEvent
{
public:
	UIMouseWheelEvent(int32_t px, int32_t py, int32_t ax, int32_t ay, int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		UIMouseEvent(UIHash("MouseWheel"), x, y, globalX, globalY, button, buttons, modifiers),
		PixelX(px), PixelY(py), AngleX(ax), AngleY(ay) {}

	const int32_t PixelX, PixelY, AngleX, AngleY;
};
using UIMouseWheelEventRaw = UIRaw<UIMouseWheelEvent>;

class UIMouseEnterEvent : public UIMouseEvent
{
public:
	UIMouseEnterEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		UIMouseEvent(UIHash("MouseEnter"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using UIMouseEnterEventRaw = UIRaw<UIMouseEnterEvent>;

class UIMouseLeaveEvent : public UIMouseEvent
{
public:
	UIMouseLeaveEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers)
		:
		UIMouseEvent(UIHash("MouseLeave"), x, y, globalX, globalY, button, buttons, modifiers) {}
};
using UIMouseLeaveEventRaw = UIRaw<UIMouseLeaveEvent>;

class UIMouseTabletEvent : public UIMouseEvent
{
public:
	UIMouseTabletEvent(int32_t x, int32_t y, int32_t globalX, int32_t globalY, uint32_t button, uint32_t buttons, uint32_t modifiers, float pressure, float rotation, float tangentialPressure, float xTile, float yTile, float z)
		:
		UIMouseEvent(UIHash("Tablet"), x, y, globalX, globalY, button, buttons, modifiers),
		Pressure(pressure), Rotation(rotation), TangentialPressure(tangentialPressure), XTile(xTile), YTile(yTile), Z(z) {}

	const float Pressure, Rotation, TangentialPressure, XTile, YTile, Z;
};
using UIMouseTabletEventRaw = UIRaw<UIMouseTabletEvent>;

class UIKeyEvent : public UIEvent
{
protected:
	UIKeyEvent(uint32_t type, uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, UIString text = UIString(), uint16_t count = 1)
		:
		UIEvent(type),
		Key(key), Modifiers(modifiers), NativeScanCode(nativeScanCode), NativeVirtualKey(nativeVirtualKey), NativeModifiers(nativeModifiers), Text(text), Count(count) {}

public:
	const uint32_t Key, Modifiers;
	const uint32_t NativeScanCode, NativeVirtualKey, NativeModifiers;
	const UIString Text;
	const uint16_t Count;
};
using UIKeyEventRaw = UIRaw<UIKeyEvent>;

class UIKeyDownEvent : public UIKeyEvent
{
public:
	UIKeyDownEvent(uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, UIString text = UIString(), uint16_t count = 1)
		:
		UIKeyEvent(UIHash("KeyDown"), key, modifiers, nativeScanCode, nativeVirtualKey, nativeModifiers, text, count) {}
};
using UIKeyDownEventRaw = UIRaw<UIKeyDownEvent>;

class UIKeyUpEvent : public UIKeyEvent
{
public:
	UIKeyUpEvent(uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, UIString text = UIString(), uint16_t count = 1)
		:
		UIKeyEvent(UIHash("KeyUp"), key, modifiers, nativeScanCode, nativeVirtualKey, nativeModifiers, text, count) {}
};
using UIKeyUpEventRaw = UIRaw<UIKeyUpEvent>;

class UITextInputEvent : public UIKeyEvent
{
public:
	UITextInputEvent(uint32_t key, uint32_t modifiers, uint32_t nativeScanCode, uint32_t nativeVirtualKey, uint32_t nativeModifiers, UIString text = UIString(), uint16_t count = 1, bool done = false, uint32_t start = 0, uint32_t length = 0)
		:
		UIKeyEvent(UIHash("TextInput"), key, modifiers, nativeScanCode, nativeVirtualKey, nativeModifiers, text, count), Done(done), Start(start), Length(length) {}

	const bool Done;
	const uint32_t Start, Length;
};
using UITextInputEventRaw = UIRaw<UITextInputEvent>;

class UIDropEvent : public UIEvent
{
public:
	UIDropEvent()
		:
		UIEvent(UIHash("Drop")) {}
};
using UIDropEventRaw = UIRaw<UIDropEvent>;

class UIDragEvent : public UIEvent
{
public:
	using mime_t = struct {};
	using rmmime = UIRaw<mime_t>;

protected:
	UIDragEvent(uint32_t type, int32_t x, int32_t y, uint32_t buttons, uint32_t modifiers, rmmime data = nullptr)
		:
		UIEvent(type),
		X(x), Y(y), Buttons(buttons), Modifiers(modifiers), MimeData(data) {}

public:
	const int32_t X, Y;
	const uint32_t Buttons, Modifiers;
	const rmmime MimeData;
};
using UIDragEventRaw = UIRaw<UIDragEvent>;

class UIDragMoveEvent : public UIDragEvent
{
public:
	UIDragMoveEvent(int32_t x, int32_t y, uint32_t buttons, uint32_t modifiers, rmmime data = nullptr)
		:
		UIDragEvent(UIHash("DragMove"), x, y, buttons, modifiers, data) {}
};
using UIDragMoveEventRaw = UIRaw<UIDragMoveEvent>;

class UIDragEnterEvent : public UIDragEvent
{
public:
	UIDragEnterEvent(int32_t x, int32_t y, uint32_t buttons, uint32_t modifiers, rmmime data = nullptr)
		:
		UIDragEvent(UIHash("DragEnter"), x, y, buttons, modifiers, data) {}
};
using UIDragEnterEventRaw = UIRaw<UIDragEnterEvent>;

class UIDragLeaveEvent : public UIEvent
{
public:
	UIDragLeaveEvent()
		:
		UIEvent(UIHash("DragLeave")) {}
};
using UIDragLeaveEventRaw = UIRaw<UIDragLeaveEvent>;