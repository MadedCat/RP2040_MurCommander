#pragma once
#include "inttypes.h"
#include "stdbool.h"
#include "string.h"
#include "display.h"
#include "util_sd.h"
#include "ps2.h"
#include "utf_handle.h"


#define IFACE_WIDTH 	64
#define IFACE_HEIGHT 	34

#define KBD_MAX_COL (9)
#define KBD_MAX_ROW (3)


enum PanelMode {
    LIST_NAMES	= 0,   ///Two column mode
    LIST_DETAIL	= 1,   ///One detailed column mode
    LIST_PREVIEW= 2,    ///Prewiew file
	LIST_OFF 	= 3,    ///Panel_off
	LIST_END 	= 4
};

enum MenuMode {
    MENU_MAIN	= 0,   ///
    MENU_CTRL	= 1,   ///
    MENU_ALT    = 2,   ///
	MENU_SHIFT  = 3,   ///
};

enum CursorSide {
    SIDE_LEFT	= 0,   
    SIDE_RIGHT	= 1,
	SIDE_END	= 2,
};

enum SortMode {
    SORT_MODE_ASC	= 0,   ///Ascending
    SORT_MODE_DESC	= 1,   ///Descending
	SORT_MODE_END	= 2,
};

enum SortType {
    SORT_TYPE_NAME	= 0,	///Sort by Name
    SORT_TYPE_EXT	= 1,	///Sort by Ext
	SORT_TYPE_DATE	= 2,	///Sort by Date
	SORT_TYPE_SIZE	= 3,	///Sort by Size
	SORT_TYPE_UNSORT= 4		///Unsorted list
};

enum DialogPos {
    PANEL_LEFT		= 0,   ///Center of Left panel
    PANEL_RIGHT		= 1,   ///Center of Right panel
    SCREEN_MIDDLE 	= 2,   ///Center of Screen
	SCREEN_FULL 	= 3    ///FULL Screen
};

enum DialogMode {
	DLG_REPAINT		= 0,   ///Need repaint dialog
    DLG_NORMAL		= 1,   ///Normal Dialog
    DLG_INFO		= 2,   ///Information Dialog
	DLG_WARN 		= 3,   ///Warning Dialog
    DLG_ERROR 		= 4    ///Error Dialog
	
};

enum DialogType {
	DIA_OK 			= 0,   ///Ok 
	DIA_OKCANCEL	= 1,   ///Ok Cancel
	DIA_OKRETCAN 	= 2,    ///Ok Retry Cancel	
	DIA_ABRTRETCAN 	= 3,    ///Abort Retry Cancel	
	DIA_DISK		= 4,   ///Choose Disk
	DIA_COPY		= 5,   ///Copy file
	DIA_MKDIR		= 6,   ///Make dir
	DIA_MENU		= 7,   ///Menu
	DIA_HELP		= 8,   ///Main help
	DIA_EDIT		= 9,   ///Edit dialog
	DIA_PREVIEW		= 10   ///Full area	
};

enum SelectType {
	SEL_ONE		= 0,   ///Select one file
	SEL_ADD		= 1,   ///Add select files
	SEL_SUB		= 2,   ///Sub select files
	SEL_INV		= 3   ///Inverse select files
};

enum TextAlign {
	TXT_LEFT	= 0,   ///Align text left
	TXT_RIGHT	= 1,   ///Align text right
	TXT_CENTER	= 2    ///Align text Center
};

class Interface {
	private:
		typedef struct DialogRec{
			int  dialog_id;
			int  cursor_pos;
			int  result;
			bool is_dialog;
			bool is_modal;
			bool need_repaint;
			DialogMode mode;
			DialogPos pos;
			DialogType type;
			char* header;
			char* message;	
		} __attribute__((packed)) DialogRec;

		uint32_t tracer = 0;

		Display* p_disp;
		SDCard* p_card;
		UTFCoverter* p_utf;
		CursorSide cursor_side;

		color_t* p_sheme;
		uint16_t p_width;
		uint16_t p_height;
		uint8_t	 page_column;
		uint8_t  page_full;
		uint16_t i_width;
		uint16_t i_height;
		bool change_dir =false;
		uint32_t last_action = 0;
		uint32_t scroll_action = 0;
		uint16_t scroll_pos =0;		

		char dlg_msg[7][64];
		DialogRec dialogs[7];
		int  c_dialog;
		int dlg_result;

		char temp_msg[128];
		
		
		bool is_help = false;

		uint8_t	 copy_file_perc;
		uint8_t	 copy_total_perc;

