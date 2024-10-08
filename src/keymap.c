/*
  Hatari - keymap.c

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.

  Map SDL key events to ST scancodes and send them to IKBD as
  pressed/released keys.  Based on Hatari configuration options,
  several different ways can be used to map SDL key events.

  See https://tho-otto.de/keyboards/ for the Atari ST keyboard layouts.
*/
const char Keymap_fileid[] = "Hatari keymap.c";

#include <assert.h>
#include <ctype.h>
#include "main.h"
#include "keymap.h"
#include "configuration.h"
#include "file.h"
#include "ikbd.h"
#include "nvram.h"
#include "joy.h"
#include "shortcut.h"
#include "str.h"
#include "tos.h"
#include "debugui.h"
#include "log.h"

/* if not able to map */
#define ST_NO_SCANCODE 0xff

/* Some ST keyboard scancodes */
#define ST_ESC		0x01
#define ST_CONTROL	0x1d
#define ST_LSHIFT	0x2a
#define ST_RSHIFT	0x36
#define ST_ALTERNATE	0x38
#define ST_CAPSLOCK	0x3a

/* List of ST scan codes to NOT de-bounce when running in maximum speed */
static const uint8_t DebounceExtendedKeys[] =
{
	ST_CONTROL,
	ST_LSHIFT,
	ST_ESC,
	ST_ALTERNATE,
	ST_RSHIFT,
	0  /* End of list */
};

typedef struct
{
	SDL_Scancode scancode;
	SDL_Keymod mods;
} SDLKey;

static const struct
{
	const char *name;
	uint8_t scancode;
	uint8_t mod;
} ST_Modifiers[] = {
	{"CONTROL", ST_CONTROL,   0x01},
	{"LSHIFT",  ST_LSHIFT,    0x02},
	{"RSHIFT",  ST_RSHIFT,    0x04},
	{"ALT",     ST_ALTERNATE, 0x08},
	{"ALT_XXX", ST_ALTERNATE, 0x10},
};

#define ALT_XXX_BIT 0x10

typedef struct
{
	uint8_t scancode;
	/* key modifiers matching ST_Modifiers[].mod bit values */
	uint8_t mods;
} ST_Key;

/* Key mappings: pair SDL key definition with ST key */
typedef struct
{
	/* Input on PC keyboard */
	SDLKey pc;
	/* Output on the ST keyboard */
	ST_Key st;
} KeyMapping;

/* Loaded PC->ST key mappings */
static KeyMapping LoadedKeymap[KBD_MAX_SCANCODE];

/* ST keys being kept down, indexed by SDL scancode */
static ST_Key KeysDown[SDL_NUM_SCANCODES];


/*-----------------------------------------------------------------------*/
/**
 * Initialization.
 */
void Keymap_Init(void)
{
	Keymap_LoadRemapFile(ConfigureParams.Keyboard.szMappingFileName);
}

/**
 * Clear key states on reset, like IKBD does
 */
void Keymap_Reset(void)
{
	memset(KeysDown, 0, sizeof(KeysDown));
}


/**
 * Default function for mapping SDL symbolic key to ST scan code.
 * This is basically the US QWERTY ST keyboard with some additional
 * international key fallbacks.
 */
static uint8_t Keymap_SymbolicToStScanCode_default(const SDL_Keysym* pKeySym)
{
	uint8_t code;

	switch (pKeySym->sym)
	{
	 case SDLK_BACKSPACE: code = 0x0E; break;
	 case SDLK_TAB: code = 0x0F; break;
	 case SDLK_CLEAR: code = 0x47; break;
	 case SDLK_RETURN: code = 0x1C; break;
	 case SDLK_ESCAPE: code = ST_ESC; break;
	 case SDLK_SPACE: code = 0x39; break;
	 case SDLK_EXCLAIM: code = 0x09; break;     /* on azerty? */
	 case SDLK_QUOTEDBL: code = 0x04; break;    /* on azerty? */
	 case SDLK_HASH: code = 0x29; break;
	 case SDLK_DOLLAR: code = 0x1b; break;      /* on azerty */
	 case SDLK_AMPERSAND: code = 0x02; break;   /* on azerty? */
	 case SDLK_QUOTE: code = 0x28; break;
	 case SDLK_LEFTPAREN: code = 0x63; break;
	 case SDLK_RIGHTPAREN: code = 0x64; break;
	 case SDLK_ASTERISK: code = 0x66; break;
	 case SDLK_PLUS: code = 0x4e; break;
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
	 case SDLK_COLON: code = 0x34; break;
	 case SDLK_SEMICOLON: code = 0x27; break;
	 case SDLK_LESS: code = 0x60; break;
	 case SDLK_EQUALS: code = 0x0D; break;
	 case SDLK_GREATER : code = 0x34; break;
	 case SDLK_QUESTION: code = 0x35; break;
	 case SDLK_AT: code = 0x28; break;
	 case SDLK_LEFTBRACKET: code = 0x1A; break;
	 case SDLK_BACKSLASH: code = 0x2B; break;
	 case SDLK_RIGHTBRACKET: code = 0x1B; break;
	 case SDLK_CARET: code = 0x2B; break;
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
	 case 167: code = 0x29; break;		/* Swiss § */
	 case 168: code = 0x1B; break;		/* Swiss ¨ */
	 case 176: code = 0x35; break;		/* Spanish ° */
	 case 178: code = 0x29; break;		/* French ² */
	 case 180: code = 0x0D; break;		/* German ' */
	 case 223: code = 0x0C; break;		/* German ß */
	 case 224: code = 0x0B; break;		/* French à */
	 case 228: code = 0x28; break;		/* German ä */
	 case 229: code = 0x1A; break;		/* Swedish å */
	 case 231: code = 0x0A; break;		/* French ç */
	 case 232: code = 0x08; break;		/* French è */
	 case 233: code = 0x03; break;		/* French é */
	 case 236: code = 0x0D; break;		/* Italian ì */
	 case 241: code = 0x27; break;		/* Spanish ñ */
	 case 242: code = 0x27; break;		/* Italian ò */
	 case 246: code = 0x27; break;		/* German ö */
	 case 249: code = 0x28; break;		/* French ù */
	 case 252: code = 0x1A; break;		/* German ü */
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
	 case SDLK_KP_EQUALS: code = 0x61; break;
	 /* Arrows + Home/End pad */
	 case SDLK_UP: code = 0x48; break;
	 case SDLK_DOWN: code = 0x50; break;
	 case SDLK_RIGHT: code = 0x4D; break;
	 case SDLK_LEFT: code = 0x4B; break;
	 case SDLK_INSERT: code = 0x52; break;
	 case SDLK_HOME: code = 0x47; break;
	 case SDLK_END: code = 0x61; break;         /* ST Undo */
	 case SDLK_PAGEUP: code = 0x63; break;      /* ST ( */
	 case SDLK_PAGEDOWN: code = 0x64; break;    /* ST ) */
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
	 case SDLK_F11: code = 0x62; break;         /* ST Help */
	 case SDLK_F12: code = 0x61; break;         /* ST Undo */
	 case SDLK_F13: code = 0x62; break;         /* ST Help */
	 /* Key state modifier keys */
	 case SDLK_CAPSLOCK: code = ST_CAPSLOCK; break;
	 case SDLK_SCROLLLOCK: code = 0x61; break;  /* ST Undo */
	 case SDLK_RSHIFT: code = ST_RSHIFT; break;
	 case SDLK_LSHIFT: code = ST_LSHIFT; break;
	 case SDLK_RCTRL: code = ST_CONTROL; break;
	 case SDLK_LCTRL: code = ST_CONTROL; break;
	 case SDLK_RALT: code = ST_ALTERNATE; break;
	 case SDLK_LALT: code = ST_ALTERNATE; break;
	 /* Miscellaneous function keys */
	 case SDLK_HELP: code = 0x62; break;
	 case SDLK_PRINTSCREEN: code = 0x62; break; /* ST Help */
	 case SDLK_UNDO: code = 0x61; break;
	 default: code = ST_NO_SCANCODE;
	}

	return code;
}

