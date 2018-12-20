/*
graphic.c
*/

#include "graphic.h"
#include "lcd.h"
#include <string.h>



//точки уложены битами в байтах с лева на право (байты), т.е. байт одна верт линия или ее половина в зависимости от размера шрифта
const BYTE fnt8x8[] = {
#include "fntSmall.h"
};

const BYTE fnt10x24[] = {
#include "10x24.h"
};

const BYTE fnt4x8[] = {
#include "fntSuperSmall.h"
};

const BYTE fnt32x32[] = {
#include "fntBig.h"
};


struct tagGraphicData graphic;

uint8_t Y_SCREEN_SIZE;
uint8_t Y_SCREEN_MAX;
DWORD screenHigh2;



void drawSymbol(BYTE x, BYTE y, char sym, BYTE mask)
{
	BYTE shft;
	DWORD vGpx;
	DWORD vmask;	//значение вертикальной маски символов данного знакогенератора
	BYTE vmask1;	//значение вертикальной маски символов данного знакогенератора если huge
	BYTE vmask2;	//значение вертикальной маски символов данного знакогенератора если huge
	BYTE vmask3;	//значение вертикальной маски символов данного знакогенератора если huge
	BYTE vmask4;	//значение вертикальной маски символов данного знакогенератора если huge
	BYTE width;	//ширина символа, а для большого это двойная ширина с начала
	DWORD graph[32];
	BYTE dontOut2;
	BYTE dontOut3;
	BYTE dontOut4;
	const BYTE * pFontData;
	BYTE fontLarge;	//!=0 if font large
	int i,ilim;
	BYTE ay;
	BYTE* pScreen;

	if(y>=Y_SCREEN_SIZE)return;
	if(x>=X_SCREEN_SIZE)return;

	dontOut2 = 0;
	dontOut3 = 0;
	dontOut4 = 0;

	
/*	
	;sym,x,y
	;mask -
	;0 =1 invert
	;1 =1 bold
	;2 =1 italic
	;4 =1 understrike
	;8 =1 future
	;16 =1 huge
*/

	;/////////////////////////////////////////////////////////////////


//	;копируем шрифт символа в буфер
//	;определяем адрес символа в таблице шрифта

	pFontData = Graphic_getCurrentFont();
	fontLarge = *pFontData;
	vmask = *(pFontData+3);
	if(fontLarge>0)
	{
		vmask<<=8;
		vmask |= *(pFontData+4);
	}
	if(fontLarge>1)
	{
		vmask<<=8;
		vmask |= *(pFontData+5);
	}


	pFontData = Graphic_getSymFontOffset(sym);
	
	width = (*pFontData++)-1;
	if(fontLarge)
		width/=(fontLarge+1);

	
	if(width==0)
	{
		width = SPACE_WIDTH;
		ilim = width*2;
		if(ilim>32)ilim = 32;
		for(i=0;i<ilim;i++)graph[i]=0;
	}else
	{
		if(width>32)
			width = 32;
		ilim = width;
		if(ilim>32)ilim = 32;
		if(fontLarge==2)
		{
			for(i=0;i<ilim;i++)
			{
				graph[i]=(*pFontData++);
				graph[i]|=(*pFontData++)<<8;
				graph[i]|=(*pFontData++)<<16;
			}
		}else
		if(fontLarge==1)
		{
			for(i=0;i<ilim;i++)
			{
				graph[i]=(*pFontData++);
				graph[i]|=(*pFontData++)<<8;
			}
		}else
		{
			for(i=0;i<ilim;i++)graph[i]=*pFontData++;
		}
		for(;i<32;i++)graph[i]=0;
	}
	//
	//определение адреса в буфере экрана
	
	ay = (y&0xf8)>>3;
	
	
	pScreen = (BYTE*)((screenHigh+(ay<<7))+x);
	
	if((UINT)pScreen>=(screenHigh2))return;	//out of screen
	
	if((UINT)pScreen+LCD_WIDTH>=(screenHigh2))
	{
		dontOut4 = 1;
		dontOut3 = 1;
		dontOut2 = 1;
	}else if((UINT)pScreen+LCD_WIDTH*2>=(screenHigh2))
	{
		dontOut4 = 1;
		dontOut3 = 1;
	}else if((UINT)pScreen+LCD_WIDTH*3>=(screenHigh2))
	{
		dontOut4 = 1;
	}
	

	shft = (y&0x07);
	vmask <<= shft;
	
	vmask=~vmask;
	
	vmask1 = LOBYTE(vmask);
	vmask2 = LO2BYTE(vmask);
	vmask3 = LO3BYTE(vmask);
	vmask4 = HIBYTE(vmask);
	
	
	for(i=0;i<width;i++)
	{
		vGpx = graph[i]<<shft;
		*pScreen = (LOBYTE(vGpx))|(*pScreen & vmask1);
		if(!dontOut2)
		{
			pScreen+=LCD_WIDTH;
			*pScreen = (LO2BYTE(vGpx))|(*pScreen & vmask2);
			if(!dontOut3)
			{
				pScreen+=LCD_WIDTH;
				*pScreen = (LO3BYTE(vGpx))|(*pScreen & vmask3);
				if(!dontOut4)
				{
					pScreen+=LCD_WIDTH;
					*pScreen = (HIBYTE(vGpx))|(*pScreen & vmask4);
					pScreen-=LCD_WIDTH;
				}
				pScreen-=LCD_WIDTH;
			}
			pScreen-=LCD_WIDTH;
		}
		pScreen++;
	}
}








