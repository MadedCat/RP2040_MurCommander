#pragma once

#include "inttypes.h"
#include "stdbool.h"
#include <stdalign.h> //Выравнивание массивов в памяти

#define SCREEN_H (240)
#define SCREEN_W (320)
#define V_BUF_SZ (SCREEN_H*SCREEN_W/2)


#define beginVGA_PIN (6)
#define PIO_VGA (pio1)
#define SM_VGA (2)

#define PIO_VGA_conv (pio1)
#define SM_VGA_conv (0)



class VGA{
	/*private:
		uint16_t alignas(512) conv_color[256];
		uint16_t *conv_color_align;*/
	public:
		VGA();
		~VGA();
	#ifdef NUM_V_BUF
		extern bool is_show_frame[NUM_V_BUF];
		extern int draw_vbuf_inx;
		extern int show_vbuf_inx;
	#endif
	
	#ifdef NUM_V_BUF
		uint8_t graphics_buf[V_BUF_SZ*NUM_V_BUF];
		bool is_show_frame[NUM_V_BUF];
		int draw_vbuf_inx=0;
		int show_vbuf_inx=0;
	#else
		uint8_t graphics_buf[V_BUF_SZ];
	#endif
	void startVGA(uint8_t mode = 0);
	//void setVGAWideMode(bool w_mode);
};

//static uint16_t VGA::conv_color;// alignas(uint32_t);

		//uint16_t temp[64] ;
		//uint16_t conv_color[1024] alignof(32);
		//alignas(512) uint16_t conv_color[256];	