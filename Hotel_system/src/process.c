#include "process.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include "isbadptr.h"
#include <common_types.h>

/*
 * 内部调用的函数
 */
void *test(void *arg);
void rqt_process(struct request *rqt);
void *process(struct customerRequest *cusrqt);
void print_request(struct customerRequest *cusrqt);

// 对于某个具体请求的处理函数，函数所需数据通过访问共享内存区得到
// void reserve(struct request *rqt);
// void cancel(struct request *rqt);
void rsv_ccl(const struct request *rqt);
void reserveblock(const struct request *rqt);
void cancelblock(const struct request *rqt);
void reserveany(const struct request *rqt);
void cancelany(const struct request *rqt);
void check(const struct request *rqt);

// 执行上面的处理函数时借助的函数
char *enumtostr(command_type command);
int find_index(const char *name);
void print_reserveInfo(int index);
bool isavailable(const struct request *rqt, int index);
bool isexit(int room_id);
void next_day(int *year, int *month, int *day);
int *get_room_ids(int room_id, int num);
void alter_roomInfo(const struct request *rqt);
void alter_reserveInfo(const struct request *rqt, int index);

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

char *enumtostr(command_type command) {
    char *cmd = (char *)malloc(sizeof(char) * 15);
    switch (command) {
    case 0:
        strcpy(cmd, "reserve");
        break;
    case 1:
        strcpy(cmd, "cancel");
        break;
    case 2:
        strcpy(cmd, "reserveblock");
        break;
    case 3:
        strcpy(cmd, "cancelblock");
        break;
    case 4:
        strcpy(cmd, "reserveany");
        break;
    case 5:
        strcpy(cmd, "cancelany");
        break;
    case 6:
        strcpy(cmd, "check");
    }
    return cmd;
}

/**
 * @brief 处理一条请求的函数
 *
 * @param rqt 指向一条请求的指针
 */
void rqt_process(struct request *rqt) {
    // 模拟客人操作时间
    sleep(rqt->time);
    sem_wait(mutexSem);
    printf("%-13s room_num=%-3d room_id=%-3d %4d %2d-%-2d reserve_days=%-2d name=%s\n",
           enumtostr(rqt->command),
           rqt->room_num,
           rqt->room_id,
           rqt->year,
           rqt->month,
           rqt->day,
           rqt->reserve_days,
           rqt->name);

    /*
        临界区，这里也可以用锁机制实现
    */
    switch (rqt->command) {
    case RESERVE:
    case CANCEL:
        rsv_ccl(rqt);
        break;
    case RESERVEBLOCK:
        reserveblock(rqt);
        break;
    case CANCELBLOCK:
        cancelblock(rqt);
        break;
    case RESERVEANY:
        reserveany(rqt);
        break;
    case CANCELANY:
        cancelany(rqt);
        break;
    case CHECK:
        check(rqt);
    }
    sem_post(mutexSem);
}

/**
 * @brief 处理一个客人所有请求的函数
 *
 * @param cusrqt struct customerRequest*, 一个客人所有的请求
 * @return void*
 */
void *process(struct customerRequest *cusrqt) {
    struct requestList *p = cusrqt->listHead;
    while (p) {
        rqt_process(p->ptr);
        p = p->next;
    }
    sem_post(processSem); // 限制最大并发进程数
}

/**
 * @brief 请求处理函数，对于每个客人的请求，创建子进程去处理
 *
 * @param all_requests 指向所有请求的指针
 * @param n 客人的数量
 */
void request_process(struct customerRequest *all_requests, int n) {
    pid_t pid;
    for (int i = 0; i < n; i++) {
        sem_wait(processSem);
        pid = fork();
        if (pid == -1) {
            printf("fork failed!\n");
            exit(1);
        } else if (pid == 0) { // 子进程，执行请求处理函数后直接结束
            process(&all_requests[i]);
            fflush(stdout); // 清空输出缓冲区，防止和父进程输出交叉
            exit(0);
        }
    }

    if (pid > 0) { // 父进程等待所有子进程执行完成
        for (int i = 0; i < n; i++)
            wait(NULL);
    }

    // 测试用
    // for (int i = 0; i < n; i++) {
    //     sem_wait(threadsSem);
    //     pid = fork();
    //     if (pid == -1) {
    //         printf("fork failed!\n");
    //         exit(1);
    //     } else if (pid == 0) {
    //         if (IsReadBadPtr(all_requests, n)) {
    //             printf("该指针指向的内存无法读取!\n");
    //             exit(1);
    //         } else {
    //             {
    //                 if (i == 0)
    //                     all_requests[i].listHead->ptr->time = 0;
    //                 print_request(&all_requests[0]);

    //                 // printf("id = %d ptr = %p\n", getpid(), all_requests);
    //                 // print_request(&all_requests[i]);
    //                 return;
    //             }
    //         }
    //         // process(&all_requests[i]);
    //     }
    // }
    // if (pid > 0) {
    //     // printf("parent ptr = %p\n", all_requests);
    //     for (int i = 0; i < n; i++)
    //         wait(NULL);
    //     for (int j = 0; j < n; j++)
    //         print_request(&all_requests[j]);
    // }

    // 以下是多线程，要分清楚
    // for (int i = 0; i < n; i++) {
    //     sem_wait(threadsSem);
    //     pthread_t tid;
    //     count++;
    //     pthread_create(&tid, NULL, process, &all_requests[i]);
    // }
    // while (count)
    //     sleep(2);
}

