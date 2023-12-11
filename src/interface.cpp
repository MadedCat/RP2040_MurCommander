#include "inttypes.h"
#include "interface.h"
#include "display.h"
#include "util_sd.h"
#include "utf_handle.h"
#include "mur_logo_big.h"
#include "help.h"
//#include <codecvt>

/*
#include "small_logo.h"
#include "mur_logo.h"
#include "mur_logo2.h"
#include "kbd_img.h"
#include "utf_handle.h"
#include "help.h"
#include "fast_menu.h"
*/

#define NAMES_LIST_WIDTH 15 //autoexec bat|
#define NAMES_LIST_POS 6 //autoexec bat|


#define DETAIL_LIST_NAME_WIDTH 15 //14
#define DETAIL_LIST_SIZE_WIDTH 21 //7
#define DETAIL_LIST_FULL_WIDTH 30  //9

#define DETAIL_LIST_LABEL_NAME_POS 6 //14
#define DETAIL_LIST_LABEL_SIZE_POS 16 //7
#define DETAIL_LIST_LABEL_DATE_POS 24  //9

#define DETAIL_LIST_FIELD_NAME_POS 1 //14
#define DETAIL_LIST_FIELD_SIZE_POS 16 //7
#define DETAIL_LIST_FIELD_DATE_POS 22  //9

#define FOOTER_SIZE_POS	15
#define FOOTER_DATE_POS	9

#define LAST_ACTION_DELAY 450
//#define SHOW_SCREEN_DELAY 400

PanelMode operator++(PanelMode& mode){
    mode = static_cast<PanelMode>((mode + 1) % LIST_END);
    return mode;
}

CursorSide operator++(CursorSide& side){
    side = static_cast<CursorSide>((side + 1) % SIDE_END);
    return side;
}

SortMode operator++(SortMode& sMode){
    sMode = static_cast<SortMode>((sMode + 1) % SORT_MODE_END);
    return sMode;
}

const char SortModeChar[2]={0xB0,0xB1};
const char* SortTypeChar[5]={
	"Name\0",
	"Ext \0",
	"Date\0",
	"Size\0",
	"UnSr\0"
};
const char* DetailChar[3]={
	"Name\0",
	"Size\0",
	"Date\0"
};

const char* bottom_num = {" 1     2     3     4     5     6     7     8     9     10     \0"};
						//"		1      2      3      4      5      6       7      8      9      10     "};
						//BF_SELECT_TEXT=5
const char* bottom_txt = {"\0\6Help \0Menu \0View \0Edit \0Copy \0ReMov\0MkDir\0DelFl\0CMenu\0\0    \0\0"};
const char* bottom_sht = {"\0\6     \0     \0     \0Edit:\0Copy \0Renam\0     \0Delet\0Save \0\0    \0\0"};
const char* bottom_alt = {"\0\6Left \0Right\0View:\0Edit:\0     \0     \0Find \0     \0     \0\0    \0\0"};
const char* bottom_ctr = {"\0\6LMod \0RMod \0Name \0Ext  \0Date \0Size \0Unsor\0ClDef\0ClAlt\0\0RsPn\0\0"};

const char* dir_ptr = {"SUB"};
const char* size_ptr[4] = {"b ","Kb","Mb","Gb"};

//const char* fs_names[SYSTEM_DISK_COUNT] = {"0:\0","F:\0"};
extern const char* fs_names[SYSTEM_DISK_COUNT];
const char* fs_disp_names[SYSTEM_DISK_COUNT] = {"SD:\0","RO:\0"};
const char* fs_descr[SYSTEM_DISK_COUNT] = {	" SD Card  ", " Rom disk "};

const char* iface_btn[5] ={
	"[Ok]\0",
	"[Ok] [Cancel]\0",
	"[Ok] [Retry] [Cancel]\0",
	"[Abort] [Retry] [Cancel]\0",
	"[Cancel]\0"
};

const char* iface_dlg_copy_msg[2] ={
	"Do you want to copy %d files\n to:%s \0",
	"Abort copying files?\0"
};


const char* iface_copy_msg[6] ={
	"Copy\0",
	"From:\0",
	"  To:\0",
	"Total\0",
	"Files:\0",
	"Bytes:\0"
};

const char* fs_err[20] = {
		"Succeeded",															//(0)
		"A hard error occurred in the low level disk I/O layer",				//(1) 
		"Assertion failed",														//(2) 
		"The physical drive cannot work",										//(3) 
		"Could not find the file",												//(4) 
		"Could not find the path",												//(5) 
		"The path name format is invalid",										//(6) 
		"Access denied due to prohibited access or directory full",				//(7) 
		"Access denied due to prohibited access",								//(8) 
		"The file/directory object is invalid",									//(9) 
		"The physical drive is write protected",								//(10)
		"The logical drive number is invalid",									//(11)
		"The volume has no work area",											//(12)
		"There is no valid FAT volume",											//(13)
		"The f_mkfs() aborted due to any problem",								//(14)
		"Could not get a grant to access the volume within defined period",		//(15)
		"The operation is rejected according to the file sharing policy",		//(16)
		"LFN working buffer could not be allocated",							//(17)
		"Number of open files > FF_FS_LOCK",									//(18)
		"Given parameter is invalid"											//(19)
};

Interface::Interface(uint8_t width, uint8_t height, Display* disp, SDCard* card,color_t* sheme){
	p_width=width;
	p_height=height;
	i_width=width;
	i_height=height;


	p_disp = disp;
	p_card = card;
	p_sheme = sheme;
	p_utf = new UTFCoverter();
	c_dialog =-1;
	dlg_result=0;

	copy_file_perc=0;
	copy_total_perc=0;
	initPanel(SIDE_LEFT);
	initPanel(SIDE_RIGHT);

	cursor_side = SIDE_LEFT;
	panels[cursor_side].draw_cursor = true;

	old_mode[0]=LIST_END;
	old_mode[1]=LIST_END;
	
	p_disp->draw_rect(0,0,SCREEN_W,SCREEN_H,p_sheme[C_FULLSCREEN+P_BGR],true);//Заливаем экран
	printf("Interface Constructor was Called!\n");	
};

void Interface::initPanel(uint8_t panel_id){
	panels[panel_id].width= (i_width/2)-1;
	panels[panel_id].mode=LIST_NAMES;
	memset((char*)panels[panel_id].dir_path,0,sizeof(panels[panel_id].dir_path));
	memset((char*)panels[panel_id].disk,0,sizeof(panels[panel_id].disk));
	memset((char*)panels[panel_id].dirs,0,sizeof(panels[panel_id].dirs));
	memset((char*)panels[panel_id].files,0,sizeof(panels[panel_id].files));
	panels[panel_id].disk_id=0;
	strncpy((char*)panels[panel_id].disk,fs_names[panels[panel_id].disk_id],3);
	strcpy((char*)panels[panel_id].dirs[0],fs_names[panels[panel_id].disk_id]);
	panels[panel_id].cursor_pos=0;
	panels[panel_id].draw_cursor = false;
	panels[panel_id].dir_depth=0;
	memset(panels[panel_id].long_filename,0,sizeof(panels[panel_id].long_filename));
	panels[panel_id].bottom_file = nullptr;
	panels[panel_id].files_count=0;
	panels[panel_id].sort_mode = SORT_MODE_ASC;
	panels[panel_id].sort_type = SORT_TYPE_NAME;

}


Interface::~Interface(){
	printf("Interface Destructor was Called!\n");
}

uint32_t Interface::millis(){
	return us_to_ms(time_us_32());
	//return (time_us_32()>>10);//0-4194190
	//return (time_us_32()/1000);
}

void Interface::draw_mur_logo_big(uint8_t xPos,uint8_t yPos){ //x-155 y-60
	for(uint8_t y=0;y<110;y++){
		for(uint8_t x=0;x<138;x++){
			uint8_t pixel = mur_logo_big[x+(y*138)];
			if (pixel<0xFF)	p_disp->draw_pixel(xPos+x,yPos+y,pixel);
		}
	}
};

void Interface::changeColorSheme(color_t* sheme){
	//p_disp->draw_rect(0,0,SCREEN_W,SCREEN_H,p_sheme.c_fullscreen[1],true);//Заливаем экран
	p_sheme = sheme;
	p_disp->draw_rect(0,0,SCREEN_W,SCREEN_H,p_sheme[C_FULLSCREEN+P_BGR],true);//Заливаем экран
}

void Interface::calcPanelLine(uint8_t line,uint8_t width,PanelMode mode){
	if(line>p_height) return;
	memset(temp_msg,0,sizeof(temp_msg));
	uint8_t pos=0;

	if(line<1){
		temp_msg[0]=0x96;
		temp_msg[(width-1)]=0x88;
		for(uint8_t i=1;i<(width-1);i++){temp_msg[i]=0x9A;};
	}
	if((line>0)&&(line<(i_height-2))){
		temp_msg[0]=0x87;
		temp_msg[(width-1)]=0x87;
		for(uint8_t i=1;i<(width-1);i++){temp_msg[i]=0x20;};
	}
	if(line==(i_height-2)){
		temp_msg[0]=0x95;
		temp_msg[(width-1)]=0x89;
		for(uint8_t i=1;i<(width-1);i++){temp_msg[i]=0x9A;};
	}


	if(mode==LIST_NAMES){
		if(line<1){
			for(uint8_t x=1;x<(width-1);x++){
				if(x%(NAMES_LIST_WIDTH)==0){
					temp_msg[x]=0x9E;
				}
			}
		}
		if(line==(i_height-4)){
			temp_msg[0]=0x94;
			temp_msg[(width-1)]=0x83;
			for(uint8_t i=1;i<(width-1);i++){temp_msg[i]=0x91;};
		}		
		if((line>0)&&(line<(i_height-3))){
			for(uint8_t x=1;x<(width-1);x++){
				if(x%(NAMES_LIST_WIDTH)==0){
					if((line<(i_height-4))){
						temp_msg[x]=0x80;
					} else{
						temp_msg[x]=0x8E;
					}
					
				}
			}
		}
	}
	if(mode==LIST_DETAIL){
		if(line<1){
			for(uint8_t x=1;x<(width-1);x++){
				pos = x%DETAIL_LIST_FULL_WIDTH;
				if((pos==DETAIL_LIST_NAME_WIDTH)||(pos==DETAIL_LIST_SIZE_WIDTH)||(pos==0)){
					temp_msg[x]=0x9E;
				}
			}
		}
		if(line==(i_height-4)){
			temp_msg[0]=0x94;
			temp_msg[(width-1)]=0x83;
			for(uint8_t i=1;i<(width-1);i++){temp_msg[i]=0x91;};
		}
		if((line>0)&&(line<(i_height-3))){
			for(uint8_t x=1;x<(width-1);x++){
				pos = x%DETAIL_LIST_FULL_WIDTH;
				if((pos==DETAIL_LIST_NAME_WIDTH)||(pos==DETAIL_LIST_SIZE_WIDTH)||(pos==0)){
					if((line<(i_height-4))){				
						temp_msg[x]=0x80;
					} else{
						temp_msg[x]=0x8E;
					}
				} 
			}
		}
	}
	if(mode==LIST_PREVIEW){

	}
	if(mode==LIST_OFF){
		for(uint8_t i=0;i<(width);i++){temp_msg[i]=0x20;};
	}
}

void Interface::drawTextOver(int x,int y,char* text,color_t colorText,color_t colorBg,int len){
	color_t tmpText =colorText;
	color_t tmpBg   =colorBg;
	uint8_t tag =0;
	for (uint8_t chr=0;chr<len;chr++){
		tag = text[chr];
		if(tag>0){
			if(tag==BF_RESET){
				tmpText =colorText;
				tmpBg   =colorBg;
			}
			if((tag<0x0F)){
				tmpText = p_sheme[(tag-1)*2];
				tmpBg   = p_sheme[(tag-1)*2+1];
				//p_disp->draw_text5x7_len(x+(chr*FONT_5x7_W),y," ",tmpText,tmpBg,1);
			}
			if(tag>=0x20){
				p_disp->draw_text5x7_len(x+(chr*FONT_5x7_W),y,&text[chr],tmpText,tmpBg,1);
			}
		}
	}
}

bool Interface::drawPanel(uint16_t offset,uint8_t width,PanelMode mode,uint8_t disk_id,char* path,SortMode sort_mode,SortType sort_type,char* files){
	//if(mode==LIST_OFF) return true;
	for(uint8_t y=0;y<(i_height-1);y++){
		calcPanelLine(y,width,mode);
		if(mode!=LIST_OFF){
			p_disp->draw_text5x7_len(offset+FONT_5x7_W,(y*FONT_5x7_H)+1,temp_msg,p_sheme[C_BORDER+P_INK],p_sheme[C_BORDER+P_BGR],width);
		} else {
			p_disp->draw_text5x7_len(offset+FONT_5x7_W,(y*FONT_5x7_H)+1,temp_msg,p_sheme[C_FULLSCREEN+P_INK],p_sheme[C_FULLSCREEN+P_BGR],width);
		}
		
		if(y==0){
			if(width>=NAMES_LIST_WIDTH){
				if((mode!=LIST_OFF)&&(mode!=LIST_PREVIEW)){
					memset(temp_msg,0,sizeof(temp_msg));
					uint8_t mwidth=(width-2);
					uint8_t len=strlen(path);
					if(len>mwidth){
						uint8_t ppos= strlen(path)-(mwidth-1);
						p_utf->cp866_win1251(&path[ppos],&temp_msg[2],mwidth);
						strncpy(&temp_msg[1],fs_disp_names[disk_id],2);
					} else{
						p_utf->cp866_win1251(path,&temp_msg[((width-2)/2)-(strlen(path)/2)+1],strlen(path));
						strncpy(&temp_msg[((width-2)/2)-(strlen(path)/2)],fs_disp_names[disk_id],2);
					}
					drawTextOver(offset+FONT_5x7_W,(y*FONT_5x7_H)+1,temp_msg,p_sheme[C_INVERSE_TEXT+P_INK],p_sheme[C_INVERSE_TEXT+P_BGR],width);
					//p_disp->draw_text5x7_len(offset+FONT_5x7_W+(FONT_5x7_W*pos),(y*FONT_5x7_H)+1,&temp_msg[1],p_sheme[C_INVERSE_TEXT+P_INK],p_sheme[C_INVERSE_TEXT+P_BGR],len);
				}
			}
		}		
	}	
	return true;
}

