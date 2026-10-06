#include "SDL3InputEnum.h"
#include <SDL3/SDL.h>

UIInputEnum SDL3InputEnum::GetMouseEnum(int key)
{
	switch (key)
	{
	case SDL_BUTTON_LEFT: return UIInputEnum::MOUSE_BUTTON_LEFT;
	case SDL_BUTTON_RIGHT: return UIInputEnum::MOUSE_BUTTON_RIGHT;
	case SDL_BUTTON_MIDDLE: return UIInputEnum::MOUSE_BUTTON_MIDDLE;
	case SDL_BUTTON_X1: return UIInputEnum::MOUSE_BUTTON_1;
	case SDL_BUTTON_X2: return UIInputEnum::MOUSE_BUTTON_2;
		// case SDL_BUTTON_3: return UIInputEnum::MOUSE_BUTTON_3;
		// case SDL_BUTTON_4: return UIInputEnum::MOUSE_BUTTON_4;
		// case SDL_BUTTON_5: return UIInputEnum::MOUSE_BUTTON_5;
		// case SDL_BUTTON_6: return UIInputEnum::MOUSE_BUTTON_6;
		// case SDL_BUTTON_7: return UIInputEnum::MOUSE_BUTTON_7;
		// case SDL_BUTTON_8: return UIInputEnum::MOUSE_BUTTON_8;
		// case SDL_BUTTON_9: return UIInputEnum::MOUSE_BUTTON_9;
		// case SDL_BUTTON_10: return UIInputEnum::MOUSE_BUTTON_10;
		// case SDL_BUTTON_11: return UIInputEnum::MOUSE_BUTTON_11;
		// case SDL_BUTTON_12: return UIInputEnum::MOUSE_BUTTON_12;
		// case SDL_BUTTON_13: return UIInputEnum::MOUSE_BUTTON_13;
		// case SDL_BUTTON_14: return UIInputEnum::MOUSE_BUTTON_14;
		// case SDL_BUTTON_15: return UIInputEnum::MOUSE_BUTTON_15;
		// case SDL_BUTTON_16: return UIInputEnum::MOUSE_BUTTON_16;
		// case SDL_BUTTON_17: return UIInputEnum::MOUSE_BUTTON_17;
		// case SDL_BUTTON_18: return UIInputEnum::MOUSE_BUTTON_18;
		// case SDL_BUTTON_19: return UIInputEnum::MOUSE_BUTTON_19;
		// case SDL_BUTTON_20: return UIInputEnum::MOUSE_BUTTON_20;
		// case SDL_BUTTON_21: return UIInputEnum::MOUSE_BUTTON_21;
		// case SDL_BUTTON_22: return UIInputEnum::MOUSE_BUTTON_22;
		// case SDL_BUTTON_23: return UIInputEnum::MOUSE_BUTTON_23;
		// case SDL_BUTTON_24: return UIInputEnum::MOUSE_BUTTON_24;
		// case SDL_WHEEL_X: return UIInputEnum::MOUSE_WHEEL_X;
		// case SDL_WHEEL_Y: return UIInputEnum::MOUSE_WHEEL_Y;
	default: return UIInputEnum::NONE;
	}
}

