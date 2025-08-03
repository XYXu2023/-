#include<stdio.h>
#include<stdlib.h>

#define N 13//元素个数
#define SIZE 3//桶的个数
#define ADDR_SIZE 3//地址范围

typedef struct node{
    int data;
    struct node *next;
}Node;
typedef struct{
    Node *firstp;
}HashTable;
//哈希表插入
void insertHashTable(HashTable ha[],int n,int p,int k){
    int addr;
    addr=k%p;
    Node* q;
    q=(Node*)malloc(sizeof(Node));
    q->data=k;
    q->next=NULL;
    if (ha[addr].firstp==NULL)
    {
        ha[addr].firstp=q;
    }else
    {
        q->next=ha[addr].firstp;
        ha[addr].firstp=q;
    }
    n++;   
}
//哈希表创建
void creatHashTable(HashTable ha[],int m,int p,int keys[],int n1) {
    for (int i = 0; i < m; i++)
    {
        ha[i].firstp=NULL;
    }
    int n=0;
    for (int i = 0; i < n1; i++)
    {
        insertHashTable(ha,n,p,keys[i]);
    }   
}

//哈希表打印
void printHashTable(HashTable ha[], int m){
    for (int i = 0; i < m; i++) {
        printf("addr %d: ", i);
        Node* p = ha[i].firstp;
        while (p != NULL) {
            printf("%d -> ", p->data);
            p = p->next;
        }
        printf("NULL\n");
    }
}


int main(){
    int keys[N]={1,2,3,4,5,6,7,8,9,10,11,12,13};
    HashTable Ha[SIZE];

    int m=SIZE;
    int p=ADDR_SIZE;
    int n1=N;

    creatHashTable(Ha,m,p,keys,n1);
    printHashTable(Ha,m);
    return 0;
}