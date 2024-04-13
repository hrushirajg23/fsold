#ifndef _INODE_H
#define _INODE_H

#include<time.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdint.h>
#include<stdbool.h>

#define NUM_DIRECT_POINTERS 10
#define NUM_INDIRECT_POINTERS 3
#define BLOCK_SIZE 1024

struct DiskINODE{
    char* filename;
    unsigned int Owner;
    int FileType;
    mode_t Permissions;
    time_t last_modified;
    time_t last_access_time;
    time_t inode_modfied_time;
    int links;
    off_t FileSize;     /* size in bytes, for regular files */
    uint32_t direct_pointers[NUM_DIRECT_POINTERS];  //page 82 bach
    uint32_t indirect_pointers[NUM_INDIRECT_POINTERS]; //page 82 bach
};

struct inode_status{
    bool locked;
    bool waiting_for_unlock;
    bool inode_differs_from_disk;
    bool file_differs_from_disk;
};

struct IncoreINODE{
    char filename[30];
    unsigned int Owner;
    unsigned int Group;
    int FileType;	
    mode_t Permissions;
    time_t last_modified;
    time_t last_access_time;
    time_t inode_modfied_time;
    int links;
    off_t FileSize;     /* size in bytes, for regular files */
    uint32_t direct_pointers[NUM_DIRECT_POINTERS];  //page 82 bach
    uint32_t indirect_pointers[NUM_INDIRECT_POINTERS]; //page 82 bach
    struct inode_status status; 
    dev_t device_number;
    unsigned int inode_number;
    struct IncoreINODE *next;   //next inode on hashqueue
    struct IncoreINODE *prev;   //prev inode on hashqueue
    struct IncoreINODE *nextfree;
    struct IncoreINODE *prevfree;
    unsigned int reference_count;  
};

typedef struct IncoreINODE IINODE;
typedef struct IncoreINODE* PIINODE;
typedef struct IncoreINODE** PPIINODE;

typedef struct DiskINODE DINODE;
typedef struct DiskINODE* PDINODE;
typedef struct DiskINODE** PPDINODE;

#endif

// int main(){
//     printf("size of disk copy %d\n",sizeof(struct DiskINODE));
//     printf("size of In-core%d\n",sizeof(struct IncoreINODE));
//     return 0;
// }

// 
