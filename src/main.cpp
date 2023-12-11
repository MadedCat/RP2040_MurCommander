#define PICO_FLASH_SPI_CLKDIV 4

#define NORMAL_MODE
//#define TURBO_MODE

//#define TFT_BUILD 

#define DEBUG_DELAY

#define FW_VERSION "v0.47 "
#define FW_AUTHOR "TecnoCat"

#define SHOW_SCREEN_DELAY 500 //в милисекундах

#define WORK_LED_PIN (25)

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
(byte & 0x80 ? '1' : '0'), \
(byte & 0x40 ? '1' : '0'), \
(byte & 0x20 ? '1' : '0'), \
(byte & 0x10 ? '1' : '0'), \
(byte & 0x08 ? '1' : '0'), \
(byte & 0x04 ? '1' : '0'), \
(byte & 0x02 ? '1' : '0'), \
(byte & 0x01 ? '1' : '0') 


#include <stdio.h>
#include <cstdlib>
#include <pico/stdlib.h>
#include "pico/multicore.h"
#include "hardware/flash.h"
#include <hardware/sync.h>
#include <hardware/irq.h>
#include "hardware/watchdog.h"
#include "hardware/clocks.h"
#include "hardware/structs/systick.h"
#include "hardware/pwm.h"
#include "hardware/vreg.h"
#include "string.h"

/*
#ifndef TFT_BUILD 
	#include "VGA.h"
#endif
#ifdef TFT_BUILD 
	#include "tft_driver.h"
#endif
*/

#include "VGA.h"
#include "tft_driver.h"

#include "ps2.h"
#include "util_i2c_PCF8575.h"

#include "utf_handle.h"
#include "display.h"
#include "interface.h"

#include "util_sd.h"

void software_reset(){
	watchdog_enable(1, 1);
	while(1);
}

void  inInit(uint gpio){
	gpio_init(gpio);
	gpio_set_dir(gpio,GPIO_IN);
	gpio_pull_up(gpio);
}
void  outInit(uint gpio){
	gpio_init(gpio);
	gpio_set_dir(gpio,GPIO_OUT);
	//gpio_pull_up(gpio);
}

volatile int us_val=0;
void d_sleep_us(uint us){
	for(uint i=0;i<us;i++){
		for(int j=0;j<25;j++){
			us_val++;
		}
	}
}

#define TIMER_PERIOD 1000 //ms

uint32_t timer_update =0;
#ifdef __cplusplus
extern "C"{
#endif
	uint32_t my_millis(){
		return us_to_ms(time_us_32());
		//return (time_us_32()>>10);//0-4194190
		//return (time_us_32()/1000);
	}
#ifdef __cplusplus
} // extern "C"
#endif


int main(void){  

	#ifdef NORMAL_MODE
		vreg_set_voltage(VREG_VOLTAGE_1_20);//def
		sleep_ms(100);
		set_sys_clock_khz(252000, false);//def
	#endif
	#ifdef TURBO_MODE
		vreg_set_voltage(VREG_VOLTAGE_1_30);
		sleep_ms(100);
		set_sys_clock_khz(290400, false);
	#endif
	/*
		вот эти пробуй:280, 288, 290400, 296, 297, 300
		дальше такие:302400, 303, 304800, 306, 307, 308, 309600, 312, 314400, 315, 316, 316800
	*/
	
	//set_sys_clock_khz(300000, true);
	//set_sys_clock_khz(284000, false);
	//
	//set_sys_clock_khz(504000, false);
	
	sleep_ms(10);
	
	
	stdio_init_all();

	sleep_ms(100);
	#ifdef DEBUG_DELAY
		sleep_ms(4000);//debug delay
	#endif
	
	printf("MurMulator %s by %s \n\n",FW_VERSION,FW_AUTHOR);
	printf("Main Program Start!!!\n");
	printf("CPU clock=%ld Hz\n",clock_get_hz(clk_sys));
	

	outInit(WORK_LED_PIN);
	
	gpio_put(WORK_LED_PIN,1);

	int settings_index=0;
	int settings_lines=0;
	int menu_inc_dec=0;

	printf("init FileSystem\n");
    /*if ((fs!=-FR_NOT_ENABLED)&&(fs!=FR_OK)) software_reset();
	printf("init FS-OK\n");*/
	SDCard *card = new SDCard();
	card->init_filesystem();

	if(card->init_fs){
		printf("FS init success\n");
	}


	#ifndef TFT_BUILD 
		VGA *vga = new VGA();
		sleep_ms(100);
		vga->startVGA();

	#endif
	#ifdef TFT_BUILD 
		TFT* vga =new TFT();
		sleep_ms(100);
		vga->startTFT();
		#ifdef LED_TFT_PIN 
			gpio_init(LED_TFT_PIN);
			gpio_set_dir(LED_TFT_PIN,GPIO_OUT);
			gpio_put(LED_TFT_PIN,true);
		#endif
	#endif
	sleep_ms(100);
	Display *disp = new Display();
	disp->init_screen(vga->graphics_buf,SCREEN_W,SCREEN_H);
	
	Interface *iface = new Interface(IFACE_WIDTH,IFACE_HEIGHT,disp,card,(color_t*)mur_cmd_default);
	

	
	

	//boot logo
	//disp->draw_rect(0,0,SCREEN_W,SCREEN_H,CL_BLACK,true);//Заливаем экран 
	//draw_mur_logo_big(SCREEN_W/2-69,SCREEN_H/2-75,2);
	

	Keyboard* kbd;

	kbd = new KeyboardI2C();
	if(!kbd->is_present){
		delete kbd;
    	kbd = new KeyboardPS2();
  	}

	printf(" \n");


	printf("Modules sizes:\n");
	printf("VGA             [%d]\n",sizeof(VGA));
	printf("SDCard          [%d]\n",sizeof(SDCard));
	printf("Display         [%d]\n",sizeof(Display));
	printf("Interface       [%d]\n",sizeof(Interface));
	printf("KeyboardI2C     [%d]\n",sizeof(KeyboardI2C));
	printf("KeyboardPS2     [%d]\n",sizeof(KeyboardPS2));
	
	
	printf("starting main loop \n");
	
	iface->drawPanels(true);

	while(1){
		
		if((my_millis()-timer_update)>TIMER_PERIOD){
			timer_update=my_millis();
			gpio_put(WORK_LED_PIN,1);
			//printf("Time is:%d \n",my_millis());
			sleep_ms(50);
			gpio_put(WORK_LED_PIN,0);
			sleep_ms(50);
			iface->processCopy();
		};

		if (kbd->decode_kbd()){ 
			printf("kbd[0][%08X]   kbd[1][%08X]   kbd[2][%08X]   kbd[2][%08X]\n",kbd->kbd_statuses[0],kbd->kbd_statuses[1],kbd->kbd_statuses[2],kbd->kbd_statuses[3]);
			
			if (((KBD_L_SHIFT)||(KBD_R_SHIFT))&&((KBD_L_ALT)||(KBD_R_ALT))&&(KBD_DELETE)){
				printf("restart\n");
				software_reset();
			};
			iface->processKeyboard(kbd);
		} //else//if (decode_PS2())
		iface->processTimers();
	}//while(1)


	software_reset();
}