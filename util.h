#ifndef _UTIL_H_
#define _UTIL_H_

#include "stdlib.h"
#include "stdio.h"
#include <string>
#include <iostream>
#include <iomanip>
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
void give_output(struct Param* parameters);

#endif // _UTIL_H
