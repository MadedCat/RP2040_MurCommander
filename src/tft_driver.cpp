#include "tft_driver.h"
#include "inttypes.h"
#include "hardware/gpio.h"
#include "pico/platform.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include <string.h>
#include "stdio.h"

//программа конвертации адреса

static uint16_t pio_program1_instructions2[] = {			 
	
	// 0x80a0,	//  0: pull   block					  
	// 0xa027,	//  1: mov	x, osr					 
	//		 	//  .wrap_target
	0x80a0,		//  2: pull   block					  
	// 0x4061,	//  3: in	 null, 1					
	0x40e8,		//  4: in	 osr, 8					 
	0x4036,		//  5: in	 x, 22					  
	0x8020,		//  6: push   block					  
	//	 .wrap
	
	
};

static const struct pio_program pio_program_conv = {
	.instructions = pio_program1_instructions2,
	.length = 4,
	.origin = -1,
};

static const uint16_t SPI_program_instructions[] = {
	//	 .wrap_target
	0x80a0, //  0: pull   block		   side 0  
	0xe03f, //  1: set	x, 31		   side 0	 
	0x6001, //  2: out	pins, 1		 side 0	 
	0x1042, //  3: jmp	x--, 1		  side 1	 
	//	 .wrap
};

static const struct pio_program SPI_program = {
	.instructions = SPI_program_instructions,
	.length = 4,
	.origin = -1,
};

static const uint16_t SPI_8_program_instructions[] = {
	//	 .wrap_target
	0x80a0, //  0: pull   block		   side 0  
	0xe027, //  1: set	x, 7		   side 0	 
	0x6001, //  2: out	pins, 1		 side 0	 
	0x1042, //  3: jmp	x--, 1		  side 1	 
	//	 .wrap
};

static const struct pio_program SPI_8_program = {
	.instructions = SPI_8_program_instructions,
	.length = 4,
	.origin = -1,
};

#define ILI9341_TFTWIDTH   	320	  ///< ILI9341 max TFT width
#define ILI9341_TFTHEIGHT  	240	  ///< ILI9341 max TFT height

#define ILI9341_NOP			0x00	 ///< No-op register
#define ILI9341_SWRESET		0x01	 ///< Software reset register
#define ILI9341_RDDID		0x04	 ///< Read display identification information
#define ILI9341_RDDST		0x09	 ///< Read Display Status

#define ILI9341_SLPIN		0x10	 ///< Enter Sleep Mode
#define ILI9341_SLPOUT		0x11	 ///< Sleep Out
#define ILI9341_PTLON		0x12	 ///< Partial Mode ON
#define ILI9341_NORON		0x13	 ///< Normal Display Mode ON

#define ILI9341_RDMODE		0x0A	 ///< Read Display Power Mode
#define ILI9341_RDMADCTL	0x0B	 ///< Read Display MADCTL
#define ILI9341_RDPIXFMT	0x0C	 ///< Read Display Pixel Format
#define ILI9341_RDIMGFMT	0x0D	 ///< Read Display Image Format
#define ILI9341_RDSELFDIAG	0x0F	 ///< Read Display Self-Diagnostic Result

#define ILI9341_INVOFF		0x20	 ///< Display Inversion OFF
#define ILI9341_INVON		0x21	 ///< Display Inversion ON
#define ILI9341_GAMMASET	0x26	 ///< Gamma Set
#define ILI9341_DISPOFF		0x28	 ///< Display OFF
#define ILI9341_DISPON		0x29	 ///< Display ON

#define ILI9341_CASET		0x2A	 ///< Column Address Set
#define ILI9341_PASET		0x2B	 ///< Page Address Set
#define ILI9341_RAMWR		0x2C	 ///< Memory Write
#define ILI9341_RAMRD		0x2E	 ///< Memory Read

