/********************************************************************
 ** Copyright(c) 2017, luzeya
 ** All rights reserved
 **
 ** 文件名：Tools.cpp
 ** 创建人：路泽亚
 ** 创建日期： 2017-07-29
 **
 ** 描  述：工具函数
 ** 注  意：1.
 ********************************************************************/
#include <cstdio>
#include <cctype>
#include <cstdlib>
#include "Tools.h"

int Tools::Search(const char *pattern, int plen,
                  const char *text,    int tlen)
{
    // 文本指针，模式指针
    int i, j;

    if (tlen < plen)
        return -1;

    for (i = 0; i <= tlen - plen; i++) {
        // 比较首尾是否一致来加快速度
        if (text[i] != pattern[0] ||
            text[i + plen - 1] != pattern[plen - 1]) {
            continue;
        }
        // 比较剩下的字符是否相等
        for (j = 1; j < plen - 1; j++) {
            if (text[i + j] != pattern[j]) {
                break;
            }
        }
        if (j == plen - 1)
            return i;
    }
    return -1;
}

#define PCHAR(p, index)                         \
    (isprint((p)[index]) ? ((p)[index]) : '.')

int Tools::Hexdump(void *data, int len)
{
    int off = 0;
    char *p = (char*)data;

    for (; len > 0; len -= 16, p += 16) {
        printf("%08X ", off);
        for (int i = 0; i < 16; i++) {
            if (i == 8) {
                printf(" ");
            }
            printf((i < len) ? " %02X" : "   ", p[i]);
        }
        printf("  |");
        for (int i = 0; i < 16; i++) {
            printf("%c", (i < len) ? (PCHAR(p, i)) : ' ');
            if (i == 7)
                printf(" ");
        }
        printf("|\n");
        off += (len > 16) ? 16 : len;
    }
    printf("%08X\n", off);
    return 0;
}

long Tools::FileFetch(const char *file, char *&data,
                      long offset,
                      long max)
{
    long long size = 0;
    FILE *fp;

    fp = fopen(file, "rb");
    if (!fp) {
        return -1;
    }
    // 获得长度
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    // 申请内存
    data = new char[size];

    // 从offset开始读取
    fseek(fp, offset, SEEK_SET);

    // 读取数据
    if (1 != fread(data, (max < size) ? max : size, 1, fp)) {
        fclose(fp);
        delete[] data;
        return -2;
    }

    fclose(fp);
    return 0;
}

long Tools::FileStore(const char *file, char *data, long len)
{
    FILE *fp;

    fp = fopen(file, "wb");
    if (!fp) {
        return -1;
    }
    if (1 != fwrite(data, len, 1, fp)) {
        fclose(fp);
        return -2;
    }
    fclose(fp);
    return 0;
}

long Tools::FileSize(const char *file)
{
    long long size = 0;
    FILE *fp;

    fp = fopen(file, "rb");
    if (!fp) {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fclose(fp);
    return size;
}