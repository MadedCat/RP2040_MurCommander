
#include <stdio.h>
#include <VFS.h>
#include "util_sd.h"
#include "ffconf.h"
#include "fcntl.h"
#include "utf_handle.h"


//const char* VolumeStr[FF_VOLUMES] = {"NA","SD"};


SDCard::SDCard(){
    p_utf = new UTFCoverter();
    printf("SDCard Constructor was Called!\n");
};
SDCard::~SDCard(){
    printf("SDCard Destructor was Called!\n");
};


//работа с каталогами и файлами

bool SDCard::init_filesystem(void){
    init_fs=false;
    file_descr=vfs_init();
    
    //file_descr = fatfs_init();
    //printf("init VFS:%d\n",file_descr);
    if (file_descr!=FR_OK) return false;
    init_fs=true;
    last_error=0;
    return true;

/*
    int err = 0;
    extern int lfs_init(void);
    extern int vfs_mount(const char *path, const void *file_system_context);
    extern int vfs_unmount(const char *path);

    
    file_descr = lfs_init();

    MUTEX_INIT(lfs_rom_ctx.pMutex);
    err = vfs_mount(LFS_ROM_LETTER, &lfs_rom_ctx);


    if(file_descr!=0){
        vfs_unmount(fs_names[1]);
    }
    printf("init LFS:%d\n",file_descr);
    extern int fatfs_init(void);

    MUTEX_INIT(fatfs_ctx.pMutex);
    err = vfs_mount(FATFS_LETTER, &fatfs_ctx);
    file_descr = fatfs_init();
    printf("init FATFS:%d\n",file_descr);    

    */
}

int SDCard::read_select_dir(char* path,char* dirs,char* files, int dir_index){
    path[0]=0;
    for(int i=0;i<=dir_index;i++){
      if (dir_index>=DIRS_DEPTH) break;
      strcat((char*)path,(char*)&dirs[i*SHORT_NAME_LEN]);
      if (i!=dir_index) strcat((char*)path,"/");
    }
    printf("open dir=%s\n",path);
    if(dir_index>0){
        memset(files,0,sizeof(FileRec)*MAX_FILES);
        strcpy(files,"..");
        memset(files+SHORT_NAME_LEN,TOP_DIR_ATTR,1);
        files+=sizeof(FileRec);
    }
    return get_files_from_dir((char*)path,(char*)files,MAX_FILES);
}

const char* SDCard::get_file_extension(const char *filename){
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return ""; 
    return dot + 1;
}
/*const char* SDCard::get_file_name(const char *filename){
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return ""; 
    return dot + 1;
}*/

void SDCard::sortfiles(char* nf_buf, int N_FILES){
    int inx=0;
    if (N_FILES==0) return;
    uint8_t tmp_buf[sizeof(FileRec)];
    while(inx!=(N_FILES-1)){
        FileRec* file1 = (FileRec*)&nf_buf[sizeof(FileRec)*inx];
        FileRec* file2 = (FileRec*)&nf_buf[sizeof(FileRec)*(inx+1)];
        if ((file1->attr&AM_DIR)>(file2->attr&AM_DIR)){inx++; continue;}
        if (((file1->attr&AM_DIR)<(file2->attr&AM_DIR))||(strcmp(file1->filename,file2->filename)>0)){
            memcpy(tmp_buf,nf_buf+sizeof(FileRec)*inx,sizeof(FileRec));
            memcpy(nf_buf+sizeof(FileRec)*inx,nf_buf+sizeof(FileRec)*(inx+1),sizeof(FileRec));
            memcpy(nf_buf+sizeof(FileRec)*(inx+1),tmp_buf,sizeof(FileRec));
            if (inx) inx--;
            continue;
        }
        inx++;
    }
}

int SDCard::get_files_from_dir(char *dir_name,char* nf_buf, int MAX_N_FILES){
    
    DIR d;
    int fd =-1;
    FILINFO dir_file_info;
    char temp[(SHORT_NAME_LEN-1)];
    char ext[3];
    //"0:/z80"
    //fd = f_opendir(&d,dir_name);
    last_error=0;
    fd=sd_opendir(&d,dir_name);
    if (fd!=FR_OK){last_error=fd;return 0;}

    memset(nf_buf,0,(MAX_FILES*sizeof(FileRec))-sizeof(FileRec));
    //printf("\nFile FD: %d\n", fd);
    int inx=0;
    while (1){   
        fd=sd_readdir(&d,&dir_file_info);
        if (fd!=FR_OK){last_error=fd;return 0;}
        if (strlen((char *)dir_file_info.fname)==0) break;

        FileRec* file = (FileRec*)&nf_buf[sizeof(FileRec)*inx];

        file->attr=dir_file_info.fattrib;
        file->date=dir_file_info.fdate;
        file->size=dir_file_info.fsize;

        //nf_buf[sizeof(FileRec)*inx+(SHORT_NAME_LEN-1)]=dir_file_info.fattrib;//; else nf_buf[SHORT_NAME_LEN*inx+(SHORT_NAME_LEN-1)]=0;
        //nf_buf[sizeof(FileRec)*inx+SHORT_NAME_LEN] = (uint8_t)&dir_file_info.fdate[0];
        //(DWORD)nf_buf[sizeof(FileRec)*inx+(SHORT_NAME_LEN+1)] = (DWORD)dir_file_info.fsize;

        if(strlen((char *)dir_file_info.fname)>(SHORT_NAME_LEN-1)){
            strncpy(temp,(char *)dir_file_info.altname,(SHORT_NAME_LEN-1)) ;
        } else {
            strncpy(temp,(char *)dir_file_info.fname,(SHORT_NAME_LEN-1)) ;
        }
        for(uint8_t ch=0;ch<(SHORT_NAME_LEN-1);ch++){
            if(file->attr&AM_DIR){
                //temp[ch]=toupper(temp[ch]);
                temp[ch]=p_utf->cp866_upper_char(temp[ch]);
            } else {
                //temp[ch]=tolower(temp[ch]);
                temp[ch]=p_utf->cp866_lower_char(temp[ch]);
            }
        }
        strncpy(file->filename,temp,(SHORT_NAME_LEN-1));
        inx++; 
        if (inx>=MAX_N_FILES) break;
        //if (dir_file_info.fname) break;
        //file_list[i] = dir_file_info.fname;
        //printf("[%s] %d\n",dir_file_info.fname,strlen(dir_file_info.fname));
    }
    sd_closedir(&d);
    //sortfiles(nf_buf,inx);
    return inx;
}

