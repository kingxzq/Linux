#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define NUM 1024
#define SIZE 32
#define SEP " "

//保存完整的命令行字符串
char cmd_line[NUM];

//保存打散之后的命令行字符串
char *g_argv[SIZE];

//环境变量的buffer
char g_myval[64];

// shell 运行原理 ： 通过让子进程执行命令，父进程等待&&解析命令
int main()
{
    extern char**environ;//获取全局环境变量的指针
    //0. 命令行解释器，一定是一个常驻内存的进程，不退出
    while(1)
    {
        //1. 打印出提示信息 [kingxzq@localhost myshell]# 
        printf("[kingxzq@localhost myshell]# ");
        fflush(stdout);
        memset(cmd_line, '\0', sizeof cmd_line);
        //2. 获取用户的键盘输入[输入的是各种指令和选项: "ls -a -l -i"]
        if(fgets(cmd_line, sizeof cmd_line, stdin) == NULL)
        {
            continue;
        }
        if (strlen(cmd_line) <= 1) { // 如果只有回车换行符，长度为1
            continue;//输入为空重新输入
        }
        cmd_line[strlen(cmd_line)-1] = '\0';//用\0将\n替换
        //"ls -a -l -i\n\0"
        //3. 命令行字符串解析："ls -a -l -i" -> "ls" "-a" "-i"
        g_argv[0] = strtok(cmd_line, SEP); //第一次调用，要传入原始字符串
        int index = 1;
        if(strcmp(g_argv[0], "ls") == 0)
        {
            g_argv[index++] = "--color=auto";//添加自动颜色
        }
        if(strcmp(g_argv[0], "ll") == 0)//ll本身为ls -l，所以单独添加一个命令
        {
            g_argv[0] = "ls";
            g_argv[index++] = "-l";
            g_argv[index++] = "--color=auto";
        }
        while(g_argv[index++] = strtok(NULL, SEP)); //第二次，如果还要解析原始字符串,传入NULL
        if(strcmp(g_argv[0], "export") == 0 && g_argv[1] != NULL)//添加环境变量
        {
            strcpy(g_myval, g_argv[1]);
            int ret = putenv(g_myval);//输入环境变量
            if(ret == 0) printf("%s export success\n", g_argv[1]);
                continue;
        }

        //4.内置命令, 让父进程（shell）自己执行的命令，我们叫做内置命令，内建命令
        //内建命令本质其实就是shell中的一个函数调用
        if(strcmp(g_argv[0], "cd") == 0) //cd命令调用
        {
            if(g_argv[1] != NULL) chdir(g_argv[1]); //cd path, cd ..

            continue;
        }
        //5. fork()
        pid_t id = fork();
        if(id == 0) //child
        {
            printf("下面功能让子进程进行的\n");
            printf("child, MYVAL: %s\n", getenv("MYVAL"));//获取我们输入的环境变量MYVAL
            printf("child, PATH: %s\n", getenv("PATH"));//获取环境变量路径
            execvp(g_argv[0], g_argv); // ls -a -l -i
            exit(1);
        }
        //father
        int status = 0;
        pid_t ret = waitpid(id, &status, 0);
        if(ret > 0) printf("exit code: %d\n", WEXITSTATUS(status));//退出码接收
    }
    return 0;
}
