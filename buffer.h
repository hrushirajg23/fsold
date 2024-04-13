#ifndef _BUFFER_H
#define _BUFFER_H
#include<sys/types.h>
#include"block.h"
#include"superblock.h"
#include"DoublyCircular.h"
#include"HashQueue.h"

#define PRIMARY_BUFFERS 96 

#define BUFF_WRITE  01
#define BUFF_READ   02
#define BUFF_BUSY   03
#define BUFF_ERROR  04

struct Buffer{
    dev_t devicenum;
    uint32_t block_number;
    struct status{
        bool locked;
        bool valid_data;
        bool delayed_write;  //the kernel must write the contents buffer contents to disk before reassigning the buffer
        bool proc_waiting;//process is current waiting for the buffer to become free
        bool busy; //the kernel is currently reading or writing contents of buffer to disk
    }buff_status;
    char* ptr_to_data;   //pointer to datablock
    struct Buffer* prevbuff;
    struct Buffer* nextbuff;
    struct Buffer* prevfree;
    struct Buffer* nextfree; 
};

typedef struct Buffer BUFFER;
typedef struct Buffer* PBUFFER;
typedef struct Buffer** PPBUFFER;

struct BufferHeaders{
    PBUFFER First;
    PBUFFER Last;
};

typedef struct BufferHeaders BUFFHEAD;
typedef struct BufferHeaders* PBUFFHEAD;


struct HashBuff{
    struct BufferHeaders headers[DEVICE_NUM];
    uint8_t size;
};

typedef struct HashBuff BUFFCACHE;
typedef struct HashBuff* PBUFFCACHE;

//use same hashfunction as inode for buffer

PBUFFER CreateNewBuffer(uint32_t,dev_t);
void initBufferCache(PBUFFCACHE,PBUFFHEAD,dev_t);
void InsertLastBuffer(PBUFFHEAD buffhead,int iCnt,dev_t device_num);
void InsertFirstBuffer(PBUFFHEAD buffhead,int iCnt,dev_t device_num);
PBUFFER DeleteFirstBuffer(PBUFFHEAD buffhead,dev_t device_num);
PBUFFER DeleteLastBuffer(PBUFFHEAD buffhead,dev_t device_num);
void InsertFirstfreebuffer(PBUFFHEAD buffhead,int buff_num,dev_t device_num);
void InsertLastfreebuffer(PBUFFHEAD buffhead,int buff_num,dev_t device_num);
PBUFFER DeleteFirstfreebuffer(PBUFFHEAD buffhead,dev_t device_num);
PBUFFER DeleteLastfreebuffer(PBUFFHEAD buffhead,dev_t device_num);
void DisplayFreeBufferList(PBUFFHEAD freehead);
void DisplayBufferCache(PBUFFCACHE bcache);

#endif