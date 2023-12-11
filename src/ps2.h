#pragma once
#include "inttypes.h"
#include <cstring>
#include <stdio.h>
#include "keyboard.h"
#include "kb_u_codes.h"


#define KBD_CLOCK_PIN	(0)
#define KBD_DATA_PIN	(1)
#define KBD_BUFFER_SIZE 46


extern uint8_t* zx_keyboard_state;

//void keyboard_Handler(void);

class KeyboardPS2:public Keyboard{
    private:
		volatile int us_val=0;
		volatile bool is_e0=false;
		volatile bool is_e1=false;
		volatile bool is_f0=false;
		uint8_t dataKB[512];
		uint8_t get_scan_code(void);
		void keys_to_str(char* str_buf,char s_char);
		void translate_scancode(uint8_t code,bool is_press, bool is_e0,bool is_e1);
		void KeyboardHandler(uint /*gpio*/, uint32_t /*event_mask*/);
		void Init_kbd(void);
		void Deinit_kbd(void);
		void d_sleep_us(uint us);
	public:
		typedef struct ps2handle {
			uint8_t head;
			uint8_t tail;
			int bitcount;
			uint16_t data;
			uint16_t raw_data;
			uint8_t parity;
			uint8_t kbd_buffer[KBD_BUFFER_SIZE];
			uint32_t last_key;
		} __attribute__((packed)) ps2handle_t;
		KeyboardPS2();
		~KeyboardPS2();
		bool decode_kbd();

};
static KeyboardPS2::ps2handle_t ps2Data;

//макросы сокращения клавиатурных команд
#define KBD_A				kbd->kbd_statuses[0]&KB_U0_A
#define KBD_B				kbd->kbd_statuses[0]&KB_U0_B
#define KBD_C				kbd->kbd_statuses[0]&KB_U0_C
#define KBD_D				kbd->kbd_statuses[0]&KB_U0_D
#define KBD_E				kbd->kbd_statuses[0]&KB_U0_E
#define KBD_F				kbd->kbd_statuses[0]&KB_U0_F
#define KBD_G				kbd->kbd_statuses[0]&KB_U0_G
#define KBD_H				kbd->kbd_statuses[0]&KB_U0_H
#define KBD_I				kbd->kbd_statuses[0]&KB_U0_I
#define KBD_J				kbd->kbd_statuses[0]&KB_U0_J
#define KBD_K				kbd->kbd_statuses[0]&KB_U0_K
#define KBD_L				kbd->kbd_statuses[0]&KB_U0_L
#define KBD_M				kbd->kbd_statuses[0]&KB_U0_M
#define KBD_N				kbd->kbd_statuses[0]&KB_U0_N
#define KBD_O				kbd->kbd_statuses[0]&KB_U0_O
#define KBD_P				kbd->kbd_statuses[0]&KB_U0_P
#define KBD_Q				kbd->kbd_statuses[0]&KB_U0_Q
#define KBD_R				kbd->kbd_statuses[0]&KB_U0_R
#define KBD_S				kbd->kbd_statuses[0]&KB_U0_S
#define KBD_T				kbd->kbd_statuses[0]&KB_U0_T
#define KBD_U				kbd->kbd_statuses[0]&KB_U0_U
#define KBD_V				kbd->kbd_statuses[0]&KB_U0_V
#define KBD_W				kbd->kbd_statuses[0]&KB_U0_W
#define KBD_X				kbd->kbd_statuses[0]&KB_U0_X
#define KBD_Y				kbd->kbd_statuses[0]&KB_U0_Y
#define KBD_Z				kbd->kbd_statuses[0]&KB_U0_Z

#define KBD_SEMICOLON		kbd->kbd_statuses[0]&KB_U0_SEMICOLON
#define KBD_QUOTE			kbd->kbd_statuses[0]&KB_U0_QUOTE
#define KBD_COMMA			kbd->kbd_statuses[0]&KB_U0_COMMA
#define KBD_PERIOD			kbd->kbd_statuses[0]&KB_U0_PERIOD
#define KBD_LEFT_BR			kbd->kbd_statuses[0]&KB_U0_LEFT_BR
#define KBD_RIGHT_BR		kbd->kbd_statuses[0]&KB_U0_RIGHT_BR


#define KBD_0				kbd->kbd_statuses[1]&KB_U1_0
#define KBD_1				kbd->kbd_statuses[1]&KB_U1_1
#define KBD_2				kbd->kbd_statuses[1]&KB_U1_2
#define KBD_3				kbd->kbd_statuses[1]&KB_U1_3
#define KBD_4				kbd->kbd_statuses[1]&KB_U1_4
#define KBD_5				kbd->kbd_statuses[1]&KB_U1_5
#define KBD_6				kbd->kbd_statuses[1]&KB_U1_6
#define KBD_7				kbd->kbd_statuses[1]&KB_U1_7
#define KBD_8				kbd->kbd_statuses[1]&KB_U1_8
#define KBD_9				kbd->kbd_statuses[1]&KB_U1_9

