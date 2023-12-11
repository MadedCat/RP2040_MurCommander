#include "keyboard.h"
#include <cstring>
#include <stdio.h>
#include "inttypes.h"

Keyboard::Keyboard(){
	is_present=false;
	memset(kbd_statuses,0,sizeof(kbd_statuses));
	printf("Keyboard Constructor was Called!\n");
};
Keyboard::~Keyboard(){
	is_present=false;
	memset(kbd_statuses,0,sizeof(kbd_statuses));
	printf("Keyboard Destructor was Called!\n");
};
/*bool Keyboard::kbd_present(){
	return false;
};*/
bool Keyboard::decode_kbd(void){
	return false;
};

void Keyboard::sleepUntilRelease(void){
	do{
		decode_kbd();
		sleep_ms(50);
	}while((kbd_statuses[0]==0)&&(kbd_statuses[1]==0)&&(kbd_statuses[2]==0)&&(kbd_statuses[3]==0));
};

void Keyboard::clearKbdBuff(void){
	memset(&kbd_statuses[0],0,sizeof(kbd_statuses));
}