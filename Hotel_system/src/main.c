/*
    宾馆预约系统
    1. 读取输入（文件）
    2. 初始化宾馆房间信息
    3. 解析请求，对于每个客人，创建一个子进程去执行其所有请求
*/

#include <stdio.h>
#include <string.h>
#include "systemInit.h"


/**
 * @brief 根据输入的filepath，初始化宾馆信息，处理用户请求
 * 
 * @param filepath 输入文件的路径，如果为NULL则是默认路径 "./input.txt"
 */
void systemRun(const char *filepath) {
    systemStart(NULL);
}

int main() {
    systemRun(NULL);
    return 0;
}