static uint8_t (*Keymap_SymbolicToStScanCode)(const SDL_Keysym* pKeySym) =
		Keymap_SymbolicToStScanCode_default;

static uint8_t Keymap_SymbolicToStScanCode_DE(const SDL_Keysym* keysym)
{
	switch (keysym->sym)
	{
	 case SDLK_HASH: return 0x29;
	 case SDLK_PLUS: return 0x1B;
	 case SDLK_MINUS: return 0x35;
	 case SDLK_SLASH: return 0x65;
	 case SDLK_y: return 0x2C;
	 case SDLK_z: return 0x15;
	 default: return Keymap_SymbolicToStScanCode_default(keysym);
	}
}

static uint8_t Keymap_SymbolicToStScanCode_FR(const SDL_Keysym* keysym)
{
	switch (keysym->sym)
	{
	 case SDLK_HASH: return 0x2B;
	 case SDLK_QUOTE: return 0x05;
	 case SDLK_LEFTPAREN: return 0x06;
	 case SDLK_RIGHTPAREN: return 0x0c;
	 case SDLK_COMMA: return 0x32;
	 case SDLK_MINUS: return 0x0D;
	 case SDLK_SEMICOLON: return 0x33;
	 case SDLK_EQUALS: return 0x35;
	 case SDLK_CARET: return 0x1A;
	 case SDLK_a: return 0x10;
	 case SDLK_m: return 0x27;
	 case SDLK_q: return 0x1E;
	 case SDLK_w: return 0x2C;
	 case SDLK_z: return 0x11;
	 case 167: return 0x07;		/* French § */
	 default: return Keymap_SymbolicToStScanCode_default(keysym);
	}
}

static uint8_t Keymap_SymbolicToStScanCode_UK(const SDL_Keysym* keysym)
{
	switch (keysym->sym)
	{
	 case SDLK_HASH: return 0x2B;
	 case SDLK_BACKSLASH: return 0x60;
	 default: return Keymap_SymbolicToStScanCode_default(keysym);
	}
}

static uint8_t Keymap_SymbolicToStScanCode_ES(const SDL_Keysym* keysym)
{
	switch (keysym->sym)
	{
	 case SDLK_SEMICOLON: return 0x28;
	 case SDLK_BACKQUOTE: return 0x1B;
	 case 231: return 0x29;		/* Spanish ç */
	 default: return Keymap_SymbolicToStScanCode_default(keysym);
	}
}

static uint8_t Keymap_SymbolicToStScanCode_IT(const SDL_Keysym* keysym)
{
	switch (keysym->sym)
	{
	 case SDLK_QUOTE: return 0x0C;
	 case SDLK_PLUS: return 0x1B;
	 case SDLK_MINUS: return 0x35;
	 case 224: return 0x28;		/* Italian à */
	 case 232: return 0x1A;		/* Italian è */
	 case 249: return 0x29;		/* Italian ù */
	 default: return Keymap_SymbolicToStScanCode_default(keysym);
	}
}

static uint8_t Keymap_SymbolicToStScanCode_SE(const SDL_Keysym* keysym)
{
	switch (keysym->sym)
	{
	 case SDLK_QUOTE: return 0x29;
	 case SDLK_PLUS: return 0x0C;
	 case SDLK_MINUS: return 0x35;
	 case 252: return 0x1b;		/* ü */
	 default: return Keymap_SymbolicToStScanCode_default(keysym);
	}
}

/* Mapping for both, French and German variant of Swiss keyboard */
static uint8_t Keymap_SymbolicToStScanCode_CH(const SDL_Keysym* keysym)
{
	switch (keysym->sym)
	{
	 case SDLK_CARET: return 0x0D;
	 case 224: return 0x28;		/* à */
	 case 232: return 0x1A;		/* è */
	 case 233: return 0x27;		/* é */
	 default: return Keymap_SymbolicToStScanCode_default(keysym);
	}
}

