#include <stdio.h>
#include "cache.h"
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


void parse_trace_and_run(struct Param* parameters) {
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
            if (!result) 
			    CacheL1.replace_block(index, tag, false);
		}
		else if (r_w == 'w') {
			result = CacheL1.writeToAddress(index, tag);
		    // time (9) WTNA no replace no dirty_bit
			if (!result) {
				if (parameters->L1_WRITE_POLICY)
				    continue;
				else if (!parameters->L1_WRITE_POLICY)
				    CacheL1.replace_block(index, tag, true);
			}
		}
	}
	f.close();

	// When I try to give_output(struct Param* parameters, class Cache CacheL1);
	// Got free(): double free detected in tcache 2
    //     Aborted (core dumped)
	give_output(parameters, CacheL1.totalReads, \
	    CacheL1.totalWrites, \
		CacheL1.missReads, \
		CacheL1.missWrites, \
		CacheL1.writeBacks, CacheL1.cache_sets);
}


void give_output(struct Param* parameters, int totalReads, int totalWrites, int missReads, int missWrites, int writeBacks, Block** cache_sets) {
	cout << "  ===== Simulator configuration =====\n";
    cout << "  L1_BLOCKSIZE:" << setw(22) << parameters->L1_BLOCKSIZE << endl;
    cout << "  L1_SIZE:" << setw(27) << parameters->L1_SIZE << endl;
    cout << "  L1_ASSOC:" << setw(26) << parameters->L1_ASSOC << endl;
    cout << "  L1_REPLACEMENT_POLICY:" << setw(13) << parameters->L1_REPLACEMENT_POLICY << endl;
    cout << "  L1_WRITE_POLICY:" << setw(19) << parameters->L1_WRITE_POLICY << endl;
    cout << "  trace_file:" << setw(24) << parameters->trace_file << endl;
    cout << "  ===================================\n\n";

    cout << "===== L1 contents =====\n";
	for(int point = 0; point < parameters->L1_SIZE / (parameters->L1_BLOCKSIZE * parameters->L1_ASSOC); point++) {
        cout << "set" << setw(4) << point << ":";
        for(int inPoint = 0; inPoint < parameters->L1_ASSOC; inPoint++) {
            cout << hex << setw(8) << cache_sets[point][inPoint].tag << ' ' << ((!parameters->L1_WRITE_POLICY && cache_sets[point][inPoint].dirty_bit) ? 'D' : ' ');
		}
		cout << endl << dec; // later, result dec count
    }

    cout << "\n  ====== Simulation results (raw) ======\n";
    cout << "  a. number of L1 reads:" << setw(16) << totalReads << endl;
	cout << "  b. number of L1 read misses:" << setw(10) << missReads << endl;
	cout << "  c. number of L1 writes:" << setw(15) << totalWrites << endl;
    cout << "  d. number of L1 write misses:" << setw(9) << missWrites << endl;
    cout << "  e. L1 miss rate:" << setw(22) << fixed << setprecision(4) << (missReads + missWrites) / (double)(totalReads + totalWrites) << endl;
    cout << "  f. number of writebacks from L1:" << setw(6) << writeBacks << endl;
    if (parameters->L1_WRITE_POLICY) {
        cout << "  g. total memory traffic:" << setw(14) << missReads + totalWrites << endl;
	} else if (!parameters->L1_WRITE_POLICY) {
		cout << "  g. total memory traffic:" << setw(14) << missReads + missWrites + writeBacks << endl;
	}

    // Average Access Time = Hit L1 + L1 Miss Rate * MissPenalty L1 
	// Hit L1 (ns) =  0.25 + 2.5 * (L1 CacheSize / 512KB) + 0.025 * (L1 BlockSize / 16B) + 0.025 * assoc;
	double L1Hit = 0.25 + 2.5 * (parameters->L1_SIZE / (512.0 * 1024)) + 0.025 * (parameters->L1_BLOCKSIZE / 16.0) + 0.025 * parameters->L1_ASSOC;
	double L1MissRate = (missReads + missWrites) / (float)(totalReads + totalWrites);
	double L1MissPenalty = 20 + 0.5 * (parameters->L1_BLOCKSIZE / 16.0);
	cout << "\n  ==== Simulation results (performance) ====\n";
    cout << "  1. average access time:" << setw(15) << L1Hit + L1MissRate * L1MissPenalty << " ns\n";

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
			cache_sets[i][j].dirty_bit = 0;
			cache_sets[i][j].tag = -1;
			cache_sets[i][j].COUNT_BLOCK = -1;
		}
	}
    COUNT_SET = (unsigned int*)malloc(sizeof(unsigned int) * setnum);
	for (int i=0; i < setnum; ++i) {
		COUNT_SET[i] = 0;
	}
	//cout << "Create an object for Cache" << endl;
}
Cache::~Cache(void) {
	//cout << cache_sets[0][0].stamp << endl;
	for (int i = 0; i < setnum; ++i) {
	    free(cache_sets[i]);
	}
	free(cache_sets);
	free(COUNT_SET);
}


