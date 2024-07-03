#include <stdio.h>
#include <stdlib.h>

// 定义二叉树节点结构
typedef struct Node
{
    int data;
    struct Node *left;
    struct Node *right;
} Node;

// 创建新节点
Node *createNode(int data)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = data;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// 定义队列节点结构
typedef struct QueueNode
{
    Node *treeNode;
    struct QueueNode *next;
} QueueNode;

// 定义队列结构
typedef struct Queue
{
    QueueNode *front;
    QueueNode *rear;
} Queue;

// 创建空队列
Queue *createQueue()
{
    Queue *q = (Queue *)malloc(sizeof(Queue));
    q->front = NULL;
    q->rear = NULL;
    return q;
}

// 入队操作
void enqueue(Queue *q, Node *treeNode)
{
    QueueNode *newQueueNode = (QueueNode *)malloc(sizeof(QueueNode));
    newQueueNode->treeNode = treeNode;
    newQueueNode->next = NULL;
    if (q->rear == NULL)
    {
        q->front = newQueueNode;
        q->rear = newQueueNode;
    }
    else
    {
        q->rear->next = newQueueNode;
        q->rear = newQueueNode;
    }
}

// 出队操作
Node *dequeue(Queue *q)
{
    if (q->front == NULL)
    {
        return NULL;
    }
    QueueNode *temp = q->front;
    Node *treeNode = temp->treeNode;
    q->front = q->front->next;
    if (q->front == NULL)
    {
        q->rear = NULL;
    }
    free(temp);
    return treeNode;
}

// 检查队列是否为空
int isQueueEmpty(Queue *q)
{
    return q->front == NULL;
}

// BFS 遍历二叉树
void bfs(Node *root)
{
    if (root == NULL)
    {
        return;
    }
    Queue *q = createQueue();
    enqueue(q, root);
    while (!isQueueEmpty(q))
    {
        Node *current = dequeue(q);
        printf("%d ", current->data);
        if (current->left != NULL)
        {
            enqueue(q, current->left);
        }
        if (current->right != NULL)
        {
            enqueue(q, current->right);
        }
    }
    // 释放队列
    free(q);
}

int main()
{
    // 创建二叉树
    Node *root = createNode(1);
    root->left = createNode(2);
    root->right = createNode(3);
    root->left->left = createNode(4);
    root->left->right = createNode(5);
    root->right->left = createNode(6);
    root->right->right = createNode(7);

    // 执行 BFS 遍历
    printf("BFS Traversal: ");
    bfs(root);

    return 0;
}