static uint8_t Keymap_SymbolicToStScanCode_NO(const SDL_Keysym* keysym)
{
	switch (keysym->sym)
	{
	 case SDLK_QUOTE: return 0x29;
	 case SDLK_PLUS: return 0x0C;
	 case SDLK_MINUS: return 0x35;
	 case 230: return 0x28;		/* æ */
	 case 233: return 0x0D;		/* é */
	 case 248: return 0x27;		/* ø */
	 case 252: return 0x1b;		/* ü */
	 default: return Keymap_SymbolicToStScanCode_default(keysym);
	}
}

static uint8_t Keymap_SymbolicToStScanCode_DK(const SDL_Keysym* keysym)
{
	switch (keysym->sym)
	{
	 case SDLK_QUOTE: return 0x0D;
	 case SDLK_PLUS: return 0x0C;
	 case SDLK_MINUS: return 0x35;
	 case SDLK_ASTERISK: return 0x1B;
	 case 230: return 0x27;		/* æ */
	 case 233: return 0x29;		/* é */
	 case 248: return 0x28;		/* ø */
	 default: return Keymap_SymbolicToStScanCode_default(keysym);
	}
}

static uint8_t Keymap_SymbolicToStScanCode_NL(const SDL_Keysym* keysym)
{
	switch (keysym->sym)
	{
	 case SDLK_HASH: return 0x2B;
	 case SDLK_BACKSLASH: return 0x60;
	 default: return Keymap_SymbolicToStScanCode_default(keysym);
	}
}

/**
 * Remap SDL scancode key to ST Scan code
 */
static uint8_t Keymap_PcToStScanCode(const SDL_Keysym* pKeySym)
{
	switch (pKeySym->scancode)
	{
	 case SDL_SCANCODE_A: return 0x1e;
	 case SDL_SCANCODE_B: return 0x30;
	 case SDL_SCANCODE_C: return 0x2e;
	 case SDL_SCANCODE_D: return 0x20;
	 case SDL_SCANCODE_E: return 0x12;
	 case SDL_SCANCODE_F: return 0x21;
	 case SDL_SCANCODE_G: return 0x22;
	 case SDL_SCANCODE_H: return 0x23;
	 case SDL_SCANCODE_I: return 0x17;
	 case SDL_SCANCODE_J: return 0x24;
	 case SDL_SCANCODE_K: return 0x25;
	 case SDL_SCANCODE_L: return 0x26;
	 case SDL_SCANCODE_M: return 0x32;
	 case SDL_SCANCODE_N: return 0x31;
	 case SDL_SCANCODE_O: return 0x18;
	 case SDL_SCANCODE_P: return 0x19;
	 case SDL_SCANCODE_Q: return 0x10;
	 case SDL_SCANCODE_R: return 0x13;
	 case SDL_SCANCODE_S: return 0x1f;
	 case SDL_SCANCODE_T: return 0x14;
	 case SDL_SCANCODE_U: return 0x16;
	 case SDL_SCANCODE_V: return 0x2f;
	 case SDL_SCANCODE_W: return 0x11;
	 case SDL_SCANCODE_X: return 0x2d;
	 case SDL_SCANCODE_Y: return 0x15;
	 case SDL_SCANCODE_Z: return 0x2c;
	 case SDL_SCANCODE_1: return 0x02;
	 case SDL_SCANCODE_2: return 0x03;
	 case SDL_SCANCODE_3: return 0x04;
	 case SDL_SCANCODE_4: return 0x05;
	 case SDL_SCANCODE_5: return 0x06;
	 case SDL_SCANCODE_6: return 0x07;
	 case SDL_SCANCODE_7: return 0x08;
	 case SDL_SCANCODE_8: return 0x09;
	 case SDL_SCANCODE_9: return 0x0a;
	 case SDL_SCANCODE_0: return 0x0b;
	 case SDL_SCANCODE_RETURN: return 0x1c;
	 case SDL_SCANCODE_ESCAPE: return ST_ESC;
	 case SDL_SCANCODE_BACKSPACE: return 0x0e;
	 case SDL_SCANCODE_TAB: return 0x0f;
	 case SDL_SCANCODE_SPACE: return 0x39;
	 case SDL_SCANCODE_MINUS: return 0x0c;
	 case SDL_SCANCODE_EQUALS: return 0x0d;
	 case SDL_SCANCODE_LEFTBRACKET: return 0x1a;
	 case SDL_SCANCODE_RIGHTBRACKET: return 0x1b;
	 case SDL_SCANCODE_BACKSLASH: return 0x29;  /* for 0x60 see NONUSBACKSLASH */
	 case SDL_SCANCODE_NONUSHASH: return 0x2b;
	 case SDL_SCANCODE_SEMICOLON: return 0x27;
	 case SDL_SCANCODE_APOSTROPHE: return 0x28;
	 case SDL_SCANCODE_GRAVE: return 0x2b;      /* ok? */
	 case SDL_SCANCODE_COMMA: return 0x33;
	 case SDL_SCANCODE_PERIOD: return 0x34;
	 case SDL_SCANCODE_SLASH: return 0x35;
	 case SDL_SCANCODE_CAPSLOCK: return ST_CAPSLOCK;
	 case SDL_SCANCODE_F1: return 0x3b;
	 case SDL_SCANCODE_F2: return 0x3c;
	 case SDL_SCANCODE_F3: return 0x3d;
	 case SDL_SCANCODE_F4: return 0x3e;
	 case SDL_SCANCODE_F5: return 0x3f;
	 case SDL_SCANCODE_F6: return 0x40;
	 case SDL_SCANCODE_F7: return 0x41;
	 case SDL_SCANCODE_F8: return 0x42;
	 case SDL_SCANCODE_F9: return 0x43;
	 case SDL_SCANCODE_F10: return 0x44;
	 case SDL_SCANCODE_F11: return 0x62;
	 case SDL_SCANCODE_F12: return 0x61;
	 case SDL_SCANCODE_PRINTSCREEN: return 0x62;
	 case SDL_SCANCODE_SCROLLLOCK: return 0x61;
	 case SDL_SCANCODE_PAUSE: return 0x61;
	 case SDL_SCANCODE_INSERT: return 0x52;
	 case SDL_SCANCODE_HOME: return 0x47;
	 case SDL_SCANCODE_PAGEUP: return 0x63;
	 case SDL_SCANCODE_DELETE: return 0x53;
	 case SDL_SCANCODE_END: return 0x2b;
	 case SDL_SCANCODE_PAGEDOWN: return 0x64;
	 case SDL_SCANCODE_RIGHT: return 0x4d;
	 case SDL_SCANCODE_LEFT: return 0x4b;
	 case SDL_SCANCODE_DOWN: return 0x50;
	 case SDL_SCANCODE_UP: return 0x48;
	 case SDL_SCANCODE_NUMLOCKCLEAR: return 0x64;
	 case SDL_SCANCODE_KP_DIVIDE: return 0x65;
	 case SDL_SCANCODE_KP_MULTIPLY: return 0x66;
	 case SDL_SCANCODE_KP_MINUS: return 0x4a;
	 case SDL_SCANCODE_KP_PLUS: return 0x4e;
	 case SDL_SCANCODE_KP_ENTER: return 0x72;
	 case SDL_SCANCODE_KP_1: return 0x6d;
	 case SDL_SCANCODE_KP_2: return 0x6e;
	 case SDL_SCANCODE_KP_3: return 0x6f;
	 case SDL_SCANCODE_KP_4: return 0x6a;
	 case SDL_SCANCODE_KP_5: return 0x6b;
	 case SDL_SCANCODE_KP_6: return 0x6c;
	 case SDL_SCANCODE_KP_7: return 0x67;
	 case SDL_SCANCODE_KP_8: return 0x68;
	 case SDL_SCANCODE_KP_9: return 0x69;
	 case SDL_SCANCODE_KP_0: return 0x70;
	 case SDL_SCANCODE_KP_PERIOD: return 0x71;
	 case SDL_SCANCODE_NONUSBACKSLASH: return 0x60;
	 //case SDL_SCANCODE_APPLICATION: return ;
	 case SDL_SCANCODE_KP_EQUALS: return 0x63;
	 case SDL_SCANCODE_F13: return 0x63;
	 case SDL_SCANCODE_F14: return 0x64;
	 case SDL_SCANCODE_HELP: return 0x62;
	 case SDL_SCANCODE_UNDO: return 0x61;
	 case SDL_SCANCODE_KP_COMMA: return 0x71;
	 case SDL_SCANCODE_CLEAR: return 0x47;
	 case SDL_SCANCODE_RETURN2: return 0x1c;
	 case SDL_SCANCODE_KP_LEFTPAREN: return 0x63;
	 case SDL_SCANCODE_KP_RIGHTPAREN: return 0x64;
	 case SDL_SCANCODE_KP_LEFTBRACE: return 0x63;
	 case SDL_SCANCODE_KP_RIGHTBRACE: return 0x64;
	 case SDL_SCANCODE_KP_TAB: return 0x0f;
	 case SDL_SCANCODE_KP_BACKSPACE: return 0x0e;
	 case SDL_SCANCODE_KP_COLON: return 0x33;
	 case SDL_SCANCODE_KP_HASH: return 0x0c;
	 case SDL_SCANCODE_KP_SPACE: return 0x39;
	 case SDL_SCANCODE_KP_CLEAR: return 0x47;
	 case SDL_SCANCODE_LCTRL: return ST_CONTROL;
	 case SDL_SCANCODE_LSHIFT: return ST_LSHIFT;
	 case SDL_SCANCODE_LALT: return ST_ALTERNATE;
	 case SDL_SCANCODE_RCTRL: return ST_CONTROL;
	 case SDL_SCANCODE_RSHIFT: return ST_RSHIFT;
	 default:
		if (!pKeySym->scancode && pKeySym->sym)
		{
			/* assume SimulateKey
			 * -> KeyUp/Down
			 *    -> Remap (with scancode mode configured)
			 *       -> PcToStScanCode
			 */
			return Keymap_SymbolicToStScanCode(pKeySym);
		}
		Log_Printf(LOG_WARN, "Unhandled scancode 0x%x!\n", pKeySym->scancode);
		return ST_NO_SCANCODE;
	}
}


