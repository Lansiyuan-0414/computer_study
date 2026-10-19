#include <stdio.h>
#include "console_utf8.h"

int hasCommonChar(const char *s1, const char *s2) {
    int mask1 = 0;
    int mask2 = 0;
    for(int i = 0 ; s1[i] != '\0' ; i++){
        int m = s1[i] - 'a';
        if(m >= 0 && m<=25)
        mask1 |= 1 << m;
    }
    for(int i = 0 ; s2[i] != '\0' ; i++){
        int m = s2[i] - 'a';
        if(m >= 0 && m<=25)
        mask2 |= 1 << m;
    }
    return !!(mask1&mask2);
}
int main(void){
     init_console_utf8();//防止乱码
    char s1[20];
    char s2[20];
    char *p1=s1;
    char *p2=s2;
    scanf("%s %s",s1,s2);
    int a = hasCommonChar(p1,p2);
    if(a == 0)
    printf("没有相同字符\n");
    else if(a == 1)
    printf("有相同字符\n");
    return 0;
}