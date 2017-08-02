/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Dat.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-08-01
 **
 ** 描  述：双数组trie树
 **        1. 将外围的业务配置等全部脱离开，ac或trie仅仅返回id或指针(配置条数)
 ** 注  意：1. 使用darray的第0个做root节点，第1个做freelist(因为这样可以用负数)
 **        2. darray[1]是freelist的管理结构(head指针)
 **        3. 对freelist来说check等于next，base等于prev
 **        4. freelist中base值(负值)，代表前一个指针(此时check也为负)
 **           正常情况下base值为负值，代表其还没有子节点(此时check为正)
 **        5. NOTE：以后对数组的操作要进行封装，用来保护!!!!
 ********************************************************************/
#include <cstdio>
#include <cctype>
#include <cassert>
#include <cstring>

#include "DAT.h"

void DAT::extend(TrieIndex index)
{
    // 在构建freelist时使用
    TrieIndex old_ncell;

    if (index <= ncell)
        return;

    if (!ncell) ncell = BEGIN + 1;

    if (!darray) {
        // 创建, 下标0,1,2,ncell的项特殊
        // 下标0的是为了防止错误
        // 下标ncell项是为了防止数组越界，并非真的要分配的节点
        darray = new Cell[index + 3];
        ncell  = index + 3;
        darray[0].base         = 0;
        darray[0].check        = 0;
        darray[0].index        = INDEX_DEFAULT;
        darray[ROOT].base      = BEGIN;
        darray[ROOT].check     = ROOT;
        darray[ROOT].index     = INDEX_DEFAULT;
        darray[FREELIST].base  = -FREELIST;
        darray[FREELIST].check = -FREELIST;
        darray[FREELIST].index = INDEX_DEFAULT;

        old_ncell = BEGIN;
    } else {
        Cell *newd = new Cell[index + 3];
        memcpy(newd, darray, ncell * sizeof(Cell));

        delete[] darray;
        darray = newd;
        old_ncell = ncell;
        ncell  = index + 3;
    }

    // 构造freelist,freelist指针用负值标识
    for (TrieIndex i = old_ncell; i < ncell; i++) {
        attachCell(i);
    }
}

void DAT::destroy()
{
    delete [] darray;
}

bool DAT::judgeBase(TrieIndex base, TrieChar *syms, TrieChar nsym)
{
    /**
     * 失败的情况
     * 小于最小值,失败
     * 加上最大的符号大于数组边界，失败
     * 加上符号位等于ROOT,FREELIST等几个节点，失败
     * 有任意一个在sym中子节点被占用的，失败
     */
    if (base < BEGIN || base > ncell - syms[nsym - 1])
        return false;
    for (TrieChar i = 0; i < nsym; i++) {
        TrieIndex child = base + syms[i];
        if (child >= 0 && child <= BEGIN)
            return false;
        if (darray[child].check >= 0)
            return false;
    }
    return true;
}

DAT::TrieIndex DAT::findBase(TrieChar *syms, TrieChar nsym)
{
    // s为当前找到的空闲节点
    TrieIndex s;
    TrieChar  sym;

    assert(nsym >= 1);
    assert(syms != 0);

    sym = syms[0];
    s = -darray[FREELIST].check;

    while (!judgeBase(s - sym, syms, nsym)) {
        if (s == FREELIST) {
            extend(ncell + 1);
        }
        s = - darray[s].check;
    }
    return s - sym;
}

void DAT::detachCell(TrieIndex index)
{
    TrieIndex prev = -darray[index].base;
    TrieIndex next = -darray[index].check;
    darray[prev].check = -next;
    darray[next].base  = -prev;

    darray[index].index = INDEX_DEFAULT;
}

void DAT::attachCell(TrieIndex index)
{
    darray[index].check   = -FREELIST;
    darray[index].base    = darray[FREELIST].base;
    darray[-darray[index].base].check = - (index);

    darray[FREELIST].base = -(index);
    if (darray[FREELIST].check == -FREELIST)
        darray[FREELIST].check = - (index);
    darray[index].index = INDEX_DEFAULT;
}

