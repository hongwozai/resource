/********************************************************************
 ** Copyright(c) 2016, luzeya
 ** All rights reserved
 **
 ** 文件名：Thread.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2016-11-27
 **
 ** 描  述：posix 线程，互斥量封装
 ** 注  意：1.
 ********************************************************************/
#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>

class ThreadError
{
public:

    ThreadError(int err) : err(err) {}

public:

    int err;
};

class Thread
{
public:

    Thread() : thread_id(0), isJoinable(false), func(wrap), arg(NULL)  {}

    // 函数参数可以在此处传入，也可以在之后传入
    Thread(void *(*run)(void *), void *arg = NULL)
        : isJoinable(false), func(run), arg(arg) {}

    // 要显式调用join
    virtual ~Thread() { join(); }

    pthread_t self()  { return thread_id; }

    bool joinable()   { return isJoinable; }

    inline void start();

    inline void *join();

    inline void detach();

    inline void cancel();

private:

    /**
     * 这两个函数用于继承式使用
     */
    // 线程真正的运行函数，子类覆盖该函数即可
    virtual void *run(void *arg) { (void)arg; return NULL; }

    // 线程运行包裹函数
    static void *wrap(void *thread) {
        Thread *th = static_cast<Thread*>(thread);
        return th->run(th->arg);
    }

// 此处线程运行的基本函数
private:

    pthread_t thread_id;

    bool isJoinable;

    // 如果不想使用继承方式运行线程，可以直接复制使用
    void *(*func)(void *);

    // 这个参数是真正运行函数的参数，继承方式可以不使用
    void *arg;

};

inline void Thread::start()
{
    int retval;
    retval = pthread_create(&thread_id, NULL,
                            this->func, (func == wrap) ? this : this->arg);
    if (retval != 0)
        throw ThreadError(retval);
    isJoinable = true;
}

inline void *Thread::join()
{
    void *retval = NULL;

    if (isJoinable) {
        int ret = pthread_join(thread_id, &retval);
        if (ret != 0)
            throw ThreadError(ret);

        isJoinable = false;
        return retval;
    }
    return NULL;
}

inline void Thread::detach()
{
    int retval = pthread_detach(thread_id);
    if (retval != 0)
        throw ThreadError(retval);
    isJoinable = false;
}

inline void Thread::cancel()
{
    int retval = pthread_cancel(thread_id);
    if (retval != 0)
        throw ThreadError(retval);
}

class Mutex
{
public:

    inline Mutex(bool isrecur = false);

    inline ~Mutex();

    inline void acquire();

    inline void release();

public:

    // 自动锁
    class Auto
    {
    public:

        Auto(Mutex &mutex) : mutex(mutex) { mutex.acquire(); }

        ~Auto() { mutex.release(); }

    private:

        Mutex &mutex;
    };

private:

    pthread_mutex_t mutex;
};

inline Mutex::Mutex(bool isrecur)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex, isrecur ? &attr : 0);
    pthread_mutexattr_destroy(&attr);
}

inline Mutex::~Mutex()
{
    pthread_mutex_destroy(&mutex);
}

inline void Mutex::acquire()
{
    pthread_mutex_lock(&mutex);
}

inline void Mutex::release()
{
    pthread_mutex_unlock(&mutex);
}

#endif /* THREAD_H */