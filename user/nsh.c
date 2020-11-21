#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void execPipe(char* argv[], int argc);

#define MAXARGS 10
#define MAXWORD 30
#define MAXLINE 100

int getcmd(char *buf, int nbuf)
{
    /*读取命令，用@作为命令输入提示符*/
    fprintf(2, "@ ");
    memset(buf, 0, nbuf);
    gets(buf, nbuf);
    if(buf[0] == 0)
    //Error
        return -1;
    return 0;
}
char whitespace[] = " \t\r\n\v";
char args[MAXARGS][MAXWORD];
void setargs(char *cmd, char* argv[], int* argc)
{
    /*提取命令中的单词*/
    //让argv的每一个元素都指向args的每一行
    for(int i = 0; i < MAXARGS; i++){
        argv[i] = &args[i][0];
    }
    int i = 0;//表示第i个歌词
    int j = 0;
    for(; cmd[j] != '\n' && cmd[j] != '\0'; j++)
    {
        //跳过之前的空格
        while(strchr(whitespace, cmd[j])){
            j++;
        }
        //指向找到的第i个单词
        argv[i++] = cmd + j;
        //只要不为空格就跳过，直到找到下一个空格
        while(strchr(whitespace, cmd[j]) == 0){
            j++;
        }
        //把单词后面紧接的空格设为'\0'
        cmd[j] = '\0';
    }
    argv[i] = 0;
    *argc = i;
}
void runcmd(char* argv[], int argc)
{
    /*执行命令*/
    for(int i = 1; i < argc; i++){
        if(!strcmp(argv[i], "|")){
            //如果遇到|是pipe，说明后面至少还有一个命令要执行
            execPipe(argv, argc);
        }
    }
    //此时仅处理一个命令；
    //此处判断argv[1]开始，后面有没有>
    for(int i = 1; i < argc; i++){
        //输出重定向
        if(!strcmp(argv[i], ">")){
            //关闭stdout
            close(1);
            //将输出重定向到后面给出的文件名对应的文件中
            open(argv[i+1], O_CREATE|O_WRONLY);
            argv[i] = 0;
        }
        //输入重定向
        if(!strcmp(argv[i], "<")){
            //关闭stdin
            close(0);
            open(argv[i+1], O_RDONLY);
            argv[i] = 0;
        }
    }
    exec(argv[0], argv);
}

void execPipe(char* argv[], int argc)
{
    int i = 0;
    for(; i < argc; i++){
        if(!strcmp(argv[i], "|")){
            argv[i] = 0;
            break;
        }
    }
    int fd[2];
    pipe(fd);
    if(fork() == 0){
        //子进程，执行左边的命令
        //关闭输出
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        runcmd(argv, i);
    }else{
        //父进程，执行右边的命令
        //关闭输入
        close(0);
        dup(fd[0]);
        close(fd[0]);
        close(fd[1]);
        runcmd(argv+i+1, argc - i - 1);
    }
}
int main()
{
    char buf[MAXLINE];
    while(getcmd(buf, sizeof(buf)) >= 0)
    {
        if(fork() == 0)
        {
            char* argv[MAXARGS];
            int argc = -1;
            setargs(buf, argv, &argc);
            runcmd(argv,argc);
        }
        wait(0);
    }
    exit(0);
}