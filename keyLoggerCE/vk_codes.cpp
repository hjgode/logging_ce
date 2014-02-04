//vkcodes.cpp

#include <windows.h>
#include "vk_codes.h"

const char* vkStringTable[] = {
	"[NOTDEF]",	//0x00,	 
	"[LBUTTON]",	//0x01,	
	"[RBUTTON]",	//0x02,	
	"[CANCEL]",	//0x03,	
	"[MBUTTON]",	//0x04,	
	"[0x05]",	//0x05,	
	"[0x06]",	//0x06,	
	"[0x07]",	//0x07,	
	"[BACK]",	//0x08,	
	"[TAB]",	//0x09,	
	"[0x0A]",	//0x0A,	
	"[0x0B]",	//0x0B,	
	"[CLEAR]",	//0x0C,	
	"[RETURN]",	//0x0D,	
	"[0x0E]",	//0x0E,	
	"[0x0F]",	//0x0F,	
	"[SHIFT]",	//0x10,	
	"[CONTROL]",	//0x11,	
	"[MENU]",	//0x12,	
	"[PAUSE]",	//0x13,	
	"[CAPITAL]",	//0x14,	
	"[HANGUL]",	//0x15,	
	"[0x16]",	//0x16,	
	"[JUNJA]",	//0x17,	
	"[FINAL]",	//0x18,	
	"[KANJI]",	//0x19,	
	"[0x1A]",	//0x1A,	
	"[ESCAPE]",	//0x1B,	
	"[CONVERT]",	//0x1C,	
	"[NOCONVERT]",	//0x1D,	
	"[0x1E]",	//0x1E,	
	"[0x1F]",	//0x1F,	
	" ",	//0x20,	// "SPACE"
	"[PRIOR]",	//0x21,	
	"[NEXT]",	//0x22,	
	"[END]",	//0x23,	
	"[HOME]",	//0x24,	
	"[LEFT]",	//0x25,	
	"[UP]",	//0x26,	
	"[RIGHT]",	//0x27,	
	"[DOWN]",	//0x28,	
	"[SELECT]",	//0x29,	
	"[PRINT]",	//0x2A,	
	"[EXECUTE]",	//0x2B,	
	"[SNAPSHOT]",	//0x2C,	
	"[INSERT]",	//0x2D,	
	"[DELETE]",	//0x2E,	
	"[HELP]",	//0x2F,	
	"0",	//0x30,	"VK_0",
	"1",	//0x31,	"VK_1",
	"2",	//0x32,	"VK_2",
	"3",	//0x33,	"VK_3",
	"4",	//0x34,	"VK_4",
	"5",	//0x35,	"VK_5",
	"6",	//0x36,	"VK_6",
	"7",	//0x37,	"VK_7",
	"8",	//0x38,	"VK_8",
	"9",	//0x39,	"VK_9",
	"[0x3A]",	//0x3A,	
	"[0x3B]",	//0x3B,	
	"[0x3C]",	//0x3C,	
	"[0x3D]",	//0x3D,	
	"[0x3E]",	//0x3E,	
	"[0x3F]",	//0x3F,	
	"[0x40]",	//0x40,	
	"a",	//0x41,	VK_A
	"b",	//0x42,	VK_B
	"c",	//0x43,	VK_C
	"d",	//0x44,	VK_D
	"e",	//0x45,	VK_E
	"f",	//0x46,	VK_F
	"g",	//0x47,	VK_G
	"h",	//0x48,	VK_H
	"i",	//0x49,	VK_I
	"j",	//0x4A,	VK_J
	"k",	//0x4B,	VK_K
	"l",	//0x4C,	VK_L
	"m",	//0x4D,	VK_M
	"n",	//0x4E,	VK_N
	"o",	//0x4F,	VK_O
	"p",	//0x50,	VK_P
	"q",	//0x51,	VK_Q
	"r",	//0x52,	VK_R
	"s",	//0x53,	VK_S
	"t",	//0x54,	VK_T
	"u",	//0x55,	VK_U
	"v",	//0x56,	VK_V
	"w",	//0x57,	VK_W
	"x",	//0x58,	VK_X
	"y",	//0x59,	VK_Y
	"z",	//0x5A,	VK_Z
	"[LWIN]",	//0x5B,	
	"[RWIN]",	//0x5C,	
	"[APPS]",	//0x5D,	
	"[0x5E]",	//0x5E,	
	"[SLEEP]",	//0x5F,	
	"[NUMPAD0]",	//0x60,	
	"[NUMPAD1]",	//0x61,	
	"[NUMPAD2]",	//0x62,	
	"[NUMPAD3]",	//0x63,	
	"[NUMPAD4]",	//0x64,	
	"[NUMPAD5]",	//0x65,	
	"[NUMPAD6]",	//0x66,	
	"[NUMPAD7]",	//0x67,	
	"[NUMPAD8]",	//0x68,	
	"[NUMPAD9]",	//0x69,	
	"[MULTIPLY]",	//0x6A,	
	"[ADD]",	//0x6B,	
	"[SEPARATOR]",	//0x6C,	
	"[SUBTRACT]",	//0x6D,	
	"[DECIMAL]",	//0x6E,	
	"[DIVIDE]",	//0x6F,	
	"[F1]",	//0x70,	VK_F1"
	"[F2]",	//0x71,	VK_F2"
	"[F3]",	//0x72,	VK_F3"
	"[F4]",	//0x73,	VK_F4"
	"[F5]",	//0x74,	VK_F5"
	"[F6]",	//0x75,	VK_F6"
	"[F7]",	//0x76,	VK_F7"
	"[F8]",	//0x77,	VK_F8"
	"[F9]",	//0x78,	VK_F9"
	"[F10]",	//0x79,	VK_F10
	"[F11]",	//0x7A,	VK_F11
	"[F12]",	//0x7B,	VK_F12
	"[F13]",	//0x7C,	VK_F13
	"[F14]",	//0x7D,	VK_F14
	"[F15]",	//0x7E,	VK_F15
	"[F16]",	//0x7F,	VK_F16
	"[F17]",	//0x80,	VK_F17
	"[F18]",	//0x81,	VK_F18
	"[F19]",	//0x82,	VK_F19
	"[F20]",	//0x83,	VK_F20
	"[F21]",	//0x84,	VK_F21
	"[F22]",	//0x85,	VK_F22
	"[F23]",	//0x86,	VK_F23
	"[F24]",	//0x87,	VK_F24
	"[0x88]",	//0x88,	
	"[0x89]",	//0x89,	
	"[0x8A]",	//0x8A,	
	"[0x8B]",	//0x8B,	
	"[0x8C]",	//0x8C,	
	"[0x8D]",	//0x8D,	
	"[0x8E]",	//0x8E,	
	"[0x8F]",	//0x8F,	
	"[NUMLOCK]",	//0x90,	
	"[SCROLL]",	//0x91,	
	"[0x92]",	//0x92,	
	"[0x93]",	//0x93,	
	"[0x94]",	//0x94,	
	"[0x95]",	//0x95,	
	"[0x96]",	//0x96,	
	"[0x97]",	//0x97,	
	"[0x98]",	//0x98,	
	"0x99 (orange plane)",	//0x99,	
	"[0x9A]",	//0x9A,	
	"[0x9B]",	//0x9B,	
	"[0x9C light]",	// 0x9C controls the CN50 screen backlight
	"[0x9D (ok)]",	// cn50 way of doing
	"[0x9E]",	//0x9E,	
	"[0x9F]",	//0x9F,	
	"[LSHIFT]",	//0xA0,	
	"[RSHIFT]",	//0xA1,	
	"[LCONTROL]",	//0xA2,	
	"[RCONTROL]",	//0xA3,	
	"[LMENU]",	//0xA4,	
	"[RMENU]",	//0xA5,	
	"[BROWSER_BACK]",	//0xA6,	
	"[BROWSER_FORWARD]",	//0xA7,	
	"[BROWSER_REFRESH]",	//0xA8,	
	"[BROWSER_STOP]",	//0xA9,	
	"[BROWSER_SEARCH]",	//0xAA,	
	"[BROWSER_FAVORITES]",	//0xAB,	
	"[BROWSER_HOME]",	//0xAC,	
	"[VOLUME_MUTE]",	//0xAD,	
	"[VOLUME_DOWN]",	//0xAE,	
	"[VOLUME_UP]",	//0xAF,	
	"[MEDIA_NEXT_TRACK]",	//0xB0,	
	"[MEDIA_PREV_TRACK]",	//0xB1,	
	"[MEDIA_STOP]",	//0xB2,	
	"[MEDIA_PLAY_PAUSE]",	//0xB3,	
	"[LAUNCH_MAIL]",	//0xB4,	
	"[LAUNCH_MEDIA_SELECT]",	//0xB5,	
	"[LAUNCH_APP1]",	//0xB6,	
	"[LAUNCH_APP2]",	//0xB7,	
	"[0xB8]",	//0xB8,	
	"[0xB9]",	//0xB9,	
	";",//"SEMICOLON",	//0xBA,	
	"=",//"EQUAL",	//0xBB,	
	",",//"COMMA",	//0xBC,	
	"-",//"HYPHEN",	//0xBD,	
	".",//"PERIOD",	//0xBE,	
	"/",//"SLASH",	//0xBF,	
	"`",//"BACKQUOTE",	//0xC0,	
	"[APP1]",	//0xC1,	
	"[APP2]",	//0xC2,	
	"[APP3]",	//0xC3,	
	"[APP4]",	//0xC4,	
	"[APP5]",	//0xC5,	
	"[APP6]",	//0xC6,	
	"[0xC7]",	//0xC7,	
	"[0xC8]",	//0xC8,	
	"[0xC9]",	//0xC9,	
	"[0xC0]",	//0xC0,	
	"[0xCA]",	//0xCA,	
	"[0xCB]",	//0xCB,	
	"[0xCC]",	//0xCC,	
	"[0xCD]",	//0xCD,	
	"[0xCE]",	//0xCE,	
	"[0xCF]",	//0xCF,	
	"[0xD0]",	//0xD0,	
	"[0xD1]",	//0xD1,	
	"[0xD2]",	//0xD2,	
	"[0xD3]",	//0xD3,	
	"[0xD4]",	//0xD4,	
	"[0xD5]",	//0xD5,	
	"[0xD6]",	//0xD6,	
	"[0xD7]",	//0xD7,	
	"[0xD8]",	//0xD8,	
	"[0xD9]",	//0xD9,	
	"[0xDA]",	//0xDA,	
	"[",//"LBRACKET",	//0xDB,	
	"\\",//"BACKSLASH",	//0xDC,	
	"]",//"RBRACKET",	//0xDD,	
	"'",//"APOSTROPHE",	//0xDE,	
	"[OFF]",	//0xDF,	
	"[0xE0]",	//0xE0,	
	"0xE1 (<scan>)",	// see CN50 dumps
	"[EXTEND_BSLASH]",	//0xE2,	
	"[0xE3]",	//0xE3,	
	"[0xE4]",	//0xE4,	
	"[PROCESSKEY]",	//0xE5,	
	"[0xE6]",	//0xE6,	
	"[0xE7]",	//0xE7,	
	"[0xE8]",	//0xE8,	
	"[0xE9]",	//0xE9,	
	"[0xEA]",	//0xEA,	
	"[0xEB]",	//0xEB,	
	"[0xEC]",	//0xEC,	
	"[0xED]",	//0xED,	
	"[0xEE]",	//0xEE,	
	"[0xEF]",	//0xEF,	
	"[0xF0]",	//0xF0,	
	"[0xF1]",	//0xF1,	
	"[0xF2]",	//0xF2,	
	"[0xF3]",	//0xF3,	
	"[0xF4]",	//0xF4,	
	"[0xF5]",	//0xF5,	
	"[ATTN]",	//0xF6,	
	"[CRSEL]",	//0xF7,	
	"[EXSEL]",	//0xF8,	
	"[EREOF]",	//0xF9,	
	"[PLAY]",	//0xFA,	
	"[ZOOM]",	//0xFB,	
	"[NONAME]",	//0xFC,	
	"[PA1]",	//0xFD,	
	"[OEM_CLEAR]",	//0xFE,	
	"[0xFF]",	//0xFF,	
};

