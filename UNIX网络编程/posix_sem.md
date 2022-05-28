<head>
   <style>
      table {
         margin: auto;
      }
   </style>
</head>

# Posix 信号量

- Posix 有名信号量：使用 Posix IPC name 标识，可用于进程或线程间的同步
- Posix 基于内存的信号量：存放在共享内存区中，可用于进程或线程间的同步
- System V 信号量：在内核中维护，可用于进程或线程间的同步

## 对于信号量的操作

1. create
   同时要求调用者指定初始值
2. wait
   if sem <= 0 wait else sem-- aka. P 操作 or 上锁
3. post
   sem++ if sem > 0 wakeup sleep thread aka. V 操作 or 解锁 or 发信号

**互斥锁与信号量比较**（在解决互斥问题时）

| 初始化互斥锁                | 初始化信号量为 1 |
| :-------------------------- | :--------------- |
| pthread_mutex_lock(&mutex)  | sem_wait(&mutex) |
| 临界区                      | 临界区           |
| pthread_mutex_unloc(&mutex) | sem_post(&mutex) |

1. 同时信号量还有一个互斥锁没有的特性，互斥锁总是由锁住它的线程打开，而信号量的 post 却不必由执行过它的 wait 操作的同一线程执行
2. 互斥锁要么被锁住，要么被解开
3. 信号量的 post 可能会丢失，如果没有等待的线程

**有名信号量** & **基于内存/无名信号量**

<table>
   <tr>
      <td>sem_open</td>
      <td>sem_init</td>
   </tr>
   <tr>
      <td colspan="2">sem_wait</td>
   </tr>
   <tr>
   <tr>
       <td colspan="2">sem_trywait</td>
   </tr>
   <tr>
   <tr>
      <td colspan="2">sem_post</td>
   </tr>
   <tr>
   <tr>
      <td colspan="2">sem_getvalue</td>
   </tr>
   <tr>
   <td>sem_close</td>
      <td rowspan="2">sem_destroy</td>
   </tr>
   <tr>
      <td>sem_unlink</td>
   </tr>
</table>

```c
#include <semaphore.h>
sem_t *sem_open(
   const char *name, // 名字
   int oflag, 
   mode_t mode, // 如果oflag指定了O_CREAT，mode指定权限位
   unsigned int value /// 同时需要给定信号量初始值
   );

// 成功返回指向信号量的指针，出错返回SEM_FAILED

#include <semaphore.h>
int sem_close(sem_t * sem);
int sem_unlink(const char *name);
int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
int sem_post(sem_t *sem, int *valp);
```