bool Cache::readFromAddress(unsigned int index, unsigned int tag) {
    totalReads++;
	// max index = 256
    for ( int i=0; i < assoc; i++) {
        // valid and tag exists
		if (cache_sets[index][i].valid_bit && cache_sets[index][i].tag==tag) {
			// time (13) LRU Least Recently Used, Age Update
            if (replace == 0) {
				for (int others=0; others < assoc; others++) {
					if( cache_sets[index][others].valid_bit && 
					    cache_sets[index][others].COUNT_BLOCK < cache_sets[index][i].COUNT_BLOCK ) {
							cache_sets[index][others].COUNT_BLOCK++;
						}
				}
			}

			// time (5) LFU Least Frequently Used
			// Update ages
			cache_sets[index][i].COUNT_BLOCK = (replace? cache_sets[index][i].COUNT_BLOCK+1 : 0);
			return true;
		}
	}
    // time (0) No before
	missReads++;
	return false;
}

void Cache::replace_block(unsigned int index, unsigned int tag, bool IsW) {
    // time (1)
	int i;
	for ( i=0; i < assoc; i++) {
		if (cache_sets[index][i].valid_bit == 0) {
			// fetch one to replace
			break;
		}
	}
	// time (4) full, replace one
    if ( i == assoc ) {
		// LFU
		if (replace) {
			unsigned int min_age = 10000;
			//cout << min_age <<endl;
			for (int k=0; k < assoc; k++) {
				if (cache_sets[index][k].COUNT_BLOCK < min_age) {
					i = k;
					min_age = cache_sets[index][i].COUNT_BLOCK;
				}
			}
		} 
		// time (12) LRU Least Recently Used
		else if (!replace) {
            unsigned int max_age = -1; 
			// cout << int(max_age) << endl;
			for (int k=0; k < assoc; k++) {
				if (int(cache_sets[index][k].COUNT_BLOCK) > int(max_age)) {
                    i = k;
					max_age = cache_sets[index][i].COUNT_BLOCK;
				}
			}
		}

	}
    
	// time (10) WBWA has been write, dirty
    if (cache_sets[index][i].valid_bit && cache_sets[index][i].dirty_bit) {
		writeBacks++;
	}

	// time (2) read miss get one to place 
	// with LFU, update stamp
	if (cache_sets[index][i].valid_bit && replace) {
        COUNT_SET[index] = cache_sets[index][i].COUNT_BLOCK;
	}

	// time (3) Update NO matter whether replace
    cache_sets[index][i].COUNT_BLOCK = (replace ? COUNT_SET[index]+1 : 0);
	cache_sets[index][i].tag         = tag;
	cache_sets[index][i].dirty_bit   = IsW; 
	cache_sets[index][i].valid_bit   = true;

	// time (11) LRU Least Recently Used
	// other blocks in a set age++ together
	if (replace == 0) {
		for (int ii=0; ii < assoc; ii++) {
            if (i != ii && int(cache_sets[index][ii].COUNT_BLOCK) != -1) {
				cache_sets[index][ii].COUNT_BLOCK++;
			}
		}
	}
}

bool Cache::writeToAddress(unsigned int index, unsigned int tag) {
    totalWrites++;
    // time (6)
    for (int i=0; i < assoc; i++) {
		if (cache_sets[index][i].valid_bit && cache_sets[index][i].tag==tag) {
		    // time (13) LRU Least Recently Used, Age Update
            if (replace == 0) {
				for (int others=0; others < assoc; others++) {
					if( cache_sets[index][others].valid_bit && 
					    cache_sets[index][others].COUNT_BLOCK < cache_sets[index][i].COUNT_BLOCK ) {
							cache_sets[index][others].COUNT_BLOCK++;
						}
				}
			}
			// time (7) LFU
		    cache_sets[index][i].COUNT_BLOCK = (replace? cache_sets[index][i].COUNT_BLOCK+1 : 0);
		    cache_sets[index][i].dirty_bit   = cache_sets[index][i].dirty_bit | (!write); 
		    return true;
		}
	}
	// time (8)
	missWrites++;
	return false;
}