/**
 * Remap a keypad key to ST scan code. We use a separate function for this
 * so that we can easily toggle between number and cursor mode with the
 * numlock key.
 */
static uint8_t Keymap_GetKeyPadScanCode(const SDL_Keysym* pKeySym)
{
	if (SDL_GetModState() & KMOD_NUM)
	{
		switch (pKeySym->sym)
		{
		 case SDLK_KP_1:  return 0x6d;  /* NumPad 1 */
		 case SDLK_KP_2:  return 0x6e;  /* NumPad 2 */
		 case SDLK_KP_3:  return 0x6f;  /* NumPad 3 */
		 case SDLK_KP_4:  return 0x6a;  /* NumPad 4 */
		 case SDLK_KP_5:  return 0x6b;  /* NumPad 5 */
		 case SDLK_KP_6:  return 0x6c;  /* NumPad 6 */
		 case SDLK_KP_7:  return 0x67;  /* NumPad 7 */
		 case SDLK_KP_8:  return 0x68;  /* NumPad 8 */
		 case SDLK_KP_9:  return 0x69;  /* NumPad 9 */
		 default:  break;
		}
	}
	else
	{
		switch (pKeySym->sym)
		{
		 case SDLK_KP_1:  return 0x6d;  /* NumPad 1 */
		 case SDLK_KP_2:  return 0x50;  /* Cursor down */
		 case SDLK_KP_3:  return 0x6f;  /* NumPad 3 */
		 case SDLK_KP_4:  return 0x4b;  /* Cursor left */
		 case SDLK_KP_5:  return 0x50;  /* Cursor down (again?) */
		 case SDLK_KP_6:  return 0x4d;  /* Cursor right */
		 case SDLK_KP_7:  return 0x52;  /* Insert - good for Dungeon Master */
		 case SDLK_KP_8:  return 0x48;  /* Cursor up */
		 case SDLK_KP_9:  return 0x47;  /* Home - again for Dungeon Master */
		 default:  break;
		}
	}
	return ST_NO_SCANCODE;
}