int getShiftedCode(int iCode){
	int iShiftedCode=-1;
	switch(iCode){
		case	'1': iShiftedCode='!' ; break;  //"VK_1"
		case	'2': iShiftedCode='@' ; break;  //"VK_2"
		case	'3': iShiftedCode='#' ; break;  //"VK_3"
		case	'4': iShiftedCode='$' ; break;  //"VK_4"
		case	'5': iShiftedCode='%' ; break;  //"VK_5"
		case	'6': iShiftedCode='^' ; break;  //"VK_6"
		case	'7': iShiftedCode='&' ; break;  //"VK_7"
		case	'8': iShiftedCode='*' ; break;  //"VK_8"
		case	'9': iShiftedCode='(' ; break;  //"VK_9"
		case	'0': iShiftedCode=')' ; break;  //"VK_0"
		case	';': iShiftedCode=':' ; break;  //"VK_SEMICOLON"
		case	'=': iShiftedCode='+' ; break;  //"VK_EQUAL"
		case	',': iShiftedCode='<' ; break;  //"VK_COMMA"
		case	'-': iShiftedCode='_' ; break;  //"VK_HYPHEN"
		case	'.': iShiftedCode='>' ; break;  //"VK_PERIOD"
		case	'/': iShiftedCode='?' ; break;  //"VK_SLASH"
		case	0xC0: iShiftedCode='~' ; break;  //"VK_BACKQUOTE"
		case	'[': iShiftedCode='{' ; break;  //"VK_LBRACKET"
		case	'\\': iShiftedCode='|' ; break;  //"VK_BACKSLASH"
		case	']': iShiftedCode='}' ; break;  //"VK_RBRACKET"
		case	'\'': iShiftedCode='\"'; break; //"VK_APOSTROPHE"
		default: iShiftedCode=-1;
	}
	return iShiftedCode;
}

char* getVKString(char* vkString, int vkInt, BOOL bShift){
	DEBUGMSG(1,(L"in getVKString: in=0x%02x (%03i), shift=%i\n", vkInt, vkInt, bShift));

	//lower upper case letters
	if (vkInt>=0x41 && vkInt<=0x5A)	//these are upper case always
		if(!bShift)
			sprintf	( vkString, "%s", vkStringTable[vkInt]);	//our table has lower case only
		else{
			sprintf	( vkString, "%c", _toupper(vkInt)  );
		}
	//shifted unshifted codes we know
	else if (getShiftedCode(vkInt)!=-1)
		if(bShift)
			sprintf	( vkString, "%s", vkStringTable[getShiftedCode(vkInt)] );
		else
			sprintf	( vkString, "%s", vkStringTable[vkInt] );
	else if (vkInt=='\r')
		sprintf	( vkString, "%s\n", vkStringTable[vkInt] );	//add a new line if code is return
	else
		sprintf	( vkString, "%s", vkStringTable[vkInt] );		//normal output

	return vkString;
}
