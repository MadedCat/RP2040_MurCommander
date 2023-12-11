/* 

WIKI:
https://github.com/Wiz-IO/wizio-pico/wiki/ARDUINO#vfs--file-systemh

OTHER USER CONFIG KEYS

LFS:    https://github.com/littlefs-project/littlefs   

        https://github.com/Wiz-IO/framework-wizio-pico/blob/main/arduino/libraries/RP2040/VFS/VFS_LFS.h

FATFS:  http://elm-chan.org/fsw/ff/00index_e.html

https://github.com/Wiz-IO/framework-wizio-pico/blob/main/arduino/libraries/RP2040/VFS/VFS_FATFS.h

*/


#define MAX_OPEN_FILES  4
#define MAX_OPEN_DIRS   2

#define USE_LFS         /* Enable littlefs                  default pats */
//#define USE_LFS_RAM   /* Use Ram disk                     R:/file_path */
#define USE_LFS_ROM     /* Use Rom disk ( internal flash )  F:/file_path */
#define USE_FATFS       /* Enable FatFS                     0:/file_path */

#define FATFS_LETTER "0:"
#define LFS_ROM_LETTER "F:"

#define FATFS_SPI               spi0
//#define FATFS_SPI_BRG           2000000
//#define FATFS_SPI_BRG           4000000
#define FATFS_SPI_BRG           8000000
#define FATFS_SPI_SCK           2 /* SPI1_SCK  */
#define FATFS_SPI_MOSI          3 /* SPI1_TX   */
#define FATFS_SPI_MISO          4 /* SPI1_RX   */
#define FATFS_CS_PIN            5 /* SPI1_CSn  */