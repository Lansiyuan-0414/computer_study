# part1_了解C语言配置文件
## 问题回答
1. gcc是一种编译器，将 .c 文件编译为可执行文件.exe。而MinGW是把 GCC、链接器、Windows 头文件和运行库等组合起来的一个工具链，辅助我们编译出一个Windows原生程序
2. 1. c_cpp_properties.json 主要给 C/C++ 扩展的 IntelliSense 用，比如配置编译器路径、头文件搜索路径、宏定义、C/C++ 标准版本等。它主要影响代码补全、跳转、红色波浪线这些编辑器功能，不直接负责真正编译。告诉VS Code代码怎么理解
   2. tasks.json 主要用来定义任务，最常见就是编译。即告诉VS Code代码怎么编译
   3. launch.json 负责怎么启动并调试已经编译好的程序，即告诉VS Code程序怎么调试这段代码
3. 即使不装插件，我们也可以在写完一段C代码后编译，但是在写代码时不会有高亮，颜色等提示，让我们对写代码更加困难，所以C插件是负责辅助让编辑器更懂C，从而提供补全、检查、跳转和调试等开发辅助功能。
## launch.json的文件注释补全
    {
    // 使⽤ IntelliSense 了解相关属性。
    // 悬停以查看现有属性的描述。
    // 欲了解更多信息，请访问: https://go.microsoft.com/fwlink/?linkid=830387     "version": "0.2.0",
    "configurations": [
        {
            "name": "gcc.exe - ⽣成和调试活动⽂件",  // 该调试任务的名字，启动调试时会在待选列表中显⽰
            "type": "cppdbg", //使用 VS Code C/C++ 扩展提供的调试功能
            "request": "launch", //启动一个新的程序并调试它。
            "program": "${fileDirname}\\${fileBasenameNoExtension}.exe", //指向需要调试的可执行文件           "args": [],//运行程序时传给 main 的命令行参数
            "stopAtEntry": false,  //启动后是否将程序停留在刚开始时
            "cwd": "${workspaceFolder}",//当前工作目录
            "environment": [],//给程序设计环境变量
            "externalConsole": false,  //是否启用外部终端
            "MIMode": "gdb",//使用gbd作为调试器
            "miDebuggerPath": "C:\\mingw64\\bin\\gdb.exe",  //gbd调试器的路径
            "setupCommands": [
                {
                    "description": "为 gdb 启⽤整⻬打印",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ],
            "preLaunchTask": "C/C++: gcc.exe build active file"  // 调试前的预执⾏任务，这⾥的值是tasks.json⽂件中对应的编译任务，也就是调试前需要先编译
        }
    ]
}
### 内置终端（图片开着vpn会显示不出来）
[![pn3XtBt.png](https://s41.ax1x.com/2026/09/27/pn3XtBt.png)](https://imgchr.com/i/pn3XtBt)
### 外置终端
[![pn3XGjA.png](https://s41.ax1x.com/2026/09/27/pn3XGjA.png)](https://imgchr.com/i/pn3XGjA)
# part2_C语言基础
## 问题回答
1. 变量类型是这个变量里准备存什么样的数据，以及编译器应该怎么理解和处理这块内存。存放年龄选择`int`类型，apple不能用一个`char`类型实现，应该用字符数组实现储存
2. 从0开始，访问了不属于该数组的内存，可能会输出一些奇怪的数字，使其他变量数据错误甚至程序崩溃
3. 循环通过反复检查条件，在条件成立时重复执行代码块，条件不成立时停止来反复执行，基本结构为`for(初始化，条件，更新){循环内容}`，`while（条件）{循环内容}`，`do{循环内容}while(条件)`;
   `for`循环的初始化是在循环开始前执行一次，用来设置循环变量的初始值。条件判断是在每次进入循环前都会判断。条件为真就继续执行循环体，为假就结束循环。更新是在每执行完一次循环体后执行一次，用来更新循环变量。`while`循环第一轮不满足条件直接不执行，但`dowhile`循环必定执行一轮，不满足条件则不执行第二轮
4. 逻辑表达式是在判断一个事件的成立与否，运算结果只有`0`和`1`，分别表示为假和为真，算数表达式就是进行数学运算，运算结果由进行运算的值决定。
## C代码
```c
# include <stdio.h>
int main(void)
{
   int i = 0;
   char a[20];char c;
   int  b;
   do{
   printf("请输入名字和年龄：");
   scanf("%s %d",a,&b);
   printf("你的姓名是%s,年龄是%d\n",a,b);
   printf("是否继续输入？(y或n)");
   scanf(" %c",&c);
   if (c=='y')
   i++;
   }while(c == 'y');
   printf("打印了%d次\n",i);
}
```
# 函数
## C代码
```c
# include<stdio.h>
int x1,x2,x3;
int y1,y2,y3;
int z1,z2,z3;

int square(int a)
{
  int b = a * a;
  return b;
}
int caculation(int a,int b,int c)
{
  int p = (a + b + c) / 3;
  int f = (square(p-a) + square(p-b) + square(p-c)) / 3;
  int zh = 3 * p - f / 3;
  return zh;
}
void print_rank(int zh1, int zh2, int zh3)
{
   if (zh1 >= zh2 && zh2 >= zh3) {
      printf("小明 > 小强 > 小林");
  } else if (zh1 >= zh3 && zh3 >= zh2) {
      printf("小明 > 小林 > 小强");
  } else if (zh2 >= zh1 && zh1 >= zh3) {
      printf("小强 > 小明 > 小林");
  } else if (zh2 >= zh3 && zh3 >= zh1) {
      printf("小强 > 小林 > 小明");
  } else if (zh3 >= zh1 && zh1 >= zh2) {
      printf("小林 > 小明 > 小强");
  } else { // zh3 >= zh2 && zh2 >= zh1
      printf("小林 > 小强 > 小明");
  }
}
int main()
{
  printf("请输入小明的三项成绩（顺序为A B C,以一个空格为间隔）：");
  scanf("%d %d %d", &x1, &x2, &x3);
  printf("请输入小强的三项成绩（顺序为A B C,以一个空格为间隔）：");
  scanf("%d %d %d", &y1, &y2, &y3);
  printf("请输入小林的三项成绩（顺序为A B C,以一个空格为间隔）：");
  scanf("%d %d %d", &z1, &z2, &z3);
  int zh1 = caculation(x1,x2,x3);
  int zh2 = caculation(y1,y2,y3);
  int zh3 = caculation(z1,z2,z3);
  print_rank(zh1,zh2,zh3);
  return 0;
}
```
对最后这个交换代码，是实现不了交换效果的，因为swap函数只是把main中a,b的值赋值进来，并在swap内交换，对于main中的a，b没有影响