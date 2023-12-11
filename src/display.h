#pragma once
#include "stdio.h"
#include "stdbool.h"
#include "font8x8.h"
#include "font5x7.h"

#define SCREEN_H (240)
#define SCREEN_W (320)

/*
uint8_t color_zx[16]={
	0b00000000,
	0b00000010,
	0b00001000,
	0b00001010,
	0b00100000,
	0b00100010,
	0b00101000,
	0b00101010,
	0b00000000,
	0b00000011,
	0b00001100,
	0b00001111,
	0b00110000,
	0b00110011,
	0b00111100,
	0b00111111 
};
*/
/*
color:
    0x00 - black
    0x01 - blue
    0x02 - red
    0x03 - pink
    0x04 - green
    0x05 - cyan
    0x06 - yellow
    0x07 - gray
    0x08 - black
    0x09 - blue+
    0x0a - red+
    0x0b - pink+
    0x0c - green+
    0x0d - cyan+
    0x0e - yellow+
    0x0f - white
*/
#define CL_BLACK     0x00
#define CL_BLUE      0x01
#define CL_RED       0x02
#define CL_PINK      0x03
#define CL_GREEN     0x04
#define CL_CYAN      0x05
#define CL_YELLOW    0x06
#define CL_GRAY      0x07
#define CL_LT_BLACK  0x08
#define CL_LT_BLUE   0x09
#define CL_LT_RED    0x0A
#define CL_LT_PINK   0x0B
#define CL_LT_GREEN  0x0C
#define CL_LT_CYAN   0x0D
#define CL_LT_YELLOW 0x0E
#define CL_WHITE     0x0F


#define C_TEXT				0
#define C_FOLDER_TEXT		2
#define C_INVERSE_TEXT		4
#define C_CURSOR			6
#define C_CURSOR_SEL		8
#define C_SELECT_TEXT		10
#define C_HIGHLIGHT_TEXT	12
#define C_BACKGROUND		14
#define C_BORDER			16
#define C_PICTURE			18
#define C_FULLSCREEN		20
#define C_INFO_DIALOG		22
#define C_WARN_DIALOG		24
#define C_ERR_DIALOG		26

#define P_INK	(0)
#define P_BGR	(1)

#define BF_NONE				0x00
#define BF_TEXT				0x01
#define BF_FOLDER_TEXT		0x02
#define BF_INVERSE_TEXT		0x03
#define BF_CURSOR			0x04
#define BF_CURSOR_SEL		0x05
#define BF_SELECT_TEXT		0x06
#define BF_HIGHLIGHT_TEXT	0x07
#define BF_BACKGROUND		0x08
#define BF_BORDER			0x09
#define BF_PICTURE			0x0A
#define BF_FULLSCREEN		0x0B
#define BF_INFO_DIALOG		0x0C
#define BF_WARN_DIALOG		0x0D
#define BF_ERR_DIALOG		0x0E
#define BF_RESET			0x10


typedef uint8_t color_t;

const color_t mur_cmd_default[14][2] = {
	{CL_LT_CYAN,	CL_BLUE			}, //text			//0x00
	{CL_GRAY,		CL_BLUE			}, //folder text	//0x01
	{CL_LT_BLUE,	CL_CYAN			}, //inverse_text	//0x02
	{CL_LT_BLUE,	CL_LT_CYAN		}, //cursor			//0x03
	{CL_LT_YELLOW,	CL_LT_CYAN		}, //cursor selected//0x04
	{CL_LT_YELLOW,	CL_BLUE			}, //select_text	//0x05
	{CL_YELLOW,		CL_LT_BLUE		}, //highlight_text	//0x06
	{CL_CYAN,		CL_BLUE			}, //background		//0x07
	{CL_CYAN,		CL_BLUE			}, //border			//0x08
	{CL_CYAN,		CL_BLUE			}, //picture		//0x09
	{CL_WHITE,		CL_BLACK		}, //fullscreen		//0x0A
	{CL_LT_GREEN,	CL_BLUE			}, //Info dialog 	//0x0B
	{CL_LT_YELLOW,	CL_BLUE			}, //Warn dialog    //0x0C
	{CL_LT_RED,		CL_LT_YELLOW	}, //Error dialog   //0x0D
};

const color_t murmul_default[14][2] = {
	{CL_LT_BLUE,	CL_GRAY			}, //text			//0x00
	{CL_WHITE,		CL_GRAY			}, //folder text	//0x01
	{CL_WHITE,		CL_BLACK		}, //inverse_text	//0x02
	{CL_LT_BLUE,	CL_LT_CYAN		}, //cursor			//0x03
	{CL_LT_YELLOW,	CL_LT_CYAN		}, //cursor			//0x04
	{CL_RED,		CL_GRAY			}, //select_text	//0x05
	{CL_CYAN,		CL_WHITE		}, //highlight_text	//0x06
	{CL_BLUE,		CL_GRAY			}, //background		//0x07
	{CL_BLACK,		CL_GRAY			}, //border			//0x08
	{CL_BLACK,		CL_GRAY			}, //picture		//0x09
	{CL_WHITE,		CL_BLACK		}, //fullscreen		//0x0A
	{CL_LT_GREEN,	CL_GRAY			}, //Info dialog 	//0x0B
	{CL_LT_YELLOW,	CL_GRAY			}, //Warn dialog    //0x0C
	{CL_LT_RED,		CL_LT_YELLOW	}, //Error dialog   //0x0D

};



//#define NUM_V_BUF (3)



class Display{
    private:
        uint8_t* screen_buf;
        int scr_W=1;
        int scr_H=1;    
	public:
        Display();
        ~Display();
        char temp_msg[128]; // Буфер для вывода строк    
        void init_screen(uint8_t* scr_buf,int scr_width,int scr_height);
        bool draw_pixel(int x,int y,color_t color);
        void draw_text(int x,int y,char* text,color_t colorText,color_t colorBg);
        void draw_text_len(int x,int y,char* text,color_t colorText,color_t colorBg,int len);
        void draw_line(int x0,int y0, int x1, int y1,color_t color);
        void draw_rect(int x,int y,int w,int h,color_t color,bool filled);
        void ShowScreenshot(uint8_t* buffer);
        void draw_text5x7(int x,int y,char* text,color_t colorText,color_t colorBg);
        void draw_text5x7_len(int x,int y,char* text,color_t colorText,color_t colorBg,int len);
};