		typedef struct PanelRec{
			uint8_t width;
			PanelMode mode;
			uint8_t disk_id;
			char disk[4];
			int dir_depth;
			char dir_path[DIRS_DEPTH*SHORT_NAME_LEN];
			char dirs[DIRS_DEPTH][SHORT_NAME_LEN];
			char files[MAX_FILES][FILE_REC_LEN];
			int files_size[MAX_FILES];
			int files_count;
			int cursor_pos;
			bool draw_cursor;
			char long_filename[260];
			FileRec* bottom_file;
			uint8_t cursor_page;
			SortMode sort_mode;
			SortType sort_type;
			bool draw;
			bool draw_files;	
		} __attribute__((packed)) PanelRecType;

		PanelRecType panels[2];
		PanelMode old_mode[2];

		uint32_t millis();

		void initPanel(uint8_t panel_id);
		void draw_mur_logo_big(uint8_t xPos,uint8_t yPos);
		void calcPanelLine(uint8_t line,uint8_t width,PanelMode mode);
		void drawTextOver(int x,int y,char* text,color_t colorText,color_t colorBg,int len);
		bool drawDialog(uint8_t width,uint8_t height,DialogMode mode,DialogPos pos,DialogType type,char* header,char* message,bool partial=false);
		bool drawPanel(uint16_t offset,uint8_t width,PanelMode mode,uint8_t disk_id,char* path,SortMode sort_mode,SortType sort_type,char* files);
		bool drawFiles(uint16_t offset,uint8_t width,PanelMode mode,uint8_t disk_id,char* path,SortMode sort_mode,SortType sort_type,char* files,
						int files_count,bool draw_cursor,int cursor_pos);
		bool drawFooter(uint16_t offset,uint8_t width,PanelMode mode,FileRec* bottom_file,char* long_name);
		void sortFiles(char* files,int num_files,SortMode sort_mode,SortType sort_type);
		int  qSortCompare(FileRec* file1,FileRec* file2,SortMode sort_mode,SortType sort_type);
		void qSort(char* files, int begin, int end,SortMode sort_mode,SortType sort_type);
		void swapFileRec(char* files,int from, int to);
		char* calcSize(uint32_t size);
		char* calcDate(uint16_t date);
		
	public:
		Interface(uint8_t width, uint8_t height, Display* disp, SDCard* card,color_t* sheme);
		~Interface();
		int countChar(char* text,char schar);
		void splitString(char* message,char* array,int* width,int* height=nullptr);
		void printText(uint16_t offset, uint8_t xPos,uint8_t yPos, uint8_t width,uint8_t height,char* text,color_t color_bg,color_t color_fg,TextAlign align=TXT_LEFT,char splitter=0x0A);
		void changeColorSheme(color_t* sheme);
		void drawPanel(CursorSide side,bool full_draw=false);
		//void drawRightPanel(bool full_draw=false);
		void drawPanels(bool full_draw,MenuMode mode=MENU_MAIN);
		void switchPanelMode(CursorSide side, PanelMode mode=LIST_END);
		void cursorInc(void);
		void cursorDec(void);
		void cursorIncPG(uint8_t page);
		void cursorDecPG(uint8_t page);
		void cursorHome(void);
		void cursorEnd(void);
		void markFile(SelectType sel_type);
		void switchPanel(void);
		void switchSortType(SortType type);
		bool trySwitchCatalog(void);
		void processKeyboard(Keyboard* kbd);
		void processTimers(void);
		void cursorDialogUp();
		void cursorDialogDown();
		void cursorDialogLeft();
		void cursorDialogRight();
		void cursorDialogEnter();	
		void cursorDialogExit();
		void newDialog(DialogMode mode,DialogPos pos,DialogType type,char* header,char* message,bool modal=false);
		void freeDialog(uint8_t dialog_id);
		int processDialog(uint8_t dialog_id,bool partial=false);
		int processDialogResult(int result);
		bool showTextFile(uint16_t offset,uint8_t width,char* file_name);
		void processCopy(void);
		void saveSwap(void);
		void loadSwap(void);
};



/*
void ShowScreenshot(uint8_t* buffer);
void draw_logo_header(uint8_t xPos,uint8_t yPos);
void draw_mur_logo();
void draw_mur_logo_big(uint8_t xPos,uint8_t yPos,uint8_t help);
void draw_help_text(int16_t startLine);
void MessageBox(char *text,char *text1,uint8_t colorFG,uint8_t colorBG,uint8_t over_emul);
void draw_main_window();
void draw_file_window();
void draw_fast_menu(uint8_t xPos,uint8_t yPos,bool drawbg,uint8_t menu,uint8_t active);
void draw_config_menu(uint8_t xPos,uint8_t yPos,bool drawbg,uint8_t active);
void draw_keyboard(uint8_t xPos,uint8_t yPos);
bool LoadTxt(char *file_name);
*/