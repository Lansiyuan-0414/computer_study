#include <stdio.h>
#include <stdlib.h>
typedef struct TreeNode {
    int data;                       // 节点存储的数据
    struct TreeNode *left;          // 左子树指针
    struct TreeNode *right;         // 右子树指针
} TreeNode;

TreeNode *create_node (int value){
    TreeNode *p = (void*)malloc(sizeof(TreeNode)) ;
    if(p == NULL)
    return NULL;
    p->data = value;
    p->left = NULL;
    p->right = NULL;    
    return p;
} 

void preoder(TreeNode *T){
    if (T == NULL)
    return;
    printf("%d ",T->data);
    preoder(T->left);
    preoder(T->right);
}

void inoder(TreeNode *T){
    if (T == NULL)
    return;
    preoder(T->left);
    printf("%d ",T->data);
    preoder(T->right);
}

void postoder(TreeNode *T){
    if (T == NULL)
    return;
    preoder(T->left);
    preoder(T->right);
    printf("%d ",T->data);
}

int depth(TreeNode *root, int current_depth, int max_depth){//统计树的深度
    if(root == NULL)
    return max_depth;
    if(max_depth < current_depth)
    max_depth = current_depth;
    max_depth = depth(root->left,current_depth + 1,max_depth);
    max_depth = depth(root->right,current_depth + 1,max_depth);
    return max_depth;
}

typedef struct Stack {//创建了一个“栈”结构体
    TreeNode **arr;//指针变量arr指向TreeNode *结构体
    int top;//栈顶位置
    int capacity;//当前栈最多存放多少TreeNode
} Stack;
Stack *createStack(int capacity) {//创建了一个栈
    Stack *stack = malloc(sizeof(Stack));// // 在堆区申请一块能够存放一个 Stack 结构体的空间，stack保存这个内存空间的首地址
    stack->arr = malloc(sizeof(TreeNode *) * capacity); // 在堆区申请一个长度为 capacity 的 TreeNode * 数组，用于存放树结点的地址
    stack->top = -1;
    stack->capacity = capacity;//记录栈的最大容量
    return stack;
}
int isEmpty(Stack *stack) {
    return stack->top == -1;//判断top是否为 -1，若是返回1，否则返回0
}
void push(Stack *stack, TreeNode *node) {
    if (stack->top == stack->capacity - 1) {//判断栈是否满了
        return;
    }
    stack->arr[++stack->top] = node;//先让top加 1，再把node的地址存入新的栈顶位置
}
TreeNode *pop(Stack *stack) {//判断栈是否为空，若为空，返回空指针，否者返回数组栈顶位置保存的树节点指针
    if (isEmpty(stack)) {
        return NULL;
    }
    return stack->arr[stack->top--];
}

void preorderTraversal(TreeNode *root) {
    if(root == NULL)
    return;
    Stack *s = createStack(100);
    push (s,root);
    while(!isEmpty(s)){
    TreeNode *node = pop(s);
    printf("%d ",node->data);
    if(node->right != NULL)
    push(s,node->right);
    if(node->left != NULL)
    push(s,node->left);
    }
}

int main(void){
    TreeNode *root = create_node(0);
    TreeNode *node1 = create_node(1);
    TreeNode *node2 = create_node(2);
    TreeNode *node3 = create_node(3);
    TreeNode *node4 = create_node(4);
    TreeNode *node5 = create_node(5);
    TreeNode *node6 = create_node(6);
    root->left = node1;
    root->right = node2;
    node1->left = node3;
    node1->right = node4;
    node2->left = node5;
    node2->right = node6;
    preoder(root);
    printf("\n");
    inoder(root);
    printf("\n");
    postoder(root);
    printf("\n");
    int treedepth = depth(root,1,0);
    printf("%d",treedepth);
    printf("\n");
    preorderTraversal(root);
    return 0;
}