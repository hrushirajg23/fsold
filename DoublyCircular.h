#ifndef _DOUBLYCIRCULAR_H
#define _DOUBLYCIRCULAR_H

#include"unix1.h"
#include"superblock.h"
#include"HashQueue.h"
#include"superblock.h"
#include"buffer.h"


typedef struct HashqueueHeader HEADER;
typedef struct HashqueueHeader* PHEADER;
typedef struct HashqueueHeader** PPHEADER;

typedef struct Hashqueue Cache;
typedef struct Hashqueue* PCache;

void InsertLast(PHEADER headptr,int inode_num,dev_t);
void InsertFirst(PHEADER headptr,int inode_num,dev_t);
PIINODE DeleteFirst(PHEADER headptr);
PIINODE DeleteLast(PHEADER headptr);
void Display(PCache cache,dev_t device_num);



void InsertFirstFree(PHEADER headptr,int iCnt,dev_t device_num);
void InsertLastFree(PHEADER headptr,int iCnt,dev_t device_num);
PIINODE DeleteFirstFree(PHEADER headptr);
PIINODE DeleteLastFree(PHEADER headptr);
void DisplayFreeList(PHEADER headptr);



#endif