char* Interface::calcSize(uint32_t size){
	static char s_text[6];
	memset(&s_text[0],0x20,5);
	s_text[5]=0;
	uint32_t temp=size;
	uint8_t ndx=0;
	if(size==0){
		sprintf(&s_text[0],"%3d",0);
		memset(&s_text[3],0x20,2);
		return s_text;
	}
	while (size>0){size/=1024;++ndx;}
	size=temp; ndx--;
	for (uint8_t i = 0; i < ndx; i++){size/=1024;}
	if(ndx==0){
		sprintf(&s_text[0],"%3d",size);
		memcpy(&s_text[3],size_ptr[ndx],1);
	} else {
		sprintf(&s_text[0],"%3d",size);
		memcpy(&s_text[3],size_ptr[ndx],2);
	}
	return s_text;
}

char* Interface::calcDate(uint16_t date){
	static char date_t[9];
	memset(&date_t[0],0x20,8);
	date_t[8]=0;
	uint8_t day = date&0x1F;
	uint8_t month = (date>>5)&0x0F;
	uint16_t year = ((date>>9)&0xFF)+1980;
	char year_t[5];
	
	sprintf(&year_t[0],"%04d",year);						
	sprintf(&date_t[0],"%02d.%02d.%02s",day,month,&year_t[2]);
	return date_t;
}

bool Interface::drawFiles(uint16_t offset,uint8_t width,PanelMode mode,uint8_t disk_id,char* path,SortMode sort_mode,SortType sort_type,
							char* files,int files_count,bool draw_cursor,int cursor_pos){
	if(mode==LIST_OFF) return true;
	if(width<NAMES_LIST_WIDTH)  return true;
	page_column = (i_height-6);
	uint8_t pos=0;
	FileRec* file = nullptr;
	FileRec* bottom_file = nullptr;
	if(mode==LIST_NAMES) page_full = page_column*round(width/NAMES_LIST_WIDTH);
	if(mode==LIST_DETAIL) page_full = page_column*round(width/DETAIL_LIST_FULL_WIDTH);

	uint8_t folder_pos = round((cursor_pos*100)/(files_count-1));
	//printf("page:%d  \n",page_column);
	//char fname[SHORT_NAME_LEN+2];
	for(uint8_t y=0;y<i_height;y++){
		memset(temp_msg,0,sizeof(temp_msg));
		if(y==1){//HEADER
			memset(temp_msg,0,sizeof(temp_msg));
			if(mode==LIST_NAMES){
				strncpy(&temp_msg[1],fs_disp_names[disk_id],3);
				temp_msg[4]= SortModeChar[(uint8_t)sort_mode];
				for(uint8_t x=0;x<(width-1);x++){
					if((x%NAMES_LIST_WIDTH)==0){
						strncpy(&temp_msg[x+NAMES_LIST_POS],SortTypeChar[(uint8_t)sort_type],5);
					}
				}
			}
			if(mode==LIST_DETAIL){
				strncpy(&temp_msg[1],fs_disp_names[disk_id],3);
				temp_msg[4]= SortModeChar[(uint8_t)sort_mode];
				for(uint8_t x=0;x<(width-1);x++){
					pos = x%DETAIL_LIST_FULL_WIDTH;
					if(pos==DETAIL_LIST_LABEL_NAME_POS){
						strncpy(&temp_msg[x],SortTypeChar[(uint8_t)sort_type],5);
					}
					if(pos==DETAIL_LIST_LABEL_SIZE_POS){
						strncpy(&temp_msg[x],DetailChar[1],5);
					}
					if(pos==DETAIL_LIST_LABEL_DATE_POS){
						strncpy(&temp_msg[x],DetailChar[2],5);
					}
				}
			}			
			drawTextOver(offset+FONT_5x7_W,(y*FONT_5x7_H)+1,temp_msg,p_sheme[C_SELECT_TEXT+P_INK],p_sheme[C_SELECT_TEXT+P_BGR],width);
		}//HEADER
		if(((y>1)&&y<(i_height-4))&&(p_card->init_fs)){//BODY
			if(mode==LIST_NAMES){
				//printf("LIST_NAMES:%d  ",y);
				uint16_t page_rel = round((cursor_pos)/page_full);
				uint16_t cursor_rel_pos=cursor_pos%(page_full);
				for(uint8_t x=0;x<width;x++){
					if((x%NAMES_LIST_WIDTH)==0){
						uint16_t cursor_rel = (x/NAMES_LIST_WIDTH)*(i_height-6)+(y-2);
						uint16_t file_rel = ((x/NAMES_LIST_WIDTH)*(i_height-6)*sizeof(FileRec))+((y-2)*sizeof(FileRec))+(page_rel*(page_full*sizeof(FileRec)));
						memset(&temp_msg[x+1],0x20,SHORT_NAME_LEN+1);
						file = (FileRec*)&files[file_rel];
						char is_dir = file->attr;
						if(is_dir&AM_DIR){
							temp_msg[x]=BF_FOLDER_TEXT;
							temp_msg[x+SHORT_NAME_LEN+2]=0x10;
						}

						const char* ext = p_card->get_file_extension(file->filename);
           				uint8_t len = strlen(file->filename)<(SHORT_NAME_LEN)?strlen(file->filename):(SHORT_NAME_LEN-1);
						memset(&temp_msg[x+1],0x20,(SHORT_NAME_LEN-1));
						//if(!(is_dir&AM_DIR)){
						if(is_dir&AM_RDO)temp_msg[x+(SHORT_NAME_LEN-3)]=0xAC;
						if(is_dir&AM_HID)temp_msg[x+(SHORT_NAME_LEN-3)]=0xAD;
						if(is_dir&AM_SYS)temp_msg[x+(SHORT_NAME_LEN-3)]=0xAE;
						if((is_dir&AM_RDO)||(is_dir&AM_HID)||(is_dir&AM_SYS)){
							temp_msg[x]=BF_BACKGROUND;
							temp_msg[x+SHORT_NAME_LEN+2]=BF_RESET;
						}
						if(is_dir&SELECTED_FILE_ATTR){
							temp_msg[x]=BF_SELECT_TEXT;
							temp_msg[x+SHORT_NAME_LEN+2]=BF_RESET;
						}
						if(strlen(ext)>0){
							p_utf->cp866_win1251(ext, &temp_msg[x+(SHORT_NAME_LEN-1)], 3);
							p_utf->cp866_win1251(file->filename, &temp_msg[x+1], (len-(strlen(ext)+1)));
						} else {
							p_utf->cp866_win1251(file->filename, &temp_msg[x+1],len);
						}
						if ((draw_cursor)&&(cursor_rel==cursor_rel_pos)){
							bottom_file = (FileRec*)&files[file_rel];
							//printf("<C:%d",x);
							if(is_dir&SELECTED_FILE_ATTR){
								temp_msg[x]=BF_CURSOR_SEL;
							} else {
								temp_msg[x]=BF_CURSOR;
							}
							temp_msg[x+SHORT_NAME_LEN+2]=BF_RESET;
							//memset(long_name,0,sizeof(long_name));
							//strcpy(long_name,p_card->get_lfn_from_dir(path,&files[file_rel]));
						}
						
					}
				}
				//printf("\n");
			}
			if(mode==LIST_DETAIL){
				//printf("LIST_DETAIL\n");
				uint16_t page_rel = 0;
				uint16_t cursor_rel = 0;
				uint16_t cursor_rel_pos=0;
				uint16_t file_rel = 0;
				char is_dir;
				memset(&temp_msg[1],0x20,width-1);
				page_rel = round((cursor_pos)/page_full);
				cursor_rel_pos=cursor_pos%(page_full);
				for(uint8_t x=0;x<width;x++){
					pos = x%DETAIL_LIST_FULL_WIDTH;
					if((pos==DETAIL_LIST_NAME_WIDTH)||(pos==DETAIL_LIST_SIZE_WIDTH)||(pos==0)){
						if(temp_msg[x]!=BF_RESET)temp_msg[x]=0x00;
					}
					if(pos==DETAIL_LIST_FIELD_NAME_POS){
						cursor_rel = (x/(DETAIL_LIST_FULL_WIDTH))*(i_height-6)+(y-2);
						file_rel = ((x/(DETAIL_LIST_FULL_WIDTH))*(i_height-6)*sizeof(FileRec))+((y-2)*sizeof(FileRec))+(page_rel*(page_full*sizeof(FileRec)));						
						//strncpy(&temp_msg[x+DETAIL_LIST_NAME_POS],SortTypeChar[(uint8_t)sort_type],5);
						file = (FileRec*)&files[file_rel];
						is_dir = file->attr;
						if(is_dir&AM_DIR){
							temp_msg[x-1]=BF_FOLDER_TEXT;
							temp_msg[x+DETAIL_LIST_FULL_WIDTH-1]=BF_RESET;
						}
						const char* ext = p_card->get_file_extension(file->filename);
						uint8_t len = strlen(file->filename)<(SHORT_NAME_LEN)?strlen(file->filename):(SHORT_NAME_LEN-1);
						memset(&temp_msg[x],0x20,(SHORT_NAME_LEN-1));
						//if(!(is_dir&AM_DIR)){
							if(is_dir&AM_RDO)temp_msg[x+(SHORT_NAME_LEN-4)]=0xAC;
							if(is_dir&AM_HID)temp_msg[x+(SHORT_NAME_LEN-4)]=0xAD;
							if(is_dir&AM_SYS)temp_msg[x+(SHORT_NAME_LEN-4)]=0xAE;
						//}
						if((is_dir&AM_RDO)||(is_dir&AM_HID)||(is_dir&AM_SYS)){
							temp_msg[x-1]=BF_BACKGROUND;
							temp_msg[x+DETAIL_LIST_FULL_WIDTH-1]=BF_RESET;
						}
		
						/*if(file->filename[0]==0x84){
							if(strlen(ext)>0){
								p_utf->ucs16_win1251(ext, &temp_msg[x+(SHORT_NAME_LEN-1)], 3);
								char tmp[SHORT_NAME_LEN];
								memset(tmp,0,SHORT_NAME_LEN);
								strncpy(tmp,file->filename,(len-4));
								p_utf->ucs16_win1251(tmp, &temp_msg[x+1], (len-4));
							} else {
								p_utf->ucs16_win1251(file->filename, &temp_msg[x+1],len);
							}
						} else{*/
							if(strlen(ext)>0){
								p_utf->cp866_win1251(ext, &temp_msg[x+(SHORT_NAME_LEN-2)], 3);
								p_utf->cp866_win1251(file->filename, &temp_msg[x], (len-4));
							} else {
								p_utf->cp866_win1251(file->filename, &temp_msg[x],len);
							}
						//}
						if ((draw_cursor)&&(cursor_rel==cursor_rel_pos)){
							bottom_file = (FileRec*)&files[file_rel];
							temp_msg[x-1]=BF_CURSOR;
							temp_msg[x+DETAIL_LIST_FULL_WIDTH-1]=BF_RESET;
							//memset(long_name,0,sizeof(long_name));
							//strcpy(long_name,p_card->get_lfn_from_dir(path,&files[file_rel]));
						}						
					}
					if(pos==DETAIL_LIST_FIELD_SIZE_POS){
						//strncpy(&temp_msg[x+DETAIL_LIST_SIZE_POS],DetailChar[1],5);
						if((is_dir&AM_DIR)||(is_dir&TOP_DIR_ATTR)){
							if(!(is_dir&TOP_DIR_ATTR)){
								temp_msg[x]=0xB3;
								memcpy(&temp_msg[x+1],dir_ptr,sizeof(dir_ptr));
								temp_msg[x+sizeof(dir_ptr)]=0xB2;
							} else {
								memset(&temp_msg[x],0x20,5);
								memset(&temp_msg[x+sizeof(dir_ptr)],0x20,8);
							}
						} else {
							if(strlen(file->filename)>0){
								memcpy(&temp_msg[x+1],calcSize(file->size),4);
							}
						}						
					}
					if(pos==DETAIL_LIST_FIELD_DATE_POS){
						if((strlen(file->filename)>0)&&(file->date>0)){
							memcpy(&temp_msg[x],calcDate(file->date),8);
						}				

					}
				}				
			}
			drawTextOver(offset+FONT_5x7_W,(y*FONT_5x7_H)+1,temp_msg,p_sheme[C_TEXT+P_INK],p_sheme[C_TEXT+P_BGR],width);
			memset(temp_msg,0,sizeof(temp_msg));
			if((mode==LIST_NAMES)||(mode==LIST_DETAIL)){
				if(y==2){
					temp_msg[width-1]=0xB4;
				}
				if((y>2)&&(y<(i_height-5))){
					temp_msg[width-1]=0xAD;
					uint8_t scroller_pos = round((i_height-9)*((float)folder_pos/100));
					//printf("folder_pos:%d\t\tscroller_pos:%d\t\t\n",folder_pos,scroller_pos);
					if(y==(scroller_pos+3)){
						temp_msg[width-1]=0xAF;
					}
				}
				if(y==(i_height-5)){
					temp_msg[width-1]=0xB5;
				}
			}			
			drawTextOver(offset+FONT_5x7_W,(y*FONT_5x7_H)+1,temp_msg,p_sheme[C_TEXT+P_INK],p_sheme[C_TEXT+P_BGR],width);
		}//BODY

		if((mode==LIST_NAMES)||(mode==LIST_DETAIL)){//FOOTER
			if(y==(i_height-3)){
				if(bottom_file!=nullptr){
					panels[cursor_side].bottom_file=bottom_file;
					drawFooter(offset,width,mode,bottom_file,"\0");
				}
			}
			if(y==(i_height-2)){
				int f_count = files_count;
				int c_pos = cursor_pos+1;
				if(panels[cursor_side].dir_depth>0){f_count--;c_pos--;};
				memset(&temp_msg[0],0,width);
				sprintf(&temp_msg[(width-12)],"(%04d/%04d)",c_pos,f_count);
				drawTextOver(offset+FONT_5x7_W,(y*FONT_5x7_H)+1,temp_msg,p_sheme[C_TEXT+P_INK],p_sheme[C_TEXT+P_BGR],width);
			}
		}//FOOTER
	}
	//printf("\n");
	return true;
	
}

