# 消息队列

## Posix消息队列

```c
#include <mqueue.h>
mqd_t mq_open(const char *name, int oflags, ...);
                    /* mode_t mode, struct mq_attr *attr */

// 成功返回消息队列描述符，失败返回-1

int mq_close(mqd_t mqdes); // 关闭消息队列，在进程终止后自动执行
int mq_unlink(const char *name); // 使其引用计数器减一（认为name本身占用一个引用数），等计数器为0后，从系统中删除该消息队列

/*每个消息队列含有以下属性*/
struct mq_attr{
    long mq_flags; // 非阻塞标志
    long mq_maxmsg; // 队列的最大消息数
    long mq_msgsize; // 每个消息的最大字节数
    long mq_curmsgs; // 队列的当前消息数
};

// 属性的获取和设置
int mq_getattr(mqd_t mqdes, struct mq_attr *attr);
int mq_setattr(mqd_t mqdes, struct mq_attr *attr, struct mq_attr *oattr);
/*attr中只设置mq_flags; oattr如果非空，之前的属性会保存在这里*/

// 在消息队列放置一个消息和取走一个消息
int mq_send(mqd_t mqdes, const char *ptr, size_t len, unsigned int prio);
ssize_t mq_receive(mqd_t mqdes, char *ptr, size_t len, unsigned int *prio);

/**
 * @brief Posix消息队列允许异步事件通知，以告知何时有一个消息放置到了某个空消息队列中，使得我们不必阻塞在mq_receive中的调用中。
 * 
 * @param notification
 */
int mq_notify(mqd_t mqdes, const struct sigevent *notification);

```

```c
#include <unistd.h>
// 解析命令行参数，opstring是一个字符串，每个字符代表一个选项，可以是除了'-',':',';'以外任何一个可见的ascll字符，特别的，如果一个选项字符后跟着一个冒号，则代表该选项后需要参数
// 每次只解析一个，成功返回1，失败返回-1
// 这函数有点奇怪，看文档好像返回的应该是检测到的char才对
int getopt(int argc, char *const argv[],const char *optstring);

```
