#include "kernel/types.h"
#include "user/user.h"
void close_pipe(int *p){
    close(p[0]);
    close(p[1]);
}
void primes(){
    int n, p, len;
    int fd[2];
    
    if((len = read(0 , &n, sizeof(int))) <= 0 || n <= 0){
        // 读取完毕
        close(1);
        exit();
    }
    // 把第一个数输出
    printf("prime %d\n", n);
    
    pipe(fd);
    if(fork() == 0){
        close(0);
        dup(fd[0]);
        close_pipe(fd);
        primes();
    }else{
        close(1);
        dup(fd[1]);
        close_pipe(fd);
        while((len = read(0, &p, sizeof(int))) > 0 && p > 0){
            if(p % n != 0){
                write(1, &p, sizeof(int));
            }
        }
        if(len <= 0 || p <= 0){
            close(1);
            exit();
        }
    }
}

int main(void)
{
    int i;
    int fd[2];

    pipe(fd);
    if(fork() == 0){
        // 子进程，进入递归
        close(0);
        dup(fd[0]);
        close_pipe(fd);
        primes();
    }else{
        // 父进程
        close(1);
        dup(fd[1]);
        close_pipe(fd);
        for(i = 2;i <= 35;i++){
            // 将2-35的数写入管道
            write(1, &i, sizeof(int));
        }
        close(1);
        wait();
    }
    exit();
}