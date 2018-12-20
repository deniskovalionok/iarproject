/*
graphic.h
*/

#ifndef _GRAPHIC_H
#define _GRAPHIC_H

#include "lcd.h"
#include "types.h"



extern const BYTE fnt8x8[];
extern const BYTE fnt4x8[];
extern const BYTE fnt32x32[];
extern const BYTE fnt10x24[];

#define screenHigh (lcdbuf)


#define SPACE_WIDTH 2


#define X_SCREEN_SIZE LCD_WIDTH
#define X_SCREEN_MAX X_SCREEN_SIZE-1

extern uint8_t Y_SCREEN_SIZE;
extern uint8_t Y_SCREEN_MAX ;
extern DWORD screenHigh2;



enum ENUM_TEXT_JUSTIFY
{
	NONE=0x10000,
	LEFT,
	CENTER,
	RIGHT
};

//display data structures///////////////////////////////////
struct tagTextData
{
	int gstrX;//text position in window coordinates
	int gstrY;
	int stepY;//steps for Y and X position changing (these are addons for symbol size)
	int stepX;
	int winX;//text output window
	int winY;
	int winSX;//window size
	int winSY;
	BOOL bWrap;	//if 1 then symbol wrapping on next line esle no
	BOOL bTextLineClear;	//if 1 then every new line of text will be cleared before displaying
	enum ENUM_TEXT_JUSTIFY justify;	//text justify on screen
	COLORREF color;
	const BYTE* pFont;
};


struct tagGraphicData
{
	struct tagTextData text;
	BOOL bLEDR;	//if true then LED is ON
	BOOL bLEDG;	//if true then LED is ON
	BOOL bLEDB;	//if true then LED is ON
	BOOL bNeedRefresh;
	BOOL bLCDON;
	int iLang;	//language index
	volatile BOOL bGraphic_SSP_Busy;	//must be set to TRUE before initialization of SSP for use, must be set to FALSE after stop using of SSP else exception occures
};

extern struct tagGraphicData graphic;








void Graphic_setTextJustify(enum ENUM_TEXT_JUSTIFY justify);

int Graphic_getRightStartPos(const char* pText);
int Graphic_getLeftStartPos(const char* pText);

int Graphic_getWordLen(const char* pText);

void Graphic_setTextWrap(BOOL wrap);
void Graphic_setTextSteps(int stepX, int stepY);
void Graphic_setTextXY(int x, int y);
void Graphic_setTextWin(int X,int Y,int SX,int SY);

//increment of gstrX of text functions
int Graphic_inc_textX(int x);


const BYTE* Graphic_getSymFontOffset(char symbol);
UINT Graphic_getFontSizeY(void);
const BYTE* Graphic_getCurrentFont(void);
void Graphic_setCurrentFont(const BYTE* pFont);
int Graphic_showSymbol(char symbol);

//void Graphic_setTextColor(COLORREF clr);

//void Graphic_testForNextLine(char symbol);
void Graphic_gotoNextLine(void);

//вывод текста
void Graphic_outputText(const char* pText);

char* Graphic_getClockDateTimeStr(char* pStr);

void Graphic_setTextLineClear(BOOL bClear);



void Graphic_drawLine(int x1, int y1, int x2, int y2, COLORREF clr);
void Graphic_drawLine_xor(int x1, int y1, int x2, int y2, COLORREF clr);

void Graphic_clearTextWin(int vsync);
void Graphic_outputTextByLang(const char* pText);
void Graphic_drawRect(int x1, int y1, int x2, int y2, COLORREF clr);
void Graphic_drawRect_xor(int x1, int y1, int x2, int y2, COLORREF clr);

void Graphic_drawVLine_xor(int x, int y1, int y2, COLORREF clr);
void Graphic_drawVLine(int x, int y1, int y2, COLORREF clr);
void Graphic_drawHLine(int x1, int y, int x2, COLORREF clr);
void Graphic_drawHLine_xor(int x1, int y, int x2, COLORREF clr);
const char* Graphic_getTextByLang(const char* pText);

void Graphic_checkForClearLine(void);
void Graphic_clearUserPart(void);

void Graphic_outputTextByLang_log(const char* pText);

//get symbol width in pixels
UINT Graphic_getSymWidth(char symbol);
void drawSymbol(BYTE x, BYTE y, char sym, BYTE mask);
void Graphic_outputTextFromLine(const char* pText, int lineIndex);
int Graphic_calcTextLines(const char* pText);
__interwork void Graphic_clearRect(RECT rect, int vsync);

void Graphic_dot(int x, int y, COLORREF clr);

#endif