UIInputEnum SDL3InputEnum::GetKeyboardEnum(int key)
{
	switch (key)
	{
	case UIInputEnum::NONE: return UIInputEnum::NONE;
	case SDLK_BACKSPACE: return UIInputEnum::KEY_BACKSPACE;
	case SDLK_TAB: return UIInputEnum::KEY_TAB;
	case SDLK_RETURN: return UIInputEnum::KEY_ENTER;
	case SDLK_ESCAPE: return UIInputEnum::KEY_ESCAPE;
	case SDLK_SPACE: return UIInputEnum::KEY_SPACE;
	case SDLK_DBLAPOSTROPHE: return UIInputEnum::KEY_QUOTE;
	case SDLK_COMMA: return UIInputEnum::KEY_COMMA;
	case SDLK_MINUS: return UIInputEnum::KEY_MINUS;
	case SDLK_PERIOD: return UIInputEnum::KEY_PERIOD;
	case SDLK_SLASH: return UIInputEnum::KEY_SLASH;
	case SDLK_0: return UIInputEnum::KEY_0;
	case SDLK_1: return UIInputEnum::KEY_1;
	case SDLK_2: return UIInputEnum::KEY_2;
	case SDLK_3: return UIInputEnum::KEY_3;
	case SDLK_4: return UIInputEnum::KEY_4;
	case SDLK_5: return UIInputEnum::KEY_5;
	case SDLK_6: return UIInputEnum::KEY_6;
	case SDLK_7: return UIInputEnum::KEY_7;
	case SDLK_8: return UIInputEnum::KEY_8;
	case SDLK_9: return UIInputEnum::KEY_9;
	case SDLK_SEMICOLON: return UIInputEnum::KEY_SEMICOLON;
	case SDLK_EQUALS: return UIInputEnum::KEY_EQUAL;
	case SDLK_A: return UIInputEnum::KEY_A;
	case SDLK_B: return UIInputEnum::KEY_B;
	case SDLK_C: return UIInputEnum::KEY_C;
	case SDLK_D: return UIInputEnum::KEY_D;
	case SDLK_E: return UIInputEnum::KEY_E;
	case SDLK_F: return UIInputEnum::KEY_F;
	case SDLK_G: return UIInputEnum::KEY_G;
	case SDLK_H: return UIInputEnum::KEY_H;
	case SDLK_I: return UIInputEnum::KEY_I;
	case SDLK_J: return UIInputEnum::KEY_J;
	case SDLK_K: return UIInputEnum::KEY_K;
	case SDLK_L: return UIInputEnum::KEY_L;
	case SDLK_M: return UIInputEnum::KEY_M;
	case SDLK_N: return UIInputEnum::KEY_N;
	case SDLK_O: return UIInputEnum::KEY_O;
	case SDLK_P: return UIInputEnum::KEY_P;
	case SDLK_Q: return UIInputEnum::KEY_Q;
	case SDLK_R: return UIInputEnum::KEY_R;
	case SDLK_S: return UIInputEnum::KEY_S;
	case SDLK_T: return UIInputEnum::KEY_T;
	case SDLK_U: return UIInputEnum::KEY_U;
	case SDLK_V: return UIInputEnum::KEY_V;
	case SDLK_W: return UIInputEnum::KEY_W;
	case SDLK_X: return UIInputEnum::KEY_X;
	case SDLK_Y: return UIInputEnum::KEY_Y;
	case SDLK_Z: return UIInputEnum::KEY_Z;
	case SDLK_LEFTBRACKET: return UIInputEnum::KEY_LEFT_BRACKET;
	case SDLK_BACKSLASH: return UIInputEnum::KEY_BACK_SLASH;
	case SDLK_RIGHTBRACKET: return UIInputEnum::KEY_RIGHT_BRACKET;
	case SDLK_GRAVE: return UIInputEnum::KEY_BACK_QUOTE;
		//case SDLK_WORLD_1: return UIInputEnum::KEY_WORLD_1;
		//case SDLK_WORLD_2: return UIInputEnum::KEY_WORLD_2;
	case SDLK_INSERT: return UIInputEnum::KEY_INSERT;
	case SDLK_DELETE: return UIInputEnum::KEY_DELETE;
	case SDLK_RIGHT: return UIInputEnum::KEY_RIGHT;
	case SDLK_LEFT: return UIInputEnum::KEY_LEFT;
	case SDLK_DOWN: return UIInputEnum::KEY_DOWN;
	case SDLK_UP: return UIInputEnum::KEY_UP;
	case SDLK_PAGEUP: return UIInputEnum::KEY_PAGE_UP;
	case SDLK_PAGEDOWN: return UIInputEnum::KEY_PAGE_DOWN;
	case SDLK_HOME: return UIInputEnum::KEY_HOME;
	case SDLK_END: return UIInputEnum::KEY_END;
	case SDLK_CAPSLOCK: return UIInputEnum::KEY_CAPS_LOCK;
	case SDLK_SCROLLLOCK: return UIInputEnum::KEY_SCROLL_LOCK;
	case SDLK_NUMLOCKCLEAR: return UIInputEnum::KEY_NUM_LOCK;
	case SDLK_PRINTSCREEN: return UIInputEnum::KEY_PRINT_SCREEN;
	case SDLK_PAUSE: return UIInputEnum::KEY_PAUSE;
	case SDLK_F1: return UIInputEnum::KEY_F1;
	case SDLK_F2: return UIInputEnum::KEY_F2;
	case SDLK_F3: return UIInputEnum::KEY_F3;
	case SDLK_F4: return UIInputEnum::KEY_F4;
	case SDLK_F5: return UIInputEnum::KEY_F5;
	case SDLK_F6: return UIInputEnum::KEY_F6;
	case SDLK_F7: return UIInputEnum::KEY_F7;
	case SDLK_F8: return UIInputEnum::KEY_F8;
	case SDLK_F9: return UIInputEnum::KEY_F9;
	case SDLK_F10: return UIInputEnum::KEY_F10;
	case SDLK_F11: return UIInputEnum::KEY_F11;
	case SDLK_F12: return UIInputEnum::KEY_F12;
	case SDLK_F13: return UIInputEnum::KEY_F13;
	case SDLK_F14: return UIInputEnum::KEY_F14;
	case SDLK_F15: return UIInputEnum::KEY_F15;
	case SDLK_F16: return UIInputEnum::KEY_F16;
	case SDLK_F17: return UIInputEnum::KEY_F17;
	case SDLK_F18: return UIInputEnum::KEY_F18;
	case SDLK_F19: return UIInputEnum::KEY_F19;
	case SDLK_F20: return UIInputEnum::KEY_F20;
	case SDLK_F21: return UIInputEnum::KEY_F21;
	case SDLK_F22: return UIInputEnum::KEY_F22;
	case SDLK_F23: return UIInputEnum::KEY_F23;
	case SDLK_F24: return UIInputEnum::KEY_F24;
	case SDLK_KP_0: return UIInputEnum::KEY_KP_0;
	case SDLK_KP_1: return UIInputEnum::KEY_KP_1;
	case SDLK_KP_2: return UIInputEnum::KEY_KP_2;
	case SDLK_KP_3: return UIInputEnum::KEY_KP_3;
	case SDLK_KP_4: return UIInputEnum::KEY_KP_4;
	case SDLK_KP_5: return UIInputEnum::KEY_KP_5;
	case SDLK_KP_6: return UIInputEnum::KEY_KP_6;
	case SDLK_KP_7: return UIInputEnum::KEY_KP_7;
	case SDLK_KP_8: return UIInputEnum::KEY_KP_8;
	case SDLK_KP_9: return UIInputEnum::KEY_KP_9;
	case SDLK_KP_DECIMAL: return UIInputEnum::KEY_KP_DECIMAL;
	case SDLK_KP_DIVIDE: return UIInputEnum::KEY_KP_DIVIDE;
	case SDLK_KP_MULTIPLY: return UIInputEnum::KEY_KP_MULTIPLY;
	case SDLK_KP_MINUS: return UIInputEnum::KEY_KP_MINUS;
	case SDLK_KP_PLUS: return UIInputEnum::KEY_KP_PLUS;
	case SDLK_KP_ENTER: return UIInputEnum::KEY_KP_ENTER;
	case SDLK_KP_EQUALS: return UIInputEnum::KEY_KP_EQUAL;
	case SDLK_LSHIFT: return UIInputEnum::KEY_SHIFT;
	case SDLK_RSHIFT: return UIInputEnum::KEY_SHIFT;
	case SDLK_LCTRL: return UIInputEnum::KEY_CTRL;
	case SDLK_RCTRL: return UIInputEnum::KEY_CTRL;
	case SDLK_LALT: return UIInputEnum::KEY_ALT;
	case SDLK_RALT: return UIInputEnum::KEY_ALT;
	case SDLK_LGUI: return UIInputEnum::KEY_SUPER;
	case SDLK_RGUI: return UIInputEnum::KEY_SUPER;
	case SDLK_MENU: return UIInputEnum::KEY_MENU;
		//case SDLK_LAST: return UIInputEnum::KEY_LAST;
	default: return UIInputEnum::NONE;
	}
}

