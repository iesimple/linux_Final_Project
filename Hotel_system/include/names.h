#ifndef HOTEL_SYSTEM_NAMES
#define HOTEL_SYSTEM_NAMES

#define ROOM_INFO_SHM "roomInfo_shm"       // 存储房间信息的共享内存区的name
#define RESERVE_INFO_SHM "reserveInfo_shm" // 存储预约信息的共享内存区的name
// 信号量
#define MUTEX_SEM "mutex_sem"   // 操作共享内存区的时候起保护作用的信号量
// #define ROOM_INFO_SEM "roomInfo_sem"       // 保护对房间信息共享内存区修改的信号量
// #define RESERVE_INFO_SEM "reserveInfo_sem" // 保护对预约信息共享内存区修改的信号量
#define PROCESS_NUM_SEM "process_num_sem"  // 限制最大并发线程数

#define FILE_PATH "../resource/in.txt" // 输入文件的默认位置

#endif