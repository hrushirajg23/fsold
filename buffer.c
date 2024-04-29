#include<stdio.h>
#include<stdlib.h>
#include"buffer.h"
#include"HashQueue.h"
#include"superblock.h"
#include"block.h"
#include"DoublyCircular.h"
#include"unix1.h"
#include<sys/types.h>
#include<pthread.h>
#include<string.h>

PBUFFER CreateNewBuffer(uint32_t block_num,dev_t device_number){
    PBUFFER buff=(PBUFFER)malloc(sizeof(BUFFER));
    buff->block_number=block_num;
    buff->devicenum=device_number;
    buff->ptr_to_block=NULL;
    //buff->ptr_to_data=NULL;
    memset(buff->ptr_to_data,'\0',sizeof(buff->ptr_to_data));
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

void lock_unlock_buff(PBUFFHEAD buffhead,uint32_t blkno,uint8_t status){
    PBUFFER mark=buffhead->First;
    do{
        if(mark->block_number==blkno){
            if(status==BUFF_LOCK){
                mark->buff_status.busy=true;
                mark->buff_status.locked=true;
            }
            else if(status==BUFF_UNLOCK){
                mark->buff_status.busy=false;
            mark->buff_status.locked=false;
            }
            break;
        }
        mark=mark->nextbuff;
    }while(mark!=buffhead->First);
}

void InsertFirstBuffer(PBUFFHEAD buffhead,uint32_t iCnt,dev_t device_num){
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

void InsertLastBuffer(PBUFFHEAD buffhead,uint32_t iCnt,dev_t device_num){
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
    printf("Now headers buff head points to blkno %u\n",buffhead->First->block_number);
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

void Removebuffer(struct BufferHeaders* buffhead,uint32_t blkno,dev_t device_num){
    PBUFFER giveback=NULL;
    if(blkno >=96 || (buffhead->First==NULL && buffhead->Last==NULL)){
        perror("Error buffer header queue is maybe empty\n ");
        exit(EXIT_FAILURE);
    }
    if(buffhead->First->block_number==blkno ){
        giveback=DeleteFirstBuffer(buffhead,device_num);
    }
    else if(buffhead->Last->block_number==blkno){
        giveback=DeleteLastBuffer(buffhead,device_num);
    }
    else{
        PBUFFER travel=buffhead->First;
        do{
            if(travel->nextbuff->block_number==blkno){
                giveback=travel->nextbuff;
                travel->nextbuff=giveback->nextbuff;
                giveback->nextbuff->prevbuff=travel;

                giveback->nextbuff=NULL;
                giveback->prevbuff=NULL;
                free(giveback);
            }
            travel=travel->nextbuff;

        }while(travel->nextbuff!=buffhead->First);    
    }
    
}   

void InsertFirstfreebuffer(PBUFFHEAD buffhead,uint32_t buff_num,dev_t device_num){
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

void InsertLastfreebuffer(PBUFFHEAD buffhead,uint32_t buff_num,dev_t device_num){
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
        hold->nextfree=NULL,hold->prevfree=NULL;
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
        hold->nextfree=NULL,hold->prevfree=NULL;
    }
    return hold;

}

/*Similar to Delete at position */
PBUFFER Removefreebuffer(PBUFFHEAD buffhead,uint32_t blkno,dev_t device_num){
    PBUFFER travel=buffhead->First;
    PBUFFER giveback=NULL;
    if(blkno>=96 || (buffhead->First==NULL && buffhead->Last==NULL)){
        perror("Free Buffer List is empty\n ");
        return NULL;
    }
    
    if(buffhead->First->block_number==blkno){
        giveback=DeleteFirstfreebuffer(buffhead,device_num);
        puts("In if part \n");
    }
    else if(buffhead->Last->block_number==blkno){
        giveback=DeleteLastfreebuffer(buffhead,device_num);
        puts("In else if part \n");
    }
    else{
        puts("In else part \n");
        do{
            if(travel->nextfree->block_number==blkno){
                giveback=travel->nextfree;
                travel->nextfree=giveback->nextfree;
                giveback->nextfree->prevfree=travel;
                giveback->nextfree==NULL;
                giveback->prevfree=NULL;
                //free(giveback);
            }
            travel=travel->nextfree;
        }while(travel->nextfree!=buffhead->First);

    }
    return giveback;
}

void DisplayBufferCache(PBUFFCACHE bcache){
    register int iCnt=0;
    PBUFFER iterate=NULL;
    for(iCnt=0;iCnt<bcache->size;iCnt++){
        printf("Buffer header number : %u\n",iCnt+1);
        iterate=bcache->headers[iCnt].First;
        do{
            printf("%d\t%s\t%s", iterate->block_number,
                    iterate->buff_status.locked ? "true" : "false",
                    iterate->buff_status.busy ? "true" : "false");

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
    printf("End of free buffer list\n");
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




/*
Confusion was if buffer was allocated for block 96 of disklist 1
what if it would've block 96 of disklist 2.
Mhanje 96th block of disklist 1 la buffer number 96 allocated asel tar 
ani disklist 1 madhe free blocks sample tar disklist 2 madhla 96th block 
jevha allocate hoil tevha 96th buffer would be already allocated for disklit 1 
cha 96th block . So tevha Scenario 5 would be executed . Process would go to sleep 
until 96th buffer becomes free .

*/
/*
    @brief: Returns a free number for a block . Has total 5 scenarios only 2-3 are executed
    

    //I added that buffer's ptr_to_block will point to the block
    //else it will create problem while writing , check bwrite description and bread neatly


*/
PBUFFER getblk(PBUFFCACHE buff_cache,PBUFFHEAD freebuffhead,uint32_t blkno,dev_t device_num){
    //scenario 1: find buffer and block number matching
    bool buffer_found=false;
    int header_num=hashfunction(blkno,device_num);
    PBUFFER travel=buff_cache->headers[header_num].First;
    do{
        if(travel->block_number==blkno){
          //  pthread_mutex_lock(&travel->mutex);

            travel->ptr_to_block=&(*(sobj.diskhead))->blocks[blkno];

            /*
            
                ptr_to_block is a pointer to block structure which has an array of 1024 bytes
            */
            if(travel->buff_status.busy==true){
                while(travel->buff_status.busy || travel->buff_status.locked){
            //        pthread_cond_wait(&travel->cond,&travel->mutex);
                    puts("waiting\n");
                    continue;
                }
            }
            travel->buff_status.busy=true;
            travel->buff_status.locked=true;
            //pthread_mutex_unlock(&travel->mutex);
            travel=Removefreebuffer(freebuffhead,travel->block_number,device_num);
            if(travel==NULL){
                perror("Couldn't allocate buffer system crahed\n");
                exit(EXIT_FAILURE);
            }
            return travel;
        }
        travel=travel->nextbuff;
    }while(travel!=buff_cache->headers[header_num].First);
    /*
    if buffer gets returned before this means buffer is in hashqueue 
    */ 
    puts("Scenario 1 and 5 failed\n");   
    if(freebuffhead->First==NULL && freebuffhead->Last==NULL){
        while(freebuffhead->First==NULL && freebuffhead->Last==NULL){
            //pthread_cond_wait(&travel->cond,&travel->mutex);
            puts("Waiting\n");
            continue;
        }
    }
    travel=DeleteFirstfreebuffer(freebuffhead,device_num);  //removed a buffer from free list
    printf("Block number of travel is %u\n",travel->block_number);
    if(travel->buff_status.delayed_write==true){
        /*
        write buffer to disk first and continuw from loop again
        */
    }
    /*
    Scenario 2 -- found a buffer in freelist but it isn't on hashqueue
    ->Using hashfunction put it in particular hashqueue
    */
    //was hashfunction(travel->blocknumber,dev_num) earlier
    header_num=hashfunction(blkno,device_num);
    printf(" header number %d for block number %u\n",header_num,blkno);
    //Remove the old from hashqueue
    
    Removebuffer(&buff_cache->headers[header_num],travel->block_number,device_num);
    travel->block_number=blkno;
    travel->ptr_to_block=&(*(sobj.diskhead))->blocks[blkno];  //added this line extra   
    printf("now travel's block number is %u\n",blkno);

    InsertFirstBuffer(&buff_cache->headers[header_num],blkno,device_num);
    lock_unlock_buff(&buff_cache->headers[header_num],blkno,BUFF_LOCK);
   
    puts("Reeturning travel\n");
    if(travel==NULL){
        puts("Problem in allocation\n");
    }
    return travel;

}


PBUFFER bread(PBUFFCACHE buff_cache,PBUFFHEAD freebuffhead,uint32_t blkno,dev_t device_num){
    PBUFFER get=getblk(buff_cache,freebuffhead,blkno,device_num);
    printf("\n Got buffer for blkno \n");
    //sobj.next_free_block_index;
    
    
    /*
        modified from: strcpy(get->ptr_to_data,((*(sobj.diskhead))->blocks->block+blkno));
    */
   //modified to
   strcpy(get->ptr_to_data,((*(sobj.diskhead))->blocks[blkno].block));
    puts("bread completed\n");
    return get;
}

//
void bwrite(PBUFFER buff){
    strcpy(buff->ptr_to_block->block,buff->ptr_to_data);

    //has more complexity but still 

    /*if what if buffer holds address of block number 95
    and super block changes its list of free blocks as number of free blocks reduce to zero 

    in this situation we will write to the wrong block 100%

    For this user a pointer to the block from the buffer
*/

}


void brelse(PBUFFCACHE buff_cache,PBUFFHEAD  freebuffhead,PBUFFER buff,dev_t device_num){
    
    //Enqueue free buffer
    printf("Releasing buffer containing block %u",buff->block_number);
    free(buff->ptr_to_data);
    InsertLastfreebuffer(freebuffhead,buff->block_number,device_num);
    int header_num=hashfunction(buff->block_number,device_num);
    lock_unlock_buff(&buff_cache->headers[header_num],buff->block_number,BUFF_UNLOCK);
}

/*




*/