#define ILI9341_PTLAR		0x30	 ///< Partial Area
#define ILI9341_MADCTL		0x36	 ///< Memory Access Control
#define ILI9341_VSCRSADD	0x37	 ///< Vertical Scrolling Start Address
#define ILI9341_PIXFMT		0x3A	 ///< COLMOD: Pixel Format Set

#define ILI9341_FRMCTR1		0xB1	 ///< Frame Rate Control (In Normal Mode/Full Colors)
#define ILI9341_FRMCTR2		0xB2	 ///< Frame Rate Control (In Idle Mode/8 colors)
#define ILI9341_FRMCTR3		0xB3	 ///< Frame Rate control (In Partial Mode/Full Colors)
#define ILI9341_INVCTR		0xB4	 ///< Display Inversion Control
#define ILI9341_DFUNCTR		0xB6	 ///< Display Function Control

#define ILI9341_PWCTR1		0xC0	 ///< Power Control 1
#define ILI9341_PWCTR2		0xC1	 ///< Power Control 2
#define ILI9341_PWCTR3		0xC2	 ///< Power Control 3
#define ILI9341_PWCTR4		0xC3	 ///< Power Control 4
#define ILI9341_PWCTR5		0xC4	 ///< Power Control 5
#define ILI9341_VMCTR1		0xC5	 ///< VCOM Control 1
#define ILI9341_VMCTR2		0xC7	 ///< VCOM Control 2

#define ILI9341_RDID1		0xDA	 ///< Read ID 1
#define ILI9341_RDID2		0xDB	 ///< Read ID 2
#define ILI9341_RDID3		0xDC	 ///< Read ID 3
#define ILI9341_RDID4		0xDD	 ///< Read ID 4

#define ILI9341_GMCTRP1		0xE0	 ///< Positive Gamma Correction
#define ILI9341_GMCTRN1		0xE1	 ///< Negative Gamma Correction
#define ILI9341_INV_DISP	0x21  ///< 255, 130, 198

//#define ILI9341_PWCTR6	 0xFC

// Color definitions
#define ILI9341_BLACK		0x0000	///<   0,   0,   0
#define ILI9341_NAVY		0x000F	///<   0,   0, 123
#define ILI9341_DARKGREEN	0x03E0	///<	0, 125,	0
#define ILI9341_DARKCYAN	0x03EF	///<	0, 125, 123
#define ILI9341_MAROON		0x7800	///< 123,	0,	0
#define ILI9341_PURPLE		0x780F	///< 123,	0, 123
#define ILI9341_OLIVE		0x7BE0	///< 123, 125,	0
#define ILI9341_LIGHTGREY	0xC618	///< 198, 195, 198
#define ILI9341_DARKGREY	0x7BEF	///< 123, 125, 123
#define ILI9341_BLUE		0x001F	///<	0,	0, 255
#define ILI9341_GREEN		0x07E0	///<	0, 255,	0
#define ILI9341_CYAN		0x07FF	///<	0, 255, 255
#define ILI9341_RED			0xF800	///<255,	0,	0
#define ILI9341_MAGENTA		0xF81F	///<255,	0, 255
#define ILI9341_YELLOW		0xFFE0	///<255, 255,	0
#define ILI9341_WHITE		0xFFFF	///<255, 255, 255
#define ILI9341_ORANGE		0xFD20	///<255, 165,	0
#define ILI9341_GREENYELLOW	0xAFE5	///<173, 255,  41
#define ILI9341_PINK		0xFC18	///<255, 130, 198