bool Interface::drawFooter(uint16_t offset,uint8_t width,PanelMode mode,FileRec* bottom_file,char* long_name){
	if((mode==LIST_NAMES)||(mode==LIST_DETAIL)){	
		memset(&temp_msg[0],0,width);
		if(width<SHORT_NAME_LEN){
			return false;
		}
		memset(&temp_msg[1],0x20,width-2);
		p_utf->cp866_win1251(bottom_file->filename, &temp_msg[DETAIL_LIST_FIELD_NAME_POS],(width-FOOTER_SIZE_POS));

		if (long_name!=nullptr){
			uint8_t len=strlen(long_name);
			if(len>0){
				p_utf->cp866_win1251(long_name, &temp_msg[DETAIL_LIST_FIELD_NAME_POS],(width-FOOTER_SIZE_POS));
			}
		}
		char is_dir = bottom_file->attr;
		if(bottom_file->attr&AM_DIR){
			if(width>=(DETAIL_LIST_SIZE_WIDTH)){
				temp_msg[width-FOOTER_SIZE_POS]=0xB3;
				memcpy(&temp_msg[width-FOOTER_SIZE_POS+1],dir_ptr,sizeof(dir_ptr));
				temp_msg[width-FOOTER_SIZE_POS+sizeof(dir_ptr)]=0xB2;
			}
			if(width>=(DETAIL_LIST_FULL_WIDTH)){
				memset(&temp_msg[width-FOOTER_DATE_POS],0x20,8);
				memcpy(&temp_msg[width-FOOTER_DATE_POS],calcDate(bottom_file->date),8);
			}
		} else 
		if(bottom_file->attr&TOP_DIR_ATTR){
			if(width>=(DETAIL_LIST_SIZE_WIDTH)) memset(&temp_msg[width-FOOTER_SIZE_POS],0x20,5);
			if(width>=(DETAIL_LIST_FULL_WIDTH)) memset(&temp_msg[width-FOOTER_DATE_POS],0x20,8);
		} else {
			if(width>=(DETAIL_LIST_SIZE_WIDTH)){
				memset(&temp_msg[width-FOOTER_SIZE_POS],0x20,5);
				memcpy(&temp_msg[width-FOOTER_SIZE_POS],calcSize(bottom_file->size),4);
			}
			if(width>=(DETAIL_LIST_FULL_WIDTH)){
				memset(&temp_msg[width-FOOTER_DATE_POS],0x20,8);
				memcpy(&temp_msg[width-FOOTER_DATE_POS],calcDate(bottom_file->date),8);
			}
		}
		drawTextOver(offset+FONT_5x7_W,((i_height-3)*FONT_5x7_H)+1,temp_msg,p_sheme[C_TEXT+P_INK],p_sheme[C_TEXT+P_BGR],width);
	}
	return true;
}

int Interface::countChar(char* text,char schar){
	int count =0;
	char* text_ptr = text;
    while ((text_ptr = strchr(text_ptr, schar)) != nullptr) {
        ++text_ptr;
		++count;
    }
	if(count>0) return count+1;
	return count;
}

void Interface::splitString(char* message,char* array,int* width,int* height){
	int len = 0;
	int maxlen = 0;
	int ptrIn = 0;
	int ptrOut = 0;
	int lines = 0;
	int sp_width = *width;
	int full_len = (strlen(message)/sp_width);
	if(full_len==0) full_len=1;
	char temp[sp_width+5];

	//printf("Split:[%02d][%02d]\n",sp_width,full_len);
	char* text_ptr = &temp[0];

	while (true){
		//printf("line:[%02d]\n",lines);
		memset(&temp[0],0,sp_width+5);
		memcpy(&temp[0],&message[ptrIn],sp_width+1);

		if (lines<full_len){
			text_ptr=&temp[0];
			if((text_ptr = strrchr(text_ptr, 0x0A)) != nullptr){
				len = (int)(text_ptr-temp);
			} else if(len==0) {
				text_ptr=&temp[0];
				//printf("Else 1\n");
				if((text_ptr = strrchr(text_ptr, 0x20)) != nullptr){
					len = (int)(text_ptr-&temp[0]);
					//printf("Spl:[%02X][%02d]\n",0x20,len);
				} else {
					len=0;
				}
			}
		}
		if(len>0){
			//printf("Exit 1\n");
			if(len>maxlen){maxlen=len;};
			ptrIn+=len+1;
			if(array!=nullptr) 
				memcpy(&array[ptrOut],&temp[0],len);	
			len=0;
			ptrOut+=sp_width;
			lines++;
		} else {
			//printf("Exit 2\n");
			len = strlen(temp);
			if(len>maxlen){maxlen=len;};
			if((len>0)&&(array!=nullptr)) 
					memcpy(&array[ptrOut],&temp[0],strlen(temp));
			lines++;
			break;
		}
    }
	if(array==nullptr){
		if(maxlen%2>0){maxlen++;};
		*width = maxlen;
		*height = lines;
	}
}

void Interface::printText(uint16_t offset, uint8_t xPos,uint8_t yPos, uint8_t width,uint8_t height,char* text,color_t color_bg,color_t color_fg,TextAlign align,char splitter){
	uint16_t ptr=0;
	uint8_t len=strlen(text);

	int prt_width = (int)width;
	//len = (strlen(text)/pos)-5;0
	char lines[height][prt_width];
	memset(&lines[0],0,sizeof(lines));
	splitString(text,(char*)&lines[0],&prt_width);

	for (uint8_t i = 0; i < height; i++){
		memset(&temp_msg[0], 0, sizeof(temp_msg));
		
		len = strlen(lines[i]);
		//printf("len:%d width:%d\n",len,width);
		if(len>0){
			if(align==TXT_CENTER){
				int pos=(width/2)-(len/2)-1;
				if(pos<0) pos=0;
				memcpy(&temp_msg[pos],&lines[i],len);
			} else
			if(align==TXT_LEFT){
				memcpy(&temp_msg[0],&lines[i],len);
			} else 
			if(align==TXT_RIGHT){
				memcpy(&temp_msg[width-len],&text[ptr],len);
			}
			drawTextOver(offset+FONT_5x7_W+(xPos*FONT_5x7_W),(yPos*FONT_5x7_H)+(i*FONT_5x7_H)+1,temp_msg,color_fg,color_bg,width);
		}
	}
}

bool Interface::showTextFile(uint16_t offset,uint8_t width,char* file_name){
	int res =0;
	size_t bytesRead;
	size_t bytesToRead;
	size_t FileSize;

	uint8_t* sd_buffer = (uint8_t*) malloc(SD_BUFFER_SIZE);
	
	if(sd_buffer==nullptr) return false;

	memset(&sd_buffer[0], 0, SD_BUFFER_SIZE);

	res = p_card->sd_open_file(&p_card->file_handle1,file_name,FA_READ);
    printf("sd_open_file=%d\n",res);
	if (res!=FR_OK){p_card->sd_close_file(&p_card->file_handle1);return false;}
   	FileSize = p_card->sd_file_size(&p_card->file_handle1);
    printf("text Filesize %u bytes\n", FileSize);
	res = p_card->sd_read_file(&p_card->file_handle1,sd_buffer,sizeof(sd_buffer),&bytesRead);

	memset(temp_msg, 0, sizeof(temp_msg));
	memcpy(&temp_msg[(width/2)-7],"File contents:",14);
	drawTextOver(offset+FONT_5x7_W,1,temp_msg,p_sheme[C_TEXT+P_INK],p_sheme[C_TEXT+P_BGR],width);
	memset(temp_msg, 0, sizeof(temp_msg));
	sprintf(temp_msg,"File size:%dk",(FileSize/1024));
	drawTextOver(offset+FONT_5x7_W,1,temp_msg,p_sheme[C_TEXT+P_INK],p_sheme[C_TEXT+P_BGR],width);
	printText(offset,1,1,width,i_height,(char*)sd_buffer,p_sheme[C_TEXT+P_INK],p_sheme[C_TEXT+P_BGR]);
	p_card->sd_close_file(&p_card->file_handle1);
	return true;	
}																															   

void Interface::switchPanelMode(CursorSide side, PanelMode mode){
	
	panels[side].draw = false;
	panels[side].draw_files=false;
	/*if((panels[SIDE_LEFT].mode==LIST_OFF)&&(panels[SIDE_RIGHT].mode==LIST_OFF)){
		cursor_side=SIDE_LEFT;
		panels[cursor_side].draw_cursor=true;
		right_draw_cursor=false;
		left_draw_files=false;
	}*/
	if(mode!=LIST_END){
		panels[side].mode=mode;
	} else {
		panels[side].mode=operator++(panels[side].mode);
	}
	/*if((left_mode==LIST_OFF)&&(right_mode!=LIST_OFF)){
		cursor_side=SIDE_RIGHT;
		left_draw_cursor=false;
		right_draw_cursor=true;
		right_draw_files=false;
	}*/
	
};

/*
void Interface::switchRightPanelMode(PanelMode mode){
	right_draw = false;
	right_draw_files=false;
	if((left_mode==LIST_OFF)&&(right_mode==LIST_OFF)){
		cursor_side=SIDE_RIGHT;
		right_draw_cursor=true;
		left_draw_cursor=false;
		right_draw_files=false;
	}
	if(mode!=LIST_END){
		right_mode=mode;
	} else {
		right_mode=operator++(right_mode);
	}
	if((right_mode==LIST_OFF)&&(left_mode!=LIST_OFF)){
		cursor_side=SIDE_LEFT;
		right_draw_cursor=false;
		left_draw_cursor=true;
		left_draw_files=false;
	}
};
*/

void Interface::swapFileRec(char* files,int from, int to){
	uint8_t tmp_buf[sizeof(FileRec)];
	memcpy(tmp_buf,files+sizeof(FileRec)*from,sizeof(FileRec));
	memcpy(files+sizeof(FileRec)*from,files+sizeof(FileRec)*to,sizeof(FileRec));
	memcpy(files+sizeof(FileRec)*to,tmp_buf,sizeof(FileRec));
}

int  Interface::qSortCompare(FileRec* file1,FileRec* file2,SortMode sort_mode,SortType sort_type){
	if(sort_type==SORT_TYPE_NAME){
		if(sort_mode==SORT_MODE_ASC){
			if(strlen(file1->filename)==0) return 1;
			if(strlen(file2->filename)==0) return 1;
			if((file1->attr&AM_DIR)>(file2->attr&AM_DIR)){
				return -1;
			}
			if((file1->attr&AM_DIR)<(file2->attr&AM_DIR)){
				return 1;
			}
			if((file1->attr&AM_DIR)==(file2->attr&AM_DIR)){
				if(strcmp(file1->filename,file2->filename)>0){
					return 1;
				}
				if(strcmp(file1->filename,file2->filename)<0){
					return -1;
				}
			}
		}
		if(sort_mode==SORT_MODE_DESC){
			if(strlen(file1->filename)==0) return 1;
			if(strlen(file2->filename)==0) return 1;
			if((file1->attr&AM_DIR)>(file2->attr&AM_DIR)){
				return -1;
			}
			if((file1->attr&AM_DIR)<(file2->attr&AM_DIR)){
				return 1;
			}
			if((file1->attr&AM_DIR)==(file2->attr&AM_DIR)){
				if(strcmp(file1->filename,file2->filename)>0){
					return -1;
				}
				if(strcmp(file1->filename,file2->filename)<0){
					return 1;
				}
			}
		}

	}
	if(sort_type==SORT_TYPE_EXT){
		if(sort_mode==SORT_MODE_ASC){
			if(strlen(file1->filename)==0) return 1;
			if(strlen(file2->filename)==0) return 1;
			if((file1->attr&AM_DIR)>(file2->attr&AM_DIR)){
				return -1;
			}
			if((file1->attr&AM_DIR)<(file2->attr&AM_DIR)){
				return 1;
			}
			if((file1->attr&AM_DIR)==(file2->attr&AM_DIR)){
				if(strcmp(p_card->get_file_extension(file1->filename),p_card->get_file_extension(file2->filename))>0){
					return 1;
				}
				if(strcmp(p_card->get_file_extension(file1->filename),p_card->get_file_extension(file2->filename))<0){
					return -1;
				}
				if(strcmp(p_card->get_file_extension(file1->filename),p_card->get_file_extension(file2->filename))==0){
					if(strcmp(file1->filename,file2->filename)>0){
						return 1;
					}
					if(strcmp(file1->filename,file2->filename)<0){
						return -1;
					}
				}
			}
		}
		if(sort_mode==SORT_MODE_DESC){
			if(strlen(file1->filename)==0) return 1;
			if(strlen(file2->filename)==0) return 1;
			if((file1->attr&AM_DIR)>(file2->attr&AM_DIR)){
				return -1;
			}
			if((file1->attr&AM_DIR)<(file2->attr&AM_DIR)){
				return 1;
			}
			if((file1->attr&AM_DIR)==(file2->attr&AM_DIR)){
				if(strcmp(p_card->get_file_extension(file1->filename),p_card->get_file_extension(file2->filename))>0){
					return -1;
				}
				if(strcmp(p_card->get_file_extension(file1->filename),p_card->get_file_extension(file2->filename))<0){
					return 1;
				}
				if(strcmp(p_card->get_file_extension(file1->filename),p_card->get_file_extension(file2->filename))==0){
					if(strcmp(file1->filename,file2->filename)>0){
						return -1;
					}
					if(strcmp(file1->filename,file2->filename)<0){
						return 1;
					}
				}
			}
		}
	}
	if(sort_type==SORT_TYPE_DATE){
		if(sort_mode==SORT_MODE_ASC){
			if(strlen(file1->filename)==0) return 1;
			if(strlen(file2->filename)==0) return 1;
			if((file1->attr&AM_DIR)>(file2->attr&AM_DIR)){
				return -1;
			}
			if((file1->attr&AM_DIR)<(file2->attr&AM_DIR)){
				return 1;
			}
			if((file1->attr&AM_DIR)==(file2->attr&AM_DIR)){
				if(file1->date>file2->date){
					return 1;
				}
				if(file1->date<file2->date){
					return -1;
				}
				if(file1->date==file2->date){
					if(strcmp(file1->filename,file2->filename)>0){
						return 1;
					}
					if(strcmp(file1->filename,file2->filename)<0){
						return -1;
					}
				}
			}
		}
		if(sort_mode==SORT_MODE_DESC){
			if(strlen(file1->filename)==0) return 1;
			if(strlen(file2->filename)==0) return 1;
			if((file1->attr&AM_DIR)>(file2->attr&AM_DIR)){
				return -1;
			}
			if((file1->attr&AM_DIR)<(file2->attr&AM_DIR)){
				return 1;
			}
			if((file1->attr&AM_DIR)==(file2->attr&AM_DIR)){
				if(file1->date>file2->date){
					return -1;
				}
				if(file1->date<file2->date){
					return 1;
				}
				if(file1->date==file2->date){
					if(strcmp(file1->filename,file2->filename)>0){
						return -1;
					}
					if(strcmp(file1->filename,file2->filename)<0){
						return 1;
					}
				}
			}
		}
	}
	if(sort_type==SORT_TYPE_SIZE){
		if(sort_mode==SORT_MODE_ASC){
			if(strlen(file1->filename)==0) return 1;
			if(strlen(file2->filename)==0) return 1;
			if((file1->attr&AM_DIR)>(file2->attr&AM_DIR)){
				return -1;
			}
			if((file1->attr&AM_DIR)<(file2->attr&AM_DIR)){
				return 1;
			}
			if((file1->attr&AM_DIR)==(file2->attr&AM_DIR)){
				if(file1->size>file2->size){
					return 1;
				}
				if(file1->size<file2->size){
					return -1;
				}
				if(file1->size==file2->size){
					if(strcmp(file1->filename,file2->filename)>0){
						return 1;
					}
					if(strcmp(file1->filename,file2->filename)<0){
						return -1;
					}
				}
			}
		}
		if(sort_mode==SORT_MODE_DESC){
			if(strlen(file1->filename)==0) return 1;
			if(strlen(file2->filename)==0) return 1;
			if((file1->attr&AM_DIR)>(file2->attr&AM_DIR)){
				return -1;
			}
			if((file1->attr&AM_DIR)<(file2->attr&AM_DIR)){
				return 1;
			}
			if((file1->attr&AM_DIR)==(file2->attr&AM_DIR)){
				if(file1->size>file2->size){
					return -1;
				}
				if(file1->size<file2->size){
					return 1;
				}
				if(file1->size==file2->size){
					if(strcmp(file1->filename,file2->filename)>0){
						return -1;
					}
					if(strcmp(file1->filename,file2->filename)<0){
						return 1;
					}
				}
			}
		}
	}
	if(sort_type==SORT_TYPE_UNSORT){
		if(sort_mode==SORT_MODE_ASC){
			if(strlen(file1->filename)==0) return 1;
			if(strlen(file2->filename)==0) return 1;
			return -1;
		}
		if(sort_mode==SORT_MODE_DESC){
			if(strlen(file1->filename)==0) return 1;
			if(strlen(file2->filename)==0) return 1;
			return 1;
		}
	}

	return 0;
}

