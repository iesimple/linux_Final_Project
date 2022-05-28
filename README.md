# linux_Final_Project

## Resource

1. **Vscode & Markdown**

    - [Markdownlint错误汇总](https://juejin.cn/post/6844903938165047309)

    - [Markdown语法](https://zhuanlan.zhihu.com/p/56943330)

### linux IPC

#### e1-MultiProcesses Programmnig

##### fork()

功能：创建一个新的进程

```c
#include <unistd.h>
#include <sys/types.h>
pid_t fork();
```

说明：本系统调用为调用进程（也称父进程）创建一子进程。事实上，子进程是父进程的一个“复制品”。父子进程为**独立进程，平等调度，用户空间独立**。

返回值：调用成功，则返回两次。对子进程返回0，对父进程返回子进程号，这也是最方便的区分父子进程的方法。调用失败则返回-1给父进程，子进程不生成。

##### wait()

功能：等待子进程返回

```c
#include <sys/types.h>
#include <sys/wait.h>
pid_t wait(int * stat_loc);
```

说明：调用进程将会阻塞直到其一个子进程终止。调用进程可取得子进程的状态信息。使用wait和exit组合可以实现父子进程同步。

返回值：等待到一个子进程返回时，返回值为该子进程号，同时stat_loc带回子进程的返回状态信息（参考exit）。若无子进程，则返回值为-1。

##### exit()

功能：终止进程

```c
#include <stdlib.h>
void exit(int status);
```

说明：调用进程被该系统调用终止，同时本函数也执行一些善后工作（如，关闭打开的文件句柄等）。 其中，参数status作为进程的退出状态，其低8位将返回到wait所带参数低16位的高8位，而实现最简单的父子通讯。

返回值：无

##### exec

```c
#include <unistd.h>
execve (char * path, char * argv[],  char * env[]);
execle (char * path, char * arg0, … , (char *)0, char *env[]);
execv (char *path, char *argv[]);
execl (char *path, char *arg0, …, (char *)0);
execvp (char *file, char *argv[]);
execlp (char *file, char *arg0, …, (char *)0);
```

- 执行程序的系统调用总称为exec，具有多种不同的调用形式。

- 调用exec并不创建新进程，前后的进程ID并未改变。exec只是用另一个新程序替换了当前进程的正文、数据、堆和栈段。

- 当exec调用成功，调用进程的用户空间将重建，代码段则由新程序代换，这就意味着：一旦exec调用成功，将不返回给调用者，原来程序中该调用下的诸语句将永远不被执行。exec调用返回给调用者的唯一可能是该调用失败了

##### pthread_create

```c
#include <pthread.h>
int pthread_create(
                 pthread_t *restrict tidp,   //新创建的线程ID指向的内存单元。
                 const pthread_attr_t *restrict attr,  //线程属性，默认为NULL
                 void *(*start_rtn)(void *), //新创建的线程从start_rtn函数的地址开始运行
                 void *restrict arg //默认为NULL。若上述函数需要参数，将参数放入结构中并将地址作为arg传入。
                  );
```
