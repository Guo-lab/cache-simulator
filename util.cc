#include <stdio.h>
#include "util.h"
#include <string.h>
#include <typeinfo>
#include <fstream>
#include <sstream>

void get_input(int argc, char *argv[], struct Param* parameters) {
	//printf("\nReceive the Input!\n\n");

	for (unsigned int i = 0; i < (unsigned int)argc; i++) {
	   //printf("argument #%d = %s\n", i, argv[i]);
	    if (i == 1)
	   	    parameters->L1_BLOCKSIZE = atoi(argv[i]);
	    else if (i == 2)
	        parameters->L1_SIZE = atoi(argv[i]);
		else if (i == 3)
            parameters->L1_ASSOC = atoi(argv[i]);
		else if (i == 4)
		    parameters->L1_REPLACEMENT_POLICY = atoi(argv[i]);
		else if (i == 5)
            parameters->L1_WRITE_POLICY = atoi(argv[5]);
		else if (i == 6)
            parameters->trace_file = string(argv[6]);
	}

	//printf("\n");

    return;
}


void parse_trace(struct Param* parameters) {
	string path = "../traces/";
	//cout << typeid(path).name() <<endl;
	//cout << typeid(parameters->trace_file).name() << endl;
	string file = path + parameters->trace_file;
	//cout << typeid(file).name() << endl;
    fstream f(file.c_str());

    Cache CacheL1(parameters->L1_SIZE, parameters->L1_ASSOC, parameters->L1_BLOCKSIZE, parameters->L1_REPLACEMENT_POLICY, parameters->L1_WRITE_POLICY, parameters->trace_file);
    
	int cachesize, assoc, blocksize, setnum;
		cachesize = parameters->L1_SIZE;
		assoc     = parameters->L1_ASSOC;
		blocksize = parameters->L1_BLOCKSIZE;
        setnum    = cachesize / (blocksize * assoc);
	bool result;

    string line;
	while(getline(f, line)) {
        char r_w;
        unsigned address; 

		istringstream separate(line);
		separate >> r_w;
		separate >> hex >> address; // convert hex address to dec
		// cout << r_w << " " << address << endl;
    
	    // https://blog.csdn.net/m0_54713489/article/details/125208655
	    // https://www.zhihu.com/question/24612442/answer/156669729
		// https://dagua.infinigo.com/portal.php?mod=view&aid=9923
		// [          32 bit address         ] 
		// [tag 31 11][index 10 5][offset 4 0]
        unsigned int tag = address / (blocksize * setnum);
		// [  address  xor  tag  ][   zero   ]
        unsigned int index = (address ^ (tag * blocksize * setnum)) / blocksize;
		
		if (r_w == 'r') {
			result = CacheL1.readFromAddress(index, tag);

		}
		else if (r_w == 'w') {
			result = CacheL1.writeToAddress(index, tag);
		}
	}
	f.close();
}


void give_output(struct Param* parameters) {
	cout << "  ===== Simulator configuration =====\n";
    cout << "  L1_BLOCKSIZE:" << setw(22) << parameters->L1_BLOCKSIZE << endl;
    cout << "  L1_SIZE:" << setw(27) << parameters->L1_SIZE << endl;
    cout << "  L1_ASSOC:" << setw(26) << parameters->L1_ASSOC << endl;
    cout << "  L1_REPLACEMENT_POLICY:" << setw(13) << parameters->L1_REPLACEMENT_POLICY << endl;
    cout << "  L1_WRITE_POLICY:" << setw(19) << parameters->L1_WRITE_POLICY << endl;
    cout << "  trace_file:" << setw(24) << parameters->trace_file << endl;
    cout << "  ===================================\n\n";

    cout << "===== L1 contents =====\n";

    cout << "\n\n  ==== Simulation results (performance) ====\n";
    cout << "  1. average access time:" ;
	
	cout << "\n\n  ==== Simulation results (performance) ====\n";
    cout << "  1. average access time:" << setw(15) << " \n";
}













/* For class Cache functions */
/* replace 0: LRU  replace 1: LFU */
/* write 0  : WBWA write   1: WTNA*/
Cache::Cache(int _cachesize, int _assoc, int _blocksize, int _replace, int _write, string _trace_file) {
	cachesize  = _cachesize;
	assoc      = _assoc;
	blocksize  = _blocksize;
	replace    = _replace;
	write      = _write;
	trace_file = _trace_file;
	setnum     = cachesize / (blocksize * assoc);
	totalReads = 0;
	missReads  = 0;
	totalWrites= 0;
	missWrites = 0;
	writeBacks = 0;

	cache_sets = (Block**)malloc(sizeof(Block*) * (cachesize / (blocksize * assoc)));
	
	for (int i = 0; i < (cachesize / (blocksize * assoc)); ++i) {
		cache_sets[i] = (Block*)malloc(sizeof(Block) * assoc);
		for (int j = 0; j < assoc; ++j) {
			cache_sets[i][j].valid_bit = 0;
			cache_sets[i][j].tag = -1;
			cache_sets[i][j].stamp = -1;
		}
	}

	//cout << "Create an object for Cache" << endl;
}
Cache::~Cache(void) {
	//cout << cache_sets[0][0].stamp << endl;
	for (int i = 0; i < setnum; ++i) {
	    free(cache_sets[i]);
	}
	free(cache_sets);
}




// (tag, index)
bool Cache::readFromAddress(unsigned int index, unsigned int tag) {
    totalReads++;
	// max index = 256
    for ( int i=0; i < assoc; i++) {
        // valid and tag exists
		if (cache_sets[index][i].valid_bit && cache_sets[index][i].tag==tag) {
			return true;
		}
		// No before
		return false;
	}
}

bool Cache::writeToAddress(unsigned int index, unsigned int tag) {
    totalWrites++;
}