void Interface::qSort(char* files, int begin, int end,SortMode sort_mode,SortType sort_type){

	FileRec* file1;
	FileRec* file2;
	
    int l = begin, r = end;
	int center = l+(r-l)/2;
	FileRec* v = (FileRec*)&files[sizeof(FileRec)*center];
    //int v = nums[l+(r-l)/2];
	
    while(l <= r){
        while(true){
			file1 = (FileRec*)&files[sizeof(FileRec)*l];
			if ((file1->attr&TOP_DIR_ATTR)){l++; continue;};
			if((qSortCompare(file1,v,sort_mode,sort_type)<0)&&(l<end)){
				l++;
			} else {
				break;
			}
		}
        while(true){
			file1 = (FileRec*)&files[sizeof(FileRec)*r];
			//if(strlen(file1->filename)==0){r--;continue;};
			if((qSortCompare(file1,v,sort_mode,sort_type)>0)&&(r>begin)){
				r--;
			} else {
				break;
			}
		}

        if(l <= r){
			swapFileRec(files,l,r);
            if (l<end){l++;}
            if (r>0){r--;}
        }
    }
    if(begin < r)
        qSort(files, begin, r,sort_mode,sort_type);
    if(l < end)
        qSort(files, l, end,sort_mode,sort_type);
}

void Interface::sortFiles(char* files,int num_files,SortMode sort_mode,SortType sort_type){
	int inx=0;
	int ptr=0;
	uint8_t tmp_buf[sizeof(FileRec)];
	uint8_t name1[sizeof(FileRec)];
	uint8_t name2[sizeof(FileRec)];
	char* dot;
	if (num_files<=1) return;
	qSort(files,0,num_files,sort_mode,sort_type);
	qSort(files,0,num_files,sort_mode,sort_type);
	qSort(files,0,num_files,sort_mode,sort_type);
	qSort(files,0,num_files,sort_mode,sort_type);
}

void Interface::drawPanel(CursorSide side,bool full_draw){
	if((p_card->init_fs)&&(panels[side].files_count==0)){
		panels[side].files_count = p_card->read_select_dir((char*)panels[side].dir_path,(char*)&panels[side].dirs[0],(char*)&panels[side].files[0],panels[side].dir_depth);
		if(p_card->last_error!=0){
			printf("Error:%d\n",p_card->last_error);
			newDialog(DLG_ERROR,PANEL_LEFT,DIA_OK,"Disk error",(char*)fs_err[p_card->last_error],true);
			return;
		} else {
			if(panels[side].dir_depth>0) panels[side].files_count++;
			sortFiles((char*)&panels[side].files[0],panels[side].files_count,panels[side].sort_mode,panels[side].sort_type);
			if(strlen(panels[side].dirs[panels[side].dir_depth+1])>0){
				for (int i = 0; i < panels[side].files_count; i++){
					if(strcmp(panels[side].dirs[panels[side].dir_depth+1],panels[side].files[i])==0){
						panels[side].cursor_pos=i;
						break;
					}
				}
				memset(panels[side].dirs[panels[side].dir_depth+1],0,sizeof(panels[side].dirs)-(SHORT_NAME_LEN*(panels[side].dir_depth+1)));
			}
			panels[side].draw_files=false;
			change_dir=false;
		}
		//printf("panels[side].files_count:%d\n",panels[side].files_count);
	}
	if ((!panels[side].draw)||(full_draw)){
		panels[side].draw = drawPanel(side==SIDE_LEFT?0:(panels[SIDE_LEFT].width*FONT_5x7_W),panels[side].width,panels[side].mode,panels[side].disk_id,(char*)panels[side].dir_path,panels[side].sort_mode,panels[side].sort_type,(char*)&panels[side].files[0]);
	}
	if ((!panels[side].draw_files)||(full_draw)){//&&(cursor_side==SIDE_LEFT)
		if(panels[side].files_count>0){
			panels[side].draw_files = drawFiles(side==SIDE_LEFT?0:(panels[SIDE_LEFT].width*FONT_5x7_W),panels[side].width,panels[side].mode,panels[side].disk_id,(char*)panels[side].dir_path,panels[side].sort_mode,panels[side].sort_type,
											(char*)&panels[side].files[0],panels[side].files_count,panels[side].draw_cursor,panels[side].cursor_pos);
			//drawFooter(0,panels[side].width,panels[side].mode,panels[side].bottom_file,panels[side].long_filename);
			//printf("panels[side].cursor_pos:%d\n",panels[side].cursor_pos);

		}
	}
}

void Interface::switchPanel(){
	switch (cursor_side){ //hide cursor on old pabel
		case SIDE_LEFT:
			//printf("SIDE_LEFT>");
			if(panels[SIDE_RIGHT].mode==LIST_OFF) return;
			panels[cursor_side].draw_cursor=false;
			panels[cursor_side].draw_files=false;
			drawPanel(cursor_side);
		break;
		case SIDE_RIGHT:
			//printf("SIDE_RIGHT>");
			if(panels[SIDE_LEFT].mode==LIST_OFF) return;
			panels[cursor_side].draw_cursor=false;
			panels[cursor_side].draw_files=false;
			drawPanel(cursor_side);
		default:
		break;
	}
	operator++(cursor_side);
	switch (cursor_side){
		case SIDE_LEFT:
			//printf("SIDE_LEFT\n");
			panels[cursor_side].draw_cursor=true;
			panels[cursor_side].draw_files=false;
			drawPanel(cursor_side);
		break;
		case SIDE_RIGHT:
			//printf("SIDE_RIGHT\n");
			panels[cursor_side].draw_cursor=true;
			panels[cursor_side].draw_files=false;
			drawPanel(cursor_side);
		default:
		break;
	}
}

void Interface::drawPanels(bool full_draw, MenuMode mode){
	if((panels[SIDE_LEFT].mode!=LIST_OFF)||(panels[SIDE_RIGHT].mode!=LIST_OFF)){
		drawPanel(SIDE_LEFT,full_draw);
		drawPanel(SIDE_RIGHT,full_draw);
	}
	if((panels[SIDE_LEFT].mode==LIST_OFF)&&(panels[SIDE_RIGHT].mode==LIST_OFF)){
		if(full_draw){
			p_disp->draw_rect(0,0,SCREEN_W,SCREEN_H,p_sheme[C_FULLSCREEN+P_BGR],true);//Заливаем экран
			p_disp->draw_rect(SCREEN_W/2-75,SCREEN_H/2-60,150,120,p_sheme[C_FULLSCREEN+P_INK],true);
			draw_mur_logo_big(SCREEN_W/2-69,SCREEN_H/2-55);
		}
	} else {
		memset(&temp_msg[0],0,i_width);
		memcpy(&temp_msg[0],&bottom_num[0],i_width);
		drawTextOver(FONT_5x7_W,((i_height-1)*FONT_5x7_H)+1,temp_msg,p_sheme[C_FULLSCREEN+P_INK],p_sheme[C_FULLSCREEN+P_BGR],i_width);
		memset(&temp_msg[0],0,i_width);
		if(mode==MENU_MAIN){
			memcpy(&temp_msg[0],&bottom_txt[0],i_width);
			drawTextOver(FONT_5x7_W,((i_height-1)*FONT_5x7_H)+1,temp_msg,p_sheme[C_FULLSCREEN+P_INK],p_sheme[C_FULLSCREEN+P_BGR],i_width);
		}
		if(mode==MENU_ALT){
			memcpy(&temp_msg[0],&bottom_alt[0],i_width);
			drawTextOver(FONT_5x7_W,((i_height-1)*FONT_5x7_H)+1,temp_msg,p_sheme[C_FULLSCREEN+P_INK],p_sheme[C_FULLSCREEN+P_BGR],i_width);
		}
		if(mode==MENU_CTRL){
			memcpy(&temp_msg[0],&bottom_ctr[0],i_width);
			drawTextOver(FONT_5x7_W,((i_height-1)*FONT_5x7_H)+1,temp_msg,p_sheme[C_FULLSCREEN+P_INK],p_sheme[C_FULLSCREEN+P_BGR],i_width);
		}
		if(mode==MENU_SHIFT){
			memcpy(&temp_msg[0],&bottom_sht[0],i_width);
			drawTextOver(FONT_5x7_W,((i_height-1)*FONT_5x7_H)+1,temp_msg,p_sheme[C_FULLSCREEN+P_INK],p_sheme[C_FULLSCREEN+P_BGR],i_width);
		}
		if(i_height<IFACE_HEIGHT){
			memset(&temp_msg[0],0x20,i_width);
			p_disp->draw_text5x7_len(1+FONT_5x7_W,(i_height*FONT_5x7_H)+1,temp_msg,p_sheme[C_FULLSCREEN+P_INK],p_sheme[C_FULLSCREEN+P_BGR],i_width);
		}
	}
}

void Interface::cursorInc(){
	panels[cursor_side].cursor_pos++;
	if (panels[cursor_side].cursor_pos>panels[cursor_side].files_count-1) panels[cursor_side].cursor_pos=panels[cursor_side].files_count-1;
	panels[cursor_side].draw_files=false;
};

void Interface::cursorDec(){
	panels[cursor_side].cursor_pos--;
	if(panels[cursor_side].cursor_pos<0) panels[cursor_side].cursor_pos=0;
	panels[cursor_side].draw_files=false;
};

void Interface::cursorIncPG(uint8_t page){
	panels[cursor_side].cursor_pos+=page;
	if (panels[cursor_side].cursor_pos>panels[cursor_side].files_count-1) panels[cursor_side].cursor_pos=panels[cursor_side].files_count-1;
	panels[cursor_side].draw_files=false;
};

void Interface::cursorDecPG(uint8_t page){
	panels[cursor_side].cursor_pos-=page;
	if(panels[cursor_side].cursor_pos<0) panels[cursor_side].cursor_pos=0;
	panels[cursor_side].draw_files=false;	
}

void Interface::cursorHome(){
	panels[cursor_side].cursor_pos=0;
	panels[cursor_side].draw_files=false;
};

void Interface::cursorEnd(){
	panels[cursor_side].cursor_pos=(panels[cursor_side].files_count-1);
	panels[cursor_side].draw_files=false;
};

void Interface::markFile(SelectType sel_type){
	if(sel_type==SEL_ONE){
		if(panels[cursor_side].bottom_file->attr&~TOP_DIR_ATTR){
			panels[cursor_side].bottom_file->attr^=SELECTED_FILE_ATTR;	
		}
		cursorInc();
		panels[cursor_side].draw_files=false;
		drawPanel(cursor_side);
	}
	if(sel_type==SEL_ADD){
		for (int x=0;x<panels[cursor_side].files_count;x++){
			FileRec* file = (FileRec*)&panels[cursor_side].files[x];
			if(file->attr&~TOP_DIR_ATTR){
				file->attr|=SELECTED_FILE_ATTR;
			}
		}
		panels[cursor_side].draw_files=false;
		drawPanel(cursor_side);
	}
	if(sel_type==SEL_SUB){
		for (int x=0;x<panels[cursor_side].files_count;x++){
			FileRec* file = (FileRec*)&panels[cursor_side].files[x];
			if(file->attr&~TOP_DIR_ATTR){
				file->attr&=~SELECTED_FILE_ATTR;
			}
		}
		panels[cursor_side].draw_files=false;
		drawPanel(cursor_side);
	}	
	if(sel_type==SEL_INV){
		for (int x=0;x<panels[cursor_side].files_count;x++){
			FileRec* file = (FileRec*)&panels[cursor_side].files[x];
			if(file->attr&~TOP_DIR_ATTR){
				file->attr^=SELECTED_FILE_ATTR;
			}
		}
		panels[cursor_side].draw_files=false;
		drawPanel(cursor_side);
	}
}

