# 代码编写记录

## LOG

5.29 文件输入的代码编写以及测试

输入文件./input.txt

结果如下：

```shell
$ ./main
20
1 2 3 5 6 7 8 9 10 11 13 15 16 17 20 21 22 23 24 25 
cutstomer1
reserve 0 10 2022 3 5 10 sxh 2
reserve 0 10 2022 3 5 10 sxh 1
cancel 0 10 2023 10 2 5 kas 3
cancel 0 10 2023 10 2 5 kas 10
reserveblock 2 1 2022 3 6 4 asd 2
reserveblock 2 1 2022 3 6 4 asd 10
cancelblock 2 1 2022 3 5 3 sacs 5
reserveany 1 0 2022 3 5 5 assad 6
cancelany 1 0 2022 4 5 10 asd 8
check 0 0 0 0 0 0 sadasd 3
----------------------------------------------
cutstomer2
reserve 0 10 2022 3 5 10 sxh 2
reserve 0 10 2022 3 5 10 sxh 1
cancel 0 10 2023 10 2 5 kas 3
cancel 0 10 2023 10 2 5 kas 10
reserveblock 2 1 2022 3 6 4 asd 2
reserveblock 2 1 2022 3 6 4 asd 10
cancelblock 2 1 2022 3 5 3 sacs 5
reserveany 1 0 2022 3 5 5 assad 6
cancelany 1 0 2022 4 5 10 asd 8
check 0 0 0 0 0 0 sadasd 3

```

5.31 reserve & cancel & check代码编写测试

## 待办事项

- reserveblock cancelblock reserverany cancelany
- 搜索优化

## Q&A

1. 裂开，搞错了线程和进程

    >在使用全局变量的时候发现，主进程和他创建的所有不同子线程都是可以访问创建的全局变量的，那么问题来了，既然都可以访问了，我要共享内存区做什么...google...回想以前学的操作系统，一个进程创建的所有线程是共享进程中的资源的，因而线程间通过全局变量就可以达到共同操作同一块数据的作用，只有进程之间才是需要共享内存区来达到协同工作的目的。

2. 指针和共享内存区

    >千万要注意的是，进程本身是不共享内存的，因而普通指针指向的空间只有一个进程可以使用，这也是共享内存区为什么要存在的理由，当然比如文件指针也可以达到进程间协同工作的目的，但是这一类的操作往往需要经过内核，而共享内存区一个至关重要的作用就是让不同的进程可以不经过内核就实现数据共享。

## Q

不理解的问题

1. 父进程fork()出一个子进程之后，子进程是复制了父进程的所有数据？

   ```c
   id = 9863 ptr = 0x559fb6a2d4e0
   id = 9865 ptr = 0x559fb6a2d4e0
   id = 9868 ptr = 0x559fb6a2d4e0
   id = 9866 ptr = 0x559fb6a2d4e0
   id = 9869 ptr = 0x559fb6a2d4e0
   id = 9867 ptr = 0x559fb6a2d4e0
   id = 9870 ptr = 0x559fb6a2d4e0
   id = 9871 ptr = 0x559fb6a2d4e0
   id = 9872 ptr = 0x559fb6a2d4e0
   id = 9864 ptr = 0x559fb6a2d4e0
   parent ptr = 0x559fb6a2d4e0
   ```

   上面是一个父进程parent创建了很多子进程，ptr是一个指针，指向一个结构体变量，按照我的理解，对于每个子进程来说，那个变量应该也被复制了一份，那么在子进程中变量地址的应该改变了吧，为什么还可以用同样的地址来访问呢？（这里做过测试，在所有子进程都通过这个指针都可以读取相同的数据，但是数据修改之后进程之间数据就不一样了）
   这里我也想到有一种可能性，是地址空间在编址的时候，每个有亲缘关系的进程都一样，也就是访问地址的“编号”一样，但是实际的空间不一样，这样就可以解释了。