static const uint8_t initcmd[] = {
	0xEF, 3, 0x03, 0x80, 0x02,
	0xCF, 3, 0x00, 0xC1, 0x30,
	0xED, 4, 0x64, 0x03, 0x12, 0x81,
	0xE8, 3, 0x85, 0x00, 0x78,
	0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
	0xF7, 1, 0x20,
	0xEA, 2, 0x00, 0x00,
	ILI9341_PWCTR1	,	1,	0x23,				// Power control VRH[5:0]
	ILI9341_PWCTR2	,	1,	0x10,				// Power control SAP[2:0];BT[3:0]
	ILI9341_VMCTR1	,	2,	0x3e,	0x28,		// VCM control
	ILI9341_VMCTR2	,	1,	0x86,				// VCM control2
	ILI9341_MADCTL	,	1,	0x28,				// Memory Access Control
	ILI9341_VSCRSADD,	1,	0x00,				// Vertical scroll zero
	ILI9341_PIXFMT	,	1,	0x55,
	ILI9341_FRMCTR1	,	2,	0x00, 0x10,
	//ILI9341_DFUNCTR,	3,	0x08, 0x82, 0x27,	// Display Function Control
	ILI9341_DFUNCTR	,	3,	0x00, 0x8f, 0x27,	// Display Function Control
	
	0xF2, 1, 0x00,						 // 3Gamma Function Disable
	ILI9341_GAMMASET , 1, 0x01,			 // Gamma curve selected
	ILI9341_GMCTRP1 , 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
	0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
	ILI9341_GMCTRN1 , 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
	0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
	ILI9341_SLPOUT  , 0x80,				// Exit Sleep
	ILI9341_DISPON  , 0x80,				// Display on
	//	ILI9341_INV_DISP , 0x80,
	0x00									// End of list
};

TFT::TFT(){
	memset(graphics_buf,0,V_BUF_SZ);
	printf("TFT Constructor was Called!\n");
};
TFT::~TFT(){
	printf("TFT Destructor was Called!\n");
};


void TFT::Write_Data(uint8_t d){
	
	sleep_ms(1);
	pio_SPI_TFT->txf[sm_SPI_TFT]=d<<24;
	sleep_ms(1);
	return;
	
	uint8_t bit_n;
	for (bit_n = 0x80; bit_n; ) {
		if (d & bit_n) gpio_put(TFT_DATA_PIN,1);
		else			gpio_put(TFT_DATA_PIN,0);
		gpio_put(TFT_CLK_PIN,1);
		// sleep_ms(1);
		bit_n >>= 1;
		gpio_put(TFT_CLK_PIN,0);
		// sleep_ms(1);
	}
}

void TFT::WriteCommand(uint8_t cmd){
	gpio_put(TFT_DC_PIN,0);
	Write_Data(cmd);
	gpio_put(TFT_DC_PIN,1);
}

void TFT::setAddrWindow(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h){
	uint16_t x2 = (x1 + w - 1),
	y2 = (y1 + h - 1);
	WriteCommand(ILI9341_CASET); // Column address set
	// sleep_ms(15);
	Write_Data(x1 >> 8);
	Write_Data(x1);
	Write_Data(x2 >> 8);
	Write_Data(x2);
	WriteCommand(ILI9341_PASET); // Row address set
	// sleep_ms(15);
	Write_Data(y1 >> 8);
	Write_Data(y1);
	Write_Data(y2 >> 8);
	Write_Data(y2);
	WriteCommand(ILI9341_RAMWR); // Write to RAM
	// sleep_ms(15);
}

void TFT::outInit(uint gpio){
	gpio_init(gpio);
	gpio_set_dir(gpio,GPIO_OUT);
	gpio_put(gpio,0);
	// gpio_pull_down(gpio);
	
}

void TFT::draw_screen_TFT(uint8_t* scr_buf){
	uint8_t *v_buf8= scr_buf;
	for(int y=0;y<ILI9341_TFTHEIGHT;y++)
	for(int x=0;x<ILI9341_TFTWIDTH/2;x++){
		uint32_t c32=conv_2pix_4_to_16[*v_buf8++];
		// Write_Data((c32>>24)& 0xFF);
		// Write_Data((c32>>16)& 0xFF);
		// Write_Data((c32>>8)& 0xFF);
		// Write_Data((c32>>0)& 0xFF);
		while(pio_sm_is_tx_fifo_full(pio_SPI_TFT, sm_SPI_TFT));
		pio_SPI_TFT->txf[sm_SPI_TFT]=c32;
		//  pause_val/=7;
		//   pause_val*=99;
		//  sleep_ms(1);
		
		
	}
}

