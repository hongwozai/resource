/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Tools.h
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-29
 **
 ** 描  述：工具函数
 ** 注  意：1.
 ********************************************************************/
#ifndef TOOLS_H
#define TOOLS_H

class Tools
{
public:

    /**
     * 使用指定长度进行搜索
     * @return 返回负值即没有命中，正值为pattern在text中的索引
     */
    static int Search(const char* pattern, int plen,
                      const char *text,    int tlen);

    /**
     * 位图的三种操作
     */
    static inline void BitSet(void *set, int n);

    static inline bool BitCheck(void *set, int n);

    static inline void BitUnset(void *set, int n);

    /**
     * 以hexdump的格式打印内存
     */
    static int Hexdump(void *data, int len);

    /**
     * 打开文件操作
     * delete[]释放data的内存
     */
    static long FileFetch(const char *file, char *&data,
                          long offset = 0,
                          long max    = 0);

    static long FileStore(const char *file, char *data, long len);

    static long FileSize(const char *file);

};

inline void Tools::BitSet(void *set, int n)
{
    ((((char*)(set))[(n) >> 3]) |= (1 << ((n) & 0x7)));
}

inline void Tools::BitUnset(void *set, int n)
{
    ((((char*)(set))[(n) >> 3]) &= (~(1 << ((n) & 0x7))));
}

inline bool BitCheck(void *set, int n)
{
    return ((((char*)(set))[(n) >> 3]) & (1 << ((n) & 0x7)));
}

#endif /* TOOLS_H */
