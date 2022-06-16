#include <common_types.h>
#include <semaphore.h>

#ifndef HOTEL_SYSTEM_PROCESS
#define HOTEL_SYSTEM_PROCESS


extern roomInfo_shm *roomInfo; // 房间信息的共享内存区
extern reserveInfo_shm *reserveInfo;    // 预约信息的共享内存区
extern sem_t *mutexSem;  // mu
extern sem_t *processSem;


/**
 * @brief 处理所有的请求
 *
 * @param all_request 存储所有请求的结构体，注意结构体中的每一项是一个客人的请求
 * @param n 请求总数/请求的客人总数
 */
void request_process(struct customerRequest *all_requests, int n);

#endif