/********************************************************************
 ** Copyright(c) 2016, luzeya
 ** All rights reserved
 **
 ** 文件名：AC.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2016-12-21
 **
 ** 描  述：简单的AC自动机
 ** 注  意：1. 内存无法申请过大，因为这里会一次性申请足够大的空间来存放转移表
 **       2. 优化有两点： 1. 使用二维数组代表trie
 **                    2. 将失败指针放入转移表中，并不专门走失败指针
 **       3. 还可以优化一点：将二维数组变成一维数组，可以省去乘法开销
 ********************************************************************/
#include <cstdio>
#include <cctype>
#include <cstring>
#include <cstdlib>

#include "AC.h"

#ifdef AC_DEBUG
#define dbg(fmt, args...) printf("[%s %d] "fmt, __FILE__, __LINE__, ##args)
#else
#define dbg(fmt, args...)
#endif

int AC::init(const char **wordvec, int len)
{
    memset(this, 0, sizeof(*this));

    // 根据字符数组来构造单词表
    if (wordvec != NULL) {
        words = (Keyword*)malloc(sizeof(Keyword) * len);
        if (!wordvec)
            return -1;

        nwords = len;
        for (int i = 0; i < len; i++) {
            words[wordslen].word = (char*)wordvec[i];
            words[wordslen].type = -1;
            words[wordslen].wordlen = strlen(wordvec[i]);
            words[wordslen].ptr = NULL;
            wordslen++;
        }
    }
    return 0;
}

int AC::add(char *word, int len, int type, void *ptr)
{
    void *temp;
    Keyword w = {type, len, word, ptr};

#define MIN_NUM 12
    // 没有空间
    if (!words) {
        words = (Keyword*)malloc(sizeof(Keyword) * MIN_NUM);
        if (!words)
            return -1;

        nwords   = MIN_NUM;
        wordslen = 0;
    }

    // 空间太小
    if (wordslen == nwords) {
        temp = realloc(words, sizeof(Keyword) * nwords * 2);
        if (!temp)
            return -1;

        words = (Keyword*)temp;
        nwords = nwords * 2;
    }
    words[wordslen++] = w;
    return 0;
}

int AC::build()
{
    if (0 != buildTable())
        return -1;
    if (0 != buildFailPtr())
        return -1;
    return 0;
}

/**
 * buildTable
 * 根据单词列表添加至表中
 * @return 内存申请成功或失败
 */
int AC::buildTable()
{
    // 单词所有字符的总长度
    int total = 0;
    // 内存分配指针，已分配根节点
    int alloc = 1;

    // realloc失败时错误处理使用
    void *temp;

    // 计算所有单词总字符数
    for (int i = 0; i < wordslen; i++) {
        total += words[i].wordlen;
    }
    dbg("total: %d\n", total);

    // 申请最大的空间（假设所有字符均不一样）
    // 状态数比字符数多一，指针数组的最后一位用作单词索引
#define MAX_MEMORY (sizeof(State) * (total + 1) * (ALPHABET_NUM + 1))
    table = (State(*)[ALPHABET_NUM+1])malloc(MAX_MEMORY);

    if (!table)
        return -1;

    memset(table, 0, MAX_MEMORY);
    dbg("total alloc: %ld\n", MAX_MEMORY);

    // 逐个添加单词
    for (int i = 0; i < wordslen; i++) {
        int cur = 0;
        for (int j = 0; j < words[i].wordlen; j++) {
            int c = words[i].word[j];

            if (icase)
                c = tolower(c);

            if (table[cur][c] != 0) {
                cur = table[cur][c];
            } else {
                table[cur][c] = alloc;
                cur = alloc;
                alloc++;
            }
        }
        table[cur][ALPHABET_NUM] = i + 1;
    }

    // alloc从0开始分配，值正好是状态的个数
    statenum = alloc;

    // 重新分配内存到合适的大小
    temp = realloc(table, sizeof(State) * (alloc) * (ALPHABET_NUM + 1));
    if (!temp) {
        free(table);
        return -2;
    }
    table = (State(*)[ALPHABET_NUM+1])temp;

    // 调试
    dbg("%d, alloc: %ld\n", alloc,
        sizeof(State) * (alloc) * (ALPHABET_NUM + 1));
    return 0;
}

