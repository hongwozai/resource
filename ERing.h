/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Ring.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-02-07
 **
 ** 描  述：环的实现（循环双链表）
 ** 注  意：
 ********************************************************************/
#ifndef ERING_H
#define ERING_H

// 链接器
struct ERingLink {
    ERingLink *prev;
    ERingLink *next;
};

template <class T>
class ERing
{
public:

    typedef ERingLink Link;

public:

    ERing(int offset = 0) : head(0), len(0), offset(offset) {}

    inline void attach(Link *item, T *data);

    inline void detach(Link *item);

    T*   containerOf(Link *item) { return (T*)((char*)item - offset); }

    void del(T* data) { detach((Link*)((char*)data + offset)); }

    T*   pop() { T *ret = containerOf(head); detach(head); return ret; }

    void push(T *data) { attach(0, data); }

    void append(T *data) { attach(0, data); head = head->next; }

    void rotate() { head = head->next; }

    void rrotate() { head = head->prev; }

    T   *first() { return head ? containerOf(head) : 0; }

    T   *end() { return head ? ( head->prev ? containerOf(head->prev) : 0) : 0; }

    bool empty() { return (head == 0); }

public:

    Link *head;

    int   len;

    int   offset;
};

template <class T>
inline void ERing<T>::attach(Link *item, T *data)
{
    Link *temp = (Link*)((char*)data + offset);

    temp->prev = (item == 0)
        ? (head == 0) ? temp : head->prev
        : item;

    temp->next = (item == 0)
        ? (head == 0) ? temp : head
        : item->next;

    if (temp->next == head) head = temp;
    if (temp->prev == temp) head = temp;
    else                    temp->prev->next = temp;
    if (temp->next != temp) temp->next->prev = temp;
    len++;
}

template <class T>
inline void ERing<T>::detach(Link *item)
{
    if (item->prev == item) head = 0;
    else                    item->prev->next = item->next;
    if (item->next != item) item->next->prev = item->prev;
    if (item == head) head = item->next;
    len--;
}

#endif /* ERING_H */
