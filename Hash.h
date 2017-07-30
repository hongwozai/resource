/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Hash.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-30
 **
 ** 描  述：静态hash表
 ** 注  意：1.
 ********************************************************************/
#ifndef HASH_H
#define HASH_H

#include <cstddef>
#include "FreeList.h"

template <class Key>
class HashDefaultFunc
{
public:

    /*!< 求hash值 */
    inline size_t operator () (const Key &key);

    /*!< 比较是否相等 */
    inline bool   operator () (const Key &one, const Key &other);
};

template <class Key>
inline size_t HashDefaultFunc<Key>::operator () (const Key &key)
{
    char  *p = (char*)&key;
    size_t h = 0;
    for (size_t i = 0; i < sizeof(Key); i++) {
        h = 31 * h + p[i];
    }
    return h;
}

template <class Key>
inline bool HashDefaultFunc<Key>::operator () (const Key &one,
                                               const Key &other)
{
    return one == other;
}

template <class Key, class Value, class Func = HashDefaultFunc<Key> >
class Hash
{
public:

    Hash(size_t mod = 10, unsigned freemod = 1) :
        freelist(freemod), buckets(0), mask((1 << mod) - 1) { create(); }

    ~Hash() { destroy(); }

    Value *find(const Key &key);

    void insert(const Key &key, const Value &value);

    bool del(const Key &key);

    Value &operator [] (const Key &key);

private:

    void create();

    void destroy();

private:

    struct Node {
        Node  *next;
        Node **pprev;
        Key    key;
        Value  value;
    };

    struct Bucket {
        Node *head;

        Bucket() : head(0) {}
    };

private:

    inline Node *findInternal(const Key &key);

private:

    /*!< 缓存链 */
    FreeList<Node> freelist;

    /*!< hash桶数组 */
    Bucket *buckets;

    /*!< 桶个数-1 */
    size_t  mask;

    /*!< 比较函数，hash函数 */
    Func    func;
};

template <class Key, class Value, class Func>
void Hash<Key, Value, Func>::create()
{
    buckets = new Bucket[mask + 1];
}

template <class Key, class Value, class Func>
void Hash<Key, Value, Func>::destroy()
{
    Node *node;
    for (size_t i = 0; i <= mask; i++) {
        node = buckets[i].head;
        if (!node)
            continue;
        if (node->next)
            node->next->pprev = node->pprev;
        if (node->pprev)
            *node->pprev = node->next;
        freelist.detach(node);
    }
    delete[] buckets;
}

template <class Key, class Value, class Func>
inline typename Hash<Key, Value, Func>::Node*
Hash<Key, Value, Func>::findInternal(const Key &key)
{
    Node  *temp;
    size_t index = func(key) & mask;

    for (temp = buckets[index].head; temp; temp = temp->next) {
        if (func(temp->key, key)) {
            return temp;
        }
    }
    return 0;
}

template <class Key, class Value, class Func>
Value *Hash<Key, Value, Func>::find(const Key &key)
{
    Node *node = findInternal(key);
    return node ? &node->value : 0;
}

template <class Key, class Value, class Func>
void Hash<Key, Value, Func>::insert(const Key &key, const Value &value)
{
    Node  *node;
    size_t index = func(key) & mask;

    // 申请
    node = freelist.attach();
    node->key   = key;
    node->value = value;

    // 头插
    node->next  = buckets[index].head;
    node->pprev = &buckets[index].head;
    if (node->next)
        node->next->pprev = &node->next;
    buckets[index].head = node;
}

template <class Key, class Value, class Func>
bool Hash<Key, Value, Func>::del(const Key &key)
{
    Node *node = findInternal(key);
    if (!node)
        return false;

    if (node->next)
        node->next->pprev = node->pprev;
    if (node->pprev)
        *node->pprev = node->next;

    // 放回链上
    freelist.detach(node);
    return true;
}

template <class Key, class Value, class Func>
Value& Hash<Key, Value, Func>::operator [] (const Key &key)
{
    Node  *node;
    size_t index = func(key) & mask;

    for (node = buckets[index].head; node; node = node->next) {
        if (func(node->key, key)) {
            return node->value;
        }
    }

    // 申请
    node = freelist.attach();
    node->key = key;

    // 头插
    node->next  = buckets[index].head;
    node->pprev = &buckets[index].head;
    if (node->next)
        node->next->pprev = &node->next;
    buckets[index].head = node;
    return node->value;
}

#endif /* HASH_H */
