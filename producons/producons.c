/*
    使用信号量解决生产者消费者问题
    三个信号量的作用
    （1）mutex 二值信号量
        生产者往共享内存区插入一个数据时，消费者从共享内存区移走一个数据时
    （2）nempty
        统计共享内存区中空槽数目
    （3）nstored
        统计贡献内存区已填写的槽位数
*/

#include <semaphore.h>
#include <stdio.h>
#include <sys/fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>
#include <errno.h>

#define NBUFF 10
#define SEM_MUTEX "mutex"
#define SEM_NEMPTR "nempty"
#define SEM_NSTORED "nstored"

extern int errno;

int nitems;

struct
{
    int buff[NBUFF];
    sem_t *mutex, *nempty, *nstored;
} shared;

void *produce(void *), *consume(void *);

int main(int argc, char **argv)
{
    // 当sem_open的oflag参数设置为O_CREAT | O_EXCL，最好先把可能存在的信号量删除一次，如下
    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_NEMPTR);
    sem_unlink(SEM_NSTORED);

    pthread_t tid_produce, tid_consume;
    if (argc != 2)
    {
        printf("usage: prodcons <#items>\n");
        exit(-1);
    }

    nitems = atoi(argv[1]);

    shared.mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    shared.nempty = sem_open(SEM_NEMPTR, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, NBUFF);
    shared.nstored = sem_open(SEM_NSTORED, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);
    
    // 创建失败
    if (shared.mutex == SEM_FAILED || shared.nempty == SEM_FAILED || shared.nstored == SEM_FAILED)
    {
        printf("%s, errno = %d\n", strerror(errno), errno);
        exit(-1);
    }
    
    // 下面这个不是很理解有什么用，没有好像没有影响
    // pthread_setconcurrency(2);

    pthread_create(&tid_produce, NULL, produce, NULL);
    pthread_create(&tid_consume, NULL, consume, NULL);

    pthread_join(tid_produce, NULL);
    pthread_join(tid_consume, NULL);

    sem_unlink(SEM_MUTEX);
    sem_unlink(SEM_NEMPTR);
    sem_unlink(SEM_NSTORED);

    exit(0);
}

void *produce(void *arg)
{
    int i;
    for (i = 0; i < nitems; i++)
    {
        sem_wait(shared.nempty);
        sem_wait(shared.mutex);
        shared.buff[i % NBUFF] = i;
        sem_post(shared.mutex);
        sem_post(shared.nstored);
    }
    return NULL;
}

void *consume(void *arg)
{
    int i;
    for (i = 0; i < nitems; i++)
    {
        sem_wait(shared.nstored);
        sem_wait(shared.mutex);
        if (shared.buff[i % NBUFF] == i) // 书上貌似错了，应该是等于，不等于的话不会有输出
            printf("buff[%d] = %d\n", i, shared.buff[i % NBUFF]);
        sem_post(shared.mutex);
        sem_post(shared.nempty);
    }
    return NULL;
}