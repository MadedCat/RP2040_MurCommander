#pragma once
#include "inttypes.h"
#include "kb_u_codes.h"
#include "util_i2c_kbd_mask.h"

#define KBD_KEYS_SIZE (108)

typedef struct kbd_markup kbd_markup;
struct kbd_markup{
	uint8_t		i2c_dev;	// Индекс устройства i2c
	uint8_t		i2c_idx;	// Индекс порта i2c (адрес байта в массиве)
	uint8_t		i2c_mask;	// Маска сканкода i2c
	uint8_t		kbd_idx;	// Индекс порта PS/2 (адрес байта в массиве)
	uint32_t	kbd_mask;	// Маска сканкода PS/2
}; //__packed

const kbd_markup default_markup[KBD_KEYS_SIZE]={
{	.i2c_dev = 1, 	.i2c_idx=3, 	.i2c_mask = I2C_KB_1, 		.kbd_idx=1, 	.kbd_mask=KB_U1_1},				//0
{	.i2c_dev = 1, 	.i2c_idx=3, 	.i2c_mask = I2C_KB_2, 		.kbd_idx=1, 	.kbd_mask=KB_U1_2},            	//1
{	.i2c_dev = 1, 	.i2c_idx=3, 	.i2c_mask = I2C_KB_3, 		.kbd_idx=1, 	.kbd_mask=KB_U1_3},            	//2
{	.i2c_dev = 1, 	.i2c_idx=3, 	.i2c_mask = I2C_KB_4, 		.kbd_idx=1, 	.kbd_mask=KB_U1_4},            	//3
{	.i2c_dev = 1, 	.i2c_idx=3, 	.i2c_mask = I2C_KB_5, 		.kbd_idx=1, 	.kbd_mask=KB_U1_5},            	//4
{	.i2c_dev = 1, 	.i2c_idx=4, 	.i2c_mask = I2C_KB_6, 		.kbd_idx=1, 	.kbd_mask=KB_U1_6},            	//5
{	.i2c_dev = 1, 	.i2c_idx=4, 	.i2c_mask = I2C_KB_7, 		.kbd_idx=1, 	.kbd_mask=KB_U1_7},            	//6
{	.i2c_dev = 1, 	.i2c_idx=4, 	.i2c_mask = I2C_KB_8, 		.kbd_idx=1, 	.kbd_mask=KB_U1_8},            	//7
{	.i2c_dev = 1, 	.i2c_idx=4, 	.i2c_mask = I2C_KB_9, 		.kbd_idx=1, 	.kbd_mask=KB_U1_9},            	//8
{	.i2c_dev = 1, 	.i2c_idx=4, 	.i2c_mask = I2C_KB_0, 		.kbd_idx=1, 	.kbd_mask=KB_U1_0},            	//9
{	.i2c_dev = 1, 	.i2c_idx=2, 	.i2c_mask = I2C_KB_Q, 		.kbd_idx=0, 	.kbd_mask=KB_U0_Q},            	//10
{	.i2c_dev = 1, 	.i2c_idx=2, 	.i2c_mask = I2C_KB_W, 		.kbd_idx=0, 	.kbd_mask=KB_U0_W},            	//11
{	.i2c_dev = 1, 	.i2c_idx=2, 	.i2c_mask = I2C_KB_E, 		.kbd_idx=0, 	.kbd_mask=KB_U0_E},            	//12
{	.i2c_dev = 1, 	.i2c_idx=2, 	.i2c_mask = I2C_KB_R, 		.kbd_idx=0, 	.kbd_mask=KB_U0_R},            	//13
{	.i2c_dev = 1, 	.i2c_idx=2, 	.i2c_mask = I2C_KB_T, 		.kbd_idx=0, 	.kbd_mask=KB_U0_T},            	//14
{	.i2c_dev = 1, 	.i2c_idx=5, 	.i2c_mask = I2C_KB_Y, 		.kbd_idx=0, 	.kbd_mask=KB_U0_Y},            	//15
{	.i2c_dev = 1, 	.i2c_idx=5, 	.i2c_mask = I2C_KB_U, 		.kbd_idx=0, 	.kbd_mask=KB_U0_U},            	//16
{	.i2c_dev = 1, 	.i2c_idx=5, 	.i2c_mask = I2C_KB_I, 		.kbd_idx=0, 	.kbd_mask=KB_U0_I},            	//17
{	.i2c_dev = 1, 	.i2c_idx=5, 	.i2c_mask = I2C_KB_O, 		.kbd_idx=0, 	.kbd_mask=KB_U0_O},            	//18
{	.i2c_dev = 1, 	.i2c_idx=5, 	.i2c_mask = I2C_KB_P, 		.kbd_idx=0, 	.kbd_mask=KB_U0_P},            	//19
{	.i2c_dev = 1, 	.i2c_idx=1, 	.i2c_mask = I2C_KB_A, 		.kbd_idx=0, 	.kbd_mask=KB_U0_A},            	//20
{	.i2c_dev = 1, 	.i2c_idx=1, 	.i2c_mask = I2C_KB_S, 		.kbd_idx=0, 	.kbd_mask=KB_U0_S},            	//21
{	.i2c_dev = 1, 	.i2c_idx=1, 	.i2c_mask = I2C_KB_D, 		.kbd_idx=0, 	.kbd_mask=KB_U0_D},            	//22
{	.i2c_dev = 1, 	.i2c_idx=1, 	.i2c_mask = I2C_KB_F, 		.kbd_idx=0, 	.kbd_mask=KB_U0_F},            	//23
{	.i2c_dev = 1, 	.i2c_idx=1, 	.i2c_mask = I2C_KB_G, 		.kbd_idx=0, 	.kbd_mask=KB_U0_G},            	//24
{	.i2c_dev = 1, 	.i2c_idx=6, 	.i2c_mask = I2C_KB_H, 		.kbd_idx=0, 	.kbd_mask=KB_U0_H},           	//25
{	.i2c_dev = 1, 	.i2c_idx=6, 	.i2c_mask = I2C_KB_J, 		.kbd_idx=0, 	.kbd_mask=KB_U0_J},           	//26
{	.i2c_dev = 1, 	.i2c_idx=6, 	.i2c_mask = I2C_KB_K, 		.kbd_idx=0, 	.kbd_mask=KB_U0_K},           	//27
{	.i2c_dev = 1, 	.i2c_idx=6, 	.i2c_mask = I2C_KB_L, 		.kbd_idx=0, 	.kbd_mask=KB_U0_L},           	//28
{	.i2c_dev = 1, 	.i2c_idx=6, 	.i2c_mask = I2C_KB_ENTER, 	.kbd_idx=1, 	.kbd_mask=KB_U1_ENTER},    		//29
{	.i2c_dev = 1, 	.i2c_idx=0, 	.i2c_mask = I2C_KB_CS, 		.kbd_idx=1, 	.kbd_mask=KB_U1_L_SHIFT},     	//30
{	.i2c_dev = 1, 	.i2c_idx=0, 	.i2c_mask = I2C_KB_Z, 		.kbd_idx=0, 	.kbd_mask=KB_U0_Z},           	//31
{	.i2c_dev = 1, 	.i2c_idx=0, 	.i2c_mask = I2C_KB_X, 		.kbd_idx=0, 	.kbd_mask=KB_U0_X},           	//32
{	.i2c_dev = 1, 	.i2c_idx=0, 	.i2c_mask = I2C_KB_C, 		.kbd_idx=0, 	.kbd_mask=KB_U0_C},           	//33
{	.i2c_dev = 1, 	.i2c_idx=0, 	.i2c_mask = I2C_KB_V, 		.kbd_idx=0, 	.kbd_mask=KB_U0_V},           	//34
{	.i2c_dev = 1, 	.i2c_idx=7, 	.i2c_mask = I2C_KB_B, 		.kbd_idx=0, 	.kbd_mask=KB_U0_B},           	//35
{	.i2c_dev = 1, 	.i2c_idx=7, 	.i2c_mask = I2C_KB_N, 		.kbd_idx=0, 	.kbd_mask=KB_U0_N},           	//36
{	.i2c_dev = 1, 	.i2c_idx=7, 	.i2c_mask = I2C_KB_M, 		.kbd_idx=0, 	.kbd_mask=KB_U0_M},           	//37
{	.i2c_dev = 1, 	.i2c_idx=7, 	.i2c_mask = I2C_KB_SS,		.kbd_idx=1, 	.kbd_mask=KB_U1_L_CTRL},      	//38
{	.i2c_dev = 1, 	.i2c_idx=7, 	.i2c_mask = I2C_KB_SPC,		.kbd_idx=1, 	.kbd_mask=KB_U1_SPACE},      	//39
{	.i2c_dev = 1, 	.i2c_idx=0, 	.i2c_mask = I2C_KB_RES, 	.kbd_idx=1, 	.kbd_mask=KB_U1_L_CTRL},     	//40
{	.i2c_dev = 1, 	.i2c_idx=0, 	.i2c_mask = I2C_KB_RES, 	.kbd_idx=1, 	.kbd_mask=KB_U1_L_ALT},      	//41
{	.i2c_dev = 1, 	.i2c_idx=0, 	.i2c_mask = I2C_KB_RES, 	.kbd_idx=2, 	.kbd_mask=KB_U2_DELETE},     	//42
{	.i2c_dev = 1, 	.i2c_idx=0, 	.i2c_mask = I2C_KB_NMI, 	.kbd_idx=1, 	.kbd_mask=KB_U1_L_CTRL},     	//43
{	.i2c_dev = 1, 	.i2c_idx=0, 	.i2c_mask = I2C_KB_NMI, 	.kbd_idx=1, 	.kbd_mask=KB_U1_L_ALT},      	//44
{	.i2c_dev = 1, 	.i2c_idx=0, 	.i2c_mask = I2C_KB_NMI, 	.kbd_idx=2, 	.kbd_mask=KB_U2_INSERT},     	//45
{	.i2c_dev = 1, 	.i2c_idx=0, 	.i2c_mask = I2C_KB_WIN, 	.kbd_idx=1, 	.kbd_mask=KB_U1_L_WIN},      	//46
{	.i2c_dev = 1, 	.i2c_idx=1, 	.i2c_mask = I2C_KB_LEFT, 	.kbd_idx=2, 	.kbd_mask=KB_U2_LEFT},    		//47
{	.i2c_dev = 1, 	.i2c_idx=1, 	.i2c_mask = I2C_KB_RIGHT, 	.kbd_idx=2, 	.kbd_mask=KB_U2_RIGHT},   		//48
{	.i2c_dev = 1, 	.i2c_idx=1, 	.i2c_mask = I2C_KB_UP,	 	.kbd_idx=2, 	.kbd_mask=KB_U2_UP},      		//49
{	.i2c_dev = 1, 	.i2c_idx=2, 	.i2c_mask = I2C_KB_DOWN, 	.kbd_idx=2, 	.kbd_mask=KB_U2_DOWN},    		//50
{	.i2c_dev = 1, 	.i2c_idx=2, 	.i2c_mask = I2C_KB_FIRE, 	.kbd_idx=1, 	.kbd_mask=KB_U1_R_ALT},   		//51
{	.i2c_dev = 1, 	.i2c_idx=2, 	.i2c_mask = I2C_KB_CTRL, 	.kbd_idx=1, 	.kbd_mask=KB_U1_L_CTRL},  		//52
{	.i2c_dev = 1, 	.i2c_idx=3, 	.i2c_mask = I2C_KB_ALT, 	.kbd_idx=1, 	.kbd_mask=KB_U1_L_ALT},   		//53
{	.i2c_dev = 1, 	.i2c_idx=3, 	.i2c_mask = I2C_KB_PG_UP, 	.kbd_idx=2, 	.kbd_mask=KB_U2_PAGE_UP}, 		//54
{	.i2c_dev = 1, 	.i2c_idx=3, 	.i2c_mask = I2C_KB_PG_DN, 	.kbd_idx=2, 	.kbd_mask=KB_U2_PAGE_DOWN},		//55
{	.i2c_dev = 1, 	.i2c_idx=7, 	.i2c_mask = I2C_KB_F1, 		.kbd_idx=0, 	.kbd_mask=KB_U3_F1},      		//56
{	.i2c_dev = 1, 	.i2c_idx=7, 	.i2c_mask = I2C_KB_F2,	 	.kbd_idx=0, 	.kbd_mask=KB_U3_F2},      		//57
{	.i2c_dev = 1, 	.i2c_idx=7, 	.i2c_mask = I2C_KB_F3,	 	.kbd_idx=0, 	.kbd_mask=KB_U3_F3},      		//58
{	.i2c_dev = 1, 	.i2c_idx=6, 	.i2c_mask = I2C_KB_F4,	 	.kbd_idx=0, 	.kbd_mask=KB_U3_F4},      		//59
{	.i2c_dev = 1, 	.i2c_idx=6, 	.i2c_mask = I2C_KB_F5,	 	.kbd_idx=0, 	.kbd_mask=KB_U3_F5},      		//60
{	.i2c_dev = 1, 	.i2c_idx=6, 	.i2c_mask = I2C_KB_F6,	 	.kbd_idx=0, 	.kbd_mask=KB_U3_F6},      		//61
{	.i2c_dev = 1, 	.i2c_idx=5, 	.i2c_mask = I2C_KB_F7,	 	.kbd_idx=0, 	.kbd_mask=KB_U3_F7},      		//62
{	.i2c_dev = 1, 	.i2c_idx=5, 	.i2c_mask = I2C_KB_F8,	 	.kbd_idx=0, 	.kbd_mask=KB_U3_F8},      		//63
{	.i2c_dev = 1, 	.i2c_idx=5, 	.i2c_mask = I2C_KB_F9,	 	.kbd_idx=0, 	.kbd_mask=KB_U3_F9},      		//64
{	.i2c_dev = 1, 	.i2c_idx=4, 	.i2c_mask = I2C_KB_F10, 	.kbd_idx=0, 	.kbd_mask=KB_U3_F10},     		//65
{	.i2c_dev = 1, 	.i2c_idx=4, 	.i2c_mask = I2C_KB_F11, 	.kbd_idx=0, 	.kbd_mask=KB_U3_F11},     		//66
{	.i2c_dev = 1, 	.i2c_idx=4, 	.i2c_mask = I2C_KB_F12, 	.kbd_idx=0, 	.kbd_mask=KB_U3_F12},     		//67
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//68
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//69
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//70
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//71
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//72
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//73
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//74
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//75
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//76
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//77
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//78
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//79
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//80
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//81
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//82
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//83
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//84
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//85
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//86
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//87
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//88
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//89
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//90
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//91
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//92
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//93
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//94
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//95
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//96
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//97
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//98
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//99
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//100
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//101
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//102
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//103
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//104
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//105
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//106
{	.i2c_dev = 0, 	.i2c_idx=0, 	.i2c_mask = 0,				.kbd_idx=0, 	.kbd_mask=0},             		//107
};