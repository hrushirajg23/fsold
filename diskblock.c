/*
First bread is used to read the blockand then uses byte offset of inode in 
block by the below formula
*/

/*
block number for inode formula:

start block of inode list + (inode number -1)/number of inodes per block
*/



#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>
#include"superblock.h"
#include"unix1.h"
#include"block.h"
#include"buffer.h"
#include<pthread.h>
plist init_disklink(plist element){
	memset(element->blocks->block,'\0',sizeof(element->blocks));
	element->next=NULL;
	return element;
}

//Link a whole list of 96 disk blocks at last
void Linknext(pplist head){
	
	plist newn=(plist)malloc(sizeof(list));
	newn=init_disklink(newn);
	plist traversal=(*head);
	if(*head==NULL){
		*head=newn;
	}
	else{
		while(traversal->next!=NULL){
			traversal=traversal->next;
		}
		traversal->next=newn;
	}
	printf("Linked Disk List\n");
}


/*
	@return: Block Number from disklist
*/
uint32_t Removeblockfromlist(pplist head){
	plist temp_to_list=NULL;
	//PBLOCK ptr_to_block=NULL;
	uint32_t blkno=0;
	sobj.diskhead=head;
	//ptr_to_block=(*head)->blocks+sobj.next_free_block_index; 
	blkno=sobj.next_free_block_index;
	sobj.freeblocks--;
	sobj.next_free_block_index--;
	if(sobj.next_free_block_index< 0){
		sobj.SBlocked=true;
		temp_to_list=(*head);
		if(temp_to_list->next==NULL){
			Linknext(head);
			sobj.next_free_block_index=BLOCKS_PER_LIST-1;
		}		
		temp_to_list=temp_to_list->next;
		sobj.diskhead=&temp_to_list;
		sobj.SBlocked=false;
		sobj.SBmodified=true;
	}
	return blkno;
}


// void CreateBlocks(pplist head){
// 	for(int i)
// }

PBUFFER allocblock(pplist head,dev_t device_num,PBUFFCACHE buff_cache,PBUFFHEAD freebuffhead){
	//pthread_mutex_lock(&sobj.lockSB);
	uint32_t blkno=0;
	PBLOCK ptr_to_block=NULL;
	while(sobj.SBlocked==true){
		puts("Superblock is busyy,sleeping...\n");
		exit(EXIT_FAILURE);
		//pthread_cond_wait(&sobj.cond,&sobj.lockSB);
	}
	
	blkno=Removeblockfromlist(head);
	
	//pthread_cond_broadcast(&sobj.cond);
		//pthread_mutex_unlock(&sobj.lockSB);
		PBUFFER buff_for_block=getblk(buff_cache,freebuffhead,blkno,device_num);
		return buff_for_block;
	
	// else{
	// 	puts("Error occured while retreiving disk block\n");
	// 	exit(EXIT_FAILURE);
	// }

}


void DisplayDiskList(plist head){
	if(head==NULL){
		puts("Disk List is empty\n");
	}
	for(int i=0;i<BLOCKS_PER_LIST;i++){
		printf("Block number %d has data %s\n",i,head->blocks[i].block);

	}
	if(head->next==NULL){
		printf("Disk list next to NULL\n");
	}
}

