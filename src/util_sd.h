#pragma once
#include <VFS.h>
#include "utf_handle.h"

/*
#include "VFS/ffconf.h"
#include "VFS/ff.h"
*/

#define SYSTEM_DISK_COUNT (2)

#define ZX_RAM_PAGE_SIZE 0x4000
#define DIRS_DEPTH (20)
#define MAX_FILES  (2000)
#define SD_BUFFER_SIZE 0x4000  //Размер буфера для работы с файлами
#define SHORT_NAME_LEN (13)
#define FILE_REC_LEN (20)

#define TOP_DIR_ATTR 0x40
#define SELECTED_FILE_ATTR 0x80

static const char* fs_names[SYSTEM_DISK_COUNT] = {"0:\0","F:\0"};

typedef struct FileRec{
    char filename[SHORT_NAME_LEN];
    uint8_t attr;
	uint16_t date;
	uint32_t size;
} __attribute__((packed)) FileRec;


class SDCard{
    private:
        uint8_t dirs[5][14];
        uint8_t files[5][14];

    public:
        SDCard();
        ~SDCard();

        UTFCoverter* p_utf;

        DIR left_dir;
        DIR right_dir;

        FIL file_handle1;
        FIL file_handle2;
        
        int file_descr  =-1;
        int file_descr1 =-1;
        int file_descr2 =-1;
        int last_error=0;
        bool init_fs;
        char activefilename[512];
        char long_file_name[512];
        char afilename[SHORT_NAME_LEN];
        //char sd_buffer[SD_BUFFER_SIZE]; //буфер для работы с файлами
        bool init_filesystem(void);
        int read_select_dir(char* path,char* dirs,char* files, int dir_index);

        const char* get_file_extension(const char *filename);
        void sortfiles(char* nf_buf, int N_FILES);
        int get_files_from_dir(char *dir_name,char* nf_buf, int MAX_N_FILES);
        char* get_file_from_dir(char *dir_name,int inx_file);
        char* get_lfn_from_dir(char *dir_name,FileRec* short_name);

        int sd_open_file(FIL *file, char* file_name, BYTE mode);
        int sd_read_file(FIL *file, void* buffer, unsigned int bytestoread, unsigned int* bytesreaded);
        int sd_write_file(FIL *file,void* buffer, UINT bytestowrite, UINT* byteswrited);
        int sd_flush_file(FIL *file);
        int sd_seek_file(FIL *file, FSIZE_t offset);
        int sd_close_file(FIL *file);
        DWORD sd_file_pos(FIL *file);
        DWORD sd_file_size(FIL *file);
        int sd_mkdir(const char* path);
        int sd_delete(const char* path);
        int sd_rename(const char* path_old, const char* path_new);
        int sd_opendir(DIR* dp,const char* path);
        int sd_readdir(DIR* dp, FILINFO* fno);
        int sd_closedir(DIR *dp);
};