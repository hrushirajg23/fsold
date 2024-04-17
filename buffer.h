#ifndef _BUFFER_H
#define _BUFFER_H

#include"block.h"
#include"superblock.h"
#include"DoublyCircular.h"
#include"HashQueue.h"
#include<sys/types.h>
#include<pthread.h>

#define PRIMARY_BUFFERS 12
#define DEVICE_NUM 4
#define BUFF_WRITE  01
#define BUFF_READ   02
#define BUFF_BUSY   03
#define BUFF_ERROR  04
#define BUFF_DELAY_WRITE 05

#define BLOCK_SIZE 1024
typedef struct Block BLOCK;
typedef struct Block* PBLOCK;

typedef struct DBList list;
typedef struct DBList* plist;
typedef struct DBList** pplist;


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
    char ptr_to_data[BLOCK_SIZE];   //pointer to datablock
    PBLOCK ptr_to_block; //may delete later
    struct Buffer* prevbuff;
    struct Buffer* nextbuff;
    struct Buffer* prevfree;
    struct Buffer* nextfree; 
    pthread_mutex_t mutex;
    pthread_cond_t cond;
};


struct BufferHeaders{
    struct Buffer* First;
    struct Buffer* Last;
};


struct HashBuff{
    struct BufferHeaders headers[DEVICE_NUM];
    uint8_t size;
};

typedef struct HashBuff BUFFCACHE;
typedef struct HashBuff* PBUFFCACHE;

typedef struct BufferHeaders BUFFHEAD;
typedef struct BufferHeaders* PBUFFHEAD;
typedef struct Buffer BUFFER;
typedef struct Buffer* PBUFFER;
typedef struct Buffer** PPBUFFER;




//use same hashfunction as inode for buffer

PBUFFER CreateNewBuffer(uint32_t,dev_t);
void initBufferCache(PBUFFCACHE,PBUFFHEAD,dev_t);
void InsertLastBuffer(PBUFFHEAD buffhead,uint32_t iCnt,dev_t device_num);
void InsertFirstBuffer(PBUFFHEAD buffhead,uint32_t iCnt,dev_t device_num);
PBUFFER DeleteFirstBuffer(PBUFFHEAD buffhead,dev_t device_num);
PBUFFER DeleteLastBuffer(PBUFFHEAD buffhead,dev_t device_num);
void Removebuffer(PBUFFHEAD buffhead,uint32_t blkno,dev_t device_num);
void InsertFirstfreebuffer(PBUFFHEAD buffhead,uint32_t buff_num,dev_t device_num);
void InsertLastfreebuffer(PBUFFHEAD buffhead,uint32_t buff_num,dev_t device_num);
PBUFFER DeleteFirstfreebuffer(PBUFFHEAD buffhead,dev_t device_num);
PBUFFER DeleteLastfreebuffer(PBUFFHEAD buffhead,dev_t device_num);
PBUFFER Removefreebuffer(PBUFFHEAD buffhead,uint32_t blkno,dev_t);
void DisplayFreeBufferList(PBUFFHEAD freehead);
void DisplayBufferCache(PBUFFCACHE bcache);
PBUFFER getblk(PBUFFCACHE,PBUFFHEAD,uint32_t,dev_t);
PBUFFER bread(PBUFFCACHE,PBUFFHEAD,uint32_t,dev_t);
void markbusy(PBUFFHEAD,uint32_t);








#endif