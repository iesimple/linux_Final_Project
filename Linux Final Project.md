# Linux Final Project

## 一、实验目的

实现一个Linux下的多进程并发程序以模拟某个宾馆的预约系统运作情况

## 二、实验方法

啊

## 三、程序框架说明

![程序框架](./img/%E7%A8%8B%E5%BA%8F%E6%A1%86%E6%9E%B6.png)



## 四、代码运行测试

### Quick Start

- ./bin目录下有编译完成可以直接运行的二进制文件main

  ```shell
  ./main [-f <filepath>]
  ```

- 从头开始

  ```shell
  mkdir build
  cd build
  cmake ..
  make
  cd ../bin
  ./main [-f <filepath>]
  ```

### 测试文件结果及说明

默认路径下的测试文件为./bin/input_test.txt

#### 结果

```shell

```

### 测试文件自动生成脚本说明

./src/input_generate.py

参照项目说明文档要求编写

## 五、实验体会

## 六、问题及解决

1. 在使用有名信号量时，最好在sem_open前提前sem_unlink，这是因为在调试的时候，如果某次程序中间出错，没有执行到最后的sem_unlink，文件系统中会存在该name的文件，同时又很不巧地是，在sem_open的oflags参数设置为O_CREAT | O_EXCL，导致打不开已经创建的信号量，在没有设置出错处理函数的时候，很难发现问题所在。

2. UNIX网络编程卷2中提到的，某些需要name的函数，可能需要px_ipx_name进行name的变换，以适应不同系统，在虚拟机里貌似不需要管，直接使用一个字符串即可，而且使用路径还会报如下错误：

   ```shell
   Invalid argument
   errno = 22
   ```

3. strcpy不要写成strcmp，不然怎么复制的过去!!!

4. getline()注意有没有读取到之前的换行符

5. strsep()注意windows下的换行符和linux下的换行符是不一样的!!!

   所以最好用strsep(str, " \r\n")

6. C里面 '' 和 "" 差别很大，前者是字符后者是字符串，在传递参数的时候""是指针，而''却会被认为是个整数!!!