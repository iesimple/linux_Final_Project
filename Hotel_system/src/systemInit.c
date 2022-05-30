#include "systemInit.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <memory.h>
#include <unistd.h>
#include <semaphore.h>
#include "process.h"
#include <names.h>
#include <common_types.h>

/*
    读取文件用到的变量
*/
int total_room, total_customer;
int room_id[MAX_NUM_ROOM];
struct customerRequests all_requests[MAX_NUM_CUSTOM];
/*
    共享内存区、信号量有关变量
*/
int fd;
sem_t *roomSem;
// extern定义，全局变量
roomInfo_shm *roomInfo;

/**
 * @brief system启动，读取输入文件的内容，初始化宾馆信息，处理每个客人的请求
 * 
 * @param filepath 输入文件的路径，如果为NULL使用默认路径
 */
void systemStart(const char *filepath) {
    // 读取文件输入
    fileReader(filepath);
    // 初始化宾馆房间信息
    system_init();
    // 请求处理开始
    request_process(&all_requests, total_customer);
    /*
        测试用，打印所有的输入信息
    */
    // print_fileInput();
}

/* 合法request
 **************************************************************
 *  reserve 房间号 年 月 日 预约天数 预约姓名 time
 *  cancel 房间号 年 月 日 预约天数 预约姓名 time
 *  reserveblock 房间数 第一个房间号 年 月 日 预约天数 预约姓名 time
 *  cancelblock 房间数 第一个房间号 年 月 日 预约天数 预约姓名 time
 *  reserveany 房间数 年 月 日 预约天数 预约姓名 time
 *  cancelany 房间数 年 月 日 预约天数 预约姓名 time
 *  check 预约姓名 time
 **************************************************************
 */
/**
 * @brief request解析
 * @param char* 一行完整的合法请求
 * @param __ssize_t 长度？
 * @return 一个request的结构体指针
 */
struct request *requestParse(char *line, __ssize_t len) {
    struct request *tmp = (struct request *)malloc(sizeof(struct request));
    int *ptr = &tmp->room_num;
    int i = 0;
    char *token = strsep(&line, " \r\n");
    if (strcmp(token, "end") == 0)
        return NULL;
    memset(tmp, 0, sizeof(struct request));
    strcpy(tmp->command, token); // command

    token = strsep(&line, " \r\n");

    if (strcmp(tmp->command, "check")) {
        while (i < 6 && token != NULL) {
            if (i == 0 && (strcmp(tmp->command, "reserve") == 0 || strcmp(tmp->command, "cancel") == 0)) // 跳过房间数
            {
                i++;
                continue;
            } else if (i == 1 && (strcmp(tmp->command, "reserveany") == 0 || strcmp(tmp->command, "cancelany") == 0)) //跳过房间号
            {
                i++;
                continue;
            }
            *(ptr + i) = atoi(token);
            i++;
            token = strsep(&line, " \r\n");
        }
    }
    strcat(tmp->name, token);
    token = strsep(&line, " \r\n");
    tmp->time = atoi(token);
    return tmp;
}

/**
 * @brief 读取文件内容解析并存储到room_id, all_request
 * 
 * @param filepath 输入文件的路径，如果为NULL使用默认路径
 */
void fileReader(const char *filepath) {
    FILE *fp;
    int num = 0;
    char *line = NULL;
    struct requestList *tmp;
    size_t linecap = 0;
    __ssize_t length;
    all_requests[num].listHead = all_requests[num].listTail = NULL;
    // 文件读取
    if (filepath)
        fp = fopen(filepath, "r");
    else
        fp = fopen(FILE_PATH, "r");
    fscanf(fp, "%d", &total_room);
    // 读取宾馆房间号信息
    for (int i = 0; i < total_room; i++)
        fscanf(fp, "%d", &room_id[i]);
    // 读取客人请求信息
    fscanf(fp, "%d", &total_customer);
    while (length = getline(&line, &linecap, fp) != -1) {
        if (line[0] < 48)
            continue;
        strcpy(all_requests[num].name, line); // 尾部会多一个回车
        while (length = getline(&line, &linecap, fp) != -1) {
            // printf("%s", line);
            tmp = (struct requestList *)malloc(sizeof(struct requestList));
            tmp->ptr = requestParse(line, length);
            if (!tmp->ptr)
                break;
            if (all_requests[num].listHead == NULL) {
                all_requests[num].listHead = tmp;
            } else {
                all_requests[num].listTail->next = tmp;
            }
            all_requests[num].listTail = tmp;
            tmp->next = NULL;
        }
        num++;
    }
    fclose(fp);
}

/**
 * @brief 房间信息（共享内存区）初始化
 * 
 * @param roomInfo_shm* 存储房间信息的共享内存区
 */
void roomInfo_init(roomInfo_shm *roomInfo) {
    for (int i = 0; i < total_room; i++) {
        roomInfo->room_id[i] = room_id[i];
        roomInfo->flag[i] = false;
    }
}

/**
 * @brief 系统初始化
 */
void system_init() {
    // 创建并打开共享内存区
    fd = shm_open(ROOM_INFO_SHM, O_CREAT | O_RDWR, 0644);
    // 设置共享内存区大小
    ftruncate(fd, sizeof(roomInfo_shm));
    // 把共享内存区映射到内存
    roomInfo = (roomInfo_shm *)mmap(NULL,
                                    sizeof(roomInfo_shm),
                                    PROT_READ | PROT_WRITE,
                                    MAP_SHARED, fd, 0);
    // 创建并打开信号量，该信号量被用于修改roomInfo时
    roomSem = sem_open(ROOM_INFO_SEM, O_CREAT, 0644);

    roomInfo_init(roomInfo);
    // 打印房间信息
    print_roomInfo();
}

/**
 * @brief 打印房间信息（共享内存区）
 */
void print_roomInfo() {
    printf("------------------\n");
    printf("| room_id | flag |\n");
    for (int i = 0; i < total_room; i++) {
        printf("| %7d | %4d |\n", roomInfo->room_id[i], roomInfo->flag[i]);
    }
    printf("------------------\n");
}

/**
 * @brief 打印输入文件读取到变量中之后的内容，用于查看读取是否有误
 */
void print_fileInput() {
    // room_id信息
    printf("%d\n", total_room);
    for (int i = 0; i < total_room; i++)
        printf("%d ", room_id[i]);
    printf("\n");
    // 客人请求信息
    for (int i = 0; i < total_customer; i++) {
        printf("%s", all_requests[i].name); // 读入的时候多了一个回车
        struct requestList *p = all_requests[i].listHead;
        while (p != NULL) {
            printf("%s %d %d %d %d %d %d %s %d\n",
                   p->ptr->command,
                   p->ptr->room_num,
                   p->ptr->room_id,
                   p->ptr->year,
                   p->ptr->month,
                   p->ptr->day,
                   p->ptr->reserve_days,
                   p->ptr->name,
                   p->ptr->time);
            p = p->next;
        }
        printf("----------------------------------------------\n");
    }
}