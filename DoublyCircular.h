#ifndef _DOUBLYCIRCULAR_H
#define _DOUBLYCIRCULAR_H

#include"unix1.h"
#include"superblock.h"
#include"HashQueue.h"


void InsertLast(PHEADER ,int inode_num,dev_t);
void InsertFirst(PHEADER ,int inode_num,dev_t);
PIINODE DeleteFirst(PHEADER);
PIINODE DeleteLast(PHEADER);
void Display(PCache,dev_t);



void InsertFirstFree(PHEADER headptr,int iCnt,dev_t device_num);
void InsertLastFree(PHEADER headptr,int iCnt,dev_t device_num);
PIINODE DeleteFirstFree(PHEADER);
PIINODE DeleteLastFree(PHEADER);
void DisplayFreeList(PHEADER);



#endif
