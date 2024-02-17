/*
  Hatari - keymap_symbolic.h

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.
*/

#ifndef HATARI_KEYMAP_SYMBOLIC_H
#define HATARI_KEYMAP_SYMBOLIC_H

/**
 * For each TOS language, provide a mapping of SDL symbolic key to ST scan code.
 * Reference: https://tho-otto.de/keyboards/
 *
 * Where alternatives exist, we should prioritize a mapping that works for the
 * same host locale as the TOS. The US mapping accomodates some extra keys as
 * a default fallback.
 */

static uint8_t Keymap_SymbolicToStScanCode_US(const SDL_Keysym* pKeySym)
{
	uint8_t code;
	switch (pKeySym->sym)
	{
	 case SDLK_BACKSPACE: code = 0x0E; break;
	 case SDLK_TAB: code = 0x0F; break;
	 case SDLK_CLEAR: code = 0x47; break;
	 case SDLK_RETURN: code = 0x1C; break;
	 case SDLK_ESCAPE: code = 0x01; break;
	 case SDLK_SPACE: code = 0x39; break;
	 case SDLK_EXCLAIM: code = 0x02; break;
	 case SDLK_QUOTEDBL: code = 0x28; break;
	 case SDLK_HASH: code = 0x04; break;
	 case SDLK_DOLLAR: code = 0x05; break;
	 case SDLK_AMPERSAND: code = 0x08; break;
	 case SDLK_QUOTE: code = 0x28; break;
	 case SDLK_LEFTPAREN: code = 0x63; break;
	 case SDLK_RIGHTPAREN: code = 0x64; break;
	 case SDLK_ASTERISK: code = 0x66; break;
	 case SDLK_PLUS: code = 0x4E; break;
	 case SDLK_COMMA: code = 0x33; break;
	 case SDLK_MINUS: code = 0x0C; break;
	 case SDLK_PERIOD: code = 0x34; break;
	 case SDLK_SLASH: code = 0x35; break;
	 case SDLK_0: code = 0x0B; break;
	 case SDLK_1: code = 0x02; break;
	 case SDLK_2: code = 0x03; break;
	 case SDLK_3: code = 0x04; break;
	 case SDLK_4: code = 0x05; break;
	 case SDLK_5: code = 0x06; break;
	 case SDLK_6: code = 0x07; break;
	 case SDLK_7: code = 0x08; break;
	 case SDLK_8: code = 0x09; break;
	 case SDLK_9: code = 0x0A; break;
	 case SDLK_COLON: code = 0x27; break;
	 case SDLK_SEMICOLON: code = 0x27; break;
	 case SDLK_LESS: code = 0x33; break;
	 case SDLK_EQUALS: code = 0x0D; break;
	 case SDLK_GREATER : code = 0x34; break;
	 case SDLK_QUESTION: code = 0x35; break;
	 case SDLK_AT: code = 0x03; break;
	 case SDLK_LEFTBRACKET: code = 0x1A; break;
	 case SDLK_BACKSLASH: code = 0x2B; break;
	 case SDLK_RIGHTBRACKET: code = 0x1B; break;
	 case SDLK_CARET: code = 0x06; break;
	 case SDLK_UNDERSCORE: code = 0x0C; break;
	 case SDLK_BACKQUOTE: code = 0x29; break;
	 case SDLK_a: code = 0x1E; break;
	 case SDLK_b: code = 0x30; break;
	 case SDLK_c: code = 0x2E; break;
	 case SDLK_d: code = 0x20; break;
	 case SDLK_e: code = 0x12; break;
	 case SDLK_f: code = 0x21; break;
	 case SDLK_g: code = 0x22; break;
	 case SDLK_h: code = 0x23; break;
	 case SDLK_i: code = 0x17; break;
	 case SDLK_j: code = 0x24; break;
	 case SDLK_k: code = 0x25; break;
	 case SDLK_l: code = 0x26; break;
	 case SDLK_m: code = 0x32; break;
	 case SDLK_n: code = 0x31; break;
	 case SDLK_o: code = 0x18; break;
	 case SDLK_p: code = 0x19; break;
	 case SDLK_q: code = 0x10; break;
	 case SDLK_r: code = 0x13; break;
	 case SDLK_s: code = 0x1F; break;
	 case SDLK_t: code = 0x14; break;
	 case SDLK_u: code = 0x16; break;
	 case SDLK_v: code = 0x2F; break;
	 case SDLK_w: code = 0x11; break;
	 case SDLK_x: code = 0x2D; break;
	 case SDLK_y: code = 0x15; break;
	 case SDLK_z: code = 0x2C; break;
	 case SDLK_DELETE: code = 0x53; break;
	 /* End of ASCII mapped keysyms */
	 case 178: code = 0x29; break; /* ² French */
	 case 180: code = 0x0D; break; /* ' German */
	 case 223: code = 0x0C; break; /* ß German */
	 case 228: code = 0x28; break; /* Ä German */
	 case 246: code = 0x27; break; /* Ö German */
	 case 249: code = 0x28; break; /* ù French */
	 case 252: code = 0x1A; break; /* Ü German */
	 /* Numeric keypad: */
	 case SDLK_KP_0: code = 0x70; break;
	 case SDLK_KP_1: code = 0x6D; break;
	 case SDLK_KP_2: code = 0x6E; break;
	 case SDLK_KP_3: code = 0x6F; break;
	 case SDLK_KP_4: code = 0x6A; break;
	 case SDLK_KP_5: code = 0x6B; break;
	 case SDLK_KP_6: code = 0x6C; break;
	 case SDLK_KP_7: code = 0x67; break;
	 case SDLK_KP_8: code = 0x68; break;
	 case SDLK_KP_9: code = 0x69; break;
	 case SDLK_KP_PERIOD: code = 0x71; break;
	 case SDLK_KP_LEFTPAREN: code = 0x63; break;
	 case SDLK_KP_RIGHTPAREN: code = 0x64; break;
	 case SDLK_KP_DIVIDE: code = 0x65; break;
	 case SDLK_KP_MULTIPLY: code = 0x66; break;
	 case SDLK_KP_MINUS: code = 0x4A; break;
	 case SDLK_KP_PLUS: code = 0x4E; break;
	 case SDLK_KP_ENTER: code = 0x72; break;
	 case SDLK_KP_EQUALS: code = 0x72; break;
	 /* Arrows + Home/End pad */
	 case SDLK_UP: code = 0x48; break;
	 case SDLK_DOWN: code = 0x50; break;
	 case SDLK_RIGHT: code = 0x4D; break;
	 case SDLK_LEFT: code = 0x4B; break;
	 case SDLK_INSERT: code = 0x52; break;
	 case SDLK_HOME: code = 0x47; break;
	 case SDLK_END: code = 0x61; break; /* Undo */
	 case SDLK_PAGEUP: code = 0x63; break; /* ( */
	 case SDLK_PAGEDOWN: code = 0x64; break; /* ) */
	 /* Function keys */
	 case SDLK_F1: code = 0x3B; break;
	 case SDLK_F2: code = 0x3C; break;
	 case SDLK_F3: code = 0x3D; break;
	 case SDLK_F4: code = 0x3E; break;
	 case SDLK_F5: code = 0x3F; break;
	 case SDLK_F6: code = 0x40; break;
	 case SDLK_F7: code = 0x41; break;
	 case SDLK_F8: code = 0x42; break;
	 case SDLK_F9: code = 0x43; break;
	 case SDLK_F10: code = 0x44; break;
	 case SDLK_F11: code = 0x62; break; /* Help */
	 case SDLK_F12: code = 0x61; break; /* Undo */
	 case SDLK_F13: code = 0x62; break; /* Help */
	 /* Key state modifier keys */
	 case SDLK_CAPSLOCK: code = 0x3A; break;
	 case SDLK_SCROLLLOCK: code = 0x61; break; /* Undo */
	 case SDLK_RSHIFT: code = 0x36; break;
	 case SDLK_LSHIFT: code = 0x2A; break;
	 case SDLK_RCTRL: code = 0x1D; break;
	 case SDLK_LCTRL: code = 0x1D; break;
	 case SDLK_RALT: code = 0x38; break;
	 case SDLK_LALT: code = 0x38; break;
	 /* Miscellaneous function keys */
	 case SDLK_HELP: code = 0x62; break;
	 case SDLK_PRINTSCREEN: code = 0x62; break; /* Help */
	 case SDLK_UNDO: code = 0x61; break;
	 default: code = ST_NO_SCANCODE;
	}
	return code;
}

