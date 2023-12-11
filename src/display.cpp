#include "display.h"

//int	null_printf(const char *str, ...){return 0;};

//#include "math.h"
#define ABS(x) (((x)<0)?(-(x)):(x))
#define SIGN(x) (((x)<0)?(-(1)):(1))

Display::Display(){
	printf("Display Constructor was Called!\n");
};
Display::~Display(){
	printf("Display Destructor was Called!\n");
};

void Display::init_screen(uint8_t* scr_buf,int scr_width,int scr_height){
	screen_buf=scr_buf;
	scr_W=scr_width;
	scr_H=scr_height;
};

bool Display::draw_pixel(int x,int y,color_t color){
	if ((x<0)|(x>=scr_W)|(y<0)|(y>=scr_H))  return false;
	uint8_t* pix=&screen_buf[(y*scr_W+x)/2];
	uint8_t tmp=*pix;
	if (x&1) *pix=(tmp&0xf)|((color&0xf)<<4);
	else *pix=((tmp&0xf0))|((color&0xf));
	return true;
}

void Display::draw_text(int x,int y,char* text,color_t colorText,color_t colorBg){
	for(int line=0;line<FONT_H;line++){
		uint8_t* symb = (uint8_t*)text;
		int yt=y+line;
		int xt=x;
		while(*symb){
			uint8_t symb_data=FONT_8x8_DATA[*symb*FONT_H+line];
			for(int i=0;i<FONT_W;i++){
				if (symb_data&(1<<(FONT_W-1)))draw_pixel(xt++,yt,colorText); else draw_pixel(xt++,yt,colorBg);
				symb_data<<=1;
			}
			symb++;
		}
	}
}

void Display::draw_text_len(int x,int y,char* text,color_t colorText,color_t colorBg,int len){
	
	//printf("text[%s]\t",text);
	
	for(int line=0;line<FONT_H;line++){
		uint8_t* symb=(uint8_t*)text;
		//printf("\nch[%2X]\t",*symb);
		//printf("addr[%4X]\t",*symb*FONT_H);
		int yt=y+line;
		int xt=x;
		int inx_symb=0;
		while(*symb){
			uint8_t symb_data=FONT_8x8_DATA[*symb*FONT_H+line];
			//printf("[%2X]\t",symb_data);
			for(int i=0;i<FONT_W;i++){
				if (symb_data&(1<<(FONT_W-1)))draw_pixel(xt++,yt,colorText); else draw_pixel(xt++,yt,colorBg);
				symb_data<<=1;
			}
			symb++;
			inx_symb++;
			if (inx_symb>=len) break;
		}
		while (inx_symb<len){
			uint8_t symb_data=0;
			for(int i=0;i<FONT_W;i++){
				if (symb_data&(1<<(FONT_W-1)))draw_pixel(xt++,yt,colorText); else draw_pixel(xt++,yt,colorBg);
				symb_data<<=1;
			}
			inx_symb++;
			
		}
	}
	//printf("\n");
}

void Display::draw_rect(int x,int y,int w,int h,color_t color,bool filled){
	int xb=x;
	int yb=y;
	int xe=x+w;
	int ye=y+h;
	
	xb=xb<0?0:xb;
	yb=yb<0?0:yb;
	xe=xe<0?0:xe;
	ye=ye<0?0:ye;
	
	xb=xb>scr_W?scr_W:xb;
	yb=yb>scr_H?scr_H:yb;
	xe=xe>scr_W?scr_W:xe;
	ye=ye>scr_H?scr_H:ye;	 
	for(int y=yb;y<=ye;y++){
		for(int x=xb;x<=xe;x++){
			if (filled){
				draw_pixel(x,y,color);
				} else {
				if ((x==xb)|(x==xe)|(y==yb)|(y==ye)) draw_pixel(x,y,color);
			};
		};
	};
}