/**
 * @brief reserve or cancel
 *
 * @param rqt 请求
 * @param flag reserve:=false, cancel:=true
 */
void rsv_ccl(const struct request *rqt) {
    if (!isexit(rqt->room_id)) {
        printf("roomid is not exit!\n");
        return;
    }
    int index = find_index(rqt->name);
    if (!isavailable(rqt, index)) {
        // room不可用
        printf("Your request is not available!\n");
        return;
    }
    alter_roomInfo(rqt);
    if (!rqt->command && index == -1) // 如果rqt是一个reserve请求但是共享内存区中没有该客人的记录
        index = reserveInfo->customer_num++;
    alter_reserveInfo(rqt, index);
}

void reserveblock(const struct request *rqt) {
    if (!isexit(rqt->room_id)) {
        printf("roomid is not exit!\n");
        return;
    }
    struct request tmp = *rqt; // 创建一个临时变量，拷贝rqt的值
    tmp.command = RESERVE;
    // 循环判断所有申请的房间是否可用
    int *ids = get_room_ids(rqt->room_id, rqt->room_num);
    for (int i = 0; i < rqt->room_num; i++) {
        if (!isavailable(&tmp, -1)) {
            printf("Your request is not available!\n");
            return;
        }
        tmp.room_id = ids[i + 1];
    }
    // 更新共享内存区
    int index = find_index(tmp.name);
    for (int i = 0; i < rqt->room_num; i++) {
        tmp.room_id = ids[i];
        alter_roomInfo(&tmp);
        if (index == -1) // 如果rqt是一个reserve请求但是共享内存区中没有该客人的记录
            index = reserveInfo->customer_num++;
        alter_reserveInfo(&tmp, index);
    }
}

void cancelblock(const struct request *rqt) {
    if (!isexit(rqt->room_id)) {
        printf("roomid is not exit!\n");
        return;
    }
    struct request tmp = *rqt; // 创建一个临时变量，拷贝rqt的值
    tmp.command = CANCEL;
    // 循环判断所有申请的房间是否可用
    int *ids = get_room_ids(rqt->room_id, rqt->room_num);
    int index = find_index(rqt->name);
    if (index == -1) {
        printf("Your request is not available!\n");
        return;
    }
    for (int i = 0; i < rqt->room_num; i++) {
        if (!isavailable(&tmp, index)) {
            printf("Your request is not available!\n");
            return;
        }
        tmp.room_id = ids[i + 1];
    }
    // 更新共享内存区
    for (int i = 0; i < rqt->room_num; i++) {
        tmp.room_id = ids[i];
        alter_roomInfo(&tmp);
        alter_reserveInfo(&tmp, index);
    }
}

/**
 * @brief 给出预约房间数、预约时间和预约天数，预约房间号由系统分配
 *
 * @param rqt {reserveany 房间数 年 月 日 预约天数 预约姓名 time}
 */
