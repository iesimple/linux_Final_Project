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

#define NBUFF 10
#define SEM_MUTEX "mutex"
#define SEM_NEMPTR "nempty"
#define SEM_NSTORED "nstored"

int nitems;

struct
{
    int buff[NBUFF];
    sem_t *mutex, *nempty, *nstored;
} shared;

void *produce(void *), *consume(void *);

int main(int argc, char **agrv)
{
    pthread_t tid_produce, tid_consume;
    if (argc != 2)
    {
        printf("usage: prodcons <#items>\n");
        exit(-1);
    }

    shared.mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 1);
    shared.mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, NBUFF);
    shared.mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, S_IRUSR | S_IWUSR, 0);

    pthread_create(&tid_produce, NULL, produce, NULL);
    pthread_create(&tid_consume, NULL, consume, NULL);

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
        sem_post(shared.nempty);
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
        if (shared.buff[i % NBUFF] != i)
            printf("buff[%d] = %d\n", i, shared.buff[i % NBUFF]);
        sem_post(shared.mutex);
        sem_post(shared.nstored);
    }
    return NULL;
}