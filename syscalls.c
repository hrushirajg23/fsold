
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
    while(ufdt.ptrtofiletable[iCnt]!=NULL){
        iCnt++;
    }
    if(iCnt>MAX_INODES){
        return -1;
    }
    ufdt.ptrtofiletable[iCnt]=fptr;
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
        ufdt.ptrtofiletable[iCnt]=NULL;
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
    while(ufdt.ptrtofiletable[iCnt]!=NULL){
        printf("File descriptor %d points to %d\n",iCnt,ufdt.ptrtofiletable[iCnt]->ptrtoinode->inode_number);
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


/**/
int searchfd(const char* name){
    int iCnt=0;
    for(iCnt=0;iCnt<MAX_INODES;iCnt++){
        if(strcmp(name,ufdt.ptrtofiletable[iCnt]->ptrtoinode->filename)==0){
            return iCnt;
        }
        if(ufdt.ptrtofiletable[iCnt]==NULL){
            return -1;
        }
    }
    return -1;
}

int writefile(int filedes,char* buffer,uint64_t count,pplist disklisthead,dev_t device_num,PBUFFCACHE buff_cache,PBUFFHEAD freebuffhead){
    
    printf("count is %d\n",count);
    off_t position=0;
    
    PIINODE ptr_to_inode=NULL;
    uint32_t blkno=0;
    uint64_t iCnt=0,written=0,index=0;
    PBUFFER buff=NULL;
    uint16_t copy=0;
    char* ptr_to_blk_data=NULL;
    ptr_to_inode=ufdt.ptrtofiletable[filedes]->ptrtoinode;
    ufdt.address=buffer;
    ufdt.RWbytes=count;
    while(iCnt<count){
        position=ufdt.ptrtofiletable[filedes]->ptrtoinode->FileSize;
        blkno=bmap(ptr_to_inode,position);

        if(ptr_to_inode->direct[blkno]==NULL){
            puts("Direct block is NULL\n");
            buff=allocblock(disklisthead,device_num,buff_cache,freebuffhead);
            ptr_to_inode->direct[blkno]=buff->ptr_to_block;
            printf("Super block list block number is %u\n",buff->block_number);
        }
        else{
            puts("Direct block conatins data\n");
            buff=getblk(buff_cache,freebuffhead,95,device_num);
           // buff->ptr_to_block=ptr_to_inode->direct+blkno;
           printf("Super block list block number is %u\n",buff->block_number);
        }
        if(count-iCnt>=BLOCK_SIZE){
            puts("Count greater than BLOCK_SIZE\n");
            strncpy(buff->ptr_to_data,ufdt.address+iCnt,BLOCK_SIZE);
            iCnt+=BLOCK_SIZE;
            copy=BLOCK_SIZE;
        }
        else{
            puts("Count less than BLOCK_SIZE\n");
            strncpy(buff->ptr_to_data,ufdt.address+iCnt,count-iCnt);
            copy=count-iCnt;
            iCnt+=(count-iCnt);
            
        }
        index=position%BLOCK_SIZE;
        printf("index is %d\n",index);
        ptr_to_blk_data=ptr_to_inode->direct[blkno]->block+index;

        written=0;
        puts("initiating disk write\n");
        printf("Buffer contains data %s and has to copy %d bytes\n",buff->ptr_to_data,copy);
        while(index<BLOCK_SIZE && written<copy){
            ptr_to_blk_data[written]=buff->ptr_to_data[written]; //fixed this on 29-04-24
            written++;
            index++;
        }
        if(written!=copy){
            iCnt-=copy-written;
        }
        ptr_to_inode->FileSize+=iCnt;

        brelse(buff_cache,freebuffhead,buff,device_num);
    }
    printf("%s\n",ptr_to_blk_data);
    printf("\nNumber of bytes written are %d\n",written);
    return iCnt;

    

}
int main()
{

    dev_t device_num=4;
    HEADER freehead;
    plist disklisthead=NULL;
    freehead.First=NULL;
    freehead.Last=NULL;
    Cache cache;
    int fd1=0,fd2=0,fd3=0,fd4=0,fd5=0,fd6=0,fd7=0;

    char* ptr=NULL;
    int iRet=0,count=0;

    char command[4][80], str[80],arr[1024]; 

    //arr is used as buffer for input from user
    //10 direct blocks = 10* 1024 =10240
    //testing for indirect blocks will be done later 

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
    fd4=createfile("python.txt",0777,&cache,&freehead,device_num);
    fd5=createfile("cloud.txt",0444,&cache,&freehead,device_num);
    fd2=openFile("penguin.txt",0644,&cache,&freehead,device_num);
    if(fd2==-1){
        perror("Couldn't open file\n");
        exit(EXIT_FAILURE);
    }
    fd3=openFile("python.txt",0777,&cache,&freehead,device_num);
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

    Linknext(&disklisthead);  //Initialise disk list
    DisplayDiskList(disklisthead);


    puts("testing write system call for python.txt \n");
     //bread(&buffcache,&freebufflisthead,95,device_num);
    // brelse(&buffcache,&freebufflisthead,)

    //fd7=searchfd("python.txt");
    // if(fd3==-1){
    //     puts("System issue\n");
    
    // }
    
    printf("Enter the data: \n");
    scanf("%[^'\n']s",arr);

    iRet=strlen(arr);
    if(iRet==0){
        puts("Incorrect parameter\n");
        
    }

    iRet=writefile(fd3,arr,iRet,&disklisthead,device_num,&buffcache,&freebufflisthead);

    if(iRet==-1){
        puts("ERROR: PERMISSION denied\n");

    }

    DisplayDiskList(disklisthead);

    fflush(stdin);
    ufdt.address=NULL;
    ufdt.offset=0;
    ufdt.RWbytes=0;
    memset(arr,0,sizeof(arr));
    printf("Enter the data: \n");
    scanf("%[^'\n']s",arr);

    iRet=strlen(arr);
    if(iRet==0){
        puts("Incorrect parameter\n");
        
    }

    iRet=writefile(fd3,arr,iRet,&disklisthead,device_num,&buffcache,&freebufflisthead);

    if(iRet==-1){
        puts("ERROR: PERMISSION denied\n");

    }
    

   DisplayDiskList(disklisthead);
    return 0;
}


//     while(1){
//         fflush(stdin);
//         strcpy(str,"");

//         puts("\n debian: > ");
    
//         fgets(str,80,stdin);

//         count=sscanf(str,"%s %s %s %s",command[0],command[1],command[2],command[3]);
//         if(count==1){
//             if(strcmp(command[0],"exit")==0){
//                 puts("\nshell says goodbye\n");
//                 DisplayDiskList(disklisthead);
//                 break;
//             }
//         }
//         if(count==2){
//             if(strcmp(command[0],"write")==0){
//                // fd3=searchfd(command[1]);
//                 // if(fd3==-1){
//                 //     puts("System issue\n");
//                 //     continue;
//                 // }
//                 printf("Enter the data: \n");
//                 scanf("%[^'\n']s",arr);

//                 iRet=strlen(arr);
//                 printf("data is of length %d\n",iRet);
//                 if(iRet==0){
//                     puts("Incorrect parameter\n");
//                     continue;
//                 }

//                 iRet=writefile(fd3,arr,iRet,&disklisthead,device_num,&buffcache,&freebufflisthead);
                
//                 puts("No prblem in call\n");
//                 if(iRet==-1){
//                     puts("ERROR: PERMISSION denied\n");

//                 }

                
//                 continue;
//             }
//     }
    
    
// }


    





/* command :- 

gcc syscalls.c diskblock.c buffer.c hashqueue.c DoublyCircularList.c -o myexe 

On Unix-like systems, including Linux, you need to link against the pthread library explicitly when using pthread functions. 
To do this, you can add -pthread option to your gcc command:

*/
