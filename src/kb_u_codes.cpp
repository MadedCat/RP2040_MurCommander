#include "kb_u_codes.h"
#include "string.h"

void convert_kb_u_to_kb_zx(uint32_t* kb_state,uint8_t* zx_kb){
    memset(zx_kb,0,8);

    if (kb_state[0]){
            if (kb_state[0]&KB_U0_A) {zx_kb[1]|=1<<0;};
            if (kb_state[0]&KB_U0_B) {zx_kb[7]|=1<<4;};
            if (kb_state[0]&KB_U0_C) {zx_kb[0]|=1<<3;};
            if (kb_state[0]&KB_U0_D) {zx_kb[1]|=1<<2;};
            if (kb_state[0]&KB_U0_E) {zx_kb[2]|=1<<2;};
            if (kb_state[0]&KB_U0_F) {zx_kb[1]|=1<<3;};
            if (kb_state[0]&KB_U0_G) {zx_kb[1]|=1<<4;};
            if (kb_state[0]&KB_U0_H) {zx_kb[6]|=1<<4;};
            if (kb_state[0]&KB_U0_I) {zx_kb[5]|=1<<2;};
            if (kb_state[0]&KB_U0_J) {zx_kb[6]|=1<<3;};

            if (kb_state[0]&KB_U0_K) {zx_kb[6]|=1<<2;};
            if (kb_state[0]&KB_U0_L) {zx_kb[6]|=1<<1;};
            if (kb_state[0]&KB_U0_M) {zx_kb[7]|=1<<2;};
            if (kb_state[0]&KB_U0_N) {zx_kb[7]|=1<<3;};
            if (kb_state[0]&KB_U0_O) {zx_kb[5]|=1<<1;};
            if (kb_state[0]&KB_U0_P) {zx_kb[5]|=1<<0;};
            if (kb_state[0]&KB_U0_Q) {zx_kb[2]|=1<<0;};
            if (kb_state[0]&KB_U0_R) {zx_kb[2]|=1<<3;};
            if (kb_state[0]&KB_U0_S) {zx_kb[1]|=1<<1;};
            if (kb_state[0]&KB_U0_T) {zx_kb[2]|=1<<4;};

            if (kb_state[0]&KB_U0_U) {zx_kb[5]|=1<<3;};
            if (kb_state[0]&KB_U0_V) {zx_kb[0]|=1<<4;};
            if (kb_state[0]&KB_U0_W) {zx_kb[2]|=1<<1;};
            if (kb_state[0]&KB_U0_X) {zx_kb[0]|=1<<2;};
            if (kb_state[0]&KB_U0_Y) {zx_kb[5]|=1<<4;};
            if (kb_state[0]&KB_U0_Z) {zx_kb[0]|=1<<1;};

            if (kb_state[0]&KB_U0_SEMICOLON) {zx_kb[7]|=1<<1;zx_kb[5]|=1<<1;};
            if (kb_state[0]&KB_U0_QUOTE) {zx_kb[7]|=1<<1;zx_kb[5]|=1<<0;};
            if (kb_state[0]&KB_U0_COMMA) {zx_kb[7]|=1<<1;zx_kb[7]|=1<<3;};
            if (kb_state[0]&KB_U0_PERIOD) {zx_kb[7]|=1<<1;zx_kb[7]|=1<<2;};
            if (kb_state[0]&KB_U0_LEFT_BR) {zx_kb[7]|=1<<1;zx_kb[4]|=1<<2;};
            if (kb_state[0]&KB_U0_RIGHT_BR) {zx_kb[7]|=1<<1;zx_kb[4]|=1<<1;};
    }

    if (kb_state[1]){
            if (kb_state[1]&KB_U1_0) {zx_kb[4]|=1<<0;};
            if (kb_state[1]&KB_U1_1) {zx_kb[3]|=1<<0;};
            if (kb_state[1]&KB_U1_2) {zx_kb[3]|=1<<1;};
            if (kb_state[1]&KB_U1_3) {zx_kb[3]|=1<<2;};
            if (kb_state[1]&KB_U1_4) {zx_kb[3]|=1<<3;};
            if (kb_state[1]&KB_U1_5) {zx_kb[3]|=1<<4;};
            if (kb_state[1]&KB_U1_6) {zx_kb[4]|=1<<4;};
            if (kb_state[1]&KB_U1_7) {zx_kb[4]|=1<<3;};
            if (kb_state[1]&KB_U1_8) {zx_kb[4]|=1<<2;};
            if (kb_state[1]&KB_U1_9) {zx_kb[4]|=1<<1;};

            if (kb_state[1]&KB_U1_ENTER) {zx_kb[6]|=1<<0;};
            if (kb_state[1]&KB_U1_SLASH) {zx_kb[7]|=1<<1;zx_kb[0]|=1<<3;};
            if (kb_state[1]&KB_U1_MINUS) {zx_kb[7]|=1<<1;zx_kb[6]|=1<<3;};

            if (kb_state[1]&KB_U1_EQUALS) {zx_kb[7]|=1<<1;zx_kb[6]|=1<<1;};
            if (kb_state[1]&KB_U1_BACKSLASH) {zx_kb[7]|=1<<1;zx_kb[0]|=1<<1;};
            if (kb_state[1]&KB_U1_CAPS_LOCK) {zx_kb[0]|=1<<0;zx_kb[3]|=1<<1;};
            if (kb_state[1]&KB_U1_TAB) {zx_kb[0]|=1<<0;zx_kb[3]|=1<<0;};
            if (kb_state[1]&KB_U1_BACK_SPACE) {zx_kb[0]|=1<<0;zx_kb[4]|=1<<0;};
            if (kb_state[1]&KB_U1_ESC) {zx_kb[0]|=1<<0;zx_kb[7]|=1<<0;};
            if (kb_state[1]&KB_U1_TILDE) {zx_kb[7]|=1<<1;zx_kb[1]|=1<<0;};
            if (kb_state[1]&KB_U1_MENU) {};

            if (kb_state[1]&KB_U1_L_SHIFT) {zx_kb[0]|=1<<0;};
            if (kb_state[1]&KB_U1_L_CTRL) {zx_kb[7]|=1<<1;};
            if (kb_state[1]&KB_U1_L_ALT) {};//ext.mode?
            if (kb_state[1]&KB_U1_L_WIN) {};
            if (kb_state[1]&KB_U1_R_SHIFT) {zx_kb[0]|=1<<0;};
            if (kb_state[1]&KB_U1_R_CTRL) {zx_kb[7]|=1<<1;};
                if (kb_state[1]&KB_U1_R_ALT) {};//ext.mode?
                if (kb_state[1]&KB_U1_R_WIN) {};
            if (kb_state[1]&KB_U1_SPACE) {zx_kb[7]|=1<<0;};

    }

    if (kb_state[2]){
            if (kb_state[2]&KB_U2_NUM_0) {zx_kb[4]|=1<<0;};
            if (kb_state[2]&KB_U2_NUM_1) {zx_kb[3]|=1<<0;};
            if (kb_state[2]&KB_U2_NUM_2) {zx_kb[3]|=1<<1;};
            if (kb_state[2]&KB_U2_NUM_3) {zx_kb[3]|=1<<2;};
            if (kb_state[2]&KB_U2_NUM_4) {zx_kb[3]|=1<<3;};
            if (kb_state[2]&KB_U2_NUM_5) {zx_kb[3]|=1<<4;};
            if (kb_state[2]&KB_U2_NUM_6) {zx_kb[4]|=1<<4;};
            if (kb_state[2]&KB_U2_NUM_7) {zx_kb[4]|=1<<3;};
            if (kb_state[2]&KB_U2_NUM_8) {zx_kb[4]|=1<<2;};
            if (kb_state[2]&KB_U2_NUM_9) {zx_kb[4]|=1<<1;};

            if (kb_state[2]&KB_U2_NUM_ENTER) {zx_kb[6]|=1<<0;};
            if (kb_state[2]&KB_U2_NUM_SLASH) {zx_kb[7]|=1<<1;zx_kb[0]|=1<<4;};
            if (kb_state[2]&KB_U2_NUM_MINUS) {zx_kb[7]|=1<<1;zx_kb[6]|=1<<3;};

            if (kb_state[2]&KB_U2_NUM_PLUS) {zx_kb[7]|=1<<1;zx_kb[6]|=1<<2;};
            if (kb_state[2]&KB_U2_NUM_MULT) {zx_kb[7]|=1<<1;zx_kb[7]|=1<<4;};
            if (kb_state[2]&KB_U2_NUM_PERIOD) {zx_kb[7]|=1<<1;zx_kb[7]|=1<<2;};

            if (kb_state[2]&KB_U2_NUM_LOCK) {};

            if (kb_state[2]&KB_U2_DELETE) {};
            if (kb_state[2]&KB_U2_SCROLL_LOCK) {};
            if (kb_state[2]&KB_U2_PAUSE_BREAK) {};
            if (kb_state[2]&KB_U2_INSERT) {};
            if (kb_state[2]&KB_U2_HOME) {};
            if (kb_state[2]&KB_U2_PAGE_UP) {};
            if (kb_state[2]&KB_U2_PAGE_DOWN) {};

            if (kb_state[2]&KB_U2_PRT_SCR) {};
            if (kb_state[2]&KB_U2_END) {};
    }


};
