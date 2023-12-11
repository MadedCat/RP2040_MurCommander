#pragma once
#include "inttypes.h"
class Keyboard{
	public:
		bool is_present;
		uint32_t kbd_statuses[4];
		Keyboard();
		~Keyboard();
		virtual bool decode_kbd();
		void sleepUntilRelease();
		void clearKbdBuff();
};