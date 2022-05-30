#include <common_types.h>
#include <semaphore.h>

#ifndef HOTEL_SYSTEM_SYSTEMINIT
#define HOTEL_SYSTEM_SYSTEMINIT

extern roomInfo_shm *roomInfo;
extern sem_t *threadsSem;

/*
    对外接口函数
*/
void systemStart();
/*测试用接口*/
// 打印room_id, all_requests
void print_roomInfo();
void print_fileInput();

/*
    内部函数
*/
// struct request *requestParse(char *line, __ssize_t len);
// void fileReader(const char *filepath);
// void roomInfo_init(roomInfo_shm *roomInfo);
// void system_init();

#endif