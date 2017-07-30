/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：ThreadPool.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-31
 **
 ** 描  述：固定个数的线程池
 ** 注  意：1.
 ********************************************************************/
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "Thread.h"
#include "Queue.h"

class ThreadPool
{
public:

    inline ThreadPool(int num);

    inline ~ThreadPool();

    inline void start();

    inline void add(void *(*run)(void *), void *arg = 0);

    inline void wait();

private:

    /*!< 此处inline不可能起作用，仅仅用作函数写在头文件里的作用 */
    static inline void *workfunc(void *args);

private:

    struct Node {
        void *(*run)(void *);
        void *arg;

        Node() {}

        Node(void *(*run)(void *), void *arg) : run(run), arg(arg) {}
    };

private:

    /*!< 不适用拷贝构造函数 */
    ThreadPool(ThreadPool &pool) {}

private:

    // 线程个数
    int num;

    // 任务队列
    Queue<Node>  queue;

    // 线程记录
    Thread *threads;

    // 线程池销毁信号
    bool isdestroy;

    // 是否已经调用果wait函数
    bool iswait;
};

inline ThreadPool::ThreadPool(int num)
    : num(num), queue(), threads(0), isdestroy(false), iswait(false)
{
    threads = new Thread[num];
    for (int i = 0; i < num; i++) {
        threads[0].set(workfunc, this);
    }
}

inline ThreadPool::~ThreadPool()
{
    if (!iswait)
        wait();
    delete[] threads;
}

inline void ThreadPool::start()
{
    for (int i = 0; i < num; i++) {
        threads[i].start();
    }
}

inline void ThreadPool::add(void *(*run)(void *), void *arg)
{
    queue.enq(Node(run, arg));
}

inline void ThreadPool::wait()
{
    isdestroy = true;
    queue.notifyDestroy();
    for (int i = 0; i < num; i++) {
        threads[i].join();
    }
    iswait = true;
}

void *ThreadPool::workfunc(void *args)
{
    Node node;
    ThreadPool *tp = (ThreadPool*)args;
    while (true) {
        tp->queue.deq(node);
        if (tp->isdestroy)
            break;
        if (node.run) {
            node.run(node.arg);
        }
    }
    return 0;
}

#endif /* THREADPOOL_H */