void TFT::ST7789v_begin(void){
	//-----------------------------------ST7789S reset sequence------------------------------------//
	gpio_put(TFT_RST_PIN,1);
	sleep_ms(2);
	gpio_put(TFT_RST_PIN,0);
	sleep_ms(200);
	gpio_put(TFT_RST_PIN,1);
	sleep_ms(20);
	#ifdef TFT_ST7789v
		gpio_put(TFT_CS_PIN,1);
		sleep_ms(50);
	#endif
	#ifdef TFT_ST7789v
		gpio_put(TFT_CS_PIN,0);
		sleep_ms(50);
	#endif
	//-------------------------------display and color format setting-----------------------------//
	WriteCommand(0x36); 
	Write_Data (0x60);
	
	WriteCommand(0x3A);  
	Write_Data (0x05);//  #06
	//--------------------------------ST7789S Frame rate setting----------------------------------//
	WriteCommand(0xB2);  
	Write_Data (0x0C);  
	Write_Data (0x0C);  
	Write_Data (0x00);  
	Write_Data (0x33);  
	Write_Data (0x33);
	
	WriteCommand(0xB7);  
	Write_Data (0x75);
	//---------------------------------ST7789S Power setting--------------------------------------//
	
	WriteCommand(0xBB);  
	Write_Data (0x21);
	
	WriteCommand(0xC0);  
	Write_Data (0x2C);
	
	WriteCommand(0xC2);  
	Write_Data (0x01);
	
	WriteCommand(0xC3); 
	Write_Data (0x17);
	
	WriteCommand(0xC4);  
	Write_Data (0x20);
	
	WriteCommand(0xC6);  
	Write_Data (0x0A);
	
	WriteCommand(0xD0);  
	Write_Data (0xA4);  
	Write_Data (0xA1);
	//--------------------------------ST7789S gamma setting---------------------------------------//
	WriteCommand(0xE0);  
	Write_Data (0xD0);  
	Write_Data (0x0F);  
	Write_Data (0x17);  
	Write_Data (0x0D);  
	Write_Data (0x0E);  
	Write_Data (0x02);  
	Write_Data (0x3D);  
	Write_Data (0x44);  
	Write_Data (0x4E);  
	Write_Data (0x0C);  
	Write_Data (0x1B);  
	Write_Data (0x1B);  
	Write_Data (0x1C);  
	Write_Data (0x1F);
	
	WriteCommand(0xE1);  
	Write_Data (0xD0);  
	Write_Data (0x0F);  
	Write_Data (0x17);  
	Write_Data (0x0D);  
	Write_Data (0x0E);  
	Write_Data (0x02);  
	Write_Data (0x3D);  
	Write_Data (0x44);  
	Write_Data (0x4E);  
	Write_Data (0x0C);  
	Write_Data (0x1B);  
	Write_Data (0x1B);  
	Write_Data (0x1C);  
	Write_Data (0x1F);
	
	//---------------------------------------------------------------------------------------------------//
	WriteCommand(0x11);
	
	sleep_ms(120); //Delay 120ms
	WriteCommand(0x20);  // invert on 0x21
	
	WriteCommand(0x29);
}

void TFT::ILI9341_begin(void) {
	
	uint8_t	 cmd, x, numArgs;
	const uint8_t *addr = initcmd;
	
	// gpio_put(ILI9341_RST_PIN,1);
	// sleep_ms(200);
	gpio_put(TFT_RST_PIN,0);
	sleep_ms(200);
	gpio_put(TFT_RST_PIN,1);
	sleep_ms(200);
	
	// If no hardware reset pin...
	WriteCommand(ILI9341_SWRESET);  // Engage software reset
	sleep_ms(150);
	
	//gpio_put(TFT_CS_PIN,1);
	//sleep_ms(50);
	
	
	//gpio_put(TFT_CS_PIN,0);
	//sleep_ms(50);
	
	
	cmd = *addr++;
	while( cmd > 0 ) {
		WriteCommand(cmd);
		x	   = *addr++;
		numArgs = x & 0x7F;
		while(numArgs--) Write_Data(*addr++);
		if(x & 0x80) sleep_ms(150);
		cmd = *addr++;
	}
	//gpio_put(TFT_CS_PIN,1);
	
}

