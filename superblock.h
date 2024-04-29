#ifndef _SUPERBLOCK_H
#define _SUPERBLOCK_H

#include<stdint.h>
//#include"freelist.h"
#include"unix1.h"
#include"block.h"
#include"buffer.h"
#include<pthread.h>  // For POSIX threading support

/*
Block Allocation Bitmap:

Most modern filesystems, including ext2, ext3, ext4, and many others, use a block allocation bitmap.
In this approach, a bitmap is used where each bit corresponds to a block on the disk.
 A bit value of 0 indicates that the corresponding block is free, while a bit value of 1
  indicates that the block is allocated.
The bitmap is stored within the superblock or another metadata structure,
 allowing efficient querying and manipulation of free blocks.
*/
#define NUM_BLOCKS 512 // Example number of blocks in the filesystem
#define MAX_INODES 100
#define DEVICE_NUM 4


typedef struct IncoreINODE IINODE;
typedef struct IncoreINODE* PIINODE;
typedef struct IncoreINODE** PPIINODE;


struct SuperBlock{
    uint64_t filesystem_size;
    uint32_t freeblocks;
   // uint8_t free_block_bitmap[NUM_BLOCKS/8];  // Bitmap representing the status of each block
    //Unix system doesn't use bitmap while  linux and other OS do use . Pg.100 J Bach
    //Unix has preffered to use linked list of disk blocks
    uint32_t next_free_block_index; //
    struct DBList** diskhead;
    //uint32_t next_free_bitmap_index;
    uint32_t inode_list_size;
    bool free_inodes[MAX_INODES];
    int indexofnextfree;
    int freeinodescount;
    pthread_mutex_t lockfieldINODE;
    pthread_mutex_t lockSB;
    pthread_cond_t cond;
    bool SBlocked;
    bool SBmodified;
};


typedef struct FileTable{
    off_t readoffset;
    off_t writeoffset;
    int count;
    mode_t mode;
    PIINODE ptrtoinode;
    int fd;
}FILETABLE,*PFILETABLE;


typedef struct uarea{
    PFILETABLE ptrtofiletable[MAX_INODES];
    off_t offset;
    uint64_t RWbytes;  //count of bytes to read or write
    mode_t read_or_write;  //indicates read or write
    char* address; //target address give by user to copy data
}UFDT;




UFDT ufdt;

FILETABLE FTABLE[MAX_INODES*5];
PIINODE IITABLE[MAX_INODES];

typedef struct SuperBlock SUPERBLOCK;
typedef struct SuperBlock* PSUPERBLOCK;
SUPERBLOCK sobj;
void InitialiseSuperBlock();
void DisplaySuperBlock();
void allocateFTentry();
void allocateIITABLEentry(PIINODE);
void DisplayUFDT();
void DisplayInCoreTable();
void DisplayFileTable();
int AllocateinUAREA();
int PutInodeinIITABLE(PIINODE ptrtoinode);
#endif
