#include <common_types.h>

#ifndef HOTEL_SYSTEM_PROCESS
#define HOTEL_SYSTEM_PROCESS


extern roomInfo_shm *roomInfo; // 声明共享内存区变量，函数中需要调用


/**
 * @brief 处理所有的请求
 * 
 * @param all_request 存储所有请求的结构体，注意结构体中的每一项是一个客人的请求
 * @param n 请求总数/请求的客人总数
 */
void request_process(struct customerRequests *all_request, int n);

#endif