/**
 * buildFailPtr
 * 广度优先遍历bfs(层次遍历)构造失败指针
 * 并根据失败指针构造状态转移图
 * @param
 * @return
 */
int AC::buildFailPtr()
{
    int head = -1, tail = 0, len = 0;
    State *queue, *failq;

    // 循环队列
    queue = (State*)malloc(sizeof(State) * statenum);
    if (!queue)
        return -1;
    failq  = (State*)malloc(sizeof(State) * statenum);
    if (!failq) {
        free(queue);
        return -2;
    }

    // 根节点入队
    queue[tail] = 0;
    failq[tail] = 0;
    tail = (tail + 1) % statenum;
    len++;

    // 构建失败指针
    while (len != 0) {
        int cur, curfail;
        // 出队
        head = (head + 1) % statenum;
        cur = queue[head];
        curfail = failq[head];
        len--;

        for (int i = 0; i < ALPHABET_NUM; i++) {
            int childfail, child;

            child = table[cur][i];
            if (child == 0)
                continue;

            // 第一层失败指针为根节点
            // 子节点的失败指针指向父节点的失败指针指向节点的相应子节点
            // 否则直接指向根节点
            if (cur == 0) {
                childfail = 0;
            } else if (table[curfail][i] != 0) {
                childfail = table[curfail][i];
            } else {
                childfail = 0;
            }

            queue[tail] = child;
            failq[tail] = childfail;
            tail = (tail + 1) % statenum;
            len++;
        }

        // 根节点不做更改
        if (cur == 0)
            continue;

        // 构造跳转表，根据失败指针填写当前节点的next数组
        for (int i = 0; i < ALPHABET_NUM; i++) {
            if (table[curfail][i] == 0)
                continue;
            if (table[cur][i] == 0)
                table[cur][i] = table[curfail][i];
        }
    }

    // 释放空间
    free(queue);
    free(failq);
    return 0;
}

/**
 * search
 *
 * @param data   数据
 * @param len    数据长度
 * @param end    单词匹配的结尾，命中即返的情况下
 * @param s      流状态保存
 * @return
 */
int AC::search(char *data, int len, int *end, Stream *s)
{
    State k = s ? s->state : 0;

    for (int i = 0; i < len; i++) {
        int index;
        unsigned char c = icase ? tolower(data[i]) : data[i];

        k     = table[k][c];
        index = table[k][ALPHABET_NUM];

        // 命中
        if (index != 0) {

            // 命中即返的情况下，找到匹配的单词末尾
            if (end)
                *end = i + 1;

            // 调试
            dbg("word index: %d\n", index - 1);

            // 立即返回索引
            return index - 1;
        }
    }
    if (s)
        s->state = k;
    return -1;
}

int AC::search(char *data, int len, walk *walk, void *arg, Stream *s)
{
    int find = 0;
    State k = s ? s->state : 0;

    for (int i = 0; i < len; i++) {
        int index;
        unsigned char c = icase ? tolower(data[i]) : data[i];

        k     = table[k][c];
        index = table[k][ALPHABET_NUM];

        // 命中
        if (index != 0) {
            int ret;
            Keyword *w = &words[index - 1];

            find++;

            // 调试
            dbg("word index: %d\n", index - 1);

            // 立即返回索引
            ret = walk(w, i + 1, arg);

            if (ret < 0) { return ret; }

            if (w->type < 0) { return find; }
        }
    }
    if (s)
        s->state = k;
    return find;
}

AC::Keyword *AC::get(int index)
{
    return &words[index - 1];
}

void AC::destroy()
{
    free(table);
    free(words);
}