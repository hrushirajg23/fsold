#include"DoublyCircular.h"
#include"unix1.h"
#include"superblock.h"
#include<stdio.h>
#include<stdlib.h>
#include"HashQueue.h"
 
void InsertFirst(PHEADER headptr,int iCnt,dev_t device_num){
    PIINODE newn=CreateNewInode(iCnt,device_num);
    if(headptr->First==NULL && headptr->Last==NULL){
        headptr->First=newn;
        headptr->Last=newn;
    }
    else{
        headptr->First->prev=newn;
        newn->next=headptr->First;
        headptr->First=newn;
    }
    headptr->First->prev=headptr->Last;
    headptr->Last->next=headptr->First;
    
}

void InsertLast(PHEADER headptr,int inode_num,dev_t device_num){
    PIINODE newn=CreateNewInode(inode_num,device_num);
    if(headptr->First==NULL && headptr->Last==NULL){
        headptr->First=newn;
        headptr->Last=newn;
    }
    else{
        newn->prev=headptr->Last;
        headptr->Last->next=newn;
        headptr->Last=newn;
    }
    headptr->Last->next=headptr->First;
    headptr->First->prev=headptr->Last;
}

PIINODE DeleteFirst(PHEADER headptr){
    PIINODE temp=NULL;
    if(headptr->First==NULL && headptr->Last==NULL){
        return NULL;
    }
    if(headptr->First==headptr->Last){
        temp=headptr->First;
        headptr->First=NULL;
        headptr->Last=NULL;
    }
    else{
        temp=headptr->First;
        headptr->First=headptr->First->next;
        temp->next=NULL;
        temp->prev=NULL;
        headptr->Last->next=headptr->First;
        headptr->First->prev=headptr->Last;
    }
    return temp;
}

PIINODE DeleteLast(PHEADER headptr){
    PIINODE temp=NULL;
    if(headptr->First==NULL && headptr->Last==NULL){
        return NULL;
    }
    if(headptr->First==headptr->Last){
        temp=headptr->Last;
        headptr->First=NULL;
        headptr->Last=NULL;
    }
    else{
        headptr->Last=headptr->Last->prev;
        temp=headptr->First->prev;
        temp->next=NULL;
        temp->prev=NULL;
        headptr->First->prev=headptr->Last;
        headptr->Last->next=headptr->First;
    }
    return temp;
}


void InsertFirstFree(PHEADER headptr,int iCnt,dev_t device_num){
    PIINODE newn=CreateNewInode(iCnt,device_num);
    if(headptr->First==NULL && headptr->Last==NULL){
        headptr->First=newn;
        headptr->Last=newn;
    }
    else{
        headptr->First->prevfree=newn;
        newn->nextfree=headptr->First;
        headptr->First=newn;
    }
    headptr->First->prevfree=headptr->Last;
    headptr->Last->nextfree=headptr->First;
    
}


void InsertLastFree(PHEADER headptr,int inode_num,dev_t device_num){
    PIINODE newn=CreateNewInode(inode_num,device_num);
    if(headptr->First==NULL && headptr->Last==NULL){
        headptr->First=newn;
        headptr->Last=newn;
    }
    else{
        newn->prevfree=headptr->Last;
        headptr->Last->nextfree=newn;
        headptr->Last=newn;
    }
    headptr->Last->nextfree=headptr->First;
    headptr->First->prevfree=headptr->Last;
}

PIINODE DeleteFirstFree(PHEADER headptr){
    PIINODE temp=NULL;
    if(headptr->First==NULL && headptr->Last==NULL){
        return NULL;
    }
    if(headptr->First==headptr->Last){
        temp=headptr->First;
        headptr->First=NULL;
        headptr->Last=NULL;
    }
    else{
        temp=headptr->First;
        headptr->First=headptr->First->nextfree;
        temp->nextfree=NULL;
        temp->prevfree=NULL;
        headptr->Last->nextfree=headptr->First;
        headptr->First->prevfree=headptr->Last;
    }
    return temp;
}

PIINODE DeleteLastFree(PHEADER headptr){
    PIINODE temp=NULL;
    if(headptr->First==NULL && headptr->Last==NULL){
        return NULL;
    }
    if(headptr->First==headptr->Last){
        temp=headptr->Last;
        headptr->First=NULL;
        headptr->Last=NULL;
    }
    else{
        headptr->Last=headptr->Last->prevfree;
        temp=headptr->First->prevfree;
        temp->nextfree=NULL;
        temp->prevfree=NULL;
        headptr->First->prevfree=headptr->Last;
        headptr->Last->nextfree=headptr->First;
    }
    return temp;
}



void Display(PCache cache,dev_t device_num ){
    register int iCnt=0;
    PIINODE temp=NULL;
    for(iCnt=0;iCnt<device_num;iCnt++){
        printf("----------------------->>>>For header number %d\n",iCnt);
        temp=cache->headers[iCnt].First;
        do{
            printf("%d\t",temp->inode_number);
            printf("%s\n",temp->filename);
            temp=temp->next;
        }while(temp!=cache->headers[iCnt].First);
        puts("\n\n");
    }
};

void DisplayFreeList(PHEADER pheader){
    if(pheader==NULL){
        perror("FreeList Empty\n");
        exit(EXIT_FAILURE);
    }
    PIINODE temp=pheader->First;
    do{
        printf("%d is free\t",temp->inode_number);
        temp=temp->nextfree;
    }while(temp!=pheader->First);

}