# include <stdio.h>
# include <stdlib.h>
#include "console_utf8.h"
typedef struct Node{
    int data;
    struct Node *next;
}Node;

Node *initlist (void){//初始化链表
    Node *head = (Node*)malloc(sizeof(Node));
    if (head != NULL){
        head->data = 0;
        head->next = NULL;
        return head;
    }
}
Node *inserthead(Node* L,int e){//头插
    Node *p =(Node*)malloc(sizeof(Node));
    if (p != NULL){
        p->data = e;
        p->next =L->next;
        L->next = p;
    }
    return p;

}
void *traverselist(Node *L){//遍历
    Node *p = L->next; 
    while (p != NULL)
    {
        printf("%d ",p->data);
        p = p->next;
    }
    printf("\n");
}
Node* inserttail(Node *L,int e){//尾插
    Node *p = (Node*)malloc(sizeof(Node));
    if(p != NULL){
        p->data = e;
        Node *q = L;
        while (q->next != NULL){
            q = q->next;
        }
        p->next = NULL;
        q->next = p;

    }
    return p;
}

int deleteNode (Node *L,int pos){//删除
    int i = 0;
    Node *p = L;
    if (pos < 0){
    printf("输入的位置错误\n");
    return 0;}
    while(i < pos-1){
    p = p->next;
    i ++;
    if (p == NULL)
    return 0;
    }
    if (p->next == NULL){
    printf("输入的位置错误\n");
    return 0;
    }

    else if (p->next != NULL){
    Node *q = p->next;
    p->next = q->next;
    free(q);
    return 1;
    }


}

int findNode(Node *L,int n){//查找
    Node *p = L;
    int len = 0;
    while(p->next != NULL){
        p = p->next;
        len ++;
        if (p->data == n){
        printf("%d是第%d个数\n",n,len);
        return 1;
        }
        
    }
    printf("链表中未查询到这个数据\n");
    return 0;
}

Node* updateNode(Node *L,int pos,int e){//更改
    Node *p = L;
    if (pos <= 0)
        return NULL;
    for(int i = 0 ; i < pos ; i++){
        p = p->next;
         if (p == NULL){
            printf("输入的位置错误\n");
            return NULL;
         }
    }
    p->data = e;
    return p;
}

int reverselist(Node *L){//反转
    Node *p = L->next;
    if (p == NULL){
    printf("该链表为空");
    return 0;
    }
    Node *q = p->next;
    Node *tmp = NULL;
    while(q != NULL){
        tmp = q->next;
        q->next = p;
        p = q;
        q = tmp;
    }
    Node *tail = L->next;
    L->next = p;
    tail->next =NULL;
    return 1;
}

int main(void){
    init_console_utf8();
    Node *list = initlist();
    inserthead (list,10);
    inserthead (list,18);
    inserthead (list,30);
    traverselist(list);
    inserttail (list,70);
    inserttail(list,90);
    inserttail(list,168);
    traverselist(list);
    deleteNode(list,3);
    traverselist(list);
    findNode(list,90);
    updateNode(list,3,198);
    traverselist(list);
    reverselist(list);
    traverselist(list);
    return 0;
    
    
}