void reserveany(const struct request *rqt) {
    int j = 1;
    int ids[rqt->room_num]; // 如果可以找到请求所需的房间，那么ids存储这些房间号
    struct request tmp = *rqt;
    tmp.command = RESERVE;
    for (int i = 0; i < rqt->room_num;) // 找到room_num间可用房间
    {
        for (; j < MAX_NUM_ROOM && roomInfo->room_id[j];) // 遍历所有存在的房间号
        {
            tmp.room_id = j;
            if (isavailable(&tmp, -1)) {
                ids[i++] = j;
                break;
            } else {
                printf("Your request is not available!\n");
                return;
            }
        }
        j++;
        if (j == MAX_NUM_ROOM) {
            printf("Your request is not available!\n");
            return;
        }
    }
    int index = find_index(rqt->name);
    if (index == -1) // 如果rqt是一个reserve请求但是共享内存区中没有该客人的记录
        index = reserveInfo->customer_num++;
    for (int i = 0; i < rqt->room_num; i++) {
        tmp.room_id = ids[i];
        alter_roomInfo(&tmp);
        alter_reserveInfo(&tmp, index);
    }
}
void cancelany(const struct request *rqt) {
    int index = find_index(rqt->name);
    if (index == -1) {
        printf("Your request is not available!\n");
        return;
    }
    int j = 1;              // j
    int ids[rqt->room_num]; // 如果可以找到请求所需的房间，那么ids存储这些房间号
    struct request tmp = *rqt;
    tmp.command = CANCEL;
    for (int i = 0; i < rqt->room_num;) // 找到room_num间可以取消的房间
    {
        for (; j < MAX_NUM_ROOM && roomInfo->room_id[j];) // 遍历所有存在的房间号
        {
            tmp.room_id = j;
            if (isavailable(&tmp, index)) {
                ids[i++] = j;
                break;
            } else {
                printf("Your request is not available!\n");
                return;
            }
        }
        j++;
        if (j == MAX_NUM_ROOM) {
            printf("Your request is not available!\n");
            return;
        }
    }

    for (int i = 0; i < rqt->room_num; i++) {
        tmp.room_id = ids[i];
        alter_roomInfo(&tmp);
        alter_reserveInfo(&tmp, index);
    }
}
void check(const struct request *rqt) {
    int index = find_index(rqt->name);
    if (index != -1) {
        print_reserveInfo(index);
    } else {
        // printf("%s", reserveInfo->name[0]);
        printf("%s have no reserve information.\n", rqt->name);
    }
}

/*
    处理具体请求时需要调用的函数
*/

/**
 * @brief 计算下一天的年月日，参数为指针
 *
 * @param year 年
 * @param month 月
 * @param day 日
 */
