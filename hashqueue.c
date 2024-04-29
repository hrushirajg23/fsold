
#include"HashQueue.h"
#include"unix1.h"
#include"freelist.h"
#include"superblock.h"
#include"DoublyCircular.h"
#include<string.h>
#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<pthread.h>

//#define DEFAULT_VALUE '\0'


/*
This function is used by buffer as well as inode hashqueues 
In Inode function element_number refers to inode_number
In Buffer function element_number refers to block number

*/
int hashfunction(int element_number,dev_t device_num){
    int header_num=0;
    header_num=element_number%device_num;
   // printf("Device number : %d",device_num);
   
    return header_num;
}

PIINODE CreateNewInode(int num,dev_t device_num){
    PIINODE newn=(PIINODE)malloc(sizeof(IINODE));
    newn->FileType=0;
    newn->device_number=device_num;
    newn->inode_number=num;
    newn->status.inode_differs_from_disk=false;
    newn->status.inode_differs_from_disk=false;
    newn->status.locked=false;
    newn->status.waiting_for_unlock=false; 
    newn->reference_count=0;
    newn->prev=NULL;
    newn->prevfree=NULL;
    newn->next=NULL;
    newn->nextfree=NULL;
    newn->FileSize=0;
    //newn->filename=NULL;
    for(int i=0;i<NUM_DIRECT_POINTERS;i++){
        newn->direct[i]=NULL;
    }
    for(int i=0;i<NUM_INDIRECT_POINTERS;i++){
        newn->indirect[i]=NULL;
    }
    return newn;
};

/*
    
*/

void CreateHashQueue(PCache ptrtocache,PHEADER freelisthead,dev_t device_num){
    register int iCnt=0;
    int header_num=0;
    ptrtocache->size=device_num;
    for(iCnt=0;iCnt< ptrtocache->size;iCnt++){
        ptrtocache->headers[iCnt].First=NULL;
        ptrtocache->headers[iCnt].Last=NULL;
    }
    for(iCnt=1;iCnt<=MAX_INODES;iCnt++){
        header_num=hashfunction(iCnt,device_num);
        InsertLast(ptrtocache->headers+header_num,iCnt,device_num);
        InsertLastFree(freelisthead,iCnt,device_num);
    }
    Display(ptrtocache,device_num);
    puts("Displaying free list\n");
    DisplayFreeList(freelisthead);
}




bool MapToHashQueue(PCache cache,int inode_num,dev_t device_num){
    printf("For inode number :%d\n",inode_num);
    int header_num=hashfunction(inode_num,device_num);
    printf("Header number :%d",header_num);
    register PIINODE temp=cache->headers[header_num].First;
    bool bFlag=false;
    do{
        if(temp->inode_number==inode_num){
            printf("\nMapped successfully\n inode number: %d in header number: %d\n",inode_num,header_num);
            bFlag=true;
            break;
        }
        temp=temp->next;
    }while(temp!=cache->headers[header_num].First);
    return bFlag;
}


PIINODE iget(PCache cache,int inode_num,PHEADER freelisthead,dev_t device_num){
    puts("Entered iget\n");
    PIINODE hold=NULL;
    while(1){
        if(MapToHashQueue(cache,inode_num,device_num)==true){
            puts("Mapped successfully\n");
            if(sobj.free_inodes[inode_num]==true){
                hold=DeleteFirstFree(freelisthead);
                sobj.free_inodes[inode_num]=false;
                sobj.freeinodescount--;
                sobj.indexofnextfree++;
                hold->reference_count++; 
                return hold;
            }
        }
        else{
            puts("Couldn't map to hashqueue\n");
            break;
        }
        if(sobj.freeinodescount==0){
            return NULL;
        }

    }
    
}

void iput(PIINODE ptr,PHEADER freelisthead,dev_t device_num){

}


PIINODE ialloc(PCache cache,  PHEADER freelisthead,dev_t device_num){
    puts("Entered ialloc\n");
    register PIINODE temp=NULL;
    while(1){
        if(sobj.freeinodescount!=0){
            int freeindex=sobj.indexofnextfree;
            printf("Freeindex number %d\n",freeindex);
            freeindex++;
            //add +1 to free index karan inodes are from 1 to 100 in Hashqueue and SB madhe 0 to 99 , so to match them we add 1
            temp=iget(cache,freeindex,freelisthead,device_num);
           // sobj.free_inodes[freeindex]=false;
           //sobj.freeinodescount--;
            //sobj.indexofnextfree++;
            return temp;
        }
        else{
            return NULL;
        }
    }
}

/*
    @param: filename,Pointer_to_Inode_Cache,device_num
    Search for inode with name given by user 

*/
PIINODE namei(const char* filename,PCache cache,dev_t device_num){
    puts("In namei\n");
    register int iCnt=0; 
    int compare=0;
    static int callount=1;
    printf("Namei called %d times\n",callount++);
    register PIINODE temp=NULL;
    for(iCnt=0;iCnt<device_num;iCnt++){
        printf("----------------------->>>>For header number %d\n",iCnt);
        temp=cache->headers[iCnt].First;
        //printf("")
        do{
            //compare=;
            if(strcmp(temp->filename,filename)==0){
                printf("%d\n",temp->inode_number);
                //temp->status.locked=true;
                return temp;
            }
            // else if(compare<0){
            //     printf("str1 is less than str2.\n");
            // }else{
            //     printf("str2 is less than str1.\n");
            // }
            
            temp=temp->next;
        }while(temp!=cache->headers[iCnt].First);
        puts("\n\n");
    }
    puts("returning NULL\n");
    return NULL;
}

void SetParametersHashqueue(PCache cache,int inode_num,dev_t device_num,const char* filename,mode_t permissions){
        int header_num=hashfunction(inode_num,device_num);
        printf("While for inode number %d setting header number is %d\n",inode_num,header_num);
        int iCnt=0;
        PIINODE travel=cache->headers[header_num].First;
        do{
            if(travel->inode_number==inode_num){
                break;
            }
            travel=travel->next;
        }while(travel!=cache->headers[iCnt].First);
        // if(travel==cache->headers[iCnt].First){
        //     perror("Error in setting paramters for hashqueue inode\n");
        //     exit(EXIT_FAILURE);
        // }
        const char* monkey=strcpy(travel->filename,filename);
        printf("%s-",travel->filename);
        travel->Permissions=permissions;
}

/*
    @brief: Conversion of Byte Offset to Block number in File system
    @param: ptr_to_inode, offset
    @returns: 1. *direct* block number in file system(i.e direct block number of file),
            2. byte offset into block ,3. bytes of I/O in block
            4. read ahead block number                  
    @example: pg 84 J bach
*/
uint32_t bmap(PIINODE ptr_to_inode,off_t offset){
    uint32_t blkno=0;
    if(offset < (BLOCK_SIZE*NUM_DIRECT_POINTERS)){
        blkno= offset/1024;
    }
    /*
    else case for indirect block numbers
    */
    return blkno;

}