bool Interface::trySwitchCatalog(){
	if(change_dir) return false;
	FileRec* file = (FileRec*)&panels[cursor_side].files[panels[cursor_side].cursor_pos][0];
	if(file->attr&AM_DIR){
		panels[cursor_side].dir_depth++;
		strncpy((char*)&panels[cursor_side].dirs[panels[cursor_side].dir_depth],(char*)&panels[cursor_side].files[panels[cursor_side].cursor_pos],(SHORT_NAME_LEN-1));
		change_dir=true;
		panels[cursor_side].cursor_pos=0;
		panels[cursor_side].files_count=0;
	}
	if(file->attr&TOP_DIR_ATTR){
		panels[cursor_side].dir_depth--;
		change_dir=true;
	}
	if(change_dir){
		panels[cursor_side].files_count=0;
		panels[cursor_side].draw=false;
		return true;
	}
	return false;
}

void Interface::switchSortType(SortType type){
	if(panels[cursor_side].sort_type==type){
		operator++(panels[cursor_side].sort_mode);
	} else {
		panels[cursor_side].sort_type=type;
		panels[cursor_side].sort_mode=SORT_MODE_ASC;
	}
	panels[cursor_side].files_count=0;
}

void Interface::processTimers(void){
	if(last_action>0){
		if((millis()-last_action)>LAST_ACTION_DELAY){
			last_action = 0;
			if((panels[cursor_side].bottom_file!=nullptr)&&(panels[cursor_side].width>SHORT_NAME_LEN)){
				if(panels[cursor_side].bottom_file->attr==TOP_DIR_ATTR) return;
				memset(panels[cursor_side].long_filename,0,sizeof(panels[cursor_side].long_filename));
				strcpy(panels[cursor_side].long_filename,p_card->get_lfn_from_dir((char*)panels[cursor_side].dir_path,(FileRec*)panels[cursor_side].bottom_file));
				drawFooter(cursor_side==SIDE_LEFT?0:(panels[SIDE_LEFT].width*FONT_5x7_W),panels[cursor_side].width,panels[cursor_side].mode,panels[cursor_side].bottom_file,panels[cursor_side].long_filename);
				//printf("Draw left panel\n");
			}
		}
	}
	if((c_dialog>=0)&&(dialogs[c_dialog].is_dialog)){
		//processDialogResult(processDialog(c_dialog,true));
	}
}

/*Dialogs*/

bool Interface::drawDialog(uint8_t width,uint8_t height,DialogMode mode,DialogPos pos,DialogType type,char* header,char* message,bool partial){
	uint16_t offset=0;
	uint8_t top=0;
	uint8_t left=0;
	uint8_t cntr=0;
	color_t colorFG=p_sheme[C_SELECT_TEXT+P_INK];
	color_t colorBG=p_sheme[C_SELECT_TEXT+P_BGR];
	int dlg_width = width;
	int dlg_height = height;


	if(type==DIA_DISK){
		if(height==0) height = SYSTEM_DISK_COUNT+1;
		if(width==0) width = (strlen(fs_disp_names[0])+strlen(fs_descr[0])+2);
	}
	if(type==DIA_COPY){
		if(height==0) height = 8;
		if(width==0) width = 50;
		pos = SCREEN_MIDDLE;
		header = (char*)iface_copy_msg[0];
	}

	if((type==DIA_OK)||(type==DIA_OKCANCEL)||(type==DIA_OKRETCAN)||(type==DIA_ABRTRETCAN)){
		if((pos==SCREEN_MIDDLE)||(pos==SCREEN_FULL)){
			if(width==0) width = (i_width*2)/3;
		} else {
			if(width==0) width = (panels[pos].width-8);
		}
		if(height==0){
			//if(width==0){dlg_width =(int)(i_width-16);};// else {dlg_width=(int)(width/2);};
			dlg_width = width;
			splitString(message,nullptr,&dlg_width,&dlg_height);
			width = dlg_width+4;
			height = dlg_height+1;
			//if(width==0) width = strlen(message);
		}
	}	

	if(mode==DLG_INFO){
		colorFG=p_sheme[C_INFO_DIALOG+P_INK];
		colorBG=p_sheme[C_INFO_DIALOG+P_BGR];
	}
	if(mode==DLG_WARN){
		colorFG=p_sheme[C_WARN_DIALOG+P_INK];
		colorBG=p_sheme[C_WARN_DIALOG+P_BGR];
	}
	if(mode==DLG_ERROR){
		colorFG=p_sheme[C_ERR_DIALOG+P_INK];
		colorBG=p_sheme[C_ERR_DIALOG+P_BGR];
	}
	offset=0;
	if(width==0) width = (panels[pos].width-4);
	top = ((i_height-3)/2)-(height/2);
	left = (panels[0].width*pos)+(panels[pos].width/2)-(width/2);
	if(pos==SCREEN_MIDDLE){
		offset=0;
		if(width==0) width = (i_width-4);
		if(height==0) height = 5;
		top = ((i_height-3)/2)-(height/2);
		left=(i_width/2)-(width/2);
	}
	if(pos==SCREEN_FULL){
		offset=0;
		if(width==0) width = i_width;
		if(height==0) height = i_height;
		top=0;
		left=0;
	}

	//printf("width:%d height:%d \n",width,height);

	if((strlen(header)==0)&&(strlen(message)==0)) return false;
	for(uint8_t y=0;y<=height;y++){
		memset(temp_msg,0,sizeof(temp_msg));
		/*draw ramk*/
		for(uint8_t x=0;x<width;x++){
			if (!partial){
				if(y==0){
					if(x==0){temp_msg[x]=0x96;};
					if((x>0)&&(x<width)){temp_msg[x]=0x9A;};
					if(x==(width-1)){temp_msg[x]=0x88;};
				}
				if((y>0)&&(y<height)){
					if((x==0)||(x==(width-1))){temp_msg[x]=0x87;};
					if((x>0)&&(x<(width-1))){temp_msg[x]=0x20;};
				}
				if((type==DIA_COPY)&&(y==4)){
					if(x==0) temp_msg[x]=0x94;
					if(x==(width-1))temp_msg[x]=0x83;
					if((x>0)&&(x<(width-1))){temp_msg[x]=0x91;};
				}			
				if(y==height){
					if(x==0){temp_msg[x]=0x95;};
					if((x>0)&&(x<width)){temp_msg[x]=0x9A;};
					if(x==(width-1)){temp_msg[x]=0x89;};
				}				
			}
			if((type==DIA_COPY)&&((y==3)||(y==7))){
				if((x>1)&&(x<(width-4))){
					temp_msg[x]=0xAC;
					if((y==3)&&(x<(round(((width-4))*((float)copy_file_perc/100))))){
						temp_msg[x]=0xAF;
						sprintf(&temp_msg[(width-4)],"%3d",copy_file_perc);
					}
					if((y==7)&&(x<(round(((width-4))*((float)copy_total_perc/100))))){
						temp_msg[x]=0xAF;
						sprintf(&temp_msg[(width-4)],"%3d",copy_total_perc);
					}
				}
				
			}

		}
		drawTextOver(offset+FONT_5x7_W+(left*FONT_5x7_W),((y+top)*FONT_5x7_H)+1,temp_msg,colorFG,colorBG,width);
		/*//draw ramk*/
		if (!partial){		
			if(y==0){
				if((strlen(header)>0)){
					uint8_t pos = ((width/2)-(strlen(header)/2));
					memcpy(&temp_msg[pos],&header[0],strlen(header));
					temp_msg[pos-1]=0x82;
					temp_msg[pos+strlen(header)]=0x93;
					//if(type==DIA_DISK){
				//	temp_msg[pos-1]=BF_SELECT_TEXT;
				//	temp_msg[pos+strlen(header)]=BF_RESET;
				//}
					drawTextOver(offset+FONT_5x7_W+(left*FONT_5x7_W),((y+top)*FONT_5x7_H)+1,temp_msg,colorFG,colorBG,width);
				}
			}
			if((y>0)&&(y<height)){
				if(type==DIA_DISK){
					memcpy(&temp_msg[2],fs_disp_names[cntr],3);
					memcpy(&temp_msg[5],fs_descr[cntr],10);
					if(cntr==dialogs[c_dialog].cursor_pos){
						temp_msg[0]=BF_CURSOR;
					} else {
						temp_msg[0]=BF_SELECT_TEXT;
					}
					temp_msg[width-1]=BF_RESET;
					drawTextOver(offset+FONT_5x7_W+(left*FONT_5x7_W),((y+top)*FONT_5x7_H)+1,temp_msg,colorFG,colorBG,width);
					cntr++;
				}
				if(type==DIA_COPY){
					if(y==1){
						memcpy(&temp_msg[3],(char*)iface_copy_msg[1],5);
					}
					if(y==2){
						memcpy(&temp_msg[3],(char*)iface_copy_msg[2],5);
					}
					/*if(y==3) {
						&temp_msg[3]
					}*/
					if(y==4){
						uint8_t pos = ((width/2)-(strlen(iface_copy_msg[3])/2));
						memcpy(&temp_msg[pos],iface_copy_msg[3],strlen(iface_copy_msg[3]));
						temp_msg[pos-1]=0x81;
						temp_msg[pos+strlen(iface_copy_msg[3])]=0x90;
					}
					if(y==5) memcpy(&temp_msg[2],(char*)iface_copy_msg[4],6);
					if(y==6) memcpy(&temp_msg[2],(char*)iface_copy_msg[5],6);
					drawTextOver(offset+FONT_5x7_W+(left*FONT_5x7_W),((y+top)*FONT_5x7_H)+1,temp_msg,colorFG,colorBG,width);
				}
			}
			/*
			if((strlen(text)>0)){
				uint8_t pos = ((width/2)-(strlen(text)/2));
				memcpy(&temp_msg[pos],&text[0],strlen(text));
				temp_msg[pos-1]=BF_SELECT_TEXT;
				temp_msg[pos+strlen(text)]=BF_RESET;
				drawTextOver(offset+FONT_5x7_W+(left*FONT_5x7_W),((y+top)*FONT_5x7_H)+1,temp_msg,colorFG,colorBG,width);
			}
			*/
		}
		if(y==height){
			if((type==DIA_OK)||(type==DIA_OKCANCEL)||(type==DIA_OKRETCAN)||(type==DIA_ABRTRETCAN)){
				uint8_t di_id =  static_cast<uint8_t>(type);
				uint8_t len = strlen(iface_btn[di_id]);
				uint8_t pos = ((width/2)-(len/2));
				memcpy(&temp_msg[pos],iface_btn[di_id],len);
				for(uint8_t x=pos;x<(pos+len);x++){
					if(cntr==dialogs[c_dialog].cursor_pos){
						if(temp_msg[x]==0x5B){temp_msg[x-1]=BF_CURSOR;};
						if(temp_msg[x]==0x5D){temp_msg[x+1]=BF_RESET;cntr++;};
					} else {
						if(temp_msg[x]==0x5B){temp_msg[x-1]=BF_SELECT_TEXT;};
						if(temp_msg[x]==0x5D){temp_msg[x+1]=BF_RESET;cntr++;};
					}
				}
				//temp_msg[pos-1]=BF_SELECT_TEXT;
				//temp_msg[pos+strlen(header)]=BF_RESET;				
				//temp_msg[pos-1]=BF_CURSOR;
				//temp_msg[pos+len]=BF_RESET;
				drawTextOver(offset+FONT_5x7_W+(left*FONT_5x7_W),((y+top)*FONT_5x7_H)+1,temp_msg,colorFG,colorBG,width);
			}
			if((type==DIA_COPY)){ //||(type==DIA_OKCANCEL)||(type==DIA_OKRETCAN)
				uint8_t di_id =  count_of(iface_btn)-1;
				uint8_t len = strlen(iface_btn[di_id]);
				uint8_t pos = ((width/2)-(len/2));
				memcpy(&temp_msg[pos],iface_btn[di_id],len);
				for(uint8_t x=pos;x<(pos+len);x++){
					if(cntr==dialogs[c_dialog].cursor_pos){
						if(temp_msg[x]==0x5B){temp_msg[x-1]=BF_CURSOR;};
						if(temp_msg[x]==0x5D){temp_msg[x+1]=BF_RESET;cntr++;};
					} else {
						if(temp_msg[x]==0x5B){temp_msg[x-1]=BF_SELECT_TEXT;};
						if(temp_msg[x]==0x5D){temp_msg[x+1]=BF_RESET;cntr++;};
					}
				}
				//temp_msg[pos-1]=BF_SELECT_TEXT;
				//temp_msg[pos+strlen(header)]=BF_RESET;				
				//temp_msg[pos-1]=BF_CURSOR;
				//temp_msg[pos+len]=BF_RESET;
				drawTextOver(offset+FONT_5x7_W+(left*FONT_5x7_W),((y+top)*FONT_5x7_H)+1,temp_msg,colorFG,colorBG,width);
			}			
		}
	}
	if (!partial){
		if(strlen(message)>0){
			if(countChar(message,0x0A)>0){
				printText(offset,left+1,top+1,width,height,message,colorBG,colorFG,TXT_CENTER,0x0A);
			} else {
				printText(offset,left+1,top+1,width,height,message,colorBG,colorFG,TXT_CENTER,0x20);
			}
			
		}	
	}
	return true;
}