//return text line width
int Graphic_getTextLineWidth(const char* pText)
{
	int x = -graphic.text.stepX;
	char sym;
	while((sym = *pText++))
	{
		if(sym=='\r')
		{//перевод строки
			break;
		}else
		{
			x += Graphic_getSymWidth(sym)+graphic.text.stepX;
		}
	};
	return x;
}

//calculation of center start position for the line
int Graphic_getCenterStartPos(const char* pText)
{
	int startpos = 0;
	int pixlen = Graphic_getTextLineWidth(pText);
	int winlen = graphic.text.winSX;
	if(winlen>pixlen)
	{
		startpos = (winlen-pixlen)/2;
	}
	return startpos;
}


//calculation of center start position for the line
int Graphic_getLeftStartPos(const char* pText)
{
	int startpos = 0;
	return startpos;
}

//calculation of center start position for the line
int Graphic_getRightStartPos(const char* pText)
{
	int startpos = 0;
	int pixlen = Graphic_getTextLineWidth(pText);
	int winlen = graphic.text.winSX;
	startpos = winlen-pixlen;
	return startpos;
}

//justify text
void Graphic_justifyText(const char* pText)
{
	if(graphic.text.justify==CENTER)
		graphic.text.gstrX = Graphic_getCenterStartPos(pText);
	else if(graphic.text.justify==LEFT)
		graphic.text.gstrX = Graphic_getLeftStartPos(pText);
	else if(graphic.text.justify==RIGHT)
		graphic.text.gstrX = Graphic_getRightStartPos(pText);
}


const char* Graphic_getTextByLang(const char* pText)
{
	for(int i=0;i<graphic.iLang;i++)
	{
		pText+=strlen(pText)+1;
	};
	return pText;
}

void Graphic_outputTextByLang(const char* pText)
{
	Graphic_outputText(Graphic_getTextByLang(pText));
}


//ret word len in pix
int Graphic_getWordLen(const char* pText)
{
	int len = 0;
	char sym;
	while((sym = *pText++))
	{
		if(sym==' ' || sym=='\r')break;
		len+=Graphic_getSymWidth(sym)+graphic.text.stepX;
	};
	len-=graphic.text.stepX;
	return len;
}


void Graphic_outputText(const char* pText)
{
	int x;
	char sym;
	int fy = Graphic_getFontSizeY();
	BOOL bWordTested = FALSE;
	Graphic_justifyText(pText);
	while((sym = *pText++))
	{
		if(sym=='\r')
		{//перевод строки
			Graphic_gotoNextLine();
			if((graphic.text.gstrY+fy-1)>graphic.text.winSY)break;
			Graphic_checkForClearLine();
			Graphic_justifyText(pText);
			bWordTested = FALSE;
		}else
		{
			if(graphic.text.bWrap && !bWordTested && sym!=' ')
			{
				bWordTested = TRUE;
				int len = Graphic_getWordLen(pText-1);
				if((graphic.text.gstrX+len)>graphic.text.winSX)
				{
					Graphic_gotoNextLine();
					if((graphic.text.gstrY+fy-1)>graphic.text.winSY)break;
					Graphic_checkForClearLine();
				}
			}
			if(sym==' ')bWordTested = FALSE; //to check next word
			x = Graphic_showSymbol(sym);
			Graphic_inc_textX(x+graphic.text.stepX);
		}
	};
}



