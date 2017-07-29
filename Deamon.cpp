/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Daemon.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-06-04
 **
 ** 描  述：守护进程
 ** 注  意：
 ********************************************************************/
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <sys/types.h>

int Daemon(int argc, char *argv[])
{
    int ret, status, fd;

    // 唯一
    fd = open(argv[0], O_RDONLY);
    if (fd < 0) {
        printf("%s Open Error:(%m)\n", argv[0]);
        return -1;
    }

    ret = flock(fd, LOCK_EX | LOCK_NB);
    if (ret < 0) {
        printf("Deamon Process Started (%m)\n");
        return -2;
    }

    // 后台守护
    printf("Start Deamon...\n");
    daemon(0, 0);

    // 创建子进程
    while (1) {
        ret = fork();
        if (ret < 0) {
            printf("Daemon Fork Error: %m\n");
            return -1;
        } else if (ret == 0) {
            // child
            if (-1 == execv(argv[1], &argv[1])) {
                printf("Daemon Execv Error: %m\n");
                return -2;
            }
            return 0;
        } else {
            // father
            waitpid(ret, &status, 0);
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        printf("Please Input Arguments!\n");
        return 1;
    }
    Daemon(argc, argv);
    return 0;
}