void Interface::cursorDialogUp(){
	dialogs[c_dialog].cursor_pos++;
	if(dialogs[c_dialog].type==DIA_DISK){
		if(dialogs[c_dialog].cursor_pos>=SYSTEM_DISK_COUNT) dialogs[c_dialog].cursor_pos=0;
	}
	if(dialogs[c_dialog].type==DIA_OK){
		if(dialogs[c_dialog].cursor_pos>=1) dialogs[c_dialog].cursor_pos=0;
	}
	if(dialogs[c_dialog].type==DIA_OKCANCEL){
		if(dialogs[c_dialog].cursor_pos>1) dialogs[c_dialog].cursor_pos=0;
	}
	if((dialogs[c_dialog].type==DIA_OKRETCAN)||(dialogs[c_dialog].type==DIA_ABRTRETCAN)){
		if(dialogs[c_dialog].cursor_pos>2) dialogs[c_dialog].cursor_pos=0;
	}
	if(dialogs[c_dialog].type==DIA_COPY){
		if(dialogs[c_dialog].cursor_pos>0) dialogs[c_dialog].cursor_pos=0;
	}
};
void Interface::cursorDialogDown(){
	dialogs[c_dialog].cursor_pos--;
	if(dialogs[c_dialog].type==DIA_DISK){
		if(dialogs[c_dialog].cursor_pos<0)	dialogs[c_dialog].cursor_pos=SYSTEM_DISK_COUNT-1;
	}
	if(dialogs[c_dialog].type==DIA_OK){
		if(dialogs[c_dialog].cursor_pos<0)	dialogs[c_dialog].cursor_pos=0;
	}
	if(dialogs[c_dialog].type==DIA_OKCANCEL){
		if(dialogs[c_dialog].cursor_pos<0)	dialogs[c_dialog].cursor_pos=1;
	}
	if((dialogs[c_dialog].type==DIA_OKRETCAN)||(dialogs[c_dialog].type==DIA_ABRTRETCAN)){
		if(dialogs[c_dialog].cursor_pos<0)	dialogs[c_dialog].cursor_pos=2;
	}
	if(dialogs[c_dialog].type==DIA_COPY){
		if(dialogs[c_dialog].cursor_pos<0) dialogs[c_dialog].cursor_pos=0;
	}
};
void Interface::cursorDialogLeft(){
	if(dialogs[c_dialog].type==DIA_DISK){
		cursorDialogUp();
	}
	if((dialogs[c_dialog].type==DIA_OK)||(dialogs[c_dialog].type==DIA_OKCANCEL)||(dialogs[c_dialog].type==DIA_OKRETCAN)||(dialogs[c_dialog].type==DIA_ABRTRETCAN)){
		cursorDialogDown();
	}
};
void Interface::cursorDialogRight(){
	if(dialogs[c_dialog].type==DIA_DISK){
		cursorDialogDown();
	}
	if((dialogs[c_dialog].type==DIA_OK)||(dialogs[c_dialog].type==DIA_OKCANCEL)||(dialogs[c_dialog].type==DIA_OKRETCAN)||(dialogs[c_dialog].type==DIA_ABRTRETCAN)){
		cursorDialogUp();
	}
};
void Interface::cursorDialogEnter(){
	//if(dialogs[c_dialog].type==DIA_DISK){
		dialogs[c_dialog].result=dialogs[c_dialog].cursor_pos+1;
		dialogs[c_dialog].is_dialog=false;
	//}
	/*if((dialogs[c_dialog].type==DIA_OK)||(dialogs[c_dialog].type==DIA_OKCANCEL)||(dialogs[c_dialog].type==DIA_OKRETCAN)){
		dialogs[c_dialog].result=dialogs[c_dialog].cursor_pos+1;
		dialogs[c_dialog].is_dialog=false;
	}*/
	if(dialogs[c_dialog].type==DIA_COPY){
		dialogs[c_dialog].result=-100;
		dialogs[c_dialog].is_dialog=false;
	}
};
void Interface::cursorDialogExit(){
	if(dialogs[c_dialog].type==DIA_DISK){
		dialogs[c_dialog].result=-100;
		dialogs[c_dialog].is_dialog=false;
	}
	if((dialogs[c_dialog].type==DIA_OK)||(dialogs[c_dialog].type==DIA_OKCANCEL)||(dialogs[c_dialog].type==DIA_OKRETCAN)||(dialogs[c_dialog].type==DIA_ABRTRETCAN)){
		dialogs[c_dialog].result=-100;
		dialogs[c_dialog].is_dialog=false;
	}
	if(dialogs[c_dialog].type==DIA_COPY){
		dialogs[c_dialog].result=-100;
		dialogs[c_dialog].is_dialog=false;
	}
}

void Interface::newDialog(DialogMode mode,DialogPos pos,DialogType type,char* header,char* message,bool modal){
	if((c_dialog>=0)&&(dialogs[c_dialog].is_modal==true)) return;
	dialogs[c_dialog].result=0;
	c_dialog++;
	printf("Dialog created:%d\n",c_dialog);
	dialogs[c_dialog].mode = mode;
	dialogs[c_dialog].type = type;
	dialogs[c_dialog].pos = pos;
	dialogs[c_dialog].header = header;
	dialogs[c_dialog].message = message;
	dialogs[c_dialog].cursor_pos=0;
	dialogs[c_dialog].result=0;
	dialogs[c_dialog].is_dialog=true;
	dialogs[c_dialog].need_repaint=true;
	dialogs[c_dialog].is_modal=modal;
	processDialog(c_dialog);
}

void Interface::freeDialog(uint8_t dialog_id){
	if(c_dialog<0) return;
	if (dialog_id==c_dialog){
		printf("Dialog closed:%d\n",c_dialog);
		dialogs[c_dialog].is_dialog=false;
		c_dialog--;
		if(c_dialog>=0){
			dialogs[c_dialog].is_dialog=true;
			dialogs[c_dialog].need_repaint=true;
		}
	} else {
		dialogs[c_dialog].header = nullptr;
		dialogs[c_dialog].message = nullptr;
		dialogs[c_dialog].cursor_pos=0;
		dialogs[c_dialog].result=0;
		dialogs[c_dialog].is_dialog=false;
	}
}

int Interface::processDialog(uint8_t dialog_id,bool partial){
	if(dialogs[dialog_id].type==DIA_DISK){
		drawDialog(0,0,DLG_NORMAL,dialogs[dialog_id].pos,dialogs[dialog_id].type,"Select disk","",partial);
		dialogs[c_dialog].need_repaint=true;
		//if (dialogs[c_dialog].is_dialog==false) return dialogs[dialog_id].result;
	}
	if((dialogs[dialog_id].type==DIA_OK)||(dialogs[dialog_id].type==DIA_OKCANCEL)||(dialogs[dialog_id].type==DIA_OKRETCAN)||(dialogs[dialog_id].type==DIA_ABRTRETCAN)){
		drawDialog(0,0,dialogs[dialog_id].mode,dialogs[dialog_id].pos,dialogs[dialog_id].type,dialogs[dialog_id].header,dialogs[dialog_id].message,partial);
		//if (dialogs[c_dialog].is_dialog==false) return dialogs[dialog_id].result;
	}
	if(dialogs[dialog_id].type==DIA_COPY){
		drawDialog(0,0,DLG_NORMAL,dialogs[dialog_id].pos,dialogs[dialog_id].type,"","",partial);
		//if (dialogs[c_dialog].is_dialog==false) return dialogs[dialog_id].result;
	}		
	return dialogs[dialog_id].result;
}

int Interface::processDialogResult(int result){
	//printf("result:%d\n",result);
	if(result>0){
		printf("result:%d\n",result);
		if(dialogs[c_dialog].type==DIA_DISK){
			uint8_t dpos = dialogs[c_dialog].pos;
			panels[dpos].disk_id=dialogs[c_dialog].cursor_pos;
			memset(panels[dpos].disk,0,sizeof(panels[dpos].disk));
			strcpy(&panels[dpos].disk[0],fs_names[dialogs[c_dialog].cursor_pos]);
			memset(panels[dpos].dir_path,0,sizeof(panels[dpos].dir_path));
			memset(panels[dpos].files,0,sizeof(panels[dpos].files));
			memset(panels[dpos].dirs,0,sizeof(panels[dpos].dirs));
			strcpy((char*)panels[dpos].dirs[0],fs_names[dialogs[c_dialog].cursor_pos]);
			panels[dpos].files_count=0;
			panels[dpos].dir_depth=0;
			panels[dpos].bottom_file=nullptr;
			dialogs[c_dialog].cursor_pos=0;
			int disk_dlg = c_dialog;
			drawPanels(true,MENU_MAIN);
			if(c_dialog==disk_dlg) freeDialog(c_dialog);
			return result;
		}
		if((dialogs[c_dialog].type==DIA_OK)||(dialogs[c_dialog].type==DIA_OKCANCEL)||(dialogs[c_dialog].type==DIA_OKRETCAN)||(dialogs[c_dialog].type==DIA_ABRTRETCAN)){
			if(!dialogs[c_dialog].is_modal) drawPanels(true,MENU_MAIN);
			freeDialog(c_dialog);
			return result;
		}
		if(dialogs[c_dialog].type==DIA_COPY){
			printf("Finish copy\n");
			dialogs[c_dialog].is_dialog=false;
			drawPanels(true,MENU_MAIN);
			freeDialog(c_dialog);
			return result;
			//return;
		}
	}
	if(result==-100){
		printf("result:%d\n",result);
		if(dialogs[c_dialog].type==DIA_COPY){
			printf("Reset copy\n");
			copy_file_perc=0;
			copy_total_perc=0;
		}
		freeDialog(c_dialog);
		drawPanels(true,MENU_MAIN);
		return result;
	}
	return 0;
}

void Interface::processCopy(void){
	if((c_dialog>=0)&&(dialogs[c_dialog].is_dialog)){
		if(dialogs[c_dialog].type==DIA_COPY){
			copy_file_perc+=10;
			if(copy_file_perc>100){
				copy_file_perc=0;
				copy_total_perc+=20;
			}
			if(copy_total_perc>=100){
				copy_file_perc=0;
				copy_total_perc=100;
				sleep_ms(1500);
				copy_total_perc=0;
				dialogs[c_dialog].result=1;
				printf("process reset\n");
			};
		}
	}
	
}

/*Dialogs*/

void Interface::saveSwap(void){
	char save_file_name_image[25];
	u_int bytesWritten=0;
	printf("Save SWAP\n");
	sprintf(save_file_name_image,"0:/filemanager.swp");
	p_card->file_descr1 = p_card->sd_open_file(&p_card->file_handle1,save_file_name_image,FA_CREATE_ALWAYS|FA_WRITE);
	if (p_card->file_descr1!=FR_OK){p_card->sd_close_file(&p_card->file_handle1);return;}
	p_card->file_descr1 = p_card->sd_write_file(&p_card->file_handle1,(uint8_t*)this,sizeof(Interface),&bytesWritten);
	printf("bytesToWrite=%d, bytesWritten=%d\n",sizeof(Interface),bytesWritten);
	p_card->sd_close_file(&p_card->file_handle1);
}

void Interface::loadSwap(void){
	char save_file_name_image[25];
	u_int bytesRead=0;
	printf("Load SWAP\n");
	sprintf(save_file_name_image,"0:/filemanager.swp");
	p_card->file_descr1 = p_card->sd_open_file(&p_card->file_handle1,save_file_name_image,FA_READ);
	if (p_card->file_descr1!=FR_OK){p_card->sd_close_file(&p_card->file_handle1);return;}
	p_card->file_descr1 = p_card->sd_read_file(&p_card->file_handle1,(uint8_t*)this,sizeof(Interface),&bytesRead);
	printf("bytesToRead=%d, bytesRead=%d\n",sizeof(Interface),bytesRead);
	p_card->sd_close_file(&p_card->file_handle1);
}

