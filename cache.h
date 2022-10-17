#ifndef _CACHE_H_
#define _CACHE_H_

#include "stdlib.h"
#include "stdio.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <typeinfo>
#include <sstream>

using namespace std;

struct Param {
    int BLOCKSIZE;
    int L1_SIZE;
    int L1_ASSOC;
    int Victim_Cache_SIZE;
    int L2_SIZE;
    int L2_ASSOC;
    string trace_file;
};


void get_input(int argc, char *argv[], struct Param* parameters);
void parse_trace_and_run(struct Param* parameters);




typedef struct {
    bool valid_bit, dirty_bit;
    unsigned int tag, COUNT_BLOCK;
} Block;




// LRU Least Recently Used NO COUNT_SET
// and WBWA write-back + write-allocate
class Cache {
    private:

        int cachesize, assoc, blocksize, setnum, victimnum;
        string trace_file;

    public:
        int totalReads, missReads;
        int totalWrites, missWrites, writeBacks;
        
        Block **cache_sets;
        Block *victim_cache;
        
        int swap;
    
        Cache* nextLevel; // 指向下一级Cache 的指针


        Cache(int _cachesize, int _assoc, int _blocksize, string _trace_file, Cache* _nextLevel, int _victimsize);
        ~Cache();

        bool readFromAddress(unsigned address);
        bool writeToAddress(unsigned address);

        void replace_block(unsigned address, bool IsW);
};

void give_output(struct Param* parameters, Cache* CacheL1);



#endif // _CACHE_H
