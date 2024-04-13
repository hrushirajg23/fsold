#include<stdio.h>
#include<stdlib.h>
#include"buffer.h"
#include"HashQueue.h"
#include"superblock.h"
#include"block.h"
#include"DoublyCircular.h"
#include"unix1.h"



PBUFFER CreateNewBuffer(uint32_t block_num,dev_t device_number){
    PBUFFER buff=(PBUFFER)malloc(sizeof(BUFFER));
    buff->block_number=block_num;
    buff->devicenum=device_number;
    buff->ptr_to_data=NULL;
    buff->nextbuff=NULL;
    buff->prevbuff=NULL;
    buff->prevfree=NULL;
    buff->nextfree=NULL;
    buff->buff_status.busy=false;
    buff->buff_status.delayed_write=false;
    buff->buff_status.locked=false;
    buff->buff_status.proc_waiting=false;
    buff->buff_status.valid_data=false;
    return buff;
}

void InsertFirstBuffer(PBUFFHEAD buffhead,int iCnt,dev_t device_num){
    PBUFFER newn=CreateNewBuffer(iCnt,device_num);
    if(buffhead->First==NULL && buffhead->Last==NULL){
        buffhead->First=newn;
        buffhead->Last=newn;
    }
    else{
       buffhead->First->prevbuff=newn;
       newn->nextbuff=buffhead->First;
        buffhead->First=newn;
    }
    buffhead->Last->nextbuff=buffhead->First;
        buffhead->First->prevbuff=buffhead->Last;

}

void InsertLastBuffer(PBUFFHEAD buffhead,int iCnt,dev_t device_num){
    PBUFFER newn=CreateNewBuffer(iCnt,device_num);
    if(buffhead->First==NULL && buffhead->Last==NULL){
        buffhead->First=newn;
        buffhead->Last=newn;
    }
    else{
        newn->prevbuff=buffhead->Last;
        buffhead->Last->nextbuff=newn;
        buffhead->Last=newn;    
    }
    buffhead->Last->nextbuff=buffhead->First;
    buffhead->First->prevbuff=buffhead->Last;
}


PBUFFER DeleteFirstBuffer(PBUFFHEAD buffhead,dev_t device_num){
    PBUFFER hold=NULL;
    if(buffhead->First==NULL && buffhead->Last==NULL){
        perror("List empty\n");
        return NULL;
    }
    if(buffhead->First==buffhead->Last){
        hold=buffhead->First;
        buffhead->First=NULL;
        buffhead->Last=NULL;
    }
    else{
        hold=buffhead->First;
        buffhead->First=buffhead->First->nextbuff;
        buffhead->First->prevbuff=buffhead->Last;
        buffhead->Last->nextbuff=buffhead->First;
    }
    return hold;
}

PBUFFER DeleteLastBuffer(PBUFFHEAD buffhead,dev_t device_num){
    PBUFFER hold=NULL;
    if(buffhead->First==NULL && buffhead->Last==NULL){
        perror("List empty\n");
        return NULL;
    }
    if(buffhead->First==buffhead->Last){
        hold=buffhead->First;
        buffhead->First=NULL;
        buffhead->Last=NULL;
    }
    else{
        hold=buffhead->Last;
        buffhead->Last=buffhead->Last->prevbuff;
        buffhead->First->prevbuff=buffhead->Last;
        buffhead->Last->nextbuff=buffhead->First;
    }
    return hold;

}

void InsertFirstfreebuffer(PBUFFHEAD buffhead,int buff_num,dev_t device_num){
    PBUFFER newn=CreateNewBuffer(buff_num,device_num);
    if(buffhead->First==NULL && buffhead->Last==NULL){
        buffhead->First=newn;
        buffhead->Last=newn;
    }
    else{
       buffhead->First->prevfree=newn;
       newn->nextfree=buffhead->First;
        buffhead->First=newn;
       
    }
    buffhead->Last->nextfree=buffhead->First;
    buffhead->First->prevfree=buffhead->Last;
}


void InsertLastfreebuffer(PBUFFHEAD buffhead,int buff_num,dev_t device_num){
    PBUFFER newn=CreateNewBuffer(buff_num,device_num);
    if(buffhead->First==NULL && buffhead->Last==NULL){
        buffhead->First=newn;
        buffhead->Last=newn;
    }
    else{
       newn->prevfree=buffhead->Last; 
       buffhead->Last->nextfree=newn;
        buffhead->Last=newn;
    }
    buffhead->Last->nextfree=buffhead->First;
    buffhead->First->prevfree=buffhead->Last;
}



PBUFFER DeleteFirstfreebuffer(PBUFFHEAD buffhead,dev_t device_num){
    PBUFFER hold=NULL;
    if(buffhead->First==NULL && buffhead->Last==NULL){
        perror("List empty\n");
        return NULL;
    }
    if(buffhead->First==buffhead->Last){
        hold=buffhead->First;
        buffhead->First=NULL;
        buffhead->Last=NULL;
    }
    else{
        hold=buffhead->First;
        buffhead->First=buffhead->First->nextfree;
        buffhead->First->prevfree=buffhead->Last;
        buffhead->Last->nextfree=buffhead->First;
    }
    return hold;
}

PBUFFER DeleteLastfreebuffer(PBUFFHEAD buffhead,dev_t device_num){
    PBUFFER hold=NULL;
    if(buffhead->First==NULL && buffhead->Last==NULL){
        perror("List empty\n");
        return NULL;
    }
    if(buffhead->First==buffhead->Last){
        hold=buffhead->First;
        buffhead->First=NULL;
        buffhead->Last=NULL;
    }
    else{
        hold=buffhead->Last;
        buffhead->Last=buffhead->Last->prevfree;
        buffhead->First->prevfree=buffhead->Last;
        buffhead->Last->nextfree=buffhead->First;
    }
    return hold;

}


void DisplayBufferCache(PBUFFCACHE bcache){
    register int iCnt=0;
    PBUFFER iterate=NULL;
    for(iCnt=0;iCnt<bcache->size;iCnt++){
        printf("Buffer header number : %u\n",iCnt+1);
        iterate=bcache->headers[iCnt].First;
        do{
            printf("%d\t",iterate->block_number);
            iterate=iterate->nextbuff;
        }while(iterate!=bcache->headers[iCnt].First);
        puts("\n\n");
    }
}

void DisplayFreeBufferList(PBUFFHEAD freehead){
    puts("Displaying FreeList >>>>>>>>>\n");
    if(freehead->First==NULL && freehead->Last==NULL){
        perror("Free List Empty\n");
        exit(EXIT_FAILURE);
    }

    PBUFFER iterate=freehead->First;
    do{
        printf("%u\t",iterate->block_number);
        iterate=iterate->nextfree;
    }while(iterate!=freehead->First);
}

void initBufferCache(PBUFFCACHE bcache,PBUFFHEAD freelisthead,dev_t device_num){
    register int iCnt=0;
    uint8_t header_number=0;
    bcache->size=device_num;
    for(iCnt=0;iCnt<bcache->size;iCnt++){
        bcache->headers[iCnt].First=NULL;
        bcache->headers[iCnt].Last=NULL;
    }
    for(iCnt=1;iCnt<=PRIMARY_BUFFERS;iCnt++){
        header_number=hashfunction(iCnt,device_num);
        InsertLastBuffer(bcache->headers+header_number,iCnt,device_num);
        InsertLastfreebuffer(freelisthead,iCnt,device_num);
    }
    DisplayBufferCache(bcache);
    puts("\n\n");
    DisplayFreeBufferList(freelisthead);
}

