
#pragma once
#include "inttypes.h"
#include "keyboard.h"
#include "util_i2c_kbd_mask.h"
#include "util_i2c_kbd_markup.h"

#define i2c_port (i2c0)
#define CLOCK_I2C_kHz (400)
#define I2C_ADDRESS (0x20)
#define PICO_I2C_SDA_PIN (0)
#define PICO_I2C_SCL_PIN (1)
#define MAX_QNT_DEVICES (2)
#define KEY_ARRAY_SIZE (8)
#define I2C_KEY_PRESSED (0)
#define I2C_KEY_PRESSED_TIMEOUT (350)
#define I2C_KEY_REPEAT_TIMEOUT (100)

// extern uint8_t* i2c_keys[8];

class KeyboardI2C:public Keyboard{
	private:
		uint8_t i2c_dev[MAX_QNT_DEVICES];
		uint8_t i2c_data[KEY_ARRAY_SIZE*MAX_QNT_DEVICES];
		uint8_t cnt_dev = 0;
		uint8_t val[2];
		uint8_t scan;
		uint8_t oldscan;
		uint32_t pressed_timeout =0;
		uint32_t repeat_timeout =0;
		int state;
		kbd_markup* layout;
		void I2C_Init_PCF8575();
		void I2C_Deinit_PCF8575();
		int I2C_PCF8575_Data_In();
		bool I2C_Scan_Devices();
		void translate_keys(uint8_t* i2c_keys);
		uint32_t millis();
		void Init_kbd(void);
		void Deinit_kbd(void);
	public:
		KeyboardI2C();
		~KeyboardI2C();
		bool decode_kbd();
		bool load_markup(kbd_markup* keyboard);
};