void Interface::processKeyboard(Keyboard* kbd){
	MenuMode mmode = MENU_MAIN;
	int result;

	if(KBD_RELEASE){
		last_action=millis();
	}	
	if((KBD_L_CTRL)||(KBD_R_CTRL)){
		mmode=MENU_CTRL;
	};
	if((KBD_L_ALT)||(KBD_R_ALT)){
		mmode=MENU_ALT;
	};
	if((KBD_L_SHIFT)||(KBD_R_SHIFT)){
		mmode=MENU_SHIFT;
	};	
		
	/*resize panels*/
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_LEFT)){
		printf("left_width:%d\t\tpanels[SIDE_RIGHT].width:%d\n",panels[SIDE_LEFT].width,panels[SIDE_RIGHT].width);
		if(panels[SIDE_LEFT].width>1){ //((i_width/4))
			panels[SIDE_LEFT].width-=1;
			panels[SIDE_RIGHT].width+=1;
			//printf("panels[SIDE_LEFT].width:%d\t\tpanels[SIDE_RIGHT].width:%d\n",panels[SIDE_LEFT].width,panels[SIDE_RIGHT].width);
		}
		if((panels[SIDE_RIGHT].mode==LIST_OFF)&&(panels[SIDE_LEFT].width<i_width-3)){
			panels[SIDE_RIGHT].width=NAMES_LIST_WIDTH;
			panels[SIDE_LEFT].width=i_width-NAMES_LIST_WIDTH-2;
		}
		if(panels[SIDE_LEFT].width<NAMES_LIST_WIDTH){
			panels[SIDE_RIGHT].width=i_width-3;
			panels[SIDE_LEFT].width=1;
			panels[SIDE_LEFT].mode=LIST_OFF;
			panels[SIDE_LEFT].draw_cursor=false;
			cursor_side=SIDE_RIGHT;
			panels[SIDE_RIGHT].draw_cursor=true;
		}
		if((panels[SIDE_RIGHT].mode==LIST_OFF)&&(panels[SIDE_RIGHT].width>=NAMES_LIST_WIDTH)){
			panels[SIDE_RIGHT].mode=LIST_NAMES;
		}

		drawPanels(true,mmode);
		return;
	}
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_RIGHT)){
		printf("panels[SIDE_LEFT].width:%d\t\tpanels[SIDE_RIGHT].width:%d\n",panels[SIDE_LEFT].width,panels[SIDE_RIGHT].width);
		if(panels[SIDE_RIGHT].width>1){ //((i_width/4))
			panels[SIDE_LEFT].width+=1;
			panels[SIDE_RIGHT].width-=1;
		}
		if((panels[SIDE_LEFT].mode==LIST_OFF)&&(panels[SIDE_RIGHT].width<i_width-3)){
			panels[SIDE_RIGHT].width=i_width-NAMES_LIST_WIDTH-2;
			panels[SIDE_LEFT].width=NAMES_LIST_WIDTH;
		}
		if(panels[SIDE_RIGHT].width<NAMES_LIST_WIDTH){
			panels[SIDE_LEFT].width=i_width-3;
			panels[SIDE_RIGHT].width=1;
			panels[SIDE_RIGHT].mode=LIST_OFF;
			panels[SIDE_LEFT].draw_cursor=true;
			cursor_side=SIDE_LEFT;
			panels[SIDE_RIGHT].draw_cursor=false;
		}		
		if((panels[SIDE_LEFT].mode==LIST_OFF)&&(panels[SIDE_LEFT].width>=NAMES_LIST_WIDTH)){
			panels[SIDE_LEFT].mode=LIST_NAMES;
		}
		drawPanels(true,mmode);
		return;
	}
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_UP)){
		if(i_height>(p_height/4)){
			i_height-=1;
			drawPanels(true,mmode);
		}
		return;
	}
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_DOWN)){
		if(i_height<p_height){
			i_height+=1;
			drawPanels(true,mmode);
		}
		return;
	}
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_F10)){
		cursor_side=SIDE_LEFT;
		panels[SIDE_LEFT].draw_cursor=true;
		panels[SIDE_RIGHT].draw_cursor=false;
		panels[SIDE_LEFT].width = (i_width/2)-1;
		panels[SIDE_LEFT].mode=LIST_NAMES;
		panels[SIDE_LEFT].sort_mode = SORT_MODE_ASC;
		panels[SIDE_LEFT].sort_type = SORT_TYPE_NAME;
		panels[SIDE_RIGHT].width= (i_width/2)-1;
		panels[SIDE_RIGHT].mode=LIST_NAMES;
		panels[SIDE_RIGHT].sort_mode = SORT_MODE_ASC;
		panels[SIDE_RIGHT].sort_type = SORT_TYPE_NAME;
		i_height=p_height;
		drawPanels(true,mmode);
		return;
	}
	/*resize panels*/
	/*main key*/
	if(((KBD_L_ALT)||(KBD_R_ALT))&&(KBD_F1)){
		if(panels[SIDE_LEFT].width>NAMES_LIST_WIDTH){
			newDialog(DLG_NORMAL,PANEL_LEFT,DIA_DISK,"","");
			kbd->clearKbdBuff();
		}
		return;
	};
	if(((KBD_L_ALT)||(KBD_R_ALT))&&(KBD_F2)){
		if(panels[SIDE_RIGHT].width>NAMES_LIST_WIDTH){
			newDialog(DLG_NORMAL,PANEL_RIGHT,DIA_DISK,"","");
			kbd->clearKbdBuff();
		}
		return;
	};	

	/*swap*/
	if(((KBD_L_ALT)||(KBD_R_ALT))&&(KBD_F12)){
		loadSwap();
		drawPanels(true,mmode);
		kbd->clearKbdBuff();
		return;		
	}
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_F12)){
		saveSwap();
		kbd->clearKbdBuff();
		return;		
	}
	/*swap*/
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_F1)){
		old_mode[0]=panels[SIDE_LEFT].mode;
		switchPanelMode(SIDE_LEFT);
		drawPanels(true,mmode);
		kbd->clearKbdBuff();
		return;		
	};
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_F2)){
		old_mode[1]=panels[SIDE_RIGHT].mode;
		switchPanelMode(SIDE_RIGHT);
		drawPanels(true,mmode);
		kbd->clearKbdBuff();
		return;
	}
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_F3)){
		switchSortType(SORT_TYPE_NAME);
		drawPanels(false,mmode);
		kbd->clearKbdBuff();
		return;
	}
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_F4)){
		switchSortType(SORT_TYPE_EXT);
		drawPanels(false,mmode);
		kbd->clearKbdBuff();
		return;
	}
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_F5)){
		switchSortType(SORT_TYPE_DATE);
		drawPanels(false,mmode);
		kbd->clearKbdBuff();
		return;
	}
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_F6)){
		switchSortType(SORT_TYPE_SIZE);
		drawPanels(false,mmode);
		kbd->clearKbdBuff();
		return;
	}
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_F7)){
		switchSortType(SORT_TYPE_UNSORT);
		drawPanels(false,mmode);
		kbd->clearKbdBuff();
		return;
	}
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_F8)){
		changeColorSheme((color_t*)mur_cmd_default);
		drawPanels(true);
		kbd->clearKbdBuff();
		return;		
	}
	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_F9)){
		changeColorSheme((color_t*)murmul_default);
		drawPanels(true);
		kbd->clearKbdBuff();
		return;		
	}


	if(((KBD_L_CTRL)||(KBD_R_CTRL))&&(KBD_O)){
		if((panels[SIDE_LEFT].mode!=LIST_OFF)||(panels[SIDE_RIGHT].mode!=LIST_OFF)){
			old_mode[0]=panels[SIDE_LEFT].mode;
			old_mode[1]=panels[SIDE_RIGHT].mode;
			switchPanelMode(SIDE_LEFT,LIST_OFF);
			switchPanelMode(SIDE_RIGHT,LIST_OFF);
			drawPanels(true,mmode);
			return;
		}
		if((panels[SIDE_LEFT].mode==LIST_OFF)&&(panels[SIDE_RIGHT].mode==LIST_OFF)){
			switchPanelMode(SIDE_LEFT,old_mode[0]);
			switchPanelMode(SIDE_RIGHT,old_mode[0]);
			old_mode[0]=LIST_OFF;
			old_mode[1]=LIST_OFF;
			drawPanels(true,mmode);
			return;
		}		
		kbd->clearKbdBuff();
	};	

	if(KBD_F1){
		newDialog(DLG_ERROR,SCREEN_MIDDLE,DIA_OKCANCEL,"Disk error",(char*)fs_err[15]);
		kbd->clearKbdBuff();
		return;
	};
	if(KBD_F2){
		newDialog(DLG_ERROR,SCREEN_MIDDLE,DIA_OKRETCAN,"Disk error",(char*)fs_err[15]);
		kbd->clearKbdBuff();
		return;
	}
	if(KBD_F3){
		newDialog(DLG_ERROR,SCREEN_MIDDLE,DIA_ABRTRETCAN,"Disk error",(char*)fs_err[15]);
		kbd->clearKbdBuff();
		return;
	}
	if(KBD_F4){
		//newDialog(DLG_NORMAL,SCREEN_FULL,DIA_EDIT,"Edit","");
		kbd->clearKbdBuff();
		return;
	}
	if(KBD_F5){
		memset(temp_msg,0,sizeof(temp_msg));
		p_utf->cp866_win1251(panels[SIDE_RIGHT].dir_path, &temp_msg[2], strlen(panels[SIDE_RIGHT].dir_path));
		memcpy(&temp_msg[1],fs_disp_names[panels[SIDE_RIGHT].disk_id],3);
		temp_msg[0]=0x20;
		sprintf(&dlg_msg[0][0],iface_dlg_copy_msg[0],123,temp_msg);
		newDialog(DLG_NORMAL,SCREEN_MIDDLE,DIA_OKCANCEL,"Copy",&dlg_msg[0][0]);
		kbd->clearKbdBuff();
		return;
	}
	if(KBD_F6){
		//newDialog(DLG_NORMAL,SCREEN_MIDDLE,DIA_COPY,"Rename/Move","");
		kbd->clearKbdBuff();
		return;
	}
	if(KBD_F7){
		//newDialog(DLG_NORMAL,SCREEN_MIDDLE,DIA_MKDIR,"Make dir","");
		kbd->clearKbdBuff();
		return;
	}
	if(KBD_F8){
		//newDialog(DLG_NORMAL,SCREEN_MIDDLE,DIA_MKDIR,"Delete files","");
		kbd->clearKbdBuff();
		return;
	}
	if(KBD_F9){
		//newDialog(DLG_NORMAL,SCREEN_MIDDLE,DIA_MENU,"MENU","");
		kbd->clearKbdBuff();
		return;
	}
	/*main key*/

	/*select files*/
	if(KBD_INSERT){
		markFile(SEL_ONE);
	}
	if(KBD_NUM_MULT){
		markFile(SEL_INV);
	}
	if(KBD_NUM_PLUS){
		markFile(SEL_ADD);
	}
	if(KBD_NUM_MINUS){
		markFile(SEL_SUB);
	}
	/*select files*/
	/*test dialog*/
	//printf("tst[%08X][%08X]\n",kbd->kbd_statuses[2],(1<<24));
	if(KBD_PRT_SCR){
		//printf("KBD_PRT_SCR\n");
	}
	if(KBD_SCROLL_LOCK){
		//printf("KBD_SCROLL_LOCK\n");
	}
	if(KBD_PAUSE_BREAK){
		//printf("KBD_PAUSE_BREAK\n");
	}
	/*test dialog*/
	/*navigation*/
	if((c_dialog<0)||(!dialogs[c_dialog].is_dialog)){
		if(KBD_UP){
			cursorDec();
		};
		if(KBD_DOWN){
			cursorInc();
		}
		if(KBD_LEFT){
			cursorDecPG(page_column);
		};
		if(KBD_RIGHT){
			cursorIncPG(page_column);
		}
		if(KBD_PAGE_UP){
			cursorDecPG(page_full);
		};
		if(KBD_PAGE_DOWN){
			cursorIncPG(page_full);
		}
		if(KBD_HOME){
			cursorHome();
		}
		if(KBD_END){
			cursorEnd();
		}
		if(KBD_TAB){
			switchPanel();
		}
		if(KBD_BACK_SPACE){
			if((cursor_side==SIDE_LEFT)&&(panels[SIDE_LEFT].dir_depth>0)){ 
				cursorHome();
				trySwitchCatalog();
			}
			if((cursor_side==SIDE_RIGHT)&&(panels[SIDE_RIGHT].dir_depth>0)){ 
				cursorHome();
				trySwitchCatalog();
			}
		}
		if(KBD_ENTER||KBD_NUM_ENTER){
			if(!trySwitchCatalog()){
				printf("its a file\n");
			};
			return;
		}
		drawPanels(false,mmode);
		return;
	}
	/*navigation*/
	
	/*dialogs*/
	if((c_dialog>=0)&&(dialogs[c_dialog].is_dialog)){
		printf("is_dialog:%d\n",c_dialog);
		if(KBD_UP){
			cursorDialogUp();
		};
		if(KBD_DOWN){
			cursorDialogDown();
		}
		if(KBD_LEFT){
			cursorDialogLeft();
		};
		if(KBD_RIGHT){
			cursorDialogRight();
		}
		if(KBD_TAB){
			cursorDialogRight();
		}
		if(KBD_ENTER||KBD_NUM_ENTER){
			cursorDialogEnter();
		}
		if(KBD_ESC){
			cursorDialogExit();
		}	
		int dlg_res = processDialogResult(processDialog(c_dialog,!dialogs[c_dialog].need_repaint));
		if((dlg_res!=0)){
			if(dialogs[c_dialog].need_repaint){
				processDialog(c_dialog,!dialogs[c_dialog].need_repaint);

				//drawPanels(true,mmode);
				//
			}
			if(c_dialog<0){
				drawPanels(true,mmode);
			};
		}
	}
	/*dialogs*/
	
}


/*
const char  *test[34]={ 
" /-----------------------------||-----------------------------\ ",
" |SD:^ Name   | Size |  Date   ||SD:^ Name     |     Name     | ",
" |TRD         |>SDIR<| 01-05-23||              |              | ",
" |SNA         |>SDIR<| 01-05-23||              |              | ",
" |autoexec bat|  295b| 01-05-23||              |              | ",
" |exolon   z80| 128Kb| 01-05-23||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |            |      |         ||              |              | ",
" |-----------------------------||-----------------------------| ",
" 1Help 2Menu 3View 4Edit 5Copy 6RenMov 7MkDir 8Delete 10Reboot  "
};
*/