/**
 * If given ST scancode is not set, return NULL, otherwise add scancode
 * to pressed keys array, output trace of the SDL->ST key mapping and
 * return pointer to the mapping
 */
static ST_Key* UpdateMapping(const char *maptype, SDL_Scancode pcscan, uint8_t stscan)
{
	ST_Key *key = &(KeysDown[pcscan]);

	if (stscan == ST_NO_SCANCODE)
		return NULL;
	key->scancode = stscan;

	LOG_TRACE(TRACE_KEYMAP, "key mapping: %02x (%s)\n", stscan, maptype);
	return key;
}

/**
 * Remap SDL Key to ST Key (when key is pressed).
 * Receives the pressed key from SDL, and returns matching ST_Key,
 * or NULL if no mapping could be found
 */
static ST_Key* Keymap_RemapKeyToSTKey(const SDL_Keysym* pKeySym)
{
	const SDL_Keycode sym = pKeySym->sym;
	const SDL_Scancode scancode = pKeySym->scancode;

	/* check for UpdateMapping() */
	assert(scancode < ARRAY_SIZE(KeysDown));
	memset(KeysDown+scancode, 0, sizeof(*KeysDown));

	/* Check for keypad first so we can handle numlock */
	if (ConfigureParams.Keyboard.nKeymapType != KEYMAP_LOADED &&
	    sym >= SDLK_KP_1 && sym <= SDLK_KP_9)
	{
		return UpdateMapping("keypad/symbolic", scancode,
				     Keymap_GetKeyPadScanCode(pKeySym));
	}

	/* Remap from PC scancodes? */
	if (ConfigureParams.Keyboard.nKeymapType == KEYMAP_SCANCODE)
	{
		return UpdateMapping("scancode/symbolic", scancode,
				     Keymap_PcToStScanCode(pKeySym));
	}

	/* Use loaded keymap? */
	if (ConfigureParams.Keyboard.nKeymapType == KEYMAP_LOADED)
	{
		int i;
		SDL_Keymod mods = pKeySym->mod;
		for (i = 0; i < ARRAY_SIZE(LoadedKeymap); i++)
		{
			KeyMapping *mapping = &LoadedKeymap[i];
			SDL_Keymod pcmod;

			if (mapping->pc.scancode == 0)
				break; /* End of table */

			if (mapping->pc.scancode != scancode)
				continue;

			pcmod = mapping->pc.mods;
			/* (at least) one shift used? */
			if (pcmod == KMOD_SHIFT)
			{
				if (!(mods & pcmod))
					continue;
			}
			/* (at least) specified mod used? */
			else if ((mods & pcmod) != pcmod)
				continue;

			KeysDown[scancode].mods = mapping->st.mods;
			return UpdateMapping("keymap", scancode,
					     mapping->st.scancode);
		}
	}

	/* Fall back to symbolic mapping */
	return UpdateMapping("symbolic", scancode,
			     Keymap_SymbolicToStScanCode(pKeySym));
}


/*-----------------------------------------------------------------------*/
/**
 * Parse SDL modifier name from 'name' and return its value, or
 * zero for error.
 */
static SDL_Keymod GetSdlModifier(const char *name) {
	struct {
		SDL_Keymod mod;
		const char *name;
	} const keymodNames[] = {
		{ KMOD_LSHIFT, "LSHIFT" },
		{ KMOD_RSHIFT, "RSHIFT" },
		{ KMOD_SHIFT,  "SHIFT" }, /* special case: either of above */

		{ KMOD_LCTRL, "LCTRL" },
		{ KMOD_RCTRL, "RCTRL" },
		{ KMOD_CTRL,  "CTRL" },   /* both of above */

		{ KMOD_LALT, "LALT" },
		{ KMOD_RALT, "RALT" },
		{ KMOD_ALT,  "ALT" },     /* both of above */

		{ KMOD_LGUI, "LGUI" },
		{ KMOD_RGUI, "RGUI" },
		{ KMOD_GUI,  "GUI" },     /* both of above */

		{ KMOD_CAPS, "CAPS" },
		{ KMOD_MODE, "MODE" },
		{ KMOD_NUM,  "NUM" },
	};
	int i;
	for (i = 0; i < ARRAY_SIZE(keymodNames); i++)
	{
		if (strcmp(name, keymodNames[i].name) == 0)
			return keymodNames[i].mod;
	};
	return 0;
}


/*-----------------------------------------------------------------------*/
/**
 * Fill "mapping" KeyMapping host part based on host "spec" string.
 * Return true on success
 */
static bool HostSpecToKeymap(const char *spec, KeyMapping* mapping)
{
	char buf[64], *token, *saveptr, *endptr;
	SDL_Scancode scancode = 0;
	SDL_Keymod mods = 0;

	if (!spec)
		return false;
	if (strlcpy(buf, spec, sizeof(buf)) >= sizeof(buf))
	{
		Log_Printf(LOG_ERROR, "PC/SDL scancode spec '%s' too long\n", spec);
		return false;
	}

	scancode = mods = 0;
	for (token = strtok_r(buf, "|", &saveptr);
	     token;
	     token = strtok_r(NULL, "|", &saveptr))
	{
		token = Str_Trim(token);
		/* text description? */
		if (isalpha(token[0]))
		{
			SDL_Scancode code;
			SDL_Keymod mod;

			/* is it modifier? */
			mod = GetSdlModifier(token);
			if (mod)
			{
				if (mods)
				{
					Log_Printf(LOG_ERROR, "extra '%s', PC/SDL modifier already set\n", token);
					return false;
				}
				mods = mod;
				continue;
			}
			/* is it non-modifier key? */
			code = SDL_GetScancodeFromName(token);
			if (code)
			{
				if (scancode)
				{
					Log_Printf(LOG_ERROR, "extra '%s', PC/SDL scancode already set\n", token);
					return false;
				}
				scancode = code;
				continue;
			}
			Log_Printf(LOG_ERROR, "unknown PC/SDL key/modifier name '%s'\n", token);
			return false;
		}
		/* no => numerical scancode */
		if (scancode)
		{
			Log_Printf(LOG_WARN, "extra '%s', PC/SDL scancode already set\n", token);
			return false;
		}
		scancode = strtol(token, &endptr, 0);
		if (!scancode)
		{
			Log_Printf(LOG_ERROR, "invalid PC/SDL scancode '%s'\n", token);
			return false;
		}
		if (*endptr)
		{
			Log_Printf(LOG_WARN, "'%s' garbage at end of '%s' PC/SDL scancode\n",
				   endptr, token);
			return false;
		}
	}
	mapping->pc.scancode = scancode;
	mapping->pc.mods = mods;
	return (scancode > 0);
}