/*
calculate amount of text lines in the text
before use it is important to call the next:
	Graphic_setTextWin(0,MODE_USER_TOP,X_SCREEN_SIZE,MODE_USER_HEIGHT);	//set text window
	Graphic_setTextXY(0,0);	//set start coords in window
	Graphic_setTextWrap(1);
	Graphic_setTextSteps(1,1);
	Graphic_setTextJustify(LEFT);
	Graphic_setCurrentFont(fnt6x16);
*/
int Graphic_calcTextLines(const char* pText)
{
	int iTextLines = 1;
	int x;
	char sym;
	//to restore them further
	int xx = graphic.text.gstrX;
	int yy = graphic.text.gstrY;

	int fy = Graphic_getFontSizeY();
	BOOL bWordTested = FALSE;
	Graphic_justifyText(pText);
	while((sym = *pText++))
	{
		if(sym=='\r')
		{//перевод строки
			iTextLines++;
			Graphic_gotoNextLine();
			//пересчет координаты х
			Graphic_justifyText(pText);
			bWordTested = FALSE;
		}else
		{
			if(graphic.text.bWrap && !bWordTested && sym!=' ')
			{
				bWordTested = TRUE;
				int len = Graphic_getWordLen(pText-1);
				if((graphic.text.gstrX+len)>graphic.text.winSX)
				{
					iTextLines++;
					Graphic_gotoNextLine();
					--pText;
//					if((graphic.text.gstrY+fy-1)>graphic.text.winSY)break;
					continue;
				}
			}
			if(sym==' ')bWordTested = FALSE; //to check next word
			x = Graphic_getSymWidth(sym);
			Graphic_inc_textX(x+graphic.text.stepX);
		}
	};
	graphic.text.gstrX = xx;
	graphic.text.gstrY = yy;
	return iTextLines;
}


/*
output text that not depends on language from pointed text line
*/
void Graphic_outputTextFromLine(const char* pText, int lineIndex)
{
	int x;
	char sym;
	int fy = Graphic_getFontSizeY();
	BOOL bWordTested = FALSE;
	Graphic_justifyText(pText);
	while((sym = *pText++))
	{
		if(sym=='\r')
		{//перевод строки
			if(lineIndex==0)
			{//переводим строку если уже нужно выводить текст
				Graphic_gotoNextLine();
				if((graphic.text.gstrY+fy-1)>graphic.text.winSY)break;
				Graphic_checkForClearLine();
			}else//иначе ждем следующей строки текста
			{
				--lineIndex;
				graphic.text.gstrX = 0;	//это нужно для перевода строки, хотя y не меняется
			}
			//пересчет координаты х
			Graphic_justifyText(pText);
			bWordTested = FALSE;
		}else
		{
			BOOL bAllowOutput = (lineIndex==0);
			if(graphic.text.bWrap && !bWordTested && sym!=' ')
			{
				bWordTested = TRUE;
				int len = Graphic_getWordLen(pText-1);
				if((graphic.text.gstrX+len)>graphic.text.winSX)
				{
					if(bAllowOutput)
					{//нужно выводить текст
						Graphic_gotoNextLine();
						if((graphic.text.gstrY+fy-1)>graphic.text.winSY)break;
						Graphic_checkForClearLine();
					}else
					{
						--lineIndex;
						graphic.text.gstrX = 0;
						--pText;	//вернемся на символ назад чтобы начать вывод этого слова
						continue;
					}
				}
			}
			if(sym==' ')bWordTested = FALSE; //to check next word
			if(bAllowOutput)
				x = Graphic_showSymbol(sym);
			else
				x = Graphic_getSymWidth(sym);
			Graphic_inc_textX(x+graphic.text.stepX);
		}
	};
}


