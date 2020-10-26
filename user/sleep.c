#include "kernel/types.h"
#include "user/user.h"
int main(int argc, char *argv[])
{
    if(argc != 2){
        // 未传入sleep tick数时提示
        fprintf(2, "参数输入错误：请传入一个休眠时间\n");
        exit();
    }
    fprintf(2, "休眠开始\n");
    sleep(atoi(argv[1]));
    fprintf(2, "休眠结束\n");
    exit();
}