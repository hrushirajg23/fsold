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


plist init_disklink(plist element){
	memset(element->blocks,0,sizeof(element->blocks));
	element->next=NULL;
	return element;
}

//Link a whole list of 96 disk blocks at last
void Linknext(pplist head){
	
	plist newn=(plist)malloc(sizeof(plist));
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
}

PBLOCK Removeblockfromlist(pplist head){
	plist temp_to_list=NULL;
	PBLOCK ptr_to_block=NULL;
	sobj.diskhead=head;
	ptr_to_block=(*head)->blocks+sobj.next_free_block_index; 
	sobj.next_free_block_index--;
	if(sobj.next_free_block_index< 0){
		temp_to_list=(*head);
		if(temp_to_list->next==NULL){
			Linknext(head);
		}		
		temp_to_list=temp_to_list->next;
		sobj.diskhead=&temp_to_list;
	}
}




//int main(){
//	BLOCK bl;
//	plist disk_list=NULL;

//	printf("%d\n",BLOCK_SIZE);
//	printf("%d\n",sizeof(BLOCK));
//	printf("%d\n",sizeof(bl.block[0]));
//	return 0;
//}