//set text justify, must be values from enum ENUM_TEXT_JUSTIFY
void Graphic_setTextJustify(enum ENUM_TEXT_JUSTIFY justify)
{
	graphic.text.justify = justify;
}

//set text window
void Graphic_setTextWin(int X,int Y,int SX,int SY)
{
	if(X+SX>X_SCREEN_SIZE || Y+SY>Y_SCREEN_SIZE)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"Text window is out of bounds");
	}
	graphic.text.winX = X;
	graphic.text.winY = Y;
	graphic.text.winSX = SX;
	graphic.text.winSY = SY;
	Graphic_setTextXY(0,0);	//set start coords in window from 0,0
}

//clear RECT that is match to text window
void Graphic_clearTextWin(int vsync)
{
	RECT rect={graphic.text.winX,graphic.text.winY,graphic.text.winX+graphic.text.winSX-1,graphic.text.winY+graphic.text.winSY-1};
	Graphic_clearRect(rect, vsync);
}


void Graphic_checkForClearLine(void)
{
	if(graphic.text.bTextLineClear)
	{//must clear the line
		int sz = Graphic_getFontSizeY()+graphic.text.stepY;
		int gstrY;
		gstrY = graphic.text.winY+graphic.text.gstrY;
		if(gstrY>Y_SCREEN_SIZE-sz)return;
		RECT rect={graphic.text.winX,gstrY,graphic.text.winX+graphic.text.winSX-1,gstrY+sz-1};
		Graphic_clearRect(rect, gstrY+16);
	}
}

void Graphic_gotoNextLine(void)
{
	graphic.text.gstrX = 0;
	int sz = Graphic_getFontSizeY()+graphic.text.stepY;
	graphic.text.gstrY += sz;
}

//set text wrap
void Graphic_setTextWrap(BOOL wrap)
{
	graphic.text.bWrap = wrap;
}

//set incrementing step values
void Graphic_setTextSteps(int stepX, int stepY)
{
	graphic.text.stepX = stepX;
	graphic.text.stepY = stepY;
}


//increment of gstrX of text functions
int Graphic_inc_textX(int x)
{
	graphic.text.gstrX+=x;
	return graphic.text.gstrX;
}

void Graphic_setTextXY(int x, int y)
{
	graphic.text.gstrX = x;
	graphic.text.gstrY = y;
}

void Graphic_setCurrentFont(const BYTE* pFont)
{
	graphic.text.pFont = pFont;
}

const BYTE* Graphic_getCurrentFont(void)
{
	return graphic.text.pFont;
}

const BYTE* Graphic_getSymFontOffset(char symbol)
{
	BYTE bLarge;
	const BYTE* pFont = Graphic_getCurrentFont();
	int si = symbol;
	int startsi = *(pFont+1);
	bLarge = *pFont;
	pFont+=8;
	if(bLarge>0)
		pFont+=5;
	if(bLarge>1)
		pFont+=5;
	for(int i=startsi;i<si;i++)
	{
		pFont+=*pFont;
	}
	return pFont;
}


//get symbol width in pixels
UINT Graphic_getSymWidth(char symbol)
{
	BYTE bLarge;
	const BYTE* pFont = Graphic_getCurrentFont();
	int width;
	bLarge = *pFont;
	pFont = Graphic_getSymFontOffset(symbol);
	width = *pFont - 1;
	if(bLarge>0)
		width/=(bLarge+1);
	if(width==0)
		width = SPACE_WIDTH;
	return width;
}

//get current font x size
//должно быть кратно 8
UINT Graphic_getFontSizeY(void)
{
	int fsy = 8;
	if(graphic.text.pFont==fnt8x8)
	{
		fsy = 8;
	}else
	if(graphic.text.pFont==fnt4x8)
	{
		fsy = 8;
	}else
	if(graphic.text.pFont==fnt32x32)
	{
		fsy = 12;
	}else
	if(graphic.text.pFont==fnt10x24)
	{
		fsy = 24;
	}
	return fsy;
}

