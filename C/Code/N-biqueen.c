# include <stdio.h>
# include <stdbool.h>
#include "console_utf8.h"
#define MAX_N 15
int N;
unsigned limits;
int sum;

void queen (unsigned cols, unsigned ld,unsigned rd ){
    unsigned available =limits & ~(cols | ld | rd);
    while(available){
        unsigned bit = available & (~available + 1);
        queen(bit | cols,((bit | ld) << 1), ((bit | rd) >> 1));
        available &= available - 1;
    }
    if(cols == limits){
        sum ++;
    }
}

int main(void)
{   
    init_console_utf8();
    printf("请输入整数n(0 < N < 16):");
    scanf("%d", &N);
    if (N <= 0 || N > MAX_N) {
    printf("输入错误\n");
    return 1;
    }
    limits = (1 << N) - 1; 
    queen(0,0,0);
    printf("%d",sum); 
    return 0;
}