/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Dat.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-31
 **
 ** 描  述：双数组trie树
 ** 注  意：1.
 ********************************************************************/
#ifndef DAT_H
#define DAT_H

/* Double-array trie */
class DAT
{
public:

    /*!< 节点 */
    typedef int TrieIndex;

    /**
     * !< 代表字符集中的字符，直接单个字节可以适应所有情况
     * NOTE:但是若想针对性加快速度，可以做字符集的映射
     *      但实际的用处未必很大，所以此处按字节匹配，仅仅保留日后更改的可能
     */
    typedef unsigned char TrieChar;

    enum {
        TRIECHAR_MAX  = 255,
        TRIEINDEX_MAX = 0x7fffffff,
    };

public:

    DAT() : darray(0), ncell(0) { }

    ~DAT() { destroy(); }

    /*!< 获得根节点索引 */
    TrieIndex getRoot() { return ROOT; }

    /*!< 增量接口：向节点index中插入代表字符c的子节点, 返回子节点index */
    TrieIndex insert(TrieIndex index, TrieChar c);

    /**
     * !< 找子节点,有返回true并改变index，没有返回false
     * 查找的关键函数，所以使其内联
     */
    inline bool next(TrieIndex &index, TrieChar c);

    /*!< 是否有该子节点 */
    bool hasChild(TrieIndex index, TrieChar c);

    /*!< 设置的索引值，业务使用 */
    void setIndex(TrieIndex s, TrieIndex i) { darray[s].index = i; }

    /*!< 获取后缀数组对应的索引 */
    TrieIndex getIndex(TrieIndex s) { return darray[s].index; }

    /*!< 获取节点个数长度 */
    TrieIndex getNodeNum() { return ncell; }

    /*!< 获得父节点 */
    TrieIndex getFather(TrieIndex s) { return darray[s].check; }

    /*!< 获得一个节点的所有子节点索引值，必须给childs分配足够的空间 */
    void getChilds(TrieIndex index, TrieIndex *childs, TrieIndex *nchilds);

    /*!< 获得该子节点对应的字符 */
    TrieChar getCh(TrieIndex child);

    /*!< 扩展pool(创建pool)，insert会extend，也可以手动调用extend */
    void extend(TrieIndex index);

    /*!< 用于调试 */
    void print();

private:

    /*!< 销毁池 */
    void destroy();

    /*!< 对冲突的节点重新定位 */
    void relocate(TrieIndex index, TrieIndex newbase);

    /**
     * !< 根据现有的子节点，找到一个可以使用的base
     * syms 至少保证有一个值
     */
    TrieIndex findBase(TrieChar *syms, TrieChar nsym);

    /*!< 分配一个cell, 即将指定节点从链上摘下来 */
    void detachCell(TrieIndex index);

    /*!< 释放一个cell, 即将节点放到链上 */
    void attachCell(TrieIndex index);

private:

    /*!< 获得一个节点的所有子节点，必须给syms分配足够的空间 */
    void getSyms(TrieIndex index, TrieChar *syms, TrieIndex *nsym);

    /*!< 判断现在这个base是否能够使用 */
    bool judgeBase(TrieIndex base, TrieChar *syms, TrieChar nsym);

private:

    struct Cell {
        TrieIndex base;
        TrieIndex check;
        TrieIndex index;
    };

    enum {
        ROOT  = 1,
        FREELIST = 2,
        // pool中最小可以使用的节点, base的最小值
        BEGIN = 3,
        INDEX_DEFAULT = -1,
    };

private:

    /*!< pool, 第0个用作root节点， 第1个用作freelist(循环链表) */
    Cell *darray;

    /*!< darray长度 */
    TrieIndex ncell;
};

inline bool DAT::next(TrieIndex &index, TrieChar c)
{
    TrieIndex base = darray[index].base;
    TrieIndex newi = base + c;

    if (newi >= ncell) {
        return false;
    }
    if (darray[newi].check == index) {
        index = newi;
        return true;
    }
    return false;
}

#endif /* DAT_H */
