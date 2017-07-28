/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：EmbedList.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-29
 **
 ** 描  述：嵌入形式的链表，类似于内核链表
 ** 注  意：1.
 ********************************************************************/
#ifndef EMBEDLIST_H
#define EMBEDLIST_H

typedef struct EmbedListLink {
    EmbedListLink *prev;
    EmbedListLink *next;
} EmbedListLink;

template <class T>
class EmbedList
{
public:

    typedef EmbedListLink Link;

public:

    EmbedList() : head(0), tail(0), len(0), offset(0) {}

    inline void  attach(Link *node, T *data);

    inline void  detach(Link *node);

    T*    containerOf(Link *data) { return (T*)((char*)data - offset); }

    void  append(T *data) { attach(tail, data); }

    void  push(T   *data) { attach(tail, data); }

    void  del(T    *data) { detach((Link*)((char*)data + offset)); }

    T*    pop() { T* ret = containerOf(head); detach(head); return ret; }

    T*    popBack() { T* ret = containerOf(tail); detach(tail); return ret; }

public:

    Link *head;

    Link *tail;

    int   len;

    int   offset;
};

template <class T>
inline void EmbedList<T>::attach(Link *node, T* data)
{
    Link *temp = (Link*)((char*)data + offset);

    temp->prev = node;
    temp->next = (node == 0) ? head : node->next;
    if (temp->prev == 0) head = temp;
    else                 temp->prev->next = temp;
    if (temp->next == 0) tail = temp;
    else                 temp->next->prev = temp;
    len++;
}

template <class T>
inline void EmbedList<T>::detach(Link *node)
{
    if (node->prev == 0) head = node->next;
    else                 node->prev->next = node->next;
    if (node->next == 0) tail = node->prev;
    else                 node->next->prev = node->prev;
    len--;
}

#endif /* EMBEDLIST_H */
