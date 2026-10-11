#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_TREE_SIZE 100
/*
 * 顺序存储二叉树结点
 * - data：结点数据
 * - used：当前位置是否有结点
 */
typedef struct {
    int data;
    bool used;
} SeqTreeNode;

/*
 * 顺序存储二叉树
 * - nodes：结点数组
 * - size：数组最大容量
 */
typedef struct {
    SeqTreeNode nodes[MAX_TREE_SIZE];
    int size;
} SeqBiTree;

void init_tree(SeqBiTree *tree){//初始化二叉树
    tree->size = MAX_TREE_SIZE;
    for (int i = 0; i < MAX_TREE_SIZE; i++) {
        tree->nodes[i].data = 0;
        tree->nodes[i].used = false;
    }
}

bool set_root(SeqBiTree *T, int value){//创建根节点
    T->nodes[1].data = value;
    T->nodes[1].used = true;
    return true;
}

bool set_left_child(SeqBiTree *T, int parent_node, int value){//写入左孩子
    if(parent_node > T->size - 1 || parent_node <= 0)// 父结点下标是否合法
    return false;
    else if(T->nodes[parent_node].used == false)//父结点是否真的存在
    return false;
    else if(2*parent_node <= T->size - 1){//左孩子下标是否越界
    T->nodes[2*parent_node].data = value;
    T->nodes[2*parent_node].used = true;
    return true;
    }
    else
    return false;
}

bool set_right_child(SeqBiTree *T, int parent_node, int value){//写入右孩子
    if(parent_node > T->size - 1 || parent_node <= 0)
    return false;
    else if(T->nodes[parent_node].used == false)
    return false;
    else if(2*parent_node + 1 <= T->size - 1){
    T->nodes[2*parent_node + 1].data = value;
    T->nodes[2*parent_node + 1].used = true;
    return true;
    }
    else
    return false;
}

void level_order(SeqBiTree *T){//层序遍历
    int n = 1;
    for(int i = 1; i <= T->size - 1; i++){
    if(T->nodes[i].used ==false)
    printf("-1 ");
    else 
    printf("%d ",T->nodes[i].data);
    if(i == (1<<n) - 1){
    printf("\n");
    n++;
    }
    }
}

int main (void){
    SeqBiTree Tree ;
    init_tree(&Tree);
    set_root(&Tree,0);
    set_left_child(&Tree,1,1);
    set_right_child(&Tree,1,2);
    set_left_child(&Tree,2,3);
    set_right_child(&Tree,2,4);
    set_left_child(&Tree,3,5);
    set_right_child(&Tree,3,6);
    level_order(&Tree);
    return 0;
}

