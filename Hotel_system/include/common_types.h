#ifndef HOTEL_SYSTEM_COMMON_TYPES_H
#define HOTEL_SYSTEM_COMMON_TYPES_H

#define MAX_THREAD_NUM 100 // 最大同时线程数，小于实际最大线程数，防止输入太多导致无法创建


#define MAX_LEN_NAME 25         // 客人名字的最大长度
#define MAX_LEN_ONE_REQUEST 9   // 一条请求的最大长度
#define MAX_NUM_ROOM 128        // 最大宾馆房间数
#define MAX_NUM_CUSTOM 255      // 最大客人数
#define FILE_PATH "./input.txt" // 调试用
// #define FILE_PATH "../input.txt" // 输入文件

/**
 * 读取输入用的结构体
 */
// 一条请求，对于某些请求，结构体中无用数据会被初始化为0
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

// 请求链表的每一项，ptr表示当前这一项指向的request，next表示下一项
struct requestList
{
    struct request *ptr;
    struct requestList *next;
};

// 一个客人的请求，请求序列以链表形式存储
struct customerRequests
{
    char name[MAX_LEN_NAME];
    struct requestList *listHead; // 请求链表的第一个请求
    struct requestList *listTail; // 请求链表的最后一个请求
};

/**
 *
 *
 */

typedef enum
{
    true = 1,
    false = 0
} bool;

// 存储房间信息的共享内存区
typedef struct
{
    int room_id[MAX_NUM_ROOM];   // 房间号
    bool flag[MAX_NUM_ROOM]; // 当前房间是否被预约
} roomInfo_shm;

#endif