void next_day(int *year, int *month, int *day) {
    int date[3] = {*year, *month, *day};
    int days[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // 2022 2023中每个月的天数
    if (*day + 1 <= days[*month]) {
        *day = *day + 1;
    } else {
        if (*month + 1 <= 12) {
            *day = 1;
            *month = *month + 1;
        } else {
            if (*year + 1 < 1) {
                *year = *year + 1;
                *month = 1;
                *day = 1;
            } else {
                printf("Date is illegal!\n");
                return;
            }
        }
    }
}

bool isexit(int room_id) {
    if (!roomInfo->room_id[room_id])
        return false;
    return true;
}

/**
 * @brief 返回申请的所有房间
 *
 * @param room_id 开始房间号
 * @param num 房间数
 * @return int* 所有房间号
 */
int *get_room_ids(int room_id, int num) {
    if (!isexit(room_id)) // 起始房间号不存在，认为用户输入错误
        printf("roomid is not exit!\n");
    else {
        int *ids = (int *)malloc(sizeof(int) * num);
        for (int i = 0; i < num; i++) {
            while (!roomInfo->room_id[room_id])                      // 找到存在的房间号，这里不对房间的预约信息做判断
                room_id = (++room_id) % (roomInfo->max_room_id + 1); // 超过最大房间号，循环，即回到最开始
            ids[i] = (++room_id) % (roomInfo->max_room_id + 1);
        }
        return ids;
    }
}

/**
 * @brief 查看希望预约的房间以及日期是否可用
 *
 * @param room_id 房间号
 * @param year 年
 * @param month 月
 * @param day 日
 * @param reserve_days 预约天数
 * @return true 房间可用
 * @return false 房间不可用
 */
bool isavailable(const struct request *rqt, int index) {
    int reserve_days = rqt->reserve_days;
    int date[3] = {rqt->year - 2022, rqt->month, rqt->day};
    if (!rqt->command) // reserve请求，因此查找roomInfo
    {
        while (roomInfo->flag[rqt->room_id][date[0]][date[1]][date[2]] == rqt->command) {
            reserve_days--;
            if (!reserve_days)
                return true;
            next_day(date, date + 1, date + 2);
        }
    } else if (index == -1) // cancel请求，但是没有给合法的index，直接返回false
    {
        // printf("%s have no reserve information.\n", rqt->name);
        return false;
    } else // 查找reserveInfo
    {
        while (reserveInfo->Array[index].flag[rqt->room_id][date[0]][date[1]][date[2]] == rqt->command) {
            reserve_days--;
            if (!reserve_days)
                return true;
            next_day(date, date + 1, date + 2);
        }
    }

    // 如果是reserve，Info = roomInfo
    // 下面的while就是在判断 "roomInfo->flag[date] == 0?"
    // 如果是cancel
    // 下面的while就是在判断 "reserveInfo->Array[index]->flag[date] == 1?"

    return false;
}

// bool isavailable(struct request *rqt, int index) {
//     int reserve_days = rqt->reserve_days;
//     int date[3] = {rqt->year - 2022, rqt->month, rqt->day};
//     bool(*Info)[MAX_NUM_ROOM][2][13][32];
//     if (!rqt->command) // reserve请求，因此查找roomInfo
//         Info = &(*roomInfo).flag;
//     else if (index == -1) // cancel请求，但是没有给合法的index，直接返回false
//     {
//         printf("%s have no reserve information.\n", rqt->name);
//         return false;
//     } else // 查找reserveInfo
//         Info = &reserveInfo->Array[index].flag;

//     // 如果是reserve，Info = roomInfo
//     // 下面的while就是在判断 "roomInfo->flag[date] == 0?"
//     // 如果是cancel
//     // 下面的while就是在判断 "reserveInfo->Array[index]->flag[date] == 1?"

//     while (*Info[rqt->room_id][date[0]][date[1]][date[2]] == rqt->command) {
//         reserve_days--;
//         if (!reserve_days)
//             return true;
//         next_day(date, date + 1, date + 2);
//         printf("a\n");
//         fflush(stdout);
//     }
//     return false;
// }

/**
 * @brief 修改roomInfo中是否被预约的信息，自动区分reserve和cancel
 *
 * @param rqt 请求
 */
void alter_roomInfo(const struct request *rqt) {
    int date[3] = {rqt->year - 2022, rqt->month, rqt->day};
    for (int j = 0; j < rqt->reserve_days; j++) {
        roomInfo->flag[rqt->room_id][date[0]][date[1]][date[2]] = !rqt->command;
        if (!rqt->command)
            strcpy(roomInfo->name[rqt->room_id][date[0]][date[1]][date[2]], rqt->name);
        next_day(date, date + 1, date + 2);
    }
}

/**
 * @brief 修改reserveInfo中是否被预约的信息，自动区分reserve和cancel
 *
 * @param rqt 请求
 * @param index name对应在reserveInfo->Array中的索引
 */
void alter_reserveInfo(const struct request *rqt, int index) {
    if (index == -1)
        printf("index error!");
    strcpy(reserveInfo->name[index], rqt->name);
    int date[3] = {rqt->year - 2022, rqt->month, rqt->day};
    for (int i = 0; i < rqt->reserve_days; i++) {
        reserveInfo->Array[index].flag[rqt->room_id][date[0]][date[1]][date[2]] = (!rqt->command);
        next_day(date, date + 1, date + 2);
    }
}

/**
 * @brief 找到存储客人预约信息的索引
 *
 * @param name 客人名字
 * @return int 索引
 */
int find_index(const char *name) {
    int index = -1;
    for (int i = 0; i < reserveInfo->customer_num; i++) {
        if (strcmp(reserveInfo->name[i], name) == 0) {
            index = i;
            break;
        }
    }
    return index;
}

/**
 * @brief 打印索引对应的预约信息
 *
 * @param index
 */
void print_reserveInfo(int index) {
    printf("--------------------------\n");
    printf("| room_id |     date     |\n");
    for (int i = 1; i < MAX_NUM_ROOM; i++) {
        // if (!rrinfo->room_id[i])
        //     break;
        if (!roomInfo->room_id[i]) // 不存在的房间号
            continue;
        for (int j = 0; j < 2; j++)          // 年
            for (int k = 1; k < 13; k++)     // 月
                for (int l = 1; l < 32; l++) // 日
                    if (reserveInfo->Array[index].flag[i][j][k][l]) {
                        printf("| %5d   |  %4d %2d %2d  |\n", i, j + 2022, k, l);
                    }
    }
    printf("--------------------------\n");
}

/**
 * @brief 测试用，打印线程接受到的请求
 *
 * @param cusrqt 指向一个客人所有请求的指针
 */
void print_request(struct customerRequest *cusrqt) {
    // 客人请求信息
    printf("%s\n", cusrqt->name); // 读入的时候多了一个回车
    struct requestList *p = cusrqt->listHead;
    while (p != NULL) {
        printf("%d %d %d %d %d %d %d %s %d\n",
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

// void *test(void *arg) {
//     struct customerRequest *cusrqt = (struct customerRequest *)arg;
//     pthread_detach(pthread_self()); // 从状态上实现线程分离, 线程执行完之后会释放自身资源，而不会成为僵尸线程
//     // 测试用，打印当前的请求
//     print_request(cusrqt);
//     // printf("id = %ld, current thread num = %d\n",pthread_self(),count);
//     sem_post(threadsSem);
// }