char* SDCard::get_file_from_dir(char *dir_name,int inx_file){

    DIR d;
    int fd =-1;
    FILINFO dir_file_info;
    //static char filename[20];
    static char filename[513];
    //filename[0]=0;
    //filename[SHORT_NAME_LEN]=0;    
    memset(filename, 0, 200);

    fd = sd_opendir(&d,dir_name);
    if (fd!=FR_OK) return filename;
    //printf("\nFile FD: %d\n", fd);
    int inx=0;
    while (1){   
        fd = sd_readdir(&d,&dir_file_info);
        if (fd!=FR_OK) return filename;
        if (strlen((char *)dir_file_info.fname)==0) break;
        if (inx++==inx_file){
            //strncpy(filename,dir_file_info.fname,SHORT_NAME_LEN) ;
            strncpy(filename,(char *)dir_file_info.fname,sizeof(dir_file_info.fname)) ;
            if (dir_file_info.fattrib&AM_DIR) strcat(filename,"*");
            break;
        }
        //if (dir_file_info.fname) break;
        //file_list[i] = dir_file_info.fname;
    }
    f_closedir(&d);
    //printf("[%s] %d\n",filename,strlen(filename));
    return filename;

}

char* SDCard::get_lfn_from_dir(char *dir_name,FileRec* short_name){

    DIR d;
    int fd =-1;
    FILINFO dir_file_info;
    fd = sd_opendir(&d,dir_name);
    if (fd!=FR_OK) return long_file_name;
    //printf("\nFile FD: %d\n", fd);
    int inx=0;
    while (1){   
        fd = sd_readdir(&d,&dir_file_info);
        if (fd!=FR_OK) return long_file_name;
        if (strlen((char *)dir_file_info.fname)==0) break;
        //printf("[%s] %d> %d\n",dir_file_info.fname,strlen(dir_file_info.fname),strncmp(dir_file_info.fname,part_name,SHORT_NAME_LEN));
        //printf("[%s] %s\n",dir_file_info.fname,dir_file_info.altname);
        memset(long_file_name, 0, sizeof(long_file_name));
        strncpy(long_file_name,(char *)dir_file_info.altname,strlen((char *)dir_file_info.altname));
        /*if(short_name->attr&AM_DIR){
            printf("Dir>%s \n",short_name->filename);
        }*/
        for(uint8_t ch=0;ch<strlen((char *)dir_file_info.fname);ch++){
            if(short_name->attr&AM_DIR){
                long_file_name[ch]=p_utf->cp866_upper_char(long_file_name[ch]);
            } else {
                long_file_name[ch]=p_utf->cp866_lower_char(long_file_name[ch]);
            }
        }        
        if (strncmp(long_file_name,short_name->filename,(SHORT_NAME_LEN-1))==0){
            inx=0;
            break;
        }
        inx++;
    }
    if(inx==0){
        memset(long_file_name, 0, sizeof(long_file_name));
        strncpy(long_file_name,(char *)dir_file_info.fname,strlen((char *)dir_file_info.fname));
    } else {
        memset(long_file_name, 0, sizeof(long_file_name));
        strncpy(long_file_name,short_name->filename,(SHORT_NAME_LEN-1));
    }
    f_closedir(&d);
    //printf(">[%s] %d\n",filename,strlen(filename));
    return long_file_name;

}

int SDCard::sd_mkdir(const char* path){
    return f_mkdir((TCHAR*)path);
}

int SDCard::sd_open_file(FIL *file,char *file_name,BYTE mode){
    return f_open(file,(TCHAR*)file_name,mode);
}

int SDCard::sd_read_file(FIL *file,void* buffer, UINT bytestoread, UINT* bytesreaded){
    return f_read(file,buffer,bytestoread,bytesreaded);
}

int SDCard::sd_write_file(FIL *file,void* buffer, UINT bytestowrite, UINT* byteswrited){
    return f_write(file,buffer,bytestowrite,byteswrited);
}

int SDCard::sd_flush_file(FIL *file){
    return f_sync (file);
}

int SDCard::sd_delete(const char* path){
    return f_unlink ((TCHAR*)path);
}

int SDCard::sd_rename(const char* path_old, const char* path_new){
    return f_rename ((TCHAR*)path_old, (TCHAR*)path_new);
}

int SDCard::sd_seek_file(FIL *file,FSIZE_t offset){
    return f_lseek(file,offset);
}

int SDCard::sd_close_file(FIL *file){
    return f_close(file);
}

DWORD SDCard::sd_file_pos(FIL *file){
    return file->fptr;
}

DWORD SDCard::sd_file_size(FIL *file){
    return file->obj.objsize;
}

int SDCard::sd_opendir(DIR* dp,const char* path){
    return f_opendir(dp,(TCHAR*)path);
}

int SDCard::sd_readdir(DIR* dp, FILINFO* fno){
    return f_readdir(dp,fno);
}

int SDCard::sd_closedir(DIR *dp){
    return f_closedir (dp);
}