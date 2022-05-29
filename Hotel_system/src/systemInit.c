#include "systemInit.h"
#include <stdio.h>
#include <string.h>

// request解析
/* 合法request
***************************************************************
    reserve 房间号 年 月 日 预约天数 预约姓名 time
    cancel 房间号 年 月 日 预约天数 预约姓名 time
    reserveblock 房间数 第一个房间号 年 月 日 预约天数 预约姓名 time
    cancelblock 房间数 第一个房间号 年 月 日 预约天数 预约姓名 time
    reserveany 房间数 年 月 日 预约天数 预约姓名 time
    cancelany 房间数 年 月 日 预约天数 预约姓名 time
    check 预约姓名 time
***************************************************************
*/
struct request *requestParse(char *line, __ssize_t len) {
    struct request *tmp = (struct request *)malloc(sizeof(struct request));
    int *ptr = &tmp->room_num;
    int i = 0;
    char *token = strsep(&line, " \r\n");
    if (strcmp(token, "end") == 0)
        return NULL;
    memset(tmp, 0, sizeof(struct request));
    strcpy(tmp->command, token); // command

    token = strsep(&line, " \r\n");

    if (strcmp(tmp->command, "check")) {
        while (i < 6 && token != NULL) {
            if (i == 0 && (strcmp(tmp->command, "reserve") == 0 || strcmp(tmp->command, "cancel") == 0)) // 跳过房间数
            {
                i++;
                continue;
            } else if (i == 1 && (strcmp(tmp->command, "reserveany") == 0 || strcmp(tmp->command, "cancelany") == 0)) //跳过房间号
            {
                i++;
                continue;
            }
            *(ptr + i) = atoi(token);
            i++;
            token = strsep(&line, " \r\n");
            printf("%s ", token);
        }
    }
    strcat(tmp->name, token);
    token = strsep(&line, " \r\n");
    tmp->time = atoi(token);
    printf("\n");
    return tmp;
}

void fileReader() {
    FILE *fp;
    int num = 0;
    char *line = NULL;
    struct requestList *tmp;
    size_t linecap = 0;
    __ssize_t length;
    all_requests[num].listHead = all_requests[num].listTail = NULL;
    // 文件读取
    fp = fopen(FILE_PATH, "r");
    fscanf(fp, "%d", &total_room);
    // 读取宾馆房间号信息
    for (int i = 0; i < total_room; i++)
        fscanf(fp, "%d", &room_id[i]);
    // 读取客人请求信息
    fscanf(fp, "%d", &total_customer);
    while (length = getline(&line, &linecap, fp) != -1) {
        if (line[0] < 48)
            continue;
        strcpy(all_requests[num].name, line); // 尾部会多一个回车
        while (length = getline(&line, &linecap, fp) != -1) {
            // printf("%s", line);
            tmp = (struct requestList *)malloc(sizeof(struct requestList));
            tmp->ptr = requestParse(line, length);
            if (!tmp->ptr)
                break;
            if (all_requests[num].listHead == NULL) {
                all_requests[num].listHead = tmp;
            } else {
                all_requests[num].listTail->next = tmp;
            }
            all_requests[num].listTail = tmp;
            tmp->next = NULL;
        }
        num++;
    }
    fclose(fp);
}

// 房间信息初始化，应该是用作共享内存区
void room_init() {
}

// 正式开始处理请求
void request_process() {
}

void print_file() {
    // room_id信息
    printf("%d\n", total_room);
    for (int i = 0; i < total_room; i++)
        printf("%d ", room_id[i]);
    printf("\n");
    // 客人请求信息
    for (int i = 0; i < total_customer; i++) {
        printf("%s", all_requests[i].name); // 读入的时候多了一个回车
        struct requestList *p = all_requests[i].listHead;
        while (p != NULL) {
            printf("%s %d %d %d %d %d %d %s %d\n",
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
}