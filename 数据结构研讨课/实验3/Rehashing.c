#include<stdio.h>
#include<stdlib.h>

#define N 13 // 元素个数
#define SIZE 17 // 哈希表大小
#define EMPTY -1 // 空位置标记

typedef struct {
    int data;
    int isUsed; // 标记是否已使用
} HashEntry;
//初始化哈希表
void InitialHash(HashEntry ha[],int m){
    for (int i = 0; i < m; i++)
    {
        ha[i].data=EMPTY;
        ha[i].isUsed=0;
    }
    
}
//第一哈希函数
int hash1(int key,int p){
    return key%p;
}
//第二哈希函数
int hash2(int key,int i){
    return i*(key%7+1);
}
//哈希表插入（再哈希检测法）
void InsertHash(HashEntry ha[],int key,int p){
    int addr=hash1(key,p);
    int startaddr=addr;
    int i=1;
    while (ha[addr].isUsed!=0)
    {
        addr=hash2(key,i);
        if (addr==startaddr)
        {
            printf("FULL!");
            return;
        }
        i++;
    }
    ha[addr].data=key;
    ha[addr].isUsed=1;
}    
//哈希表打印
void PrintHash(HashEntry ha[], int m) {
    for (int i = 0; i < m; i++) {
        printf("addr %d: ", i);
        if (ha[i].isUsed) {
            printf("%d", ha[i].data);
        } else {
            printf("EMPTY");
        }
        printf("\n");
    }
}

int main(){
    int keys[N]={11,25,73,4,52,10,37,86,15,47,13,62,93};
    HashEntry Ha[SIZE];
    int m=SIZE;
    int p=SIZE;
    InitialHash(Ha, m);
    
    for (int i = 0; i < N; i++) {
        InsertHash(Ha,keys[i],p);
    }
    
    PrintHash(Ha, m);
    return 0;
}