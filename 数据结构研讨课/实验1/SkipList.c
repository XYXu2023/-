#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

#define LEVEL 16

typedef struct SkipListNode
{
    int val;
    struct SkipListNode **forward;
}SkipListNode;

typedef struct SkipList{
    int level;
    SkipListNode* head;
}SkipList;
//创建新节点
SkipListNode* CreatNode(int val,int level){
    SkipListNode* newnode=(SkipListNode*)malloc(sizeof(SkipListNode));
    newnode->val=val;
    newnode->forward=(SkipListNode**)malloc(sizeof(SkipListNode*)*(level+1));
    return newnode;
}
//创建跳表
SkipList* CreatList(){
    SkipList* list=(SkipList*)malloc(sizeof(SkipList));
    list->level=0;
    list->head=CreatNode(INT_MIN,LEVEL);
    for (int i = 0; i < LEVEL; i++)
    {
        list->head->forward[i]=NULL;
    }
    return list;
}
//生成随机层数
int randomlevel(){
    int level=0;
    while (rand()%2==0&&level<LEVEL)
    {
        level++;
    }
    return level;
}
//插入节点
void InsertNode(SkipList* list,int val){
    SkipListNode* update[LEVEL];
    SkipListNode* current=list->head;
    for (int i = list->level; i >= 0 ; i--)
    {
        while (current->forward[i]!=NULL&&current->forward[i]->val<val)
        {
            current = current->forward[i];
        }
        update[i]=current;
        
    }
    int newlevel=randomlevel();
    if(newlevel>list->level){
        for (int i = list->level+1; i <=newlevel; i++)
        {
            update[i]=list->head;
        }
        list->level=newlevel;
    }
    SkipListNode* newnode =CreatNode(val,newlevel);
    for (int i = 0; i <=newlevel; i++)
    {
        newnode->forward[i]=update[i]->forward[i];
        update[i]->forward[i]= newnode;
    }
    
}
//查找
SkipListNode* SearchNode(SkipList* list,int val){
    SkipListNode* current = list->head;
    for (int i = list->level; i >=0; i--)
    {
        while (current->forward[i]!=NULL&&current->forward[i]->val<val)
        {
            current=current->forward[i];
        }
        
    }
    if (current->forward[0]!=NULL&& current->forward[0]->val==val)
    {
        return current->forward[0];
    }
    return NULL;
}
//释放
void freeNode(SkipListNode* node) {
    free(node->forward);
    free(node);
}
// 删除节点
void DeleteNode(SkipList* list, int val)
{
    SkipListNode* update[LEVEL];
    SkipListNode* current = list->head;
    for (int i = list->level; i >= 0; i--)
    {
        while (current->forward[i] != NULL && current->forward[i]->val < val)
        {
            current = current->forward[i];
        }
        update[i] = current; 
    }
    current = current->forward[0];
    if (current != NULL && current->val == val)
    {
        for (int i = 0; i <= list->level; i++)
        {
            if (update[i]->forward[i] != current)
                break;
            update[i]->forward[i] = current->forward[i];  
        }
        while (list->level > 0 && list->head->forward[list->level] == NULL)
        {
            list->level--;  
        }
        freeNode(current);
    }
    else
    {
        printf("Node with value %d not found.\n", val);
    }
}

// 打印跳表
void printSkipList(SkipList* list) {
    for (int i = list->level - 1; i >= 0; i--) {
        SkipListNode* current = list->head->forward[i];
        printf("Level %d: ", i);
        while (current != NULL) {
            printf("%d -> ", current->val);
            current = current->forward[i];
        }
        printf("NULL\n");
    }
}

 
// 释放跳表
void freeSkipList(SkipList* list) {
    SkipListNode* current = list->head->forward[0];
    SkipListNode* next;
 
    while (current != NULL) {
        next = current->forward[0];
        freeNode(current);
        current = next;
    }
 
    free(list->head->forward);
    free(list->head);
    free(list);
}

//测量时间
void measuretime(int node,int insertcount){
    printf("Node:%d\n",node);
    SkipList* list = CreatList();
    
    for (int i = 1; i <= node; i++)
    {
        InsertNode(list, i);
    }
    
    clock_t start = clock();
    for (int i = 1; i < 4000; i++)
    {
        DeleteNode(list,i);
    }
    clock_t end = clock();
    printf("Skip List deletion time: %.6f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    for (int i = 1; i <= insertcount; i++)
    {
        InsertNode(list, i);
    }
    end = clock();
    printf("Skip List insertion time: %.6f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    SkipListNode* foundNode = SearchNode(list, 5000);
    printf("Found node with value: %d\n", foundNode->val);
    freeSkipList(list);
}

int main(){
    srand(time(0));
    measuretime(20000,1000);
    measuretime(15000,8000);
    measuretime(12000,6000);
    measuretime(10000,5000);
    measuretime(8000,4000);
    measuretime(5000,3000);
    return 0;

}
