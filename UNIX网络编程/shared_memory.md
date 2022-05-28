# 共享内存区

## 创建并初始化共享内存区

### 相关函数

```c
// 正是这个映射，使得我们在操作文件时跳过了内核
#include <sys/mman.h>
void *mmap(
    void *addr, // 描述符fd被映射到内存中的地址，一般为NULL，表示由linux内核指定
    size_t len, // 文件被映射到内存区的长度
    int prot, // 权限，PROT_READ | PROT_WRITE
    int flags, // MAP_SHARE or MAP_PRITE
    int fd, // 文件描述符
    off_t offset // 偏移量，从文件的哪里开始
    );

// 若成功，则返回为被映射取的起始地址，若出错则返回MAP_FAILED
```

```c
// mmap成功返回之后，fd参数可以关闭，对已建立的映射关系没有影响
#include <sys/mman.h>
int mumap(
    void *addr, // mmap成功返回的地址
    size_t len // 映射区的大小
    );

// 成功返回0，出错返回-1
```

```c
// 对内存中的内容修改之后，内核一般会在之后的某个时刻相应地更新文件
// 但有时我们希望内存中的内容和文件（一般是硬盘中）一致
#include <sys/mman.h>
int msync(
    void *addr, // mmap成功返回的地址
    size_t len, // 映射区的大小
    int flags // MS_ASYNC or MS_SYNC or MS_INVALIDATE
    );

// 成功返回0，出错返回-1
```

### BSD匿名内存映射

```c
// 彻底避免文件的创建和打开（不需要fd描述符）
void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset);
// flags = MAP_SHARE | MAP_ANON
// fd = -1
```

>内核允许映射区的大小（mmap第二个参数）大于实际文件的大小，但是内核会跟踪文件的实际大小，同时使我们无法访问超出实际文件的映射区域。

## Posix共享内存区

无亲缘关系进程间共享内存区

1. 内存映射文件

   ```c
   fd = open(pathname, ...);
   ptr = mmap(..., fd, ...);
   ```

2. 共享内存区对象

   ```c
   fd = shm_open(name, ...);
   ptr = mmap(..., fd, ...);
   ```

```c
#include <sys/mmap.h>
int shm_open(const char* name, int oflag, mode_t mode);
// 成功返回非负描述符，失败返回-1

int shm_unlink(const char* name);
// 成功返回0，失败返回-1
```

```c
// 普通文件和共享内存区的大小都可以通过该函数修改
#include <unistd.h>
int ftruncate(int fd, off_t length);

// 获取有关对象的信息
#include <sys/types.h>
#include <sys/stat.h>
int fstat(int fd, struct stat *buf);
/*
当fd指代一个共享内存区对象时，buf中只有以下四个变量含有信息
*/
struct stat {
    ...
    mode_t st_mode;
    uid_t st_uid;
    gid_t st_gid;
    off_t st_size;
    ...
};

```


