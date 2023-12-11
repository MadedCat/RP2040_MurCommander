
#include <stdio.h>
#include <cstring>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "kb_u_codes.h"
#include "util_i2c_PCF8575.h"
#include "util_i2c_kbd_markup.h"

/*
adress		A0		A1		A2
			-		-		-	0x20
			+		-		-	0x21
			-		+		-	0x22
			+		+		-	0x23
			-		-		+	0x24
			+		-		+	0X25
			-		+		+	0X26
			+		+		+	0x27
*/


void KeyboardI2C::I2C_Init_PCF8575(){
	i2c_init(i2c_port, CLOCK_I2C_kHz*1000);
	gpio_set_function(PICO_I2C_SDA_PIN, GPIO_FUNC_I2C);
	gpio_set_function(PICO_I2C_SCL_PIN, GPIO_FUNC_I2C);
	gpio_pull_up(PICO_I2C_SDA_PIN);
	gpio_pull_up(PICO_I2C_SCL_PIN);
	// printf ("i2c_port=[%8s] SDA = [%d] SCL = [%d]\n", i2c0, PICO_I2C_SDA_PIN, PICO_I2C_SCL_PIN);
};

void KeyboardI2C::I2C_Deinit_PCF8575(){
	i2c_deinit(i2c_port);
	gpio_set_function(PICO_I2C_SDA_PIN, GPIO_FUNC_NULL);
	gpio_set_function(PICO_I2C_SCL_PIN, GPIO_FUNC_NULL);
	gpio_disable_pulls(PICO_I2C_SDA_PIN);
	gpio_disable_pulls(PICO_I2C_SCL_PIN);  
};

int KeyboardI2C::I2C_PCF8575_Data_In(){ 
	int ret=0;
	for (uint8_t dev=0;dev<cnt_dev;dev++){
		if(i2c_dev[dev]>0){
			for(int i=0; i<8; i++ ){
				int addr = i+(dev*(KEY_ARRAY_SIZE/cnt_dev));
				i2c_write_blocking(i2c_port, i2c_dev[dev], &val[0], 2, false);
				ret = i2c_read_blocking(i2c_port, i2c_dev[dev], &i2c_data[addr], 1, true);
				val[1] = 0x01|val[1]<<1;
			}
			val[1] = 0xFE;
		}
	}
	return ret;
};

bool KeyboardI2C::I2C_Scan_Devices(){
	int ask_device;
	for(int i=I2C_ADDRESS; i<(I2C_ADDRESS+8); i++ ){
		ask_device=i2c_write_blocking(i2c_port, i, &val[0], 1, true);
		if(ask_device>0){
			i2c_dev[cnt_dev] = i;
			printf ("Device (%1d) address [%02X]\n",cnt_dev,i2c_dev[cnt_dev]);
			cnt_dev++;
		}
		if (cnt_dev>MAX_QNT_DEVICES) {cnt_dev = 0;}
	}

	if (cnt_dev) {return true;}
	return false;
};

bool KeyboardI2C::load_markup(kbd_markup* keyboard){
	layout=keyboard;
	return true;
}


void KeyboardI2C::translate_keys(uint8_t* i2c_keys){
	memset(kbd_statuses,0,sizeof(kbd_statuses));
	if (layout!=nullptr){
		for (uint8_t key = 0; key < KBD_KEYS_SIZE; key++){
			kbd_markup* keystroke = &layout[key];
			//{.i2c_dev = 1, .i2c_idx=3, .i2c_mask = I2C_KB_1, .kbd_idx=1, .kbd_mask=KB_U1_1},
			if(keystroke->i2c_dev>0){
				uint8_t i2c_addr = ((keystroke->i2c_dev-1)*KEY_ARRAY_SIZE)+(keystroke->i2c_idx);
				uint8_t data = (uint8_t)i2c_keys[i2c_addr];
				if(((data&keystroke->i2c_mask)==I2C_KEY_PRESSED)){
					kbd_statuses[keystroke->kbd_idx]|=(uint32_t)keystroke->kbd_mask;
					//printf(">keystroke[%d]   data[%02X]   kbd_mask[%08X] \n",key,data,(uint32_t)keystroke->kbd_mask);
					//printf("keystroke[%d]->i2c_dev[%02X]->i2c_idx[%02X]->i2c_mask[%02X],i2c_addr[%02X],data[%02X],kbd_idx[%02X]->kbd_mask[%08X]-kbd_res[%08X] \n",key,keystroke->i2c_dev,keystroke->i2c_idx,keystroke->i2c_mask,i2c_addr,data,keystroke->kbd_idx,(uint32_t)keystroke->kbd_mask,kbd_statuses[keystroke->kbd_idx]);
				} 
			}
		}
	}
}

uint32_t KeyboardI2C::millis(){
	return (time_us_32()>>10);//0-4194190
	//return (time_us_32()/1000);
}


bool KeyboardI2C::decode_kbd(){
	state = I2C_PCF8575_Data_In();
	scan = 0;
	for(uint8_t i=0; i<(KEY_ARRAY_SIZE*cnt_dev); i++ ){
		scan |= ~i2c_data[i];
	}
	if(scan==0){
		pressed_timeout = 0;
		repeat_timeout = 0;
	}
	if ((scan != oldscan)) {
		//printf("scan = %02X oldscan = %02X \n" ,scan,oldscan);
		translate_keys(i2c_data);
		oldscan = scan;
		pressed_timeout = millis();
		return true;
	}
	if((scan>0)&&(pressed_timeout>0)&&((millis()-pressed_timeout)>I2C_KEY_PRESSED_TIMEOUT)){
		pressed_timeout = 0;
		repeat_timeout = millis();
		translate_keys(i2c_data);
		return true;
	}
	if((scan>0)&&(repeat_timeout>0)&&((millis()-repeat_timeout)>I2C_KEY_REPEAT_TIMEOUT)){
		repeat_timeout = millis();
		translate_keys(i2c_data);
		return true;
	}
	oldscan = scan;
	return false;	
};


KeyboardI2C::KeyboardI2C(){
	val[0]=0xFF;
	val[1]=0xFE;
	I2C_Init_PCF8575();
	load_markup((kbd_markup*)default_markup);
	memset(i2c_dev,0,MAX_QNT_DEVICES);
	memset(i2c_data,0xFF,KEY_ARRAY_SIZE*MAX_QNT_DEVICES);
	if(I2C_Scan_Devices()){is_present=true;} else {is_present=false;};
	printf("KeyboardI2C Constructor was Called!\n"); 
};

KeyboardI2C::~KeyboardI2C(){
	I2C_Deinit_PCF8575();
	printf("KeyboardI2C Destructor was Called!\n"); 
};


 