const uint16_t pallete16b[]={
	//светло серый
	// 0b0000000000000000,
	// 0b0000000000011100,
	// 0b0000011100100000,
	// 0b0000011100111100,
	// 0b1110000000000000,
	// 0b1110000000011100,
	// 0b1110011100100000,
	// 0b1110011100111100,
	
	//средне серый
	// 0b0000000000000000,
	// 0b0000000000011000,
	// 0b0000011000100000,
	// 0b0000011000111000,
	// 0b1100000000000000,
	// 0b1100000000011000,
	// 0b1100011000100000,
	// 0b1100011000111000,
	
	
	//средне темно серый
	0b0000000000000000,
	0b0000000000010100,
	0b0000010100100000,
	0b0000010100110100,
	0b1010000000000000,
	0b1010000000010100,
	0b1010010100100000,
	0b1010010100110100,
	
	// тёмно серый
	// 0b0000000000000000,
	// 0b0000000000010000,
	// 0b0000010000100000,
	// 0b0000010000110000,
	// 0b1000000000000000,
	// 0b1000000000010000,
	// 0b1000010000100000,
	// 0b1000010000110000,
	
	0b0000000000000000,
	0b0000000000011111,
	0b0000011111100000,
	0b0000011111111111,
	0b1111100000000000,
	0b1111100000011111,
	0b1111111111100000,
	0b1111111111111111
};

void TFT::pio_set_x(PIO pio, int sm, uint32_t v){
	uint instr_shift = pio_encode_in(pio_x, 4);
	uint instr_mov = pio_encode_mov(pio_x, pio_isr);
	for (int i=0;i<8;i++){
		const uint32_t nibble = (v >> (i * 4)) & 0xf;
		pio_sm_exec(pio, sm, pio_encode_set(pio_x, nibble));
		pio_sm_exec(pio, sm, instr_shift);
	}
	pio_sm_exec(pio, sm, instr_mov);
}

