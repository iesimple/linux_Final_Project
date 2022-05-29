/*
    宾馆预约系统
    1. 读取输入（文件）
    2. 初始化宾馆房间信息
    3. 解析请求，对于每个客人，创建一个子进程去执行其所有请求
*/

#include <stdio.h>
#include <string.h>
#include "common_types.h"
#include "systemInit.h"

void systemStart()
{
    // 读取文件输入
    fileReader();
    // 初始化宾馆房间信息
    room_init();
    // 请求处理开始
    request_process();
    /*测试用*/
    print_file();
}

int main()
{
    systemStart();
    return 0;
}