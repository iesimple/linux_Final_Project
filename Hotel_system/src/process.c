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

/*
    对于某个具体请求的处理函数，函数所需数据通过访问共享内存区得到
*/
// void reserve(struct request *rqt);
// void cancel(struct request *rqt);
void rsv_ccl(struct request *rqt);
void reserveblock(struct request *rqt);
void cancelblock(struct request *rqt);
void reserveany(struct request *rqt);
void cancelany(struct request *rqt);
void check(struct request *rqt);

/*
    执行上面的处理函数时借助的函数
*/
int find_index(char *name);
void print_reserveInfo(reserveRoom *rrinfo);
bool isavailable(struct request *rqt, int index);
void next_day(int *year, int *month, int *day);
void alter_roomInfo(struct request *rqt);
void alter_reserveInfo(struct request *rqt, int index);

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
 * @brief 处理一条请求的函数
 *
 * @param rqt 指向一条请求的指针
 */
void rqt_process(struct request *rqt) {
    // 模拟客人操作时间
    sleep(rqt->time);
    sem_wait(roomSem);
    printf("command %d %d %d %d %d %d %d %s\n",
           rqt->command,
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
    sem_post(roomSem);
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
            return;
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
void rsv_ccl(struct request *rqt) {
    int index = find_index(rqt->name);
    if (!isavailable(rqt, index)) {
        printf("Your request is not available!\n");
        return;
    }
    alter_roomInfo(rqt);
    if (!rqt->command && index == -1) // 如果rqt是一个reserve请求但是共享内存区中没有该客人的记录
        index = reserveInfo->customer_num++;
    alter_reserveInfo(rqt, index);
}

void reserveblock(struct request *rqt) {
}
void cancelblock(struct request *rqt) {
}
void reserveany(struct request *rqt) {
}
void cancelany(struct request *rqt) {
}
void check(struct request *rqt) {
    int index = find_index(rqt->name);
    if (index != -1) {
        print_reserveInfo(&reserveInfo->Array[index]);
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
            } else
                printf("Date is illegal!\n");
        }
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
bool isavailable(struct request *rqt, int index) {
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
        printf("%s have no reserve information.\n", rqt->name);
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
void alter_roomInfo(struct request *rqt) {
    int date[3] = {rqt->year - 2022, rqt->month, rqt->day};
    for (int i = 0; i < rqt->reserve_days; i++) {
        roomInfo->flag[rqt->room_id][date[0]][date[1]][date[2]] = !rqt->command;
        next_day(date, date + 1, date + 2);
    }
}

/**
 * @brief 修改reserveInfo中是否被预约的信息，自动区分reserve和cancel
 *
 * @param rqt 请求
 * @param index name对应在reserveInfo->Array中的索引
 */
void alter_reserveInfo(struct request *rqt, int index) {
    if (index == -1)
        printf("index error!");
    // 修改reserveInfo中是否被预约的信息
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
int find_index(char *name) {
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
void print_reserveInfo(reserveRoom *rrinfo) {
    printf("-------------------------\n");
    printf("| room_id |     date     |\n");
    for (int i = 0; i < MAX_NUM_ROOM; i++) {
        // if (!rrinfo->room_id[i])
        //     break;
        for (int j = 0; j < 2; j++)          // 年
            for (int k = 1; k < 13; k++)     // 月
                for (int l = 1; l < 32; l++) // 日
                    if (rrinfo->flag[i][j][k][l]) {
                        printf("| %5d   |  %4d %2d %2d  |\n", i, j + 2022, k, l);
                    }
    }
    printf("-------------------------\n");
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