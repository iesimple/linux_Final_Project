/*
    宾馆预约系统
    1. 读取输入（文件）
    2. 初始化宾馆房间信息
    3. 解析请求，对于每个客人，创建一个子进程去执行其所有请求
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "systemInit.h"

#define MAX_LEN_PATH 30
/**
 * @brief 根据输入的filepath，初始化宾馆信息，处理用户请求
 *
 * @param filepath 输入文件的路径，如果为NULL则是默认路径 "../resource/in.txt"
 */
void systemRun(const char *filepath) {
    systemStart(filepath);
}

int main(int argc, char **argv) {
    // 解析命令行参数，opstring是一个字符串
    // 每个字符代表一个选项，可以是除了'-',':',';'以外任何一个可见的ascll字符
    // 特别的，如果一个选项字符后跟着一个冒号，则代表该选项后需要参数
    // 参数会保存在optarg
    const char *filepath = NULL;
    char c;
    if (c = getopt(argc, argv, "f:") != -1)
        filepath = optarg;
    else // 没有提供输入文件路径，使用默认地址
        printf("文件输入将使用默认地址，请确保路径下存在输入文件!\n");
    systemRun(filepath);
    return 0;
}