void Display::draw_line(int x0,int y0, int x1, int y1,color_t color){
	int dx=ABS(x1-x0);
	int dy=ABS(y1-y0);
	if (dx==0) { if (dy==0) return; for(int y=y0;y!=y1;y+=SIGN(y1-y0)) draw_pixel(x0,y,color) ;return;};
	if (dy==0) { if (dx==0) return; for(int x=x0;x!=x1;x+=SIGN(x1-x0)) draw_pixel(x,y0,color) ;return;};
	if (dx>dy){
		float k=(float)(x1-x0)/ABS(y1-y0);
		//float kold=0;
		float xf=x0+k;
		int x=x0;
		for(int y=y0;y!=y1;y+=SIGN(y1-y0)){
			int i=0;
			for(;i<ABS(xf-x);i++){ 
				draw_pixel(x+i*SIGN(x1-x0),y,color) ;
			}
			x+=i*SIGN(x1-x0);
			xf+=k;
		}
		} else {
		float k=(float)(y1-y0)/ABS(x1-x0);
		//float kold=0;
		float yf=y0+k;
		int y=y0;
		for(int x=x0;x!=x1;x+=SIGN(x1-x0)){
			int i=0;
			for(;i<ABS(yf-y);i++){ 
				draw_pixel(x,y+i*SIGN(y1-y0),color);
			}
			y+=i*SIGN(y1-y0);
			yf+=k;
		}
	}
}


// #define M_PI		3.14159265358979323846
// void draw_circle(int x0,int y0,  int r,color_t color)
// {
//	if (r==0) return;
//	int x=x0+r;
//	int y=y0; 
//	for(float a=0;a<=2*M_PI;a+=1.0/(10+3*r))
//	{
//		int x1=x0+r*cos(a)+0.5;
//		int y1=y0+r*sin(a)+0.5;
//		draw_line(x,y,x1,y1,color);
//		x=x1;
//		y=y1;
//	}

//	 draw_line(x,y,x0+r,y0,color);

// }

void Display::draw_text5x7(int x,int y,char* text,color_t colorText,color_t colorBg){
	if(y>SCREEN_H) return;
	if(x>SCREEN_W) return;	
	for(int line=0;line<FONT_5x7_H;line++){
		uint8_t* symb=(uint8_t*)text;
		int yt=y+line;
		int xt=x;
		while(*symb){
			uint8_t symb_data=FONT_5x7_DATA[*symb*FONT_5x7_H+line];
			//uint8_t symb_char =*symb;
			//if(symb_char>1){
				for(int i=0;i<FONT_5x7_W;i++){
					if (symb_data&(1<<(FONT_5x7_W-1)))draw_pixel(xt++,yt,colorText); else draw_pixel(xt++,yt,colorBg);
					symb_data<<=1;
				}
			//}
			symb++;
		}
	}
}

void Display::draw_text5x7_len(int x,int y,char* text,color_t colorText,color_t colorBg,int len){
	//printf("CT[%02X] CB[%02X]\n",colorText,colorBg);
	//printf("text[%s]\t",text);
	if(y>SCREEN_H) return;
	if(x>SCREEN_W) return;
	for(int line=0;line<FONT_5x7_H;line++){
		uint8_t* symb=(uint8_t*)text;
		//printf("\nch[%2X]\t",*symb);
		//printf("addr[%4X]\t",*symb*FONT_5x7_H);
		int yt=y+line;
		int xt=x;
		int inx_symb=0;
		while(*symb){
			uint8_t symb_data=FONT_5x7_DATA[*symb*FONT_5x7_H+line];
			//uint8_t symb_char =*symb;
			//printf("[%2X]\t",symb_char);
			//if(symb_char>1){
				//printf("[%2X]\t",symb_data);
				for(int i=0;i<FONT_5x7_W;i++){
					if (symb_data&(1<<(FONT_5x7_W-1)))draw_pixel(xt++,yt,colorText); else draw_pixel(xt++,yt,colorBg);
					symb_data<<=1;
				}
			//}
			symb++;
			inx_symb++;
			if (inx_symb>=len) break;
		};
		while (inx_symb<len){
			uint8_t symb_data=0;
			for(int i=0;i<FONT_5x7_W;i++){
				if (symb_data&(1<<(FONT_5x7_W-1)))draw_pixel(xt++,yt,colorText); else draw_pixel(xt++,yt,colorBg);
				symb_data<<=1;
			}
			inx_symb++;
		};
	}
	//printf("\n");
}


