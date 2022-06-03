#ifndef HOTEL_SYSTEM_COMMON_TYPES_H
#define HOTEL_SYSTEM_COMMON_TYPES_H

#define MAX_NUM_THREAD 100 // 最大同时进程数，小于实际最大线程数，防止输入太多导致无法创建
#define BUFF_SIZE 10
#define MAX_LEN_NAME 25       // 客人名字的最大长度
#define MAX_LEN_ONE_REQUEST 9 // 一条请求的最大长度
#define MAX_NUM_ROOM 128      // 最大宾馆房间数
#define MAX_NUM_CUSTOM 255    // 最大客人数

/**
 * 读取输入用的结构体
 */
// 一条请求，对于某些请求，结构体中无用数据会被初始化为0

typedef enum
{
    RESERVE,
    CANCEL,
    RESERVEBLOCK,
    CANCELBLOCK,
    RESERVEANY,
    CANCELANY,
    CHECK
} command_type;

struct request
{
    command_type command; // 请求，这是一个枚举类型
    int room_num;         // 房间数
    int room_id;          // 房间号 or 第一个房间号
    int year;             // 年月日
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
struct customerRequest
{
    char name[MAX_LEN_NAME];
    struct requestList *listHead; // 请求链表的第一个请求
    struct requestList *listTail; // 请求链表的最后一个请求
};

/**
 *
 *
 */

typedef struct
{
    char name[MAX_LEN_NAME];
    int index;
} name_index_Buff;

typedef enum
{
    true = 1,
    false = 0
} bool;

typedef struct
{
    bool room_id[MAX_NUM_ROOM]; // 预约了房间号为i的房间则room_id[i]=true
    bool flag[MAX_NUM_ROOM][2][13][32];
} reserveRoom;

// 存储所有用户所有预约信息
typedef struct
{
    char name[MAX_NUM_CUSTOM][MAX_LEN_NAME]; // 可能后续拿来作权限识别
    int customer_num;
    reserveRoom Array[MAX_NUM_CUSTOM];
} reserveInfo_shm;

// 存储房间信息的共享内存区
// 日期，年月日都按照最大开空间，另外设置函数判断日期是否合理
typedef struct
{
    bool room_id[MAX_NUM_ROOM]; // 房间号i存在则room_id[i]=true
    // flag[i][j][k][l] - 房间i在j-k-l这一天是否被预约
    // 2 - 2022 or 2023
    // 13 - 12个月，为了方便访问
    // 32 - 31天，为了方便访问
    bool flag[MAX_NUM_ROOM][2][13][32];
    char name[MAX_NUM_ROOM][2][13][32][MAX_LEN_NAME];
} roomInfo_shm;

#endif