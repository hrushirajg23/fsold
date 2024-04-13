#ifndef _HASHQUEUE_H
#define _HASHQUEUE_H

#include"unix1.h"
#include"superblock.h"

struct HashqueueHeader{
    PIINODE First;
    PIINODE Last;
    //int (*hashfunction)(dev_t device_num,int inode_num);
};

//Cache global_cache;

struct Hashqueue{
    struct HashqueueHeader headers[4];
    int size; 
};

typedef struct Hashqueue Cache;
typedef struct Hashqueue* PCache;
//typedef struct HashqueueHeader** PPCache;

typedef struct HashqueueHeader HEADER;
typedef struct HashqueueHeader* PHEADER;
typedef struct HashqueueHeader** PPHEADER;


int hashfunction(int inode_number,dev_t);
PIINODE CreateNewInode(int num,dev_t);
void CreateHashQueue(PCache ptrtocache,PHEADER,dev_t);
PIINODE namei(const char* filename,PCache cache,dev_t);
PIINODE ialloc(PCache cache,  PHEADER freelisthead,dev_t);
void iput(PIINODE ptr,PHEADER freelisthead,dev_t);
PIINODE iget(PCache cache,int inode_num,PHEADER freelisthead,dev_t);
bool MapToHashQueue(PCache cache,int inode_num,dev_t);
PCache PointToINODEcache();
void SetParametersHashqueue(PCache cache,int inode_num,dev_t device_num,const char* filename,mode_t); 
#endif