void DAT::relocate(TrieIndex index, TrieIndex newbase)
{
    TrieIndex oldbase = darray[index].base;
    for (TrieIndex i = 0; i <= TRIECHAR_MAX; i++) {
        if (darray[oldbase + i].check == index) {
            // 链接到freelist上面(循环链表头部)
            attachCell(oldbase + i);
            // 添加真正的节点
            detachCell(newbase + i);
            darray[newbase + i].check = index;
        }
    }
    darray[index].base = newbase;
}

DAT::TrieIndex DAT::insert(TrieIndex index, TrieChar c)
{
    TrieIndex nsym = 1;
    TrieChar syms[TRIECHAR_MAX];
    TrieIndex base;

    if (!darray) {
        extend(TRIECHAR_MAX);
    }

    base = darray[index].base;
    if (base < 0) {
        // 这个节点还没有子节点
        syms[0] = c;
        base = findBase(syms, nsym);
        darray[index].base = base;

    } else {

        // 这个节点已经有子节点了，并且对应的子节点属于自己，什么都不用做
        if (base < (ncell - c) &&
            darray[base + c].check == index) {
            return base + c;
        }

        // 如果数组长度不够，那么扩展之后可以直接加入，不必relocate
        if (base >= (ncell - c)) {
            extend(ncell + c);
            goto alloc;
        }

        /**
         * 这个子节点别人在使用(或是base+c为负，没有位置), 有冲突
         * 先找到一个完全没有冲突的base
         * 再将该节点用newbase全部重新换位置
         */
        if (base < (ncell - c) && darray[base + c].check >= 0) {
            getSyms(index, syms, &nsym);
            syms[nsym++] = c;
            base = findBase(syms, nsym);
            relocate(index, base);
        }
    }

alloc:
    // 此处确保base小于0
    detachCell(base + c);
    darray[base + c].base  = -1;
    darray[base + c].check = index;
    return base + c;
}

void DAT::getChilds(TrieIndex index, TrieIndex *childs, TrieIndex *nchilds)
{
    TrieChar syms[TRIECHAR_MAX];
    getSyms(index, syms, nchilds);
    for (TrieIndex i = 0; i < *nchilds; i++) {
        childs[i] = darray[index].base + syms[i];
    }
}

void DAT::getSyms(TrieIndex index, TrieChar *syms, TrieIndex *nsym)
{
    TrieChar len = 0;

    assert(syms != 0);
    assert(nsym != 0);
    for (TrieIndex i = 0; i <= TRIECHAR_MAX; i++) {
        if ((darray[index].base + i > ncell - 1))
            goto exit;
        if (darray[darray[index].base + i].check == index) {
            syms[len++] = i;
        }
    }
exit:
    *nsym = len;
}

DAT::TrieChar DAT::getCh(TrieIndex child)
{
    if (darray[child].check >= 0)
        return child - darray[darray[child].check].base;
    return 0;
}

bool DAT::hasChild(TrieIndex index, TrieChar c)
{
    if (darray[index].base + c > ncell - 1)
        return false;
    if (darray[darray[index].base + c].check == index) {
        return true;
    }
    return false;
}


void DAT::print()
{
    printf("----%d------------------------------\n", ncell);
    for (TrieIndex i = 0; i < ncell; i++) {
        TrieChar  c;
        TrieIndex index = darray[i].index;

        // 打印当前节点代表的字符
        c = (darray[i].check >= 0) ? getCh(i) : '.';
        c = isprint(c) ? c : '.';

        if (!(i == FREELIST) && darray[i].check < 0)
            continue;

        printf("%d\t%d\t%d\t%c\t%d\n", i, darray[i].base, darray[i].check,
               // 附加信息
               c, index
            );
    }
    printf("=====================================\n");
}