/*-------Graphics--------*/
/*
void draw_main_window(){
	draw_rect(0,0,SCREEN_W,SCREEN_H,CL_BLACK,true);//Заливаем экран 
	//draw_rect(7,6,SCREEN_W-15,SCREEN_H-14,COLOR_BORDER,true);//Основная рамка
	//draw_rect(8,8+FONT_H,SCREEN_W-17,SCREEN_H-25,COLOR_BACKGOUND,true);//Фон главного окна
	//draw_logo_header(189,7);
	//memset(temp_msg,0,sizeof(temp_msg));
	//sprintf(temp_msg,"%s %s",FW_VERSION,FW_AUTHOR);
	//draw_text_len(FONT_W,FONT_H-1,temp_msg,CL_GRAY,COLOR_BORDER,16);
	//memset(temp_msg, 0, sizeof(temp_msg));
	/*for(uint8_t y=0;y<34;y++){
		memset(temp_msg,0,sizeof(temp_msg));
		printf("%s\n",test[y]);
		if (convert_utf8_to_windows1251(test[y], temp_msg, strlen(test[y]))>0){
			draw_text5x7_len(0,(y*FONT_5x7_H)+1,temp_msg,CL_CYAN,CL_BLUE,65);
		};
		memset(temp_msg, 0, sizeof(temp_msg));
	}/
}


void ShowScreenshot(uint8_t* buffer){
    //draw_rect(17+FONT_W*14,16,182,192,0x0,true);
    for(uint8_t segment=0;segment<3;segment++){
		for(uint8_t symbol_row=0;symbol_row<8;symbol_row++){
        	for(uint8_t y=0;y<8;y++){
        		for(uint8_t x=4;x<32;x++){			
	                uint16_t pixel_addr = (segment*2048)+x+(y*256)+(symbol_row*32);
					uint16_t attr_addr = 0x1800+x+(symbol_row*32)+(segment*256);
    	            int yt=(y+16+(segment*64)+(symbol_row*8));//16+11
        	        int xt=((x*8)+17+FONT_W*14-(FONT_W*4));//17+FONT_W*14+11
					//printf("x:[%d]\ty:[%d]\txt:[%d]\tyt:[%d]\tseg:[%d]\tsymb:[%d]\taddr[%d]\n",x,y,xt,yt,segment,symb,pixel_addr);
					//printf("seg:[%d]\tsymbol_row:[%d]\taddr[%d]\tattr[%d]\n",segment,symbol_row,pixel_addr,attr_addr);
                	uint8_t pixel_data=buffer[pixel_addr];
					uint8_t color_data=buffer[attr_addr];
                	for(int i=0;i<8;i++){
						if((xt<312)&&(yt<210)){
							uint8_t bright_color = (color_data&0b01000000)>>3;
							uint8_t fg_color = color_data&0b00000111;
							uint8_t bg_color = (color_data&0b00111000)>>3;
							//printf("bright_color:[%02X]\tfg_color:[%02X]\tbg_color[%02X]\n",bright_color,fg_color,bg_color);
                    		if (pixel_data&(1<<(8-1)))draw_pixel(xt++,yt,fg_color^bright_color); else draw_pixel(xt++,yt,bg_color^bright_color); 
						}
                    	pixel_data<<=1;
                	}
            	}
			}
        }
    }
}

void draw_logo_header(uint8_t xPos,uint8_t yPos){
	//draw_rect(7,6,SCREEN_W-15,FONT_H,COLOR_FULLSCREEN,true);
	//draw_rect(7,6,SCREEN_W-195,FONT_H,COLOR_BORDER,true);
	for(uint8_t y=0;y<8;y++){ //Рисуем верхнее лого
		for(uint8_t x=0;x<43;x++){
			uint8_t pixel = small_logo[x+(y*43)];
			if(pixel>0) draw_pixel(xPos+x,yPos+y,pixel);
		}
	}
	//draw_text_len(FONT_W*29,FONT_H-1,"MURMULATOR",COLOR_TEXT,COLOR_FULLSCREEN,10);
	draw_text_len(xPos+43,yPos,"MURMULATOR",COLOR_BACKGOUND,COLOR_BORDER,10);
}

void draw_mur_logo(){
	//draw_rect(16+FONT_W*14,15,184,192,COLOR_BORDER,true);	
	draw_text_len(186,70,"NO SIGNAL",COLOR_BACKGOUND,COLOR_TEXT,9);
	for(uint8_t y=0;y<83;y++){
		for(uint8_t x=0;x<84;x++){
			uint8_t pixel = mur_logo[x+(y*84)];
			draw_pixel(180+x,90+y,pixel);
		}
	}
}

void draw_mur_logo_big(uint8_t xPos,uint8_t yPos,uint8_t help){ //x-155 y-60
	for(uint8_t y=0;y<110;y++){
		for(uint8_t x=0;x<138;x++){
			uint8_t pixel = mur_logo2[x+(y*138)];
			if (pixel<0xFF)	draw_pixel(xPos+x,yPos+y,pixel);
		}
	}
	/*if(help==1){
		draw_text_len(xPos+FONT_W+2,yPos+102+FONT_H*2,"   F1 - HELP   ",CL_BLUE,CL_WHITE,15);
		draw_text_len(xPos+FONT_W+2,yPos+102+FONT_H*3,"WIN,HOME-RETURN",CL_BLUE,CL_WHITE,15);	
		if(joy_connected){
			draw_text_len(xPos+FONT_W+2,yPos+102+FONT_H*4,"JOY[START]-EXIT",CL_BLUE,CL_WHITE,15);
		}
	} else 
	if(help==2){
		memset(temp_msg,0,sizeof(temp_msg));
		sprintf(temp_msg,"%s %s",FW_VERSION,FW_AUTHOR);
		draw_text_len(xPos+FONT_W+2,yPos+102+FONT_H*2,temp_msg,CL_BLUE,CL_WHITE,15);
		memset(temp_msg, 0, sizeof(temp_msg));
		draw_text_len(xPos+FONT_W+2,yPos+102+FONT_H*4," WIN,HOME-MENU ",CL_BLUE,CL_WHITE,15);	
		draw_text_len(xPos+FONT_W+2,yPos+102+FONT_H*5,"   F1 - HELP   ",CL_BLUE,CL_WHITE,15);
		if(joy_connected){
			draw_text_len(xPos+FONT_W+2,yPos+102+FONT_H*6,"JOY[START]-MENU",CL_BLUE,CL_WHITE,15);
		}
		
	}/
	
}

void draw_help_text(int startLine){
	for(uint8_t y=0;y<SCREEN_HELP_LINES;y++){
		memset(temp_msg,0,sizeof(temp_msg));
		//printf("%s\n",help_text[y]);
		if((startLine+y)>HELP_LINES){
			//draw_text_len(8,20+(y*FONT_H),"                                     ",CL_BLACK,CL_WHITE,37);
			draw_text5x7_len(9,20+(y*FONT_5x7_H),"                                                           ",CL_BLACK,CL_WHITE,59);
			continue;
		}
		if (convert_utf8_to_windows1251(help_text[startLine+y], temp_msg, strlen(help_text[startLine+y]))>0){
			//draw_text_len(8,20+(y*8),temp_msg,CL_BLACK,CL_WHITE,37);
			draw_text5x7_len(9,20+(y*FONT_5x7_H),temp_msg,CL_BLACK,CL_WHITE,59);
		};
		//printf("%s\n",temp_msg);
		memset(temp_msg, 0, sizeof(temp_msg));
	}
}

void MessageBox(char *text,char *text1,uint8_t colorFG,uint8_t colorBG,uint8_t over_emul){
	/*uint8_t max_len= strlen(text)>strlen(text1)?strlen(text):strlen(text1);
	uint8_t left_x =(SCREEN_W/2)-((max_len/2)*FONT_W);
	uint8_t left_y = strlen(text1)==0 ? (SCREEN_H/2)-(FONT_H/2):(SCREEN_H/2)-FONT_H;
	uint8_t height = strlen(text1)>0 ? FONT_H*2+5:FONT_H+5;
	//if(over_emul) zx_machine_enable_vbuf(false);
	draw_rect(left_x-2,left_y-2,(max_len*FONT_W)+3,height,colorBG,true);
	draw_rect(left_x-2,left_y-2,(max_len*FONT_W)+3,height,colorFG,false);
	draw_text(left_x,left_y+1,text,colorFG,colorBG);
	if (strlen(text1)>0) draw_text(left_x,left_y+FONT_H+1,text1,colorFG,colorBG);
	//printf("X:%d,Y:%d\n",left_x,left_y);
	switch (over_emul){
		case 1:
		g_delay_ms(3000);
		break;
		case 2:
		g_delay_ms(750);
		break;
		case 3:
		g_delay_ms(250);
		break;
		case 4:
		g_delay_ms(1000);
		break;
		default:
		break;
	}/
}



void draw_file_window(){
	draw_rect(17+FONT_W*14,208,182,22,COLOR_BACKGOUND,true); //Фон отображения скринов
	draw_rect(8+FONT_W*14,15,8,SCREEN_H-23,COLOR_BORDER,false);  //Рамка полосы прокрутки
	
	//draw_rect(7,SCREEN_H-FONT_H-8,9+FONT_W*14,FONT_H+1,COLOR_BORDER,false);//панель подсказок под файлами
	//draw_text_len(8,SCREEN_H-FONT_H-7,"F1-HLP,HOME-RET",COLOR_TEXT,COLOR_BACKGOUND,15);//get_file_from_dir("0:/z80",i)
	
	//*draw_rect(17+FONT_W*14,16,182,SCREEN_H-25,COLOR_BACKGOUND,true);  
	//draw_rect(17+FONT_W*14+11,16+11,160,120,0x0,true);
	//*draw_rect(17+FONT_W*14,16,182,192,COLOR_BACKGOUND,true);
	//*draw_line(17+FONT_W*14,209,17+FONT_W*37,209,COLOR_PIC_BG);
	
}

void draw_fast_menu(uint8_t xPos,uint8_t yPos,bool drawbg,uint8_t menu,uint8_t active){
	//printf("xPos:%d  yPos:%d  width:%d  height:%d\n",xPos,yPos,width,height);
	//draw_rect(0,0,SCREEN_W,SCREEN_H,COLOR_FULLSCREEN,true);//Заливаем экран 
	uint8_t lines = 0;
	for(uint8_t i=0;i<11;i++){
		if(*fast_menu[menu][i]==0){
			lines=i;
			break;
		}
	}
	uint8_t width =(16*FONT_W)+1;
	uint8_t height =(lines*FONT_H)+FONT_H;
	//printf("xPos:%d  yPos:%d  width:%d  height:%d lines:%d\n",xPos,yPos,width,height,lines);
	if(drawbg){
		draw_rect(xPos,yPos,width,height,COLOR_BORDER,true);//Основная рамка
		draw_rect(xPos+1,yPos+8,width-2,height-9,COLOR_BACKGOUND,true);//Фон главного окна
		draw_logo_header(xPos+3,yPos);
	}
	/*
	for(uint8_t y=0;y<lines;y++){
		//memset(temp_msg,0,sizeof(temp_msg));
		//printf("%s\n",help_text[y]);
		//if((startLine+y)>HELP_LINES){
		//	draw_text_len(8,20+(y*8),"                                     ",CL_BLACK,CL_WHITE,37);
		//	continue;
		//}
		//if (convert_utf8_to_windows1251(help_text[startLine+y], temp_msg, strlen(help_text[startLine+y]))>0){
		//};
		draw_text_len(xPos+1,(yPos+8)+(y*8),fast_menu[menu][y],CL_BLACK,y==active?CL_LT_CYAN:CL_WHITE,16);
		if(menu==1){
			draw_text_len(xPos+1+FONT_W,(yPos+8)+(y*8),"*",saves[y+1]==1?CL_RED:CL_GREEN,y==active?CL_LT_CYAN:CL_WHITE,1);
		}
		if(menu==2){
			draw_text_len(xPos+1+FONT_W,(yPos+8)+(y*8),"*",saves[y+1]==1?CL_GREEN:CL_RED,y==active?CL_LT_CYAN:CL_WHITE,1);
		}
	}
	/
	//memset(temp_msg, 0, sizeof(temp_msg));
}

void draw_config_menu(uint8_t xPos,uint8_t yPos,bool drawbg,uint8_t active){
	//printf("xPos:%d  yPos:%d  width:%d  height:%d\n",xPos,yPos,width,height);
	//draw_rect(0,0,SCREEN_W,SCREEN_H,COLOR_FULLSCREEN,true);//Заливаем экран 
	/*
	uint8_t lines = 0;
	for(uint8_t i=0;i<CONFIG_LINES;i++){
		if(*config_menu[i]==0){
			lines=i;
			break;
		}
	}
	uint8_t width =(28*FONT_W)+1;
	uint8_t height =(lines*FONT_H)+FONT_H;
	//printf("xPos:%d  yPos:%d  width:%d  height:%d lines:%d\n",xPos,yPos,width,height,lines);
	if(drawbg){
		draw_rect(xPos,yPos,width,height,COLOR_BORDER,true);//Основная рамка
		draw_rect(xPos+1,yPos+8,width-2,height-9,COLOR_BACKGOUND,true);//Фон главного окна
		draw_logo_header((xPos+width)-(16*FONT_W)+4,yPos);
		draw_text_len(xPos,yPos,"[SETTINGS]",CL_WHITE,CL_BLACK,10);
	}
	for(uint8_t y=0;y<lines;y++){
		draw_text_len(xPos+5,(yPos+8)+(y*8),config_menu[y],CL_BLACK,y==active?CL_LT_CYAN:CL_WHITE,27);
		if(y==0){
			draw_text_len(xPos+5+(10*FONT_W),(yPos+8)+(y*8),tap_load_config[cfg_tap_load_mode],CL_BLACK,y==active?CL_LT_CYAN:CL_WHITE,16);
		}
		if(y==1){
			draw_text_len(xPos+5+(14*FONT_W),(yPos+8)+(y*8),joy_config[cfg_def_joy_mode],CL_BLACK,y==active?CL_LT_CYAN:CL_WHITE,12);
		}
		if(y==2){
			draw_text_len(xPos+5+(23*FONT_W),(yPos+8)+(y*8),reboot_config[cfg_res_before_mode],CL_BLACK,y==active?CL_LT_CYAN:CL_WHITE,3);
		}
		if(y==3){
			draw_text_len(xPos+5+(10*FONT_W),(yPos+8)+(y*8),sound_config[cfg_sound_mode],CL_BLACK,y==active?CL_LT_CYAN:CL_WHITE,15);
		}
		
		
	}
	//memset(temp_msg, 0, sizeof(temp_msg));
	/
}

void draw_keyboard(uint8_t xPos,uint8_t yPos){ //x:33 y:84
	int kbd_col=0;
	int kbd_row=0;
	for(uint8_t y=0;y<156;y++){
		for(uint8_t x=0;x<254;x++){
			uint8_t pixel = kbd_img[x+(y*254)];
			if((x>=kbd_xfpos[kbd_row][kbd_col])&&(x<=kbd_xtpos[kbd_row][kbd_col])){
				if((y>=kbd_yfpos[kbd_row])&&(y<=kbd_ytpos[kbd_row])){
					if(pixel==0x07){pixel=0xF4;}; //0x0F
				}
			}
			if (pixel<0xFF)	draw_pixel(xPos+x,yPos+y,pixel);
			//printf("X:%d Y:%d C:[%02X]\n",x,y,pixel);
		}
	}
}

bool LoadTxt(char *file_name){
	int res =0;
	size_t bytesRead;
	size_t bytesToRead;
	size_t FileSize;

	memset(sd_buffer, 0, sizeof(sd_buffer));

	res = sd_open_file(&file_handle1,file_name,FA_READ);
    printf("sd_open_file=%d\n",res);
	if (res!=FR_OK){sd_close_file(&file_handle1);return false;}
   	FileSize = sd_file_size(&file_handle1);
    printf("text Filesize %u bytes\n", FileSize);
	res = sd_read_file(&file_handle1,sd_buffer,sizeof(sd_buffer),&bytesRead);
	
	/*uint16_t ptr=0;
	do{
		printf("[%04X]",ptr);
		for (uint8_t col=0;col<16;col++){
			printf("\t%02X",sd_buffer[ptr]);
			ptr++;
		}
		printf("\n");
	} while(ptr<sizeof(sd_buffer));
	printf("\n");/

	draw_text_len(18+FONT_W*14,16,"File contents:",COLOR_TEXT,COLOR_BACKGOUND,14);
	memset(temp_msg, 0, sizeof(temp_msg));
	sprintf(temp_msg,"File size:%dk",(FileSize/1024));
	draw_text_len(18+FONT_W*14,216, temp_msg,COLOR_TEXT,COLOR_BACKGOUND,22);
	uint16_t ptr=0;
	uint8_t len=0;
	for (uint8_t i = 0; i < 26; i++){
		memset(temp_msg, 0, sizeof(temp_msg));
		for (uint8_t j = 0; j < 37; j++){
			if(sd_buffer[ptr+j]==0x0A){
				len=j+1;
				break;
			}
			len=j;
		}
		//printf("ptr:%d len:%d \n",ptr,len);
		memcpy(temp_msg,&sd_buffer[ptr],len);
		ptr+=len;
		if(len>0){
			draw_text5x7_len(20+(FONT_5x7_W*22),25+(FONT_5x7_H)*i,temp_msg,COLOR_TEXT,COLOR_BACKGOUND,36);
		}
		if(len==0){
			uint8_t k=0;
			while(sd_buffer[ptr+k]!=0x0A){
				k++;
			}
			ptr+=k;
		}
		if(ptr>FileSize){break;}
	}
	sd_close_file(&file_handle1);
	return true;
}
*/
/*-------Graphics--------*/
