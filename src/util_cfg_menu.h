#pragma once
#include <pico/stdlib.h>
#include "util_cfg.h"

const char __in_flash() *config_menu[CONFIG_LINES]={
	"Tap load:[                ]",
	"Def joy mode:[            ]",
	"Reboot ZX before load:[   ]",
	"Snd mode:[                ]",
	"           [LOAD]          ",
	"          [DEFAULT]        ",
	"           [SAVE]          ",
	"\0",
	"\0",
	"\0",
	"\0"
};

const char __in_flash() *tap_load_config[3]={
	" NORMAL LOADING ",
	"AUTOSTOP LOADING",
	"  FAST LOADING  "
};

const char __in_flash() *joy_config[5]={
	"  External  ",
	"  Kempston  ",
	" Interface2 ",
	"   Cursor   ",
	" -=[NONE]=- "
};

const char __in_flash() *reboot_config[2]={
	" No",
	"Yes"
};

const char __in_flash() *sound_config[5]={
	"-=[Sound OFF]=-",
	"  Only Beeper  ",
	" Beeper+SoftAY ",
	" Beeper+SoftTS ",
	" HW TurboSound "
};
