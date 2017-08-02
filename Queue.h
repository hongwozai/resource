/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Queue.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-31
 **
 ** 描  述：线程安全的链式异步队列
 ** 注  意：1.
 ********************************************************************/
#ifndef QUEUE_H
#define QUEUE_H

#include "Thread.h"
#include "FreeList.h"
#include "Allocator.h"

template <class Type, class Allocator = Allocator<Type> >
class Queue
{
public:

    Queue() : mutex(),
              cond(mutex),
              head(0),
              tail(0),
              freelist(2),
              len(0),
              isdestroy(false) {}

    ~Queue() {}

    // 销毁通知
    void notifyDestroy();

    // 入队
    void enq(const Type &node);

    // 出队
    void deq(Type &node);

private:

    struct Node {
        Node *next;
        Type  type;
    };

    typedef typename Allocator::template rebind<Node>::other NodeAlloc;

private:

    void create();

    // 销毁队列函数
    void destroy();


private:

    /*!< 互斥量 */
    Mutex mutex;

    /*!< 条件变量 */
    Cond cond;

    /*!< 队列指针 */
    Node *head, *tail;

    /*!< 缓存链 */
    FreeList<Node, NodeAlloc> freelist;

    // 队列长度
    int    len;

    // 销毁信号
    bool   isdestroy;
};

template <class Type, class Allocator>
void Queue<Type, Allocator>::notifyDestroy()
{
    Mutex::Auto _l(mutex);
    isdestroy = true;
    cond.notifyAll();
}

template <class Type, class Allocator>
void Queue<Type, Allocator>::destroy()
{
    while (head) {
        Node *temp = head;
        head = head->next;
        freelist.detach(temp);
    }
}

template <class Type, class Allocator>
void Queue<Type, Allocator>::enq(const Type &value)
{
    Node *node;
    Mutex::Auto _l(mutex);

    // 申请节点
    node = freelist.attach();
    node->next = 0;
    node->type = value;

    // 入队
    if (!tail) {
        head = tail = node;
    } else {
        tail->next = node;
        tail = node;
    }
    len++;

    cond.notify();
}

template <class Type, class Allocator>
void Queue<Type, Allocator>::deq(Type &value)
{
    Node *node;
    Mutex::Auto _l(mutex);

    // 当没有销毁信号并且队列为空的时候进行等待
    while (isdestroy == false && head == 0) {
        cond.wait();
    }
    if (isdestroy == true) {
        return;
    }

    // 出队（这时队列一定不为空，可以取消判断）
    node = head;
    head = head->next;
    value = node->type;
    freelist.detach(node);
    len--;
}

#endif /* QUEUE_H */
