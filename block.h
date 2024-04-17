#ifndef _BLOCK_H
#define _BLOCK_H

//BLOCK_SIZE is entered is 1024 bytes

#include<stdint.h>
#include"buffer.h"
#include<sys/types.h>
#define BLOCKS_PER_LIST 96
#define BLOCK_SIZE 1024
//#pragma pack(4)
struct Block{
	char block[BLOCK_SIZE];  //each element of array is of 8bits/1 byte
};


struct DBList{
	struct Block blocks[BLOCKS_PER_LIST];
	struct DBList* next;

};


typedef struct HashBuff BUFFCACHE;
typedef struct HashBuff* PBUFFCACHE;

typedef struct BufferHeaders BUFFHEAD;
typedef struct BufferHeaders* PBUFFHEAD;
typedef struct Buffer BUFFER;
typedef struct Buffer* PBUFFER;
typedef struct Buffer** PPBUFFER;

typedef struct Block BLOCK;
typedef struct Block* PBLOCK;

typedef struct DBList list;
typedef struct DBList* plist;
typedef struct DBList** pplist;



plist init_disklink(plist element);
void Linknext(pplist head);
uint32_t Removeblockfromlist(pplist head);
void CreateBlocks(pplist head); //This creates 
PBUFFER allocblock(pplist head,dev_t device_num,PBUFFCACHE buff_cache,PBUFFHEAD freebuffhead);
void DisplayDiskList(plist head);
//PBLOCK ptr_to_block(uint32_t blkno,dev_t device_num);
#endif


/*
1 block = 1024 bytes
offset = 2086

To get block number 2086/1024=2
TO reach till offset block number's 0th byte + (2086%1024)=38th byte  



*/