/*-----------------------------------------------------------------------*/
/**
 * Parse modifier name from 'name' and return 'mods' value with
 * corresponding bit added to it, or return zero if there was no match
 */
static uint8_t AddSTModifier(uint8_t mods, const char *name)
{
	uint8_t mod, i;
	for (i = 0; i < ARRAY_SIZE(ST_Modifiers); i++)
	{
		if (strcmp(ST_Modifiers[i].name, name) != 0)
			continue;

		mod = ST_Modifiers[i].mod;
		if (mod & mods)
		{
			Log_Printf(LOG_WARN, "ST modifier '%s' specified twice\n", name);
			return 0;
		}
		return (mods | mod);
	}
	Log_Printf(LOG_ERROR, "unknown/unsupported ST modifier '%s'\n", name);
	return 0;
}


/*-----------------------------------------------------------------------*/
/**
 * Fill "mapping" KeyMapping guest (ST) part based on guest "spec" string.
 * Return true on success
 */
static bool GuestSpecToKeymap(const char *spec, KeyMapping* mapping)
{
	char buf[64], *token, *saveptr, *endptr;
	uint8_t scancode, mods;

	if (!spec)
		return false;
	if (strlcpy(buf, spec, sizeof(buf)) >= sizeof(buf))
	{
		Log_Printf(LOG_ERROR, "ST scancode spec '%s' too long\n", spec);
		return false;
	}

	scancode = mods = 0;
	for (token = strtok_r(buf, "|", &saveptr);
	     token;
	     token = strtok_r(NULL, "|", &saveptr))
	{
		token = Str_Trim(token);
		if (isalpha(token[0]))
		{
			mods = AddSTModifier(mods, token);
			if (!mods)
				return false;
			continue;
		}
		if (scancode)
		{
			Log_Printf(LOG_WARN, "extra '%s', ST scancode already set\n", token);
			return false;
		}
		scancode = strtol(token, &endptr, 0);
		if (!scancode)
		{
			Log_Printf(LOG_ERROR, "invalid ST scancode '%s'\n", token);
			return false;
		}
		if (*endptr)
		{
			Log_Printf(LOG_WARN, "'%s' garbage at end of '%s' ST scancode\n",
				   endptr, token);
			return false;
		}
	}
	if ((mods & ALT_XXX_BIT) && (mods & ~ALT_XXX_BIT))
	{
		Log_Printf(LOG_ERROR, "'ALT_XXX' ST modifier specified with other modifier(s)\n");
		return false;
	}
	mapping->st.scancode = scancode;
	mapping->st.mods = mods;

	return (scancode > 0);
}


/*-----------------------------------------------------------------------*/
/**
 * Load keyboard remap file
 */
void Keymap_LoadRemapFile(const char *pszFileName)
{
	FILE *in;
	int idx, linenro, fails;

	/* Initialize table with default values */
	memset(LoadedKeymap, 0, sizeof(LoadedKeymap));

	if (strlen(pszFileName) == 0)
		return;

	/* Attempt to load mapping file */
	if (!File_Exists(pszFileName))
	{
		Log_Printf(LOG_WARN, "Keymap_LoadRemapFile: keymap file '%s' missing\n", pszFileName);
		return;
	}
	in = fopen(pszFileName, "r");
	if (!in)
	{
		Log_Printf(LOG_ERROR, "Keymap_LoadRemapFile: failed to open keymap file '%s'\n", pszFileName);
		return;
	}

	Log_Printf(LOG_DEBUG, "Keymap_LoadRemapFile: Loading '%s' keymap file\n", pszFileName);

	idx = linenro = fails = 0;
	while (!feof(in))
	{
		char *line, *saveptr, buf[1024];
		const char *host, *guest;

		if (idx >= ARRAY_SIZE(LoadedKeymap))
		{
			Log_Printf(LOG_WARN, "Mappings specified already for"
				   "supported number (%d) of keys, skipping"
				   "rest of '%s' at line %d\n",
				   ARRAY_SIZE(LoadedKeymap), pszFileName, linenro);
			fails++;
			break;
		}

		/* Read line from file */
		if (fgets(buf, sizeof(buf), in) == NULL)
			break;
		linenro++;

		/* Remove white-space from start of line */
		line = Str_Trim(buf);

		/* Ignore empty line and comments */
		if (strlen(line) == 0 || line[0] == ';' || line[0] == '#')
			continue;

		/* get the host (PC) key spec */
		host = Str_Trim(strtok_r(line, ",", &saveptr));
		if (!HostSpecToKeymap(host, &LoadedKeymap[idx]))
		{
			Log_Printf(LOG_WARN, "Failed to parse host (PC/SDL) part '%s' of line %d in: %s\n",
				   host, linenro, pszFileName);
			fails++;
			continue;
		}

		/* Get the guest (ST) key spec */
		guest = Str_Trim(strtok_r(NULL, "\n", &saveptr));
		if (!GuestSpecToKeymap(guest, &LoadedKeymap[idx]))
		{
			Log_Printf(LOG_WARN, "Failed to parse guest (ST) part '%s' of line %d in: %s\n",
				   guest, linenro, pszFileName);
			fails++;
			continue;
		}
		LOG_TRACE(TRACE_KEYMAP, "key mapping from file: host %s => guest %s\n",
			  host, guest);
		idx += 1;
	}
	fclose(in);

	if (fails)
		Log_AlertDlg(LOG_ERROR, "%d keymap file parsing failures\n(see console log for details)", fails);
}


