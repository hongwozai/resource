/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Timer.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-28
 **
 ** 描  述：时间测量，定时器，微秒级
 ** 注  意：1.
 ********************************************************************/
#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

class Timer
{
public:

    Timer() :
        startTime(),
        endTime(),
        averageTime(0),
        maxTime(0),
        count(0) {}

    ~Timer() {}

    // 开始计时
    void start();

    // 结束计时
    void end();

    // 直接打印结果在屏幕上
    void report(const char *re = "");

    // 返回微秒级别，是平局时间
    uint64_t getTime() { return averageTime; }

    // 返回最大时间
    uint64_t getMaxTime() { return maxTime; }

    // 返回经过的次数
    uint64_t getCount() { return count; }

public:

    // 自动计时内部类
    class Auto
    {
    public:

        Auto(Timer &timer) : timer(timer) { timer.start(); }

        ~Auto() { timer.end(); }

    private:

        Timer &timer;
    };

private:

    // 缓冲区用于存放各个平台的原始数据，最后再进行计算
    // 最后一次的开始时间
    char startTime[32];

    // 最后一次的结束时间
    char endTime[32];

    // 总共的平均时间
    uint64_t averageTime;

    // 最大时间
    uint64_t maxTime;

    // 次数
    uint64_t count;
};

#endif /* TIMER_H */
