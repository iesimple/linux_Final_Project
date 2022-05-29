
#include <stdio.h>
#include "common_types.h"

#ifndef HOTEL_SYSTEM_SYSTEMINIT
#define SYSTEMINIT

#include <stdlib.h>

int total_room, total_customer;
int room_id[MAX_NUM_ROOM];
struct customerRequests all_requests[MAX_NUM_CUSTOM];

struct request *requestParse(char *line, __ssize_t len);
void fileReader();
void room_init();
void request_process();

/*测试用接口*/
// 打印room_id, all_requests
void print_file();

#endif