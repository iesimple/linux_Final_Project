#include "process.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

int count = 0; // 统计当前线程数量，维持主进程的运行

/*
 * 内部调用的函数
 */
void *test(void *arg);
void rqt_process(struct request *rqt);
void *process(void *arg);
void print_request(struct customerRequest *cusrqt);

/*
    对于某个具体请求的处理函数，函数所需数据通过访问共享内存区得到
*/
void reserve(struct request *rqt);
void cancel(struct request *rqt);
void reserveblock(struct request *rqt);
void cancelblock(struct request *rqt);
void reserveany(struct request *rqt);
void cancelany(struct request *rqt);
void check(struct request *rqt);

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
    sem_wait(roomInfo);
    /*
        临界区，这里也可以用锁机制实现
    */
    switch (rqt->command) {
    case RESERVE:
        reserve(rqt);
        break;
    case CANCEL:
        cancel(rqt);
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
    // 模拟客人操作时间
    sleep(rqt->time);
    sem_post(roomInfo);
}

/**
 * @brief 处理一个客人所有请求的函数
 *
 * @param arg struct customerRequest*, 一个客人所有的请求
 * @return void*
 */
void *process(void *arg) {
    struct customerRequest *cusrqt = (struct customerRequest *)arg;
    pthread_detach(pthread_self());
    struct requestList *p = cusrqt->listHead;
    while (p) {
        rqt_process(p->ptr);
        p = p->next;
    }
    sem_post(threadsSem); // 限制最大并发线程数
    count--;
}

/**
 * @brief 请求处理函数，对于每个客人的请求，创建子线程去处理
 *
 * @param all_requests 指向所有请求的指针
 * @param n 客人的数量
 */
void request_process(struct customerRequest *all_requests, int n) {
    for (int i = 0; i < n; i++) {
        sem_wait(threadsSem);
        pthread_t tid;
        count++;
        pthread_create(&tid, NULL, process, &all_requests[i]);
    }
    while (count)
        sleep(2);
}

void reserve(struct request *rqt) {
}
void cancel(struct request *rqt) {
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
    
}

/**
 * @brief 测试用，打印线程接受到的请求
 *
 * @param cusrqt 指向一个客人所有请求的指针
 */
void print_request(struct customerRequest *cusrqt) {
    // 客人请求信息
    printf("%s", cusrqt->name); // 读入的时候多了一个回车
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

void *test(void *arg) {
    struct customerRequest *cusrqt = (struct customerRequest *)arg;
    pthread_detach(pthread_self()); // 从状态上实现线程分离, 线程执行完之后会释放自身资源，而不会成为僵尸线程
    // 测试用，打印当前的请求
    print_request(cusrqt);
    // printf("id = %ld, current thread num = %d\n",pthread_self(),count);
    sem_post(threadsSem);
    count--;
}