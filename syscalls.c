
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
#include"block.h"
#include"buffer.h"

void InitialiseIITABLE(){
    int iCnt=0;
    for(iCnt=0;iCnt<MAX_INODES;iCnt++){
        IITABLE[iCnt]=NULL;
    }
}
void InitialiseFileTable(){
    int iCnt=0;
    for(iCnt=0;iCnt<MAX_INODES*5;iCnt++){
        FTABLE[iCnt].ptrtoinode=NULL;
        FTABLE[iCnt].count=0;
        FTABLE[iCnt].mode=0;
        FTABLE[iCnt].readoffset=0;
        FTABLE[iCnt].writeoffset=0;
    }
}


int PutInodeinIITABLE(PIINODE ptrtoinode){
    int iCnt=0;
    puts("In function PutInodeinIITABLE\n");
    while(IITABLE[iCnt]!=NULL && iCnt<MAX_INODES){
        if(ptrtoinode->inode_number==IITABLE[iCnt]->inode_number){
            puts("Already present\n");
            return iCnt; //already present;
        }
        iCnt++;
        printf("%d index of IITABLE\n",iCnt);
    }
    if(iCnt>MAX_INODES){
        perror("INCORE TABLE OVERFLOW\n");
        return -1;
    }
    IITABLE[iCnt]=ptrtoinode;
    printf("%d index of IITABLE has inode number %d\n",iCnt,ptrtoinode->inode_number);
    
    return iCnt;

}

int AllocateinUAREA(FILETABLE *fptr){
    int iCnt=0;
    while(ufdt[iCnt].ptrtofiletable!=NULL){
        iCnt++;
    }
    if(iCnt>MAX_INODES){
        return -1;
    }
    ufdt[iCnt].ptrtofiletable=fptr;
    return iCnt;
}

int AllocateinFILETABLE(int incoreindex,mode_t mode){
    int iCnt=0;
    while(FTABLE[iCnt].ptrtoinode!=NULL && iCnt<MAX_INODES*5){
        iCnt++;
    }
    if(iCnt>MAX_INODES*5){
        return -1;
    }
    FTABLE[iCnt].ptrtoinode=IITABLE[incoreindex];
    FTABLE[iCnt].mode=mode;
    return iCnt;
}

void InitialiseUAREA(){
    int iCnt=0;
    for(iCnt=0;iCnt<MAX_INODES;iCnt++){
        ufdt[iCnt].ptrtofiletable=NULL;
    }
}

int createfile(const char* filename,mode_t permissions,PCache ptrtocache,PHEADER freelisthead,dev_t device_num){
    
    PIINODE temp=namei(filename,ptrtocache,device_num);
    int fd=0;
    int iCnt=0;
    int PushinInCore=0;
    if(temp!=NULL){
        if(permissions!=temp->Permissions){
            return -1;
        }
    }
    else{
        puts("In else part of create file\n");
        temp=ialloc(ptrtocache,freelisthead,device_num);
        printf("~~~~~~~~~~~~~~~allocated inode~~~~~~~~~%d\n",temp->inode_number);
        SetParametersHashqueue(ptrtocache,temp->inode_number,device_num,filename,permissions); 
        /*
        sets file name and other parameters for
        the inode and hashqueue,different than seeting for freelist inode 
        */
        if(temp==NULL){
            perror("Allocation failed\n");
            exit(EXIT_FAILURE);
        }
        printf("Inode number %d allocated\n",temp->inode_number);

        puts("Displaying free list after ialloc\n");
        DisplayFreeList(freelisthead);
        puts("Displayed free list successfully\n");
        temp->Permissions=permissions;
        strcpy(temp->filename,filename);
      //  puts("Copied file name and permissions\n");
        PushinInCore=PutInodeinIITABLE(temp);
        printf("Pushing in IITABLE index %d\n",PushinInCore);
        if(PushinInCore<0){
            perror("IITABLE error\n");
            exit(EXIT_FAILURE);
        }
    }
    iCnt=AllocateinFILETABLE(PushinInCore,permissions);
    // while(FTABLE[iCnt].ptrtoinode!=NULL){
    //     iCnt++;
    // }
    // if(iCnt>MAX_INODES*5){
    //     perror("NO ENTRY FREE IN FILE TABLE \n");
    //     return -1;
    // }
    FTABLE[iCnt].ptrtoinode=temp;
    fd=AllocateinUAREA(FTABLE+iCnt); //or (&FTABLE[iCnt])
    
    return fd;
}

void DisplayFileTable(){
    int iCnt=0;
    puts("\n----------------------------Displaying FileTable----------------------------\n");
    while(FTABLE[iCnt].ptrtoinode!=NULL){
        printf("File table Entry : %d\n Mode: %o \nPoints to inode %d\n",iCnt,FTABLE[iCnt].mode,FTABLE[iCnt].ptrtoinode->inode_number);
        iCnt++;
    }
    if(iCnt==0){
        puts("FIle TABLE empty\n");
    }
}