static uint8_t Keymap_SymbolicToStScanCode_DE(const SDL_Keysym* pKeySym)
{
	uint8_t code;
	switch (pKeySym->sym)
	{
	 case SDLK_BACKSPACE: code = 0x0E; break;
	 case SDLK_TAB: code = 0x0F; break;
	 case SDLK_CLEAR: code = 0x47; break;
	 case SDLK_RETURN: code = 0x1C; break;
	 case SDLK_ESCAPE: code = 0x01; break;
	 case SDLK_SPACE: code = 0x39; break;
	 case SDLK_EXCLAIM: code = 0x02; break;
	 case SDLK_QUOTEDBL: code = 0x03; break;
	 case SDLK_HASH: code = 0x29; break;
	 case SDLK_DOLLAR: code = 0x05; break;
	 case SDLK_AMPERSAND: code = 0x07; break;
	 case SDLK_QUOTE: code = 0x0D; break;
	 case SDLK_LEFTPAREN: code = 0x63; break;
	 case SDLK_RIGHTPAREN: code = 0x64; break;
	 case SDLK_ASTERISK: code = 0x66; break;
	 case SDLK_PLUS: code = 0x1B; break;
	 case SDLK_COMMA: code = 0x33; break;
	 case SDLK_MINUS: code = 0x35; break;
	 case SDLK_PERIOD: code = 0x34; break;
	 case SDLK_SLASH: code = 0x65; break;
	 case SDLK_0: code = 0x0B; break;
	 case SDLK_1: code = 0x02; break;
	 case SDLK_2: code = 0x03; break;
	 case SDLK_3: code = 0x04; break;
	 case SDLK_4: code = 0x05; break;
	 case SDLK_5: code = 0x06; break;
	 case SDLK_6: code = 0x07; break;
	 case SDLK_7: code = 0x08; break;
	 case SDLK_8: code = 0x09; break;
	 case SDLK_9: code = 0x0A; break;
	 case SDLK_COLON: code = 0x34; break;
	 case SDLK_SEMICOLON: code = 0x33; break;
	 case SDLK_LESS: code = 0x60; break;
	 case SDLK_EQUALS: code = 0x0B; break;
	 case SDLK_GREATER : code = 0x60; break;
	 case SDLK_QUESTION: code = 0x0C; break;
	 case SDLK_AT: code = 0x1A; break;
	 case SDLK_LEFTBRACKET: code = 0x27; break;
	 case SDLK_BACKSLASH: code = 0x1A; break;
	 case SDLK_RIGHTBRACKET: code = 0x28; break;
	 case SDLK_CARET: code = 0x29; break;
	 case SDLK_UNDERSCORE: code = 0x35; break;
	 case SDLK_BACKQUOTE: code = 0x2B; break; /* ~ | */
	 case SDLK_a: code = 0x1E; break;
	 case SDLK_b: code = 0x30; break;
	 case SDLK_c: code = 0x2E; break;
	 case SDLK_d: code = 0x20; break;
	 case SDLK_e: code = 0x12; break;
	 case SDLK_f: code = 0x21; break;
	 case SDLK_g: code = 0x22; break;
	 case SDLK_h: code = 0x23; break;
	 case SDLK_i: code = 0x17; break;
	 case SDLK_j: code = 0x24; break;
	 case SDLK_k: code = 0x25; break;
	 case SDLK_l: code = 0x26; break;
	 case SDLK_m: code = 0x32; break;
	 case SDLK_n: code = 0x31; break;
	 case SDLK_o: code = 0x18; break;
	 case SDLK_p: code = 0x19; break;
	 case SDLK_q: code = 0x10; break;
	 case SDLK_r: code = 0x13; break;
	 case SDLK_s: code = 0x1F; break;
	 case SDLK_t: code = 0x14; break;
	 case SDLK_u: code = 0x16; break;
	 case SDLK_v: code = 0x2F; break;
	 case SDLK_w: code = 0x11; break;
	 case SDLK_x: code = 0x2D; break;
	 case SDLK_y: code = 0x2C; break;
	 case SDLK_z: code = 0x15; break;
	 case SDLK_DELETE: code = 0x53; break;
	 case 180: code = 0x0D; break; /* ' */
	 case 223: code = 0x0C; break; /* ß */
	 case 228: code = 0x28; break; /* Ä */
	 case 246: code = 0x27; break; /* Ö */
	 case 252: code = 0x1A; break; /* Ü */
	 case SDLK_KP_0: code = 0x70; break;
	 case SDLK_KP_1: code = 0x6D; break;
	 case SDLK_KP_2: code = 0x6E; break;
	 case SDLK_KP_3: code = 0x6F; break;
	 case SDLK_KP_4: code = 0x6A; break;
	 case SDLK_KP_5: code = 0x6B; break;
	 case SDLK_KP_6: code = 0x6C; break;
	 case SDLK_KP_7: code = 0x67; break;
	 case SDLK_KP_8: code = 0x68; break;
	 case SDLK_KP_9: code = 0x69; break;
	 case SDLK_KP_PERIOD: code = 0x71; break;
	 case SDLK_KP_LEFTPAREN: code = 0x63; break;
	 case SDLK_KP_RIGHTPAREN: code = 0x64; break;
	 case SDLK_KP_DIVIDE: code = 0x65; break;
	 case SDLK_KP_MULTIPLY: code = 0x66; break;
	 case SDLK_KP_MINUS: code = 0x4A; break;
	 case SDLK_KP_PLUS: code = 0x4E; break;
	 case SDLK_KP_ENTER: code = 0x72; break;
	 case SDLK_KP_EQUALS: code = 0x72; break;
	 case SDLK_UP: code = 0x48; break;
	 case SDLK_DOWN: code = 0x50; break;
	 case SDLK_RIGHT: code = 0x4D; break;
	 case SDLK_LEFT: code = 0x4B; break;
	 case SDLK_INSERT: code = 0x52; break;
	 case SDLK_HOME: code = 0x47; break;
	 case SDLK_END: code = 0x61; break; /* Undo */
	 case SDLK_PAGEUP: code = 0x63; break; /* ( */
	 case SDLK_PAGEDOWN: code = 0x64; break; /* ) */
	 case SDLK_F1: code = 0x3B; break;
	 case SDLK_F2: code = 0x3C; break;
	 case SDLK_F3: code = 0x3D; break;
	 case SDLK_F4: code = 0x3E; break;
	 case SDLK_F5: code = 0x3F; break;
	 case SDLK_F6: code = 0x40; break;
	 case SDLK_F7: code = 0x41; break;
	 case SDLK_F8: code = 0x42; break;
	 case SDLK_F9: code = 0x43; break;
	 case SDLK_F10: code = 0x44; break;
	 case SDLK_F11: code = 0x62; break; /* Help */
	 case SDLK_F12: code = 0x61; break; /* Undo */
	 case SDLK_F13: code = 0x62; break; /* Help */
	 case SDLK_CAPSLOCK: code = 0x3A; break;
	 case SDLK_SCROLLLOCK: code = 0x61; break; /* Undo */
	 case SDLK_RSHIFT: code = 0x36; break;
	 case SDLK_LSHIFT: code = 0x2A; break;
	 case SDLK_RCTRL: code = 0x1D; break;
	 case SDLK_LCTRL: code = 0x1D; break;
	 case SDLK_RALT: code = 0x38; break;
	 case SDLK_LALT: code = 0x38; break;
	 case SDLK_HELP: code = 0x62; break;
	 case SDLK_PRINTSCREEN: code = 0x62; break; /* Help */
	 case SDLK_UNDO: code = 0x61; break;
	 default: code = ST_NO_SCANCODE;
	}
	return code;
}

