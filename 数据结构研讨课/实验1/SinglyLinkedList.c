#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

typedef struct ListNode{
    int val;
    struct ListNode*next;
}ListNode;

ListNode* CreatNode(int val){
    ListNode* Node=(ListNode*)malloc(sizeof(*Node));
    Node->val=val;
    Node->next=NULL;
    return Node;
}
ListNode* CreatList(int size){
    ListNode* head =CreatNode(1);
    ListNode* ptr =head;
    for (int i = 2; i <=size; i++)
    {
        ListNode* NewNode=(ListNode*)malloc(sizeof(*NewNode));
        ptr->next=NewNode;
        NewNode->val= i;
        NewNode->next=NULL;
        ptr=NewNode;
    }
    return head;
}

ListNode* InsertList(ListNode*head,int position,int data){
    ListNode* ptr=head;
    ListNode*NewNode=(ListNode*)malloc(sizeof(ListNode));
    if (position==0)
    {   
        NewNode->val=data;
        NewNode->next=head;
        return NewNode;
    }else{   
    for (int i = 0; i < position-1 && ptr!=NULL; i++)
    {
        ptr=ptr->next;
    }
    if (ptr == NULL) return head;
    NewNode->val=data;
    NewNode->next= ptr->next;
    ptr->next=NewNode;
    }
    return head;
}

ListNode* DeleteList(ListNode* head,int position){
    if (head == NULL) {
    printf("Error: List is empty!\n");
    return NULL;
    }
    if (position==0)
    {
        ListNode* temp=head;
        head=head->next;
        free(temp);
        return head;
    }
    
    ListNode* ptr =head;
    for (int i = 0; i < position-1 && ptr!=NULL; i++)
    {
        ptr=ptr->next;
    }
    if (ptr == NULL || ptr->next == NULL) return head;
    ListNode* temp=ptr->next;
    ptr->next=ptr->next->next;
    free(temp);  
    return head;
}

ListNode* FindList(ListNode* head,int data){
    ListNode* ptr=head;
    if (ptr == NULL) {  
    printf("NOT FIND");
    return NULL;
    }
    while (ptr!=NULL&&ptr->val!=data)
    {
        ptr=ptr->next;
    }
    if (ptr==NULL)
    {
        printf("NOT FIND");
        return NULL;
    }else{
        return ptr;
    }  
}

ListNode* DestroyList(ListNode* head){
    ListNode* ptr=head;
    while (ptr!=NULL)
    {
        ListNode*temp=ptr;
        ptr=ptr->next;
        free(temp);
    }
    return NULL;
}

void measuretime(int node,int insertcount){
    printf("Node:%d\n",node);
    ListNode*list= CreatList(node);

    clock_t start = clock();
    for (int i = 1; i < insertcount; i++)
    {
        list=DeleteList(list,i);  
    }
    clock_t end = clock();
    printf("Single Linked List deletion time: %.6f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);

    start = clock();
    for (int i = 0; i < insertcount; i++) {
        list = InsertList(list, i, 100 + i);  
    }
    end = clock();
    printf("Single Linked List insertion time for %d nodes: %.6f seconds\n",insertcount,(double)(end - start) / CLOCKS_PER_SEC);

    list=DestroyList(list);

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