/*-----------------------------------------------------------------------*/
/**
 * Scan list of keys to NOT de-bounce when running in maximum speed, eg ALT,SHIFT,CTRL etc...
 * @return true if key requires de-bouncing
 */
static bool Keymap_DebounceSTKey(uint8_t STScanCode)
{
	int i=0;

	/* Are we in fast forward, and have disabled key repeat? */
	if ((ConfigureParams.System.bFastForward == true)
	    && (ConfigureParams.Keyboard.bDisableKeyRepeat))
	{
		/* We should de-bounce all non extended keys,
		 * e.g. leave ALT, SHIFT, CTRL etc... held */
		while (DebounceExtendedKeys[i])
		{
			if (STScanCode == DebounceExtendedKeys[i])
				return false;
			i++;
		}

		/* De-bounce key */
		return true;
	}

	/* Do not de-bounce key */
	return false;
}


/*-----------------------------------------------------------------------*/
/**
 * Debounce any PC key held down if running with key repeat disabled.
 * This is called each ST frame, so keys get held down for one VBL which
 * is enough for 68000 code to scan.
 */
void Keymap_DebounceAllKeys(void)
{
	uint8_t nScanCode;

	/* Return if we aren't in fast forward or have not disabled key repeat */
	if ((ConfigureParams.System.bFastForward == false)
	        || (!ConfigureParams.Keyboard.bDisableKeyRepeat))
	{
		return;
	}

	/* Now run through each key looking for ones held down */
	for (nScanCode = 1; nScanCode < ARRAY_SIZE(Keyboard.KeyStates); nScanCode++)
	{
		/* Is key held? */
		if (Keyboard.KeyStates[nScanCode])
		{
			/* Does this require de-bouncing? */
			if (Keymap_DebounceSTKey(nScanCode))
			{
				IKBD_PressSTKey(nScanCode, false);
				Keyboard.KeyStates[nScanCode] = false;
			}
		}
	}

}


/*-----------------------------------------------------------------------*/
/* Returns false if SDL_Keycode is for modifier key that
 * won't be converted to ST scancode, true otherwise
 */
static bool IsKeyTranslatable(SDL_Keycode symkey)
{
	switch (symkey)
	{
	case SDLK_RALT:
	case SDLK_LGUI:
	case SDLK_RGUI:
	case SDLK_MODE:
	case SDLK_NUMLOCKCLEAR:
		return false;
	}
	return true;
}


/*-----------------------------------------------------------------------*/
/**
 * Set modifiers keys indicated by bits in 'mods' up or down
 * based on 'down'.
 *
 * Return true if modifiers were present and trace output
 * requires newline, false otherwise.
 */
static bool InsertModifiers(uint8_t mods, bool down)
{
	const char *separator;
	uint8_t i, scancode;
	if (!mods)
		return false;

	separator = "key mod(s):";
	for (i = 0; i < ARRAY_SIZE(ST_Modifiers); i++)
	{
		if (!(mods & ST_Modifiers[i].mod))
			continue;

		scancode = ST_Modifiers[i].scancode;
		LOG_TRACE(TRACE_KEYMAP, "%s %s (0x%02x)",
			  separator, ST_Modifiers[i].name, scancode);
		separator = " |";

		if (down)
		{
			/* because modifiers may already be down
			 * due to normal key events (or mapped
			 * scancodes that happen to be modifiers),
			 * KeyStates are counted
			 */
			if (Keyboard.KeyStates[scancode]++ > 0)
				continue;
		}
		else
		{
			if (--Keyboard.KeyStates[scancode] > 0)
				continue;
		}
		IKBD_PressSTKey(scancode, down);
	}
	return true;
}


/*-----------------------------------------------------------------------*/
/**
 * Insert key presses and releases for ALT_XXX digits
 * when relevant modifier bit is set.
 *
 * Return true if scancode should be ignored and trace output
 * requires newline, false otherwise.
 */
static bool InsertAltXXXDigits(ST_Key *stkey)
{
	/* map 0, 1-3, 5-6, 7-9 to corresponding numpad scancodes */
	static const uint8_t keypad[] = {
		112,
		109, 110, 111,
		106, 107, 108,
		103, 104, 105
	};
	char buf[8], *xxx;

	if (!(stkey->mods & ALT_XXX_BIT))
		return false;

	sprintf(buf, "%d", stkey->scancode);
	for (xxx = buf; *xxx; xxx++)
	{
		uint8_t scancode;
		int digit;

		digit = *xxx - '0';
		assert(digit >= 0 && digit <= 9);
		scancode = keypad[digit];
		LOG_TRACE(TRACE_KEYMAP, " + '%d' (0x%02x)",
			  digit, scancode);

		/* press and release each keycode
		 * without checking whether they
		 * are already pressed
		 */
		IKBD_PressSTKey(scancode, true);
		IKBD_PressSTKey(scancode, false);
	}
	return true;
}


/*-----------------------------------------------------------------------*/
/**
 * User pressed a key down
 */
void Keymap_KeyDown(const SDL_Keysym *sdlkey)
{
	uint8_t STScanCode;
	ST_Key* stkey;
	int symkey = sdlkey->sym;
	int modkey = sdlkey->mod;

	LOG_TRACE(TRACE_KEYMAP, "key down: sym=%i scan=%i mod=0x%x name='%s'\n",
	          symkey, sdlkey->scancode, modkey, Keymap_GetKeyName(symkey));

	if (ShortCut_CheckKeys(modkey, symkey, true))
		return;

	/* If using joystick emulation via keyboard, DON'T send keys to keyboard processor!!!
	 * Some games use keyboard as pause! */
	if (Joy_KeyDown(symkey, modkey))
		return;

	/* Ignore modifier keys that are not passed to the ST */
	if (!IsKeyTranslatable(symkey))
		return;

	stkey = Keymap_RemapKeyToSTKey(sdlkey);
	if (stkey == NULL)
		return;

	STScanCode = stkey->scancode;
	LOG_TRACE(TRACE_KEYMAP, "key map: sym=0x%x to ST-scan=0x%02x\n", symkey, STScanCode);

	assert(Keyboard.KeyStates[scancode] == 0);
	if (InsertModifiers(stkey->mods, true))
	{
		bool done = InsertAltXXXDigits(stkey);
		LOG_TRACE(TRACE_KEYMAP, "\n");
		if (done)
			return;
	}
	if (!Keyboard.KeyStates[STScanCode])
	{
		/* Set down */
		Keyboard.KeyStates[STScanCode]++;
		IKBD_PressSTKey(STScanCode, true);
	}
}


