#include<stdio.h>
#include<stdlib.h>
struct Node{
    int data;
    struct Node *next;
};

typedef struct Node* PIINODE;

struct HashqueueHeader{
    PIINODE First;
    PIINODE Last;
    //int (*hashfunction)(dev_t device_num,int inode_num);
};

struct Hashqueue{
    struct HashqueueHeader headers[4];
    int size; 
};


void testing(struct Hashqueue* ptr){
    printf("\nHeader address %d\n",ptr->headers);
    for(int iCnt=0;iCnt<4;iCnt++)
    {
        printf("%d\n",ptr->headers+iCnt);
    }
}

int main(){
    
    struct Hashqueue obj;
    
    testing(&obj);
    return 0;
}