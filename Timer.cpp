/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Timer.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-28
 **
 ** 描  述：定时器，微秒级别
 ** 注  意：1.
 ********************************************************************/
#include <ctime>
#include <cstdio>

extern "C" {
#include <sys/time.h>
}

#include "Timer.h"

void Timer::start()
{
    struct timeval *tv = (struct timeval*)startTime;
    gettimeofday(tv, 0);
}

void Timer::end()
{
    uint64_t curTime;
    struct timeval *start = (struct timeval*)startTime;
    struct timeval *end = (struct timeval*)endTime;

    gettimeofday(end, 0);
    curTime =
        (end->tv_sec * 1000 * 1000 + end->tv_usec) -
        (start->tv_sec * 1000 * 1000 + start->tv_usec);
    maxTime = (curTime > maxTime) ? curTime : maxTime;
    averageTime += ((double)curTime - averageTime) / (count + 1);
    count++;
}

void Timer::report(const char *re)
{
    printf("Timer(%s): average: %lu us, max %lu us, count: %lu\n",
           re ? re : "",
           averageTime,
           maxTime,
           count);
}