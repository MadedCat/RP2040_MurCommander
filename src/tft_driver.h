#pragma once
#include "hardware/pio.h"


#define SCREEN_H (240)
#define SCREEN_W (320)
#define V_BUF_SZ (SCREEN_H*SCREEN_W/2)

#define TFT_ILI9341
//#define TFT_ST7789v
#define TFT_CLK_PIN (17)
#define TFT_DATA_PIN (18)
#define TFT_RST_PIN (19)
#define TFT_DC_PIN (20)
#ifdef TFT_ST7789v
    #define TFT_CS_PIN (21)
#endif
// #define TFT_TST_PIN (19)

#define pio_SPI_TFT pio1
#define sm_SPI_TFT 0

#define pio_SPI_TFT_conv pio0
#define sm_SPI_TFT_conv 2

class TFT{
    private:
        uint32_t  conv_arr[512];
        uint32_t* conv_2pix_4_to_16;
        volatile uint64_t pause_val=1.1;
        void pio_set_x(PIO pio, int sm, uint32_t v);
        void outInit(uint gpio);
        void Write_Data(uint8_t d);
        void WriteCommand(uint8_t cmd);
        void setAddrWindow(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h);
	public:
		TFT();
		~TFT();
	#ifdef NUM_V_BUF
		extern  bool is_show_frame[NUM_V_BUF];
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
        void draw_screen_TFT(uint8_t* scr_buf);
        void ILI9341_begin(void);
        void ST7789v_begin(void);
	    void startTFT();
};


//void draw_screen_TFT();
