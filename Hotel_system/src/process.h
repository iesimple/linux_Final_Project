#include <common_types.h>
#include <semaphore.h>

#ifndef HOTEL_SYSTEM_PROCESS
#define HOTEL_SYSTEM_PROCESS

extern roomInfo_shm *roomInfo; // 声明共享内存区变量，函数中需要调用
extern reserveInfo_shm *reserveInfo;
extern sem_t *roomSem;
extern sem_t *processSem;


/**
 * @brief 处理所有的请求
 *
 * @param all_request 存储所有请求的结构体，注意结构体中的每一项是一个客人的请求
 * @param n 请求总数/请求的客人总数
 */
void request_process(struct customerRequest *all_requests, int n);

#endif