#define KBD_ENTER			kbd->kbd_statuses[1]&KB_U1_ENTER
#define KBD_SLASH			kbd->kbd_statuses[1]&KB_U1_SLASH
#define KBD_MINUS			kbd->kbd_statuses[1]&KB_U1_MINUS
#define KBD_EQUALS			kbd->kbd_statuses[1]&KB_U1_EQUALS
#define KBD_BACKSLASH		kbd->kbd_statuses[1]&KB_U1_BACKSLASH
#define KBD_CAPS_LOCK		kbd->kbd_statuses[1]&KB_U1_CAPS_LOCK
#define KBD_TAB				kbd->kbd_statuses[1]&KB_U1_TAB
#define KBD_BACK_SPACE		kbd->kbd_statuses[1]&KB_U1_BACK_SPACE
#define KBD_ESC				kbd->kbd_statuses[1]&KB_U1_ESC
#define KBD_TILDE			kbd->kbd_statuses[1]&KB_U1_TILDE
#define KBD_MENU			kbd->kbd_statuses[1]&KB_U1_MENU
#define KBD_L_SHIFT			kbd->kbd_statuses[1]&KB_U1_L_SHIFT
#define KBD_L_CTRL			kbd->kbd_statuses[1]&KB_U1_L_CTRL
#define KBD_L_ALT			kbd->kbd_statuses[1]&KB_U1_L_ALT	
#define KBD_L_WIN			kbd->kbd_statuses[1]&KB_U1_L_WIN	
#define KBD_R_SHIFT			kbd->kbd_statuses[1]&KB_U1_R_SHIFT
#define KBD_R_CTRL			kbd->kbd_statuses[1]&KB_U1_R_CTRL
#define KBD_R_ALT			kbd->kbd_statuses[1]&KB_U1_R_ALT
#define KBD_R_WIN			kbd->kbd_statuses[1]&KB_U1_R_WIN
#define KBD_SPACE			kbd->kbd_statuses[1]&KB_U1_SPACE


#define KBD_NUM_0			kbd->kbd_statuses[2]&KB_U2_NUM_0
#define KBD_NUM_1			kbd->kbd_statuses[2]&KB_U2_NUM_1
#define KBD_NUM_2			kbd->kbd_statuses[2]&KB_U2_NUM_2
#define KBD_NUM_3			kbd->kbd_statuses[2]&KB_U2_NUM_3
#define KBD_NUM_4			kbd->kbd_statuses[2]&KB_U2_NUM_4
#define KBD_NUM_5			kbd->kbd_statuses[2]&KB_U2_NUM_5
#define KBD_NUM_6			kbd->kbd_statuses[2]&KB_U2_NUM_6
#define KBD_NUM_7			kbd->kbd_statuses[2]&KB_U2_NUM_7
#define KBD_NUM_8			kbd->kbd_statuses[2]&KB_U2_NUM_8
#define KBD_NUM_9			kbd->kbd_statuses[2]&KB_U2_NUM_9
#define KBD_NUM_ENTER		kbd->kbd_statuses[2]&KB_U2_NUM_ENTER
#define KBD_NUM_SLASH		kbd->kbd_statuses[2]&KB_U2_NUM_SLASH
#define KBD_NUM_MINUS		kbd->kbd_statuses[2]&KB_U2_NUM_MINUS
#define KBD_NUM_PLUS		kbd->kbd_statuses[2]&KB_U2_NUM_PLUS
#define KBD_NUM_MULT		kbd->kbd_statuses[2]&KB_U2_NUM_MULT
#define KBD_NUM_PERIOD		kbd->kbd_statuses[2]&KB_U2_NUM_PERIOD
#define KBD_NUM_LOCK		kbd->kbd_statuses[2]&KB_U2_NUM_LOCK

#define KBD_DELETE			kbd->kbd_statuses[2]&KB_U2_DELETE
#define KBD_SCROLL_LOCK		kbd->kbd_statuses[2]&KB_U2_SCROLL_LOCK
#define KBD_PAUSE_BREAK		kbd->kbd_statuses[2]&KB_U2_PAUSE_BREAK
#define KBD_INSERT			kbd->kbd_statuses[2]&KB_U2_INSERT
#define KBD_HOME			kbd->kbd_statuses[2]&KB_U2_HOME
#define KBD_PAGE_UP			kbd->kbd_statuses[2]&KB_U2_PAGE_UP
#define KBD_PAGE_DOWN		kbd->kbd_statuses[2]&KB_U2_PAGE_DOWN
#define KBD_PRT_SCR			kbd->kbd_statuses[2]&KB_U2_PRT_SCR
#define KBD_END				kbd->kbd_statuses[2]&KB_U2_END
#define KBD_UP				kbd->kbd_statuses[2]&KB_U2_UP
#define KBD_DOWN			kbd->kbd_statuses[2]&KB_U2_DOWN
#define KBD_LEFT			kbd->kbd_statuses[2]&KB_U2_LEFT
#define KBD_RIGHT			kbd->kbd_statuses[2]&KB_U2_RIGHT

#define KBD_F1				kbd->kbd_statuses[3]&KB_U3_F1
#define KBD_F2				kbd->kbd_statuses[3]&KB_U3_F2
#define KBD_F3				kbd->kbd_statuses[3]&KB_U3_F3
#define KBD_F4				kbd->kbd_statuses[3]&KB_U3_F4
#define KBD_F5				kbd->kbd_statuses[3]&KB_U3_F5
#define KBD_F6				kbd->kbd_statuses[3]&KB_U3_F6
#define KBD_F7				kbd->kbd_statuses[3]&KB_U3_F7
#define KBD_F8				kbd->kbd_statuses[3]&KB_U3_F8
#define KBD_F9				kbd->kbd_statuses[3]&KB_U3_F9
#define KBD_F10				kbd->kbd_statuses[3]&KB_U3_F10
#define KBD_F11				kbd->kbd_statuses[3]&KB_U3_F11
#define KBD_F12				kbd->kbd_statuses[3]&KB_U3_F12

#define KBD_PRESS			(kbd->kbd_statuses[0]!=0)||(kbd->kbd_statuses[1]!=0)||(kbd->kbd_statuses[2]!=0)||(kbd->kbd_statuses[3]!=0)
#define KBD_RELEASE			(kbd->kbd_statuses[0]==0)&&(kbd->kbd_statuses[1]==0)&&(kbd->kbd_statuses[2]==0)&&(kbd->kbd_statuses[3]==0)