UIInputEnum SDL3InputEnum::GetGamepadEnum(int key)
{
	switch (key)
	{
	case SDL_GAMEPAD_BUTTON_START: return UIInputEnum::GAMEPAD_START;
	case SDL_GAMEPAD_BUTTON_BACK: return UIInputEnum::GAMEPAD_BACK;
	case SDL_GAMEPAD_BUTTON_WEST: return UIInputEnum::GAMEPAD_FACELEFT;
	case SDL_GAMEPAD_BUTTON_EAST: return UIInputEnum::GAMEPAD_FACERIGHT;
	case SDL_GAMEPAD_BUTTON_NORTH: return UIInputEnum::GAMEPAD_FACEUP;
	case SDL_GAMEPAD_BUTTON_SOUTH: return UIInputEnum::GAMEPAD_FACEDOWN;
	case SDL_GAMEPAD_BUTTON_DPAD_LEFT: return UIInputEnum::GAMEPAD_DPADLEFT;
	case SDL_GAMEPAD_BUTTON_DPAD_RIGHT: return UIInputEnum::GAMEPAD_DPADRIGHT;
	case SDL_GAMEPAD_BUTTON_DPAD_UP: return UIInputEnum::GAMEPAD_DPADUP;
	case SDL_GAMEPAD_BUTTON_DPAD_DOWN: return UIInputEnum::GAMEPAD_DPADDOWN;
		// case SDL_GAMEPAD_BUTTON_L1: return UIInputEnum::GAMEPAD_L1;
		// case SDL_GAMEPAD_BUTTON_R1: return UIInputEnum::GAMEPAD_R1;
		// case SDL_GAMEPAD_BUTTON_L2: return UIInputEnum::GAMEPAD_L2;
		// case SDL_GAMEPAD_BUTTON_R2: return UIInputEnum::GAMEPAD_R2;
		// case SDL_GAMEPAD_BUTTON_L3: return UIInputEnum::GAMEPAD_L3;
		// case SDL_GAMEPAD_BUTTON_R3: return UIInputEnum::GAMEPAD_R3;
		// case SDL_GAMEPAD_BUTTON_LSTICKLEFT: return UIInputEnum::GAMEPAD_LSTICKLEFT;
		// case SDL_GAMEPAD_BUTTON_LSTICKRIGHT: return UIInputEnum::GAMEPAD_LSTICKRIGHT;
		// case SDL_GAMEPAD_BUTTON_LSTICKUP: return UIInputEnum::GAMEPAD_LSTICKUP;
		// case SDL_GAMEPAD_BUTTON_LSTICKDOWN: return UIInputEnum::GAMEPAD_LSTICKDOWN;
		// case SDL_GAMEPAD_BUTTON_RSTICKLEFT: return UIInputEnum::GAMEPAD_RSTICKLEFT;
		// case SDL_GAMEPAD_BUTTON_RSTICKRIGHT: return UIInputEnum::GAMEPAD_RSTICKRIGHT;
		// case SDL_GAMEPAD_BUTTON_RSTICKUP: return UIInputEnum::GAMEPAD_RSTICKUP;
		// case SDL_GAMEPAD_BUTTON_RSTICKDOWN: return UIInputEnum::GAMEPAD_RSTICKDOWN;
	default: return UIInputEnum::NONE;
	}
}

UIInputEnums SDL3InputEnum::GetModifierEnum(int key)
{
	UIInputEnums result = 0;
	if (key & SDL_KMOD_LALT) result |= UIInputEnum::KEY_MOD_ALT;
	if (key & SDL_KMOD_RALT) result |= UIInputEnum::KEY_MOD_ALT;
	if (key & SDL_KMOD_LCTRL) result |= UIInputEnum::KEY_MOD_CTRL;
	if (key & SDL_KMOD_RCTRL) result |= UIInputEnum::KEY_MOD_CTRL;
	if (key & SDL_KMOD_LSHIFT) result |= UIInputEnum::KEY_MOD_SHIFT;
	if (key & SDL_KMOD_RSHIFT) result |= UIInputEnum::KEY_MOD_SHIFT;
	if (key & SDL_KMOD_LGUI) result |= UIInputEnum::KEY_MOD_SUPER;
	if (key & SDL_KMOD_RGUI) result |= UIInputEnum::KEY_MOD_SUPER;
	return result;
}

