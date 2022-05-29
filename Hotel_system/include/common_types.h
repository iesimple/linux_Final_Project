#ifndef HOTEL_SYSTEM_COMMON_TYPES_H
#define HOTEL_SYSTEM_COMMON_TYPES_H

#define MAX_LEN_NAME 25       // 客人名字的最大长度
#define MAX_LEN_ONE_REQUEST 9 // 一条请求的最大长度
#define MAX_NUM_ROOM 128 // 最大宾馆房间数
#define MAX_NUM_CUSTOM 255 // 最大客人数
#define FILE_PATH "./input.txt" // 调试用
// #define FILE_PATH "../input.txt" // 输入文件

// 一条请求
struct request
{
    char command[15]; // 每条请求开始的字符串，最长为reserveblock
    int room_num;     // 房间数
    int room_id;      // 房间号 or 第一个房间号
    int year;         // 年月日
    int month;
    int day;
    int reserve_days;        // 预约天数
    char name[MAX_LEN_NAME]; // 预约姓名，这里的姓名应该是可以和发起请求的姓名不同的
    int time;                // 处理时间，秒
};

struct requestList
{
    struct request *ptr;
    struct requestList *next;
};

// 一个客人的请求
struct customerRequests
{
    char name[MAX_LEN_NAME];
    struct requestList *listHead; // 请求链表的第一个请求
    struct requestList *listTail; // 请求链表的最后一个请求
};

#endif