static uint8_t Keymap_SymbolicToStScanCode_FR(const SDL_Keysym* pKeySym)
{
	uint8_t code;
	switch (pKeySym->sym)
	{
	 case SDLK_BACKSPACE: code = 0x0E; break;
	 case SDLK_TAB: code = 0x0F; break;
	 case SDLK_CLEAR: code = 0x47; break;
	 case SDLK_RETURN: code = 0x1C; break;
	 case SDLK_ESCAPE: code = 0x01; break;
	 case SDLK_SPACE: code = 0x39; break;
	 case SDLK_EXCLAIM: code = 0x09; break;
	 case SDLK_QUOTEDBL: code = 0x04; break;
	 case SDLK_HASH: code = 0x2B; break;
	 case SDLK_DOLLAR: code = 0x1B; break;
	 case SDLK_AMPERSAND: code = 0x02; break;
	 case SDLK_QUOTE: code = 0x05; break;
	 case SDLK_LEFTPAREN: code = 0x63; break;
	 case SDLK_RIGHTPAREN: code = 0x64; break;
	 case SDLK_ASTERISK: code = 0x66; break;
	 case SDLK_PLUS: code = 0x4E; break;
	 case SDLK_COMMA: code = 0x32; break;
	 case SDLK_MINUS: code = 0x0D; break;
	 case SDLK_PERIOD: code = 0x33; break;
	 case SDLK_SLASH: code = 0x34; break;
	 case SDLK_0: code = 0x0B; break;
	 case SDLK_1: code = 0x02; break;
	 case SDLK_2: code = 0x03; break;
	 case SDLK_3: code = 0x04; break;
	 case SDLK_4: code = 0x05; break;
	 case SDLK_5: code = 0x06; break;
	 case SDLK_6: code = 0x07; break;
	 case SDLK_7: code = 0x08; break;
	 case SDLK_8: code = 0x09; break;
	 case SDLK_9: code = 0x0A; break;
	 case SDLK_COLON: code = 0x34; break;
	 case SDLK_SEMICOLON: code = 0x33; break;
	 case SDLK_LESS: code = 0x60; break;
	 case SDLK_EQUALS: code = 0x35; break;
	 case SDLK_GREATER : code = 0x60; break;
	 case SDLK_QUESTION: code = 0x32; break;
	 case SDLK_AT: code = 0x2B; break;
	 case SDLK_LEFTBRACKET: code = 0x1A; break;
	 case SDLK_BACKSLASH: code = 0x28; break;
	 case SDLK_RIGHTBRACKET: code = 0x1B; break;
	 case SDLK_CARET: code = 0x1A; break;
	 case SDLK_UNDERSCORE: code = 0x0D; break;
	 case SDLK_BACKQUOTE: code = 0x29; break;
	 case SDLK_a: code = 0x10; break;
	 case SDLK_b: code = 0x30; break;
	 case SDLK_c: code = 0x2E; break;
	 case SDLK_d: code = 0x20; break;
	 case SDLK_e: code = 0x12; break;
	 case SDLK_f: code = 0x21; break;
	 case SDLK_g: code = 0x22; break;
	 case SDLK_h: code = 0x23; break;
	 case SDLK_i: code = 0x17; break;
	 case SDLK_j: code = 0x24; break;
	 case SDLK_k: code = 0x25; break;
	 case SDLK_l: code = 0x26; break;
	 case SDLK_m: code = 0x27; break;
	 case SDLK_n: code = 0x31; break;
	 case SDLK_o: code = 0x18; break;
	 case SDLK_p: code = 0x19; break;
	 case SDLK_q: code = 0x1E; break;
	 case SDLK_r: code = 0x13; break;
	 case SDLK_s: code = 0x1F; break;
	 case SDLK_t: code = 0x14; break;
	 case SDLK_u: code = 0x16; break;
	 case SDLK_v: code = 0x2F; break;
	 case SDLK_w: code = 0x2C; break;
	 case SDLK_x: code = 0x2D; break;
	 case SDLK_y: code = 0x15; break;
	 case SDLK_z: code = 0x11; break;
	 case SDLK_DELETE: code = 0x53; break;
	 case 178: code = 0x29; break; /* ² */
	 case 249: code = 0x28; break; /* ù */
	 case SDLK_KP_0: code = 0x70; break;
	 case SDLK_KP_1: code = 0x6D; break;
	 case SDLK_KP_2: code = 0x6E; break;
	 case SDLK_KP_3: code = 0x6F; break;
	 case SDLK_KP_4: code = 0x6A; break;
	 case SDLK_KP_5: code = 0x6B; break;
	 case SDLK_KP_6: code = 0x6C; break;
	 case SDLK_KP_7: code = 0x67; break;
	 case SDLK_KP_8: code = 0x68; break;
	 case SDLK_KP_9: code = 0x69; break;
	 case SDLK_KP_PERIOD: code = 0x71; break;
	 case SDLK_KP_LEFTPAREN: code = 0x63; break;
	 case SDLK_KP_RIGHTPAREN: code = 0x64; break;
	 case SDLK_KP_DIVIDE: code = 0x65; break;
	 case SDLK_KP_MULTIPLY: code = 0x66; break;
	 case SDLK_KP_MINUS: code = 0x4A; break;
	 case SDLK_KP_PLUS: code = 0x4E; break;
	 case SDLK_KP_ENTER: code = 0x72; break;
	 case SDLK_KP_EQUALS: code = 0x72; break;
	 case SDLK_UP: code = 0x48; break;
	 case SDLK_DOWN: code = 0x50; break;
	 case SDLK_RIGHT: code = 0x4D; break;
	 case SDLK_LEFT: code = 0x4B; break;
	 case SDLK_INSERT: code = 0x52; break;
	 case SDLK_HOME: code = 0x47; break;
	 case SDLK_END: code = 0x61; break; /* Undo */
	 case SDLK_PAGEUP: code = 0x63; break; /* ( */
	 case SDLK_PAGEDOWN: code = 0x64; break; /* ) */
	 case SDLK_F1: code = 0x3B; break;
	 case SDLK_F2: code = 0x3C; break;
	 case SDLK_F3: code = 0x3D; break;
	 case SDLK_F4: code = 0x3E; break;
	 case SDLK_F5: code = 0x3F; break;
	 case SDLK_F6: code = 0x40; break;
	 case SDLK_F7: code = 0x41; break;
	 case SDLK_F8: code = 0x42; break;
	 case SDLK_F9: code = 0x43; break;
	 case SDLK_F10: code = 0x44; break;
	 case SDLK_F11: code = 0x62; break; /* Help */
	 case SDLK_F12: code = 0x61; break; /* Undo */
	 case SDLK_F13: code = 0x62; break; /* Help */
	 case SDLK_CAPSLOCK: code = 0x3A; break;
	 case SDLK_SCROLLLOCK: code = 0x61; break; /* Undo */
	 case SDLK_RSHIFT: code = 0x36; break;
	 case SDLK_LSHIFT: code = 0x2A; break;
	 case SDLK_RCTRL: code = 0x1D; break;
	 case SDLK_LCTRL: code = 0x1D; break;
	 case SDLK_RALT: code = 0x38; break;
	 case SDLK_LALT: code = 0x38; break;
	 case SDLK_HELP: code = 0x62; break;
	 case SDLK_PRINTSCREEN: code = 0x62; break; /* Help */
	 case SDLK_UNDO: code = 0x61; break;
	 default: code = ST_NO_SCANCODE;
	}
	return code;
}

