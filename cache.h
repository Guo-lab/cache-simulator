#ifndef _CACHE_H_
#define _CACHE_H_

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
void parse_trace_and_run(struct Param* parameters);

typedef struct {
    bool valid_bit, dirty_bit;
    unsigned int tag, COUNT_BLOCK;
} Block;

void give_output(struct Param* parameters, int totalReads, int totalWrites, int missReads, int missWrites, int writeBacks, Block** cache_sets);



class Cache {
    private:
        /* Cache 类的数据成员：
            Cache 的配置参数，如cache 容量、相联读、块大小等；
            tag 部分；
            各种计数器变量；
        */
        int cachesize, assoc, blocksize, replace, write, setnum, cachelevel;
        string trace_file;

        //CACHE *nextLevel; // 指向下一级Cache 的指针

        // https://blog.csdn.net/Zhangsama1/article/details/123693862     
    
    public:
        int totalReads, missReads;
        int totalWrites, missWrites, writeBacks;
        
        Block **cache_sets;

        unsigned int *COUNT_SET;   

        Cache(int _cachesize, int _assoc, int _blocksize, int _replace, int _write, string _trace_file);
        ~Cache();

        // Cache 类的成员函数
        bool readFromAddress(unsigned int index, unsigned int tag);
        bool writeToAddress(unsigned int index, unsigned int tag);
        // Cache 其他功能函数
        void replace_block(unsigned int index, unsigned int tag, bool IsW);
};


#endif // _CACHE_H
