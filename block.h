#ifndef _BLOCK_H
#define _BLOCK_H

//BLOCK_SIZE is entered is 1024 bytes

#include<stdint.h>
#define BLOCKS_PER_LIST 96
#define BLOCK_SIZE 1024
struct Block{
	uint8_t block[BLOCK_SIZE];  //each element of array is of 8bits/1 byte
};


struct DBList{
	struct Block blocks[BLOCKS_PER_LIST];
	struct DBList* next;

};

typedef struct Block BLOCK;
typedef struct Block* PBLOCK;

typedef struct DBList list;
typedef struct DBList* plist;
typedef struct DBList** pplist;



plist init_disklink(plist element);
void Linknext(pplist head);
struct Block* Removeblockfromlist(pplist head);


#endif