static uint8_t Keymap_SymbolicToStScanCode_UK(const SDL_Keysym* pKeySym)
{
	uint8_t code;
	switch (pKeySym->sym)
	{
	 case SDLK_BACKSPACE: code = 0x0E; break;
	 case SDLK_TAB: code = 0x0F; break;
	 case SDLK_CLEAR: code = 0x47; break;
	 case SDLK_RETURN: code = 0x1C; break;
	 case SDLK_ESCAPE: code = 0x01; break;
	 case SDLK_SPACE: code = 0x39; break;
	 case SDLK_EXCLAIM: code = 0x02; break;
	 case SDLK_QUOTEDBL: code = 0x03; break;
	 case SDLK_HASH: code = 0x2B; break;
	 case SDLK_DOLLAR: code = 0x05; break;
	 case SDLK_AMPERSAND: code = 0x08; break;
	 case SDLK_QUOTE: code = 0x28; break;
	 case SDLK_LEFTPAREN: code = 0x63; break;
	 case SDLK_RIGHTPAREN: code = 0x64; break;
	 case SDLK_ASTERISK: code = 0x66; break;
	 case SDLK_PLUS: code = 0x4E; break;
	 case SDLK_COMMA: code = 0x33; break;
	 case SDLK_MINUS: code = 0x0C; break;
	 case SDLK_PERIOD: code = 0x34; break;
	 case SDLK_SLASH: code = 0x35; break;
	 case SDLK_0: code = 0x0B; break;
	 case SDLK_1: code = 0x02; break;
	 case SDLK_2: code = 0x03; break;
	 case SDLK_3: code = 0x04; break;
	 case SDLK_4: code = 0x05; break;
	 case SDLK_5: code = 0x06; break;
	 case SDLK_6: code = 0x07; break;
	 case SDLK_7: code = 0x08; break;
	 case SDLK_8: code = 0x09; break;
	 case SDLK_9: code = 0x0A; break;
	 case SDLK_COLON: code = 0x27; break;
	 case SDLK_SEMICOLON: code = 0x27; break;
	 case SDLK_LESS: code = 0x33; break;
	 case SDLK_EQUALS: code = 0x0D; break;
	 case SDLK_GREATER : code = 0x34; break;
	 case SDLK_QUESTION: code = 0x35; break;
	 case SDLK_AT: code = 0x28; break;
	 case SDLK_LEFTBRACKET: code = 0x1A; break;
	 case SDLK_BACKSLASH: code = 0x60; break;
	 case SDLK_RIGHTBRACKET: code = 0x1B; break;
	 case SDLK_CARET: code = 0x06; break;
	 case SDLK_UNDERSCORE: code = 0x0C; break;
	 case SDLK_BACKQUOTE: code = 0x29; break;
	 case SDLK_a: code = 0x1E; break;
	 case SDLK_b: code = 0x30; break;
	 case SDLK_c: code = 0x2E; break;
	 case SDLK_d: code = 0x20; break;
	 case SDLK_e: code = 0x12; break;
	 case SDLK_f: code = 0x21; break;
	 case SDLK_g: code = 0x22; break;
	 case SDLK_h: code = 0x23; break;
	 case SDLK_i: code = 0x17; break;
	 case SDLK_j: code = 0x24; break;
	 case SDLK_k: code = 0x25; break;
	 case SDLK_l: code = 0x26; break;
	 case SDLK_m: code = 0x32; break;
	 case SDLK_n: code = 0x31; break;
	 case SDLK_o: code = 0x18; break;
	 case SDLK_p: code = 0x19; break;
	 case SDLK_q: code = 0x10; break;
	 case SDLK_r: code = 0x13; break;
	 case SDLK_s: code = 0x1F; break;
	 case SDLK_t: code = 0x14; break;
	 case SDLK_u: code = 0x16; break;
	 case SDLK_v: code = 0x2F; break;
	 case SDLK_w: code = 0x11; break;
	 case SDLK_x: code = 0x2D; break;
	 case SDLK_y: code = 0x15; break;
	 case SDLK_z: code = 0x2C; break;
	 case SDLK_DELETE: code = 0x53; break;
	 case SDLK_KP_0: code = 0x70; break;
	 case SDLK_KP_1: code = 0x6D; break;
	 case SDLK_KP_2: code = 0x6E; break;
	 case SDLK_KP_3: code = 0x6F; break;
	 case SDLK_KP_4: code = 0x6A; break;
	 case SDLK_KP_5: code = 0x6B; break;
	 case SDLK_KP_6: code = 0x6C; break;
	 case SDLK_KP_7: code = 0x67; break;
	 case SDLK_KP_8: code = 0x68; break;
	 case SDLK_KP_9: code = 0x69; break;
	 case SDLK_KP_PERIOD: code = 0x71; break;
	 case SDLK_KP_LEFTPAREN: code = 0x63; break;
	 case SDLK_KP_RIGHTPAREN: code = 0x64; break;
	 case SDLK_KP_DIVIDE: code = 0x65; break;
	 case SDLK_KP_MULTIPLY: code = 0x66; break;
	 case SDLK_KP_MINUS: code = 0x4A; break;
	 case SDLK_KP_PLUS: code = 0x4E; break;
	 case SDLK_KP_ENTER: code = 0x72; break;
	 case SDLK_KP_EQUALS: code = 0x72; break;
	 case SDLK_UP: code = 0x48; break;
	 case SDLK_DOWN: code = 0x50; break;
	 case SDLK_RIGHT: code = 0x4D; break;
	 case SDLK_LEFT: code = 0x4B; break;
	 case SDLK_INSERT: code = 0x52; break;
	 case SDLK_HOME: code = 0x47; break;
	 case SDLK_END: code = 0x61; break; /* Undo */
	 case SDLK_PAGEUP: code = 0x63; break; /* ( */
	 case SDLK_PAGEDOWN: code = 0x64; break; /* ) */
	 case SDLK_F1: code = 0x3B; break;
	 case SDLK_F2: code = 0x3C; break;
	 case SDLK_F3: code = 0x3D; break;
	 case SDLK_F4: code = 0x3E; break;
	 case SDLK_F5: code = 0x3F; break;
	 case SDLK_F6: code = 0x40; break;
	 case SDLK_F7: code = 0x41; break;
	 case SDLK_F8: code = 0x42; break;
	 case SDLK_F9: code = 0x43; break;
	 case SDLK_F10: code = 0x44; break;
	 case SDLK_F11: code = 0x62; break; /* Help */
	 case SDLK_F12: code = 0x61; break; /* Undo */
	 case SDLK_F13: code = 0x62; break; /* Help */
	 case SDLK_CAPSLOCK: code = 0x3A; break;
	 case SDLK_SCROLLLOCK: code = 0x61; break; /* Undo */
	 case SDLK_RSHIFT: code = 0x36; break;
	 case SDLK_LSHIFT: code = 0x2A; break;
	 case SDLK_RCTRL: code = 0x1D; break;
	 case SDLK_LCTRL: code = 0x1D; break;
	 case SDLK_RALT: code = 0x38; break;
	 case SDLK_LALT: code = 0x38; break;
	 case SDLK_HELP: code = 0x62; break;
	 case SDLK_PRINTSCREEN: code = 0x62; break; /* Help */
	 case SDLK_UNDO: code = 0x61; break;
	 default: code = ST_NO_SCANCODE;
	}
	return code;
}