//ret symbol width
//show symbol
int Graphic_showSymbol(char symbol)
{
	int gstrX, gstrY;
	gstrX = graphic.text.gstrX+graphic.text.winX;
	gstrY = graphic.text.winY+graphic.text.gstrY;
	if(gstrX>=X_SCREEN_SIZE || gstrY>=Y_SCREEN_SIZE)return 0;

	
	drawSymbol(gstrX, gstrY, symbol, 0x0);
	
	return Graphic_getSymWidth(symbol);
}



void Graphic_drawRect(int x1, int y1, int x2, int y2, COLORREF clr)
{
	Graphic_drawHLine(x1, y1, x2, clr);
	Graphic_drawHLine(x1, y2, x2, clr);
	Graphic_drawVLine(x1, y1, y2, clr);
	Graphic_drawVLine(x2, y1, y2, clr);
}


void Graphic_drawRect_xor(int x1, int y1, int x2, int y2, COLORREF clr)
{
	Graphic_drawHLine_xor(x1, y1, x2, clr);
	Graphic_drawHLine_xor(x1, y2, x2, clr);
	Graphic_drawVLine_xor(x1, y1, y2, clr);
	Graphic_drawVLine_xor(x2, y1, y2, clr);
}



//draw vert line
void Graphic_drawVLine(int x, int y1, int y2, COLORREF clr)
{
	int j;
	DWORD high, startChunkY, endChunkY;
	DWORD startMask, endMask;
	BYTE* scrAddr;
	
	if(x<0 || x>X_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid x argument of line!");
	if(y1<0 || y1>Y_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid y1 argument of line!");
	if(y2<0 || y2>Y_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid y2 argument of line!");
	
	int y;
	if(y1>y2)
	{
		y=y1;
		y1=y2;
		y2=y;
	}

	startChunkY = y1>>3;
	endChunkY = y2>>3;
	
	high = endChunkY-startChunkY+1;	//high in chunk
	
	startMask = 0xff;
	startMask<<=(y1&7);

	scrAddr = (BYTE*)(screenHigh+(startChunkY<<7))+x;
	endMask = 0x00;
	
	for(j=0;j<high-1;j++)
	{
		*scrAddr |= startMask;
		scrAddr+=LCD_WIDTH;
		startMask = 0xff;
	}
	
	endMask = 0xff;
	endMask<<=((y2&7)+1);
	endMask=~endMask;
	*scrAddr |= startMask&endMask;
}



//draw horz line
void Graphic_drawHLine(int x1, int y, int x2, COLORREF clr)
{
	int x;

	if(y<0 || y>Y_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid y argument of line!");
	if(x1<0 || x1>X_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid x1 argument of line!");
	if(x2<0 || x2>X_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid x2 argument of line!");

	if(x1>x2)
	{
		x=x1;
		x1=x2;
		x2=x;
	}

	BYTE startChunkY = y>>3;
	DWORD len = x2-x1+1;
	BYTE* scrAddr = (BYTE*)(screenHigh+(startChunkY<<7))+x1;
	BYTE mask = 0x01<<(y&0x7);

	if(len>0)
	{
		do
		{
			*scrAddr|=mask;
			scrAddr++;
		}while(--len);
	}
	
}



//draw vert line by xor
void Graphic_drawVLine_xor(int x, int y1, int y2, COLORREF clr)
{
	int j;
	DWORD high, startChunkY, endChunkY;
	DWORD startMask, endMask;
	BYTE* scrAddr;
	
	if(x<0 || x>X_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid x argument of line!");
	if(y1<0 || y1>Y_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid y1 argument of line!");
	if(y2<0 || y2>Y_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid y2 argument of line!");
	
	int y;
	if(y1>y2)
	{
		y=y1;
		y1=y2;
		y2=y;
	}

	startChunkY = y1>>3;
	endChunkY = y2>>3;
	
	high = endChunkY-startChunkY+1;	//high in chunk
	
	startMask = 0xff;
	startMask<<=(y1&7);

	scrAddr = (BYTE*)(screenHigh+(startChunkY<<7))+x;
	endMask = 0x00;
	
	for(j=0;j<high-1;j++)
	{
		*scrAddr ^= startMask;
		scrAddr+=LCD_WIDTH;
		startMask = 0xff;
	}
	
	endMask = 0xff;
	endMask<<=((y2&7)+1);
	endMask=~endMask;
	*scrAddr ^= startMask&endMask;
}








//draw horz line by xor
void Graphic_drawHLine_xor(int x1, int y, int x2, COLORREF clr)
{
	int x;
	
	if(y<0 || y>Y_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid y argument of line!");
	if(x1<0 || x1>X_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid x1 argument of line!");
	if(x2<0 || x2>X_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid x2 argument of line!");
	
	if(x1>x2)
	{
		x=x1;
		x1=x2;
		x2=x;
	}

	BYTE startChunkY = y>>3;
	DWORD len = x2-x1+1;
	BYTE* scrAddr = (BYTE*)(screenHigh+(startChunkY<<7))+x1;
	BYTE mask = 0x01<<(y&0x7);

	if(len>0)
	{
		do
		{
			*scrAddr^=mask;
			scrAddr++;
		}while(--len);
	}

}


void Graphic_dot(int x, int y, COLORREF clr)
{
	int adr = (y>>3)*LCD_WIDTH+x;
	if(adr>=LCDBUFSIZE)return;	//failed
	unsigned char byte = 1;
	y = y&0x07;
	if(y)
		byte = 1<<y;
	lcdbuf[adr] |= byte;//pixel color
}





//draw line
void Graphic_drawLine(int x1, int y1, int x2, int y2, COLORREF clr)
{
	int e,denom,yinc = 1;

	if(x1<0 || x1>X_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid x1 argument of line!");
	if(x2<0 || x2>X_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid x2 argument of line!");
	if(y1<0 || y1>Y_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid y1 argument of line!");
	if(y2<0 || y2>Y_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid y2 argument of line!");



	if(x2<x1)
	{//перенапрявляем рисование слева на право
		e=x2;x2=x1;x1=e;
		e=y2;y2=y1;y1=e;
	}
	x2 = x2-x1; y2 = y2-y1;
	if(y2<0){yinc = -1; y2 = -y2;}
	if(y2>x2)
	{
		e=x2; x2=y2; y2=e;
		denom = x2<<1;
		y2 = y2<<1;
		e = -x2;
		while(x2-->=0)
		{
			Graphic_dot(x1, y1, clr);
			if((e+=y2)>0)
			{
				x1++;
				e-=denom;
			}
			y1+=yinc;
		};
	}else
	{
		denom = x2<<1;
		y2 = y2<<1;
		e = -x2;
		while(x2-->=0)
		{
			Graphic_dot(x1, y1, clr);
			if((e+=y2)>0)
			{
				y1+=yinc;
				e-=denom;
			}
			x1++;
		};
	}
}

void Graphic_drawLine_xor(int x1, int y1, int x2, int y2, COLORREF clr)
{
	int e,denom,yinc = 1;

	if(x1<0 || x1>X_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid x1 argument of line!");
	if(x2<0 || x2>X_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid x2 argument of line!");
	if(y1<0 || y1>Y_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid y1 argument of line!");
	if(y2<0 || y2>Y_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid y2 argument of line!");

	if(x2<x1)
	{//перенапрявляем рисование слева на право
		e=x2;x2=x1;x1=e;
		e=y2;y2=y1;y1=e;
	}
	x2 = x2-x1; y2 = y2-y1;
	if(y2<0){yinc = -1; y2 = -y2;}
	if(y2>x2)
	{
		e=x2; x2=y2; y2=e;
		denom = x2<<1;
		y2 = y2<<1;
		e = -x2;
		while(x2-->=0)
		{
			Graphic_dot_xor(x1, y1, clr);
			if((e+=y2)>0)
			{
				x1++;
				e-=denom;
			}
			y1+=yinc;
		};
	}else
	{
		denom = x2<<1;
		y2 = y2<<1;
		e = -x2;
		while(x2-->=0)
		{
			Graphic_dot_xor(x1, y1, clr);
			if((e+=y2)>0)
			{
				y1+=yinc;
				e-=denom;
			}
			x1++;
		};
	}
}




//if vsync = 0xffff then no wait of sync
__interwork void Graphic_clearRect(RECT rect, int vsync)
{
	DWORD startChunkY, endChunkY;
	int i,j, high;
	int width, width41, width42, width43;
	DWORD startMask, endMask;
	DWORD commonMask;
	BYTE* scrAddr;
	BYTE* scrAddrTmp;
	WORD temp;

	if(rect.left>X_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid rect.left argument!");
	if(rect.right>X_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid rect.right argument!");
	if(rect.top>Y_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid rect.top argument!");
	if(rect.bottom>Y_SCREEN_MAX)
		exception(__FILE__, __FUNCTION__, __LINE__, "invalid rect.bottom argument!");


	
	if(rect.left>rect.right)
	{
		temp = rect.left;
		rect.left = rect.right;
		rect.right = temp;
	}

	if(rect.top>rect.bottom)
	{
		temp = rect.top;
		rect.top = rect.bottom;
		rect.bottom = temp;
	}
	
	width = rect.right-rect.left+1;
	
	DWORD len = (width)*(rect.bottom-rect.top+1);
	if(rect.left>=X_SCREEN_SIZE)rect.left=X_SCREEN_SIZE-1;
	if(rect.top>=Y_SCREEN_SIZE)rect.top=Y_SCREEN_SIZE-1;
	if(rect.right>=X_SCREEN_SIZE)rect.right=X_SCREEN_SIZE-1;
	if(rect.bottom>=Y_SCREEN_SIZE)rect.bottom=Y_SCREEN_SIZE-1;
	if(len==0)
	{
		exception(__FILE__,__FUNCTION__,__LINE__,"RECT height*width must be >0");
	}
		
	startChunkY = rect.top>>3;
	endChunkY = rect.bottom>>3;
	
	high = endChunkY-startChunkY+1;	//high in chunk
	
	startMask = 0xff;
	startMask<<=(rect.top&7);
	startMask&=0xff;
	startMask |= startMask<<24 | startMask<<16 | startMask<<8;
	startMask=~startMask;


	scrAddr = (BYTE*)((DWORD)screenHigh+(startChunkY<<7))+rect.left;
	endMask = 0x00;

	width41 = 4-(UINT)scrAddr&3;
	width41 &= 0x3;
	width42 = (width-width41)>>2;
	width43 = (width-width41)&3;
	
	for(j=0;j<high-1;j++)
	{
		scrAddrTmp = scrAddr;
		commonMask = startMask|endMask;
		if(commonMask==0)
		{
			i=width41;
			while(--i>=0)
				*scrAddrTmp++ = 0;
			i=width42;
			while(--i>=0)
			{
				*(DWORD*)scrAddrTmp = 0;
				scrAddrTmp+=sizeof(DWORD);
			}
			i=width43;
			while(--i>=0)
				*scrAddrTmp++ = 0;
		}else
		{
			i=width41;
			while(--i>=0)
			{
				*scrAddrTmp &= commonMask;
				scrAddrTmp++;
			}
			i=width42;
			while(--i>=0)
			{
				*(DWORD*)scrAddrTmp &= commonMask;
				scrAddrTmp+=sizeof(DWORD);
			}
			i=width43;
			while(--i>=0)
			{
				*scrAddrTmp &= commonMask;
				scrAddrTmp++;
			}
		}
		startMask = 0x00;
		scrAddr+=LCD_WIDTH;
	}	
	endMask = 0xff;
	endMask<<=((rect.bottom&7)+1);
	endMask&=0xff;
	endMask |= endMask<<24 | endMask<<16 | endMask<<8;
	commonMask = startMask|endMask;
	scrAddrTmp = scrAddr;
	if(commonMask==0)
	{
		i=width41;
		while(--i>=0)
			*scrAddrTmp++ = 0;
		i=width42;
		while(--i>=0)
		{
			*(DWORD*)scrAddrTmp = 0;
			scrAddrTmp+=sizeof(DWORD);
		}
		i=width43;
		while(--i>=0)
			*scrAddrTmp++ = 0;
	}else
	{
		i=width41;
		while(--i>=0)
		{
			*scrAddrTmp &= commonMask;
			scrAddrTmp++;
		}
		i=width42;
		while(--i>=0)
		{
			*(DWORD*)scrAddrTmp &= commonMask;
			scrAddrTmp+=sizeof(DWORD);
		}
		i=width43;
		while(--i>=0)
		{
			*scrAddrTmp &= commonMask;
			scrAddrTmp++;
		}
	}

}