void DisplayInCoreTable(){
    int iCnt=0;
    puts("\n----------------------------Displaying InCore Inode Table----------------------------\n");
    while(IITABLE[iCnt]!=NULL){
        printf("InCore Inode %d with name %s\t has permission %o\n",IITABLE[iCnt]->inode_number,IITABLE[iCnt]->filename,IITABLE[iCnt]->Permissions);
        iCnt++;
    }
    if(iCnt==0){
        puts("InCore Inode Table empty\n");

    }
}

void DisplayUFDT(){
    int iCnt=0;
    puts("\n----------------------------Displaying UFDTT----------------------------\n");
    while(ufdt[iCnt].ptrtofiletable!=NULL){
        printf("File descriptor %d points to %d\n",iCnt,ufdt[iCnt].ptrtofiletable->ptrtoinode->inode_number);
        iCnt++;
    }
    if(iCnt==0){
        puts("UFDT is empty\n");
    }
}

void InitialiseSuperBlock(pplist diskhead){
    sobj.SBmodified=0;
    sobj.freeblocks=NUM_BLOCKS;
    sobj.diskhead=diskhead;
    sobj.next_free_block_index=BLOCKS_PER_LIST-1;
    for(int i=0;i<MAX_INODES;i++){
        sobj.free_inodes[i]=true;
    }
    sobj.inode_list_size=MAX_INODES;
    sobj.freeinodescount=MAX_INODES;
    sobj.indexofnextfree=0;
    sobj.inode_list_size=MAX_INODES;
    sobj.SBlocked=0;
}

void DisplaySuperBlock(plist head){
	for(int i=sobj.next_free_block_index;i>=0;i--){
        printf("%d block is free\n",i);
    }	
	puts("\n");
}


int openFile(const char* filename,mode_t mode,PCache cache,PHEADER freelisthead,dev_t device_num){
    PIINODE temp=namei(filename,cache,device_num);
    printf("Opening inode %d\n",temp->inode_number);
    printf("namei %s \n",temp->filename);
    if(temp==NULL || temp->Permissions!=mode){
        return -1;
    }
    int incoreindex=PutInodeinIITABLE(temp);
    if(incoreindex==-1){
        perror("Couldn't allocate in InCore Table\n");
        return -1;
    }
    int ftindex=AllocateinFILETABLE(incoreindex,mode);
    if(ftindex==-1){
        perror("Couldn't allocate in filetable\n");
        return -1;
    }
    int fd=AllocateinUAREA(&FTABLE[ftindex]);
    return fd;
}

int main(){

    dev_t device_num=4;
    HEADER freehead;
    plist disklisthead=NULL;
    freehead.First=NULL;
    freehead.Last=NULL;
    Cache cache;
    int fd1=0,fd2=0,fd3=0,fd4=0,fd5=0,fd6=0;

    InitialiseSuperBlock(&disklisthead);
    InitialiseIITABLE();
    InitialiseUAREA();
    InitialiseFileTable();
    puts("Super BLock initialised\n");
    CreateHashQueue(&cache,&freehead,device_num);
    // puts("Hashqueue Created\n");
    fd1=createfile("penguin.txt",0644,&cache,&freehead,device_num);

   if(fd1==-1){
        perror("Couldn't create file\n");
        exit(EXIT_FAILURE);
    }
    fd4=createfile("python.txt",0444,&cache,&freehead,device_num);
    fd5=createfile("cloud.txt",0444,&cache,&freehead,device_num);
    fd2=openFile("penguin.txt",0644,&cache,&freehead,device_num);
    if(fd2==-1){
        perror("Couldn't open file\n");
        exit(EXIT_FAILURE);
    }
    fd3=openFile("python.txt",0644,&cache,&freehead,device_num);
    if(fd2==-1){
        perror("Couldn't open file\n");
        exit(EXIT_FAILURE);
    }
    //printf("Free innodes are: \n%d\n",sobj.freeinodescount);
    DisplayFileTable();
    DisplayInCoreTable();
    DisplayUFDT();
    puts("Displaying Super Block\n");
   DisplaySuperBlock(disklisthead);
    puts("\n\n");
	

    puts("=======================Displayin hashqueue for cheeck============================\n");
    Display(&cache,device_num);


    BUFFCACHE buffcache;
    BUFFHEAD freebufflisthead;
    freebufflisthead.First=NULL;
    freebufflisthead.Last=NULL;
    initBufferCache(&buffcache,&freebufflisthead,device_num);
    return 0;
}


/* */