static uint8_t Keymap_SymbolicToStScanCode_ES(const SDL_Keysym* pKeySym) { (void)pKeySym; return ST_NO_SCANCODE; }
static uint8_t Keymap_SymbolicToStScanCode_IT(const SDL_Keysym* pKeySym) { (void)pKeySym; return ST_NO_SCANCODE; }
static uint8_t Keymap_SymbolicToStScanCode_SE(const SDL_Keysym* pKeySym) { (void)pKeySym; return ST_NO_SCANCODE; }
static uint8_t Keymap_SymbolicToStScanCode_CH_FR(const SDL_Keysym* pKeySym) { (void)pKeySym; return ST_NO_SCANCODE; }
static uint8_t Keymap_SymbolicToStScanCode_CH_DE(const SDL_Keysym* pKeySym) { (void)pKeySym; return ST_NO_SCANCODE; }
static uint8_t Keymap_SymbolicToStScanCode_TR(const SDL_Keysym* pKeySym) { (void)pKeySym; return ST_NO_SCANCODE; }
static uint8_t Keymap_SymbolicToStScanCode_FI(const SDL_Keysym* pKeySym) { (void)pKeySym; return ST_NO_SCANCODE; }
static uint8_t Keymap_SymbolicToStScanCode_NO(const SDL_Keysym* pKeySym) { (void)pKeySym; return ST_NO_SCANCODE; }
static uint8_t Keymap_SymbolicToStScanCode_DK(const SDL_Keysym* pKeySym) { (void)pKeySym; return ST_NO_SCANCODE; }
static uint8_t Keymap_SymbolicToStScanCode_SA(const SDL_Keysym* pKeySym) { (void)pKeySym; return ST_NO_SCANCODE; }
static uint8_t Keymap_SymbolicToStScanCode_NL(const SDL_Keysym* pKeySym) { (void)pKeySym; return ST_NO_SCANCODE; }
static uint8_t Keymap_SymbolicToStScanCode_CS(const SDL_Keysym* pKeySym) { (void)pKeySym; return ST_NO_SCANCODE; }
static uint8_t Keymap_SymbolicToStScanCode_HU(const SDL_Keysym* pKeySym) { (void)pKeySym; return ST_NO_SCANCODE; }

static uint8_t Keymap_SymbolicToStScanCode_RU(const SDL_Keysym* pKeySym)
{
	/* no information available, default to US */
	return Keymap_SymbolicToStScanCode_US(pKeySym);
}

static uint8_t Keymap_SymbolicToStScanCode_GR(const SDL_Keysym* pKeySym)
{
	/* no information available, default to US */
	return Keymap_SymbolicToStScanCode_US(pKeySym);
}

#endif