void TFT::startTFT(){
	
	uint8_t* v_buf= graphics_buf;

	//настройка SM для конвертации
	uint offset0 = pio_add_program(pio_SPI_TFT_conv, &pio_program_conv);
	uint sm_c=sm_SPI_TFT_conv;
	conv_2pix_4_to_16=(uint32_t*)((uint32_t)conv_arr&0xfffffc00)+0x400;
	pio_set_x(pio_SPI_TFT_conv,sm_c,((uint32_t)conv_2pix_4_to_16>>10));
	
	
	pio_sm_config c_c = pio_get_default_sm_config();
	sm_config_set_wrap(&c_c, offset0 + 0, offset0 + (pio_program_conv.length-1));
	sm_config_set_in_shift(&c_c, true, false, 32);
	
	
	
	pio_sm_init(pio_SPI_TFT_conv, sm_c, offset0, &c_c);
	pio_sm_set_enabled(pio_SPI_TFT_conv, sm_c, true);
	
	
	
	for(int i=0;i<256;i++) {
		uint8_t ci=i;
		uint8_t cl=(ci&0xf0)>>4;
		uint8_t ch=ci&0xf;
		//убрать ярко-чёрный
		if (ch==0x08) ch=0;
		if (cl==0x08) cl=0;
		ch=(ch&0b1001)|((ch&0b0100)>>1)|((ch&0b0010)<<1);
		cl=(cl&0b1001)|((cl&0b0100)>>1)|((cl&0b0010)<<1);
		//конвертация в 6 битный формат цвета
		uint16_t ch16=pallete16b[ch&0xf];
		uint16_t cl16=pallete16b[cl&0xf];
		conv_2pix_4_to_16[i]=((ch16)<<16)|(cl16);
	}
	
	outInit(TFT_CLK_PIN);
	outInit(TFT_DATA_PIN);
	outInit(TFT_RST_PIN);
	outInit(TFT_DC_PIN);
	#ifdef TFT_ST7789v
		outInit(TFT_CS_PIN);
		gpio_put(TFT_CS_PIN,1);
	#endif
	
	
	
	//инициализируем на 8 бит программу
	uint offset = pio_add_program(pio_SPI_TFT, &SPI_8_program);
	pio_sm_config c = pio_get_default_sm_config();
	sm_config_set_wrap(&c, offset, offset + (SPI_8_program.length-1)); 
	sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
	
	
	
	
	//настройка side set
	sm_config_set_sideset_pins(&c,TFT_CLK_PIN);
	sm_config_set_sideset(&c,1,false,false);
	for(int i=0;i<1;i++){		   
		pio_gpio_init(pio_SPI_TFT, TFT_CLK_PIN+i);
	}
	
	pio_sm_set_pins_with_mask(pio_SPI_TFT, sm_SPI_TFT, 1u<<TFT_CLK_PIN, 1u<<TFT_CLK_PIN);
	pio_sm_set_pindirs_with_mask(pio_SPI_TFT, sm_SPI_TFT,  1u<<TFT_CLK_PIN,  1u<<TFT_CLK_PIN);
	//
	
	
	
	pio_gpio_init(pio_SPI_TFT, TFT_DATA_PIN);//резервируем под выход PIO
	
	pio_sm_set_consecutive_pindirs(pio_SPI_TFT, sm_SPI_TFT, TFT_DATA_PIN, 1, true);//конфигурация пинов на выход
	
	sm_config_set_out_shift(&c, false, false, 32);
	sm_config_set_out_pins(&c, TFT_DATA_PIN, 1);
	
	
	
	pio_sm_init(pio_SPI_TFT, sm_SPI_TFT, offset, &c);
	pio_sm_set_enabled(pio_SPI_TFT, sm_SPI_TFT, true);
	
	float fdiv=2;//(clock_get_hz(clk_sys)/130000000.0);//частота SPI*2
	uint32_t fdiv32=(uint32_t) (fdiv * (1 << 16));
	fdiv32=fdiv32&0xfffff000;//округление делителя
	pio_SPI_TFT->sm[sm_SPI_TFT].clkdiv=fdiv32; //делитель для конкретной sm
	
	
	sleep_ms(20);
	#ifdef TFT_ILI9341
		ILI9341_begin();
	#endif
	#ifdef TFT_ST7789v
		ST7789v_begin();
	#endif
	
	sleep_ms(20);
	#ifdef TFT_ST7789v
		gpio_put(TFT_CS_PIN,0);
	#endif
	sleep_ms(20);
	
	setAddrWindow(0,0,ILI9341_TFTWIDTH,ILI9341_TFTHEIGHT);
	// setAddrWindow(0,0,ILI9341_TFTHEIGHT,ILI9341_TFTWIDTH);
	
	//подменяем на программу 32 бита spi
	sleep_ms(1);
	pio_sm_set_enabled(pio_SPI_TFT, sm_SPI_TFT, false);
	pio_SPI_TFT->instr_mem[offset+1]=SPI_program_instructions[1];
	pio_sm_set_enabled(pio_SPI_TFT, sm_SPI_TFT, true);
	
	
	
	//канал для передачи даных экранного буфера  в конвертор и канал для его управления
	int dma_0=dma_claim_unused_channel(true);
	int dma_ctrl0=dma_claim_unused_channel(true);
	
	
	dma_channel_config c_conv = dma_channel_get_default_config(dma_0);
	
	channel_config_set_transfer_data_size(&c_conv, DMA_SIZE_8);
	
	channel_config_set_read_increment(&c_conv, true);
	channel_config_set_write_increment(&c_conv, false);
	
	uint dreq0=DREQ_PIO1_TX0+sm_SPI_TFT_conv;
	if (pio_SPI_TFT_conv==pio0) dreq0=DREQ_PIO0_TX0+sm_SPI_TFT_conv;
	
	
	
	channel_config_set_dreq(&c_conv, dreq0);
	
	channel_config_set_chain_to(&c_conv, dma_ctrl0);						// chain to other channel
	
	dma_channel_configure(
		dma_0,
		&c_conv,
		&pio_SPI_TFT_conv->txf[sm_SPI_TFT_conv], // Write address 
		v_buf,			 // read address 
		SCREEN_H*SCREEN_W/2, //
		false			 // Don't start yet
	);
	
	
	static uint32_t addr_v_buf;
	addr_v_buf=(uint32_t)v_buf;
	
	
	dma_channel_config c_contrl = dma_channel_get_default_config(dma_ctrl0);
	
	channel_config_set_transfer_data_size(&c_contrl, DMA_SIZE_32);
	
	channel_config_set_read_increment(&c_contrl, false);
	channel_config_set_write_increment(&c_contrl, false);
	
	
	channel_config_set_chain_to(&c_contrl, dma_0);						// chain to other channel
	
	dma_channel_configure(
		dma_ctrl0,
		&c_contrl,
		&dma_hw->ch[dma_0].read_addr, // Write address 
		&addr_v_buf,			 // read address 
		1, //
		false			 // Don't start yet
	);
	
	
	
	//канал для передачи даных экранного буфера  в конвертор и канал для его управления
	int dma_addr=dma_claim_unused_channel(true);
	int dma_SPI=dma_claim_unused_channel(true);
	
	
	dma_channel_config c_addr = dma_channel_get_default_config(dma_addr);
	
	channel_config_set_transfer_data_size(&c_addr, DMA_SIZE_32);
	
	channel_config_set_read_increment(&c_addr, false);
	channel_config_set_write_increment(&c_addr, false);
	
	uint dreq1=DREQ_PIO1_RX0+sm_SPI_TFT_conv;
	if (pio_SPI_TFT_conv==pio0) dreq1=DREQ_PIO0_RX0+sm_SPI_TFT_conv;
	channel_config_set_dreq(&c_addr, dreq1);
	
	
	channel_config_set_chain_to(&c_addr, dma_SPI);						// chain to other channel
	
	dma_channel_configure(
		dma_addr,
		&c_addr,
		&dma_hw->ch[dma_SPI].read_addr, // Write address 
		&pio_SPI_TFT_conv->rxf[sm_SPI_TFT_conv],			 // read address 
		1, //
		false			 // Don't start yet
	);
	
	
	dma_channel_config c_SPI = dma_channel_get_default_config(dma_SPI);
	
	channel_config_set_transfer_data_size(&c_SPI, DMA_SIZE_32);
	
	channel_config_set_read_increment(&c_SPI, false);
	channel_config_set_write_increment(&c_SPI, false);
	
	uint dreq2=DREQ_PIO1_TX0+sm_SPI_TFT;
	if (pio_SPI_TFT==pio0) dreq2=DREQ_PIO0_TX0+sm_SPI_TFT;
	channel_config_set_dreq(&c_SPI, dreq2);
	
	
	channel_config_set_chain_to(&c_SPI, dma_addr);						// chain to other channel
	
	dma_channel_configure(
		dma_SPI,
		&c_SPI,
		&pio_SPI_TFT->txf[sm_SPI_TFT], // Write address 
		conv_2pix_4_to_16,			 // read address 
		1, //
		false			 // Don't start yet
	);
	
	
	
	dma_start_channel_mask((1u << dma_0)) ;
	dma_start_channel_mask((1u << dma_addr)) ;
}