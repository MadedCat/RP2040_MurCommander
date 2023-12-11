#pragma once
#include "inttypes.h"

#define FAST_MENU_LINES 7
#define FAST_SUBMENU_LINES 10

//const uint8_t *fast_menu_lines[5]={10,10,10,2,2};

const char *fast_menu[5][11]={
    { 
        "   Quick Save   ",
        "   Quick Load   ",
        " [FILE BROWSER] ",
        "  [Full Save>]  ",
        "  [Full Load>]  ",
        "     [HELP]     ",
        "   [KEYBOARD]   ",
        "   [SETTINGS]   ",
        "   Soft reset   ",
        "   Hard reset   ",
        "\0",        
    },{
        "[ ]  SAVE 1     ",
        "[ ]  SAVE 2     ",
        "[ ]  SAVE 3     ",
        "[ ]  SAVE 4     ",
        "[ ]  SAVE 5     ",
        "[ ]  SAVE 6     ",
        "[ ]  SAVE 7     ",
        "[ ]  SAVE 8     ",
        "[ ]  SAVE 9     ",
        "[ ]  SAVE 10    ",
        "\0",        
    },{
        "[ ]  LOAD 1     ",
        "[ ]  LOAD 2     ",
        "[ ]  LOAD 3     ",
        "[ ]  LOAD 4     ",
        "[ ]  LOAD 5     ",
        "[ ]  LOAD 6     ",
        "[ ]  LOAD 7     ",
        "[ ]  LOAD 8     ",
        "[ ]  LOAD 9     ",
        "[ ]  LOAD 10    ",
        "\0",
    },{
        "   Quick Save   ",
        "   Full Save>   ",
        "\0",
        "\0",
        "\0",
        "\0",
        "\0",
        "\0",
        "\0",
        "\0",
        "\0",
    },{
        "   Quick Load   ",
        "   Full Load>   ",
        "\0",
        "\0",
        "\0",
        "\0",
        "\0",
        "\0",
        "\0",
        "\0",
        "\0",
    }
};



/*const char *fast_menu_f2[2]={ 
"   Quick Save   ",
"   Full Save>   ",
};

const char *fast_menu_f3[2]={ 
"   Quick Load   ",
"   Full Load>   ",
};

const char *fast_menu_fl[10]={ 
"[ ]  LOAD 1     ",
"[ ]  LOAD 2     ",
"[ ]  LOAD 3     ",
"[ ]  LOAD 4     ",
"[ ]  LOAD 5     ",
"[ ]  LOAD 6     ",
"[ ]  LOAD 7     ",
"[ ]  LOAD 8     ",
"[ ]  LOAD 9     ",
"[ ]  LOAD 10    ",
};

const char *fast_menu_fs[10]={ 
"[ ]  SAVE 1     ",
"[ ]  SAVE 2     ",
"[ ]  SAVE 3     ",
"[ ]  SAVE 4     ",
"[ ]  SAVE 5     ",
"[ ]  SAVE 6     ",
"[ ]  SAVE 7     ",
"[ ]  SAVE 8     ",
"[ ]  SAVE 9     ",
"[ ]  SAVE 10    ",
};*/