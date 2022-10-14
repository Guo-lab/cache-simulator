#ifndef _UTIL_H_
#define _UTIL_H_

#include "stdlib.h"
#include "stdio.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

struct Param {
    int L1_BLOCKSIZE;
    int L1_SIZE;
    int L1_ASSOC;
    int L1_REPLACEMENT_POLICY;
    int L1_WRITE_POLICY;
    string trace_file;
};

void get_input(int argc, char *argv[], struct Param* parameters);
void parse_trace(struct Param* parameters);
void give_output(struct Param* parameters);


class Cache {
    private:
        /* Cache 类的数据成员：
            Cache 的配置参数，如cache 容量、相联读、块大小等；
            tag 部分；
            各种计数器变量；
        */
        int cachesize, assoc, blocksize, replace, write, setnum, cachelevel;
        string trace_file;
        int totalReads, missReads;
        int totalWrites, missWrites, writeBacks;
        //CACHE *nextLevel; // 指向下一级Cache 的指针

        // https://blog.csdn.net/Zhangsama1/article/details/123693862
        typedef struct {
            bool valid_bit;
            unsigned int tag, stamp;
        } Block;
        Block **cache_sets;
    
    public:
        Cache(int _cachesize, int _assoc, int _blocksize, int _replace, int _write, string _trace_file);
        ~Cache();

        // Cache 类的成员函数
        bool readFromAddress(unsigned int index, unsigned int tag);
        bool writeToAddress(unsigned int index, unsigned int tag);
        // Cache 其他功能函数
};


#endif // _UTIL_H
