#include<stdio.h>
#include<stdlib.h>

#define MAX_SIZE 100

typedef struct{
    int data[MAX_SIZE];
    int size;
}CompleteBinaryTree;

typedef struct TreeNode{
    int data;
    struct TreeNode* left;
    struct TreeNode* right;
}TreeNode;

typedef struct{
    TreeNode** nodes;
    int front;
    int rear;
    int capacity;
}Queue;

//顺序存储
void initTree(CompleteBinaryTree* tree) {
    tree->size = 0;
 }
 
void insertNode(CompleteBinaryTree* tree, int value) {
    if (tree->size >= MAX_SIZE) {
        printf("Error: Tree is full!\n");
        return;
    }
    tree->data[tree->size] = value;
    tree->size++;
 }

void levelOrderTraversal(CompleteBinaryTree*tree){
    if ((tree->size==0))
    {
        printf("Empty\n");
        return;
    }
    printf("Orde:\n");
    for (int i = 0; i < tree->size; i++)
    {
        printf("%d ",tree->data[i]);
    }
    printf("\n");
}
//链式存储
int isEmpty(Queue*queue){
    if (queue->front==queue->rear)
    {
        return 1;
    }else
    {
        return 0;
    }
    
    
}

void initQueue(Queue* queue, int capacity) {
    queue->nodes = (TreeNode**)malloc(sizeof(TreeNode*) * capacity);
    queue->front = queue->rear = 0;
    queue->capacity = capacity;
}

void enqueue(Queue* queue, TreeNode* node) {
    if (queue->rear == queue->capacity) {
        printf("Queue is full!\n");
        return;
    }
    queue->nodes[queue->rear++] = node;
}

TreeNode* dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        printf("Queue is empty!\n");
        return NULL;
    }
    return queue->nodes[queue->front++];
}

TreeNode*creatNode(int value){
    TreeNode*newnode=(TreeNode*)malloc(sizeof(TreeNode));
    newnode->data=value;
    newnode->left=newnode->right=NULL;
    return newnode;
}
void Insert(TreeNode**root,int value,Queue*queue){
    TreeNode*newnode=creatNode(value);
    if (*root==NULL)
    {
        *root=newnode;
        return;
    }
    enqueue(queue, *root);
    while (!isEmpty(queue))
    {
         TreeNode* current = dequeue(queue);
        if (current->left == NULL) {
        current->left = newnode;
        break;
    }
 
        if (current->right == NULL) {
        current->right = newnode;
        break;
        }
    enqueue(queue, current->right);
    }  
}
void levelOrderTraversalWithQueue(TreeNode* root) {
    if (root == NULL) {
        printf("Empty\n");
        return;
    }
    Queue queue;
    initQueue(&queue, MAX_SIZE);
    enqueue(&queue, root);
    printf("Order:\n");

    while (!isEmpty(&queue)) {
        TreeNode* current = dequeue(&queue);
        printf("%d ", current->data);
        if (current->left != NULL) {
            enqueue(&queue, current->left);
        }
        if (current->right != NULL) {
            enqueue(&queue, current->right);
        }
    }

    printf("\n");
}

int main() {
    CompleteBinaryTree tree;
    initTree(&tree);
    for (int i = 0; i < 9; i++)
    {
        insertNode(&tree, i);
    } 
    levelOrderTraversal(&tree);

    TreeNode* root = NULL;
    Queue queue;
    initQueue(&queue, MAX_SIZE);
    for (int i = 0; i < 9; i++)
    {
        Insert(&root, i, &queue);
    }
    levelOrderTraversalWithQueue(root);

    return 0;
}