/*-----------------------------------------------------------------------*/
/**
 * User released a key
 */
void Keymap_KeyUp(const SDL_Keysym *sdlkey)
{
	uint8_t STScanCode;
	ST_Key* stkey;
	int symkey = sdlkey->sym;
	int modkey = sdlkey->mod;

	LOG_TRACE(TRACE_KEYMAP, "key up: sym=%i scan=%i mod=0x%x name='%s'\n",
	          symkey, sdlkey->scancode, modkey, Keymap_GetKeyName(symkey));

	/* Ignore short-cut keys here */
	if (ShortCut_CheckKeys(modkey, symkey, false))
		return;

	/* If using keyboard emulation, DON'T send keys to keyboard processor!!!
	 * Some games use keyboard as pause! */
	if (Joy_KeyUp(symkey, modkey))
		return;

	/* Ignore modifier keys that are not passed to the ST */
	if (!IsKeyTranslatable(symkey))
		return;

	stkey = &KeysDown[sdlkey->scancode];
	if (stkey == NULL)
	{
		Log_Printf(LOG_ERROR, "No mapping for key %d found!\n", sdlkey->scancode);
		return;
	}

	/* Release key (only if was pressed) */
	STScanCode = stkey->scancode;
	assert(Keyboard.KeyStates[STScanCode] > 0);
	if (Keyboard.KeyStates[STScanCode])
	{
		IKBD_PressSTKey(STScanCode, false);
		Keyboard.KeyStates[STScanCode]--;
	}
	if (InsertModifiers(stkey->mods, false))
	{
		LOG_TRACE(TRACE_KEYMAP, "\n");
	}
	/* Trace state of ST modifiers keys */
	LOG_TRACE(TRACE_KEYMAP,"  LSHIFT:%d RSHIFT:%d CTRL:%d ALT:%d\n",
		  Keyboard.KeyStates[ST_LSHIFT],
		  Keyboard.KeyStates[ST_RSHIFT],
		  Keyboard.KeyStates[ST_CONTROL],
		  Keyboard.KeyStates[ST_ALTERNATE]);
}

/*-----------------------------------------------------------------------*/
/**
 * Simulate press or release of a key corresponding to given character
 */
void Keymap_SimulateCharacter(char asckey, bool press)
{
	SDL_Keysym sdlkey;

	sdlkey.mod = KMOD_NONE;
	sdlkey.scancode = 0;
	if (isupper((unsigned char)asckey))
	{
		if (press)
		{
			sdlkey.sym = SDLK_LSHIFT;
			Keymap_KeyDown(&sdlkey);
		}
		sdlkey.sym = tolower((unsigned char)asckey);
		sdlkey.mod = KMOD_LSHIFT;
	}
	else
	{
		sdlkey.sym = asckey;
	}
	if (press)
	{
		Keymap_KeyDown(&sdlkey);
	}
	else
	{
		Keymap_KeyUp(&sdlkey);
		if (isupper((unsigned char)asckey))
		{
			sdlkey.sym = SDLK_LSHIFT;
			Keymap_KeyUp(&sdlkey);
		}
	}
}


/**
 * Maps a key name to its SDL keycode
 */
int Keymap_GetKeyFromName(const char *name)
{
	return SDL_GetKeyFromName(name);
}


/**
 * Maps an SDL keycode to a name
 */
const char *Keymap_GetKeyName(int keycode)
{
	if (!keycode)
		return "";

	return SDL_GetKeyName(keycode);
}


/**
 * Informs symbolic keymap of loaded TOS country.
 */
void Keymap_SetCountry(int countrycode)
{
	uint8_t (*func)(const SDL_Keysym* pKeySym);

	/* Prefer keyboard layout selected by user */
	if (ConfigureParams.Keyboard.nKbdLayout >= 0 &&
	    ConfigureParams.Keyboard.nKbdLayout <= 31)
	{
		countrycode = ConfigureParams.Keyboard.nKbdLayout;
	}
	else if (countrycode == TOS_LANG_ALL)
	{
		if (NvRam_Present())
		{
			countrycode = NvRam_GetKbdLayoutCode();
		}
		else if (ConfigureParams.Keyboard.nCountryCode >= 0 &&
		         ConfigureParams.Keyboard.nCountryCode <= 31)
		{
			countrycode = ConfigureParams.Keyboard.nCountryCode;
		}
	}

	switch (countrycode)
	{
	 case TOS_LANG_DE:    func = Keymap_SymbolicToStScanCode_DE; break;
	 case TOS_LANG_FR:    func = Keymap_SymbolicToStScanCode_FR; break;
	 case TOS_LANG_UK:    func = Keymap_SymbolicToStScanCode_UK; break;
	 case TOS_LANG_ES:    func = Keymap_SymbolicToStScanCode_ES; break;
	 case TOS_LANG_IT:    func = Keymap_SymbolicToStScanCode_IT; break;
	 case TOS_LANG_FI:    /* Finish seems to be the same as Swedish */
	 case TOS_LANG_SE:    func = Keymap_SymbolicToStScanCode_SE; break;
	 case TOS_LANG_CH_FR:
	 case TOS_LANG_CH_DE: func = Keymap_SymbolicToStScanCode_CH; break;
	 case TOS_LANG_NO:    func = Keymap_SymbolicToStScanCode_NO; break;
	 case TOS_LANG_DK:    func = Keymap_SymbolicToStScanCode_DK; break;
	 case TOS_LANG_NL:    func = Keymap_SymbolicToStScanCode_NL; break;
	 default: func = Keymap_SymbolicToStScanCode_default; break;
	}

	Keymap_SymbolicToStScanCode = func;
}
