#include "cache.h"


void get_input(int argc, char *argv[], struct Param* parameters) {

	for (unsigned int i = 0; i < (unsigned int)argc; i++) {

	    if (i == 1)
	   	    parameters->BLOCKSIZE = atoi(argv[i]);
	    else if (i == 2)
	        parameters->L1_SIZE = atoi(argv[i]);
		else if (i == 3)
            parameters->L1_ASSOC = atoi(argv[i]);
		else if (i == 4)
		    parameters->Victim_Cache_SIZE = atoi(argv[i]);
		else if (i == 5)
            parameters->L2_SIZE = atoi(argv[5]);
		else if (i == 6)
            parameters->L2_ASSOC = atoi(argv[6]);
		else if (i == 7)
            parameters->trace_file = string(argv[7]);
	}

    return;
}


void parse_trace_and_run(struct Param* parameters) {
	string path = "../traces/";
	//cout << typeid(path).name() <<endl;
	//cout << typeid(parameters->trace_file).name() << endl;
	string file = path + parameters->trace_file;
	//cout << typeid(file).name() << endl;
    fstream f(file.c_str());
	
    Cache* CacheL2;
    if (parameters->L2_SIZE == 0) {
		CacheL2 = NULL;
		// cout << "Case 8make: No CacheL2" << endl;
	} else {
		CacheL2 = new Cache(parameters->L2_SIZE, parameters->L2_ASSOC, parameters->BLOCKSIZE, parameters->trace_file, NULL, 0);
	}    
    // Cache CacheL1(parameters->L1_SIZE, parameters->L1_ASSOC, parameters->BLOCKSIZE, parameters->trace_file, CacheL2, parameters->Victim_Cache_SIZE);
    Cache* CacheL1 = new Cache(parameters->L1_SIZE, parameters->L1_ASSOC, parameters->BLOCKSIZE, parameters->trace_file, CacheL2, parameters->Victim_Cache_SIZE);
    
	string line;
	while(getline(f, line)) {
        char r_w;
        unsigned address; 

		istringstream separate(line);
		separate >> r_w;
		separate >> hex >> address; // convert hex address to dec
		// cout << r_w << " " << address << endl;
    
	    // Address parser not here, as L1 and L2 Cache are different.
		
		bool result;
		if (r_w == 'r') {
			result = CacheL1->readFromAddress(address);
            if (!result) {
			    CacheL1->replace_block(address, false);
			}
		}
		else if (r_w == 'w') {
			result = CacheL1->writeToAddress(address);
			if (!result) {
				CacheL1->replace_block(address, true);
			}
		}
	}
	f.close();

	give_output(parameters, CacheL1);

	CacheL1->~Cache();
	if (CacheL2 != NULL) {
	    CacheL2->~Cache();
	}
}


void give_output(struct Param* parameters, Cache* CacheL1) {
	cout << setiosflags(ios::left);
    cout << "===== Simulator configuration =====\n";
    cout << setw(30) << "BLOCKSIZE:" << parameters->BLOCKSIZE << endl;
    cout << setw(30) << "L1_SIZE:" << parameters->L1_SIZE << endl;
    cout << setw(30) << "L1_ASSOC:" << parameters->L1_ASSOC << endl;
    cout << setw(30) << "Victim_Cache_SIZE:" << parameters->Victim_Cache_SIZE << endl;
    cout << setw(30) << "L2_SIZE:" << parameters->L2_SIZE << endl;
    cout << setw(30) << "L2_ASSOC:" << parameters->L2_ASSOC << endl;
    cout << setw(30) << "trace_file:" << parameters->trace_file << endl;
    cout << "===================================\n";

    cout << "===== L1 contents =====\n";
    for (int scan=0; scan < CacheL1->setnum; scan++) {
        cout << "set " << scan << ": ";
		int ii, jj, min;
		Block temp;
		for (ii=0; ii < CacheL1->assoc - 1; ii++) {
            min = ii;
			for (jj = ii+1; jj < CacheL1->assoc; jj++) {
				if (CacheL1->cache_sets[scan][jj].COUNT_BLOCK < CacheL1->cache_sets[scan][min].COUNT_BLOCK) {
					min = jj;
				}
			}
			if (min != ii) {
				temp.COUNT_BLOCK = CacheL1->cache_sets[scan][min].COUNT_BLOCK;
				temp.tag         = CacheL1->cache_sets[scan][min].tag;
				temp.valid_bit   = CacheL1->cache_sets[scan][min].valid_bit;
				temp.dirty_bit   = CacheL1->cache_sets[scan][min].dirty_bit;
				CacheL1->cache_sets[scan][min].COUNT_BLOCK = CacheL1->cache_sets[scan][ii].COUNT_BLOCK;
				CacheL1->cache_sets[scan][min].tag         = CacheL1->cache_sets[scan][ii].tag;
				CacheL1->cache_sets[scan][min].valid_bit   = CacheL1->cache_sets[scan][ii].valid_bit;
				CacheL1->cache_sets[scan][min].dirty_bit   = CacheL1->cache_sets[scan][ii].dirty_bit;
				CacheL1->cache_sets[scan][ii].COUNT_BLOCK = temp.COUNT_BLOCK;
				CacheL1->cache_sets[scan][ii].tag         = temp.tag;
				CacheL1->cache_sets[scan][ii].valid_bit   = temp.valid_bit;
				CacheL1->cache_sets[scan][ii].dirty_bit   = temp.dirty_bit;
			}
		}
		for (int inScan = 0; inScan < CacheL1->assoc; inScan++) {
            cout << hex << CacheL1->cache_sets[scan][inScan].tag << (CacheL1->cache_sets[scan][inScan].dirty_bit ? " D  " : "    ");
		}
        cout << endl << dec;
	}

	if (parameters->Victim_Cache_SIZE != 0) {
        cout << "===== Victim Cache contents =====\n";
        cout << "set " << 0 << ": ";
		int ii, jj, min;
		Block temp;
	    for (ii=0; ii < CacheL1->victimnum - 1; ii++) {
            min = ii;
			for (jj = ii+1; jj < CacheL1->victimnum; jj++) {
	    	    if (CacheL1->victim_cache[jj].COUNT_BLOCK < CacheL1->victim_cache[min].COUNT_BLOCK) {
		    	    min = jj;
			    }
		    }
			if (min != ii) {
		    	temp.COUNT_BLOCK = CacheL1->victim_cache[min].COUNT_BLOCK;
		    	temp.tag         = CacheL1->victim_cache[min].tag;
		    	temp.valid_bit   = CacheL1->victim_cache[min].valid_bit;
		    	temp.dirty_bit   = CacheL1->victim_cache[min].dirty_bit;
		    	CacheL1->victim_cache[min].COUNT_BLOCK = CacheL1->victim_cache[ii].COUNT_BLOCK;
		    	CacheL1->victim_cache[min].tag         = CacheL1->victim_cache[ii].tag;
		    	CacheL1->victim_cache[min].valid_bit   = CacheL1->victim_cache[ii].valid_bit;
		    	CacheL1->victim_cache[min].dirty_bit   = CacheL1->victim_cache[ii].dirty_bit;
		    	CacheL1->victim_cache[ii].COUNT_BLOCK = temp.COUNT_BLOCK;
		    	CacheL1->victim_cache[ii].tag         = temp.tag;
		    	CacheL1->victim_cache[ii].valid_bit   = temp.valid_bit;
		    	CacheL1->victim_cache[ii].dirty_bit   = temp.dirty_bit;
		    }
		}
		for (int inScan = 0; inScan < CacheL1->victimnum; inScan++) {
            cout << hex << CacheL1->victim_cache[inScan].tag << (CacheL1->victim_cache[inScan].dirty_bit ? " D  " : "    ");
	    }
        cout << endl << dec;
	}
	if (parameters->L2_SIZE != 0) {
	    cout << "===== L2 contents =====\n";	
        for (int scan=0; scan < CacheL1->nextLevel->setnum; scan++) {
            cout << "set " << scan << ": ";
		    int ii, jj, min;
		    Block temp;
		    for (ii=0; ii < CacheL1->nextLevel->assoc - 1; ii++) {
                min = ii;
		    	for (jj = ii+1; jj < CacheL1->nextLevel->assoc; jj++) {
		    		if (CacheL1->nextLevel->cache_sets[scan][jj].COUNT_BLOCK < CacheL1->nextLevel->cache_sets[scan][min].COUNT_BLOCK) {
		    			min = jj;
		    		}
		    	}
		    	if (min != ii) {
		    		temp.COUNT_BLOCK = CacheL1->nextLevel->cache_sets[scan][min].COUNT_BLOCK;
		    		temp.tag         = CacheL1->nextLevel->cache_sets[scan][min].tag;
		    		temp.valid_bit   = CacheL1->nextLevel->cache_sets[scan][min].valid_bit;
		    		temp.dirty_bit   = CacheL1->nextLevel->cache_sets[scan][min].dirty_bit;
		    		CacheL1->nextLevel->cache_sets[scan][min].COUNT_BLOCK = CacheL1->nextLevel->cache_sets[scan][ii].COUNT_BLOCK;
		    		CacheL1->nextLevel->cache_sets[scan][min].tag         = CacheL1->nextLevel->cache_sets[scan][ii].tag;
		    		CacheL1->nextLevel->cache_sets[scan][min].valid_bit   = CacheL1->nextLevel->cache_sets[scan][ii].valid_bit;
		    		CacheL1->nextLevel->cache_sets[scan][min].dirty_bit   = CacheL1->nextLevel->cache_sets[scan][ii].dirty_bit;
		    		CacheL1->nextLevel->cache_sets[scan][ii].COUNT_BLOCK = temp.COUNT_BLOCK;
		    		CacheL1->nextLevel->cache_sets[scan][ii].tag         = temp.tag;
		    		CacheL1->nextLevel->cache_sets[scan][ii].valid_bit   = temp.valid_bit;
		    		CacheL1->nextLevel->cache_sets[scan][ii].dirty_bit   = temp.dirty_bit;
		    	}
		    }
		    for (int inScan = 0; inScan < CacheL1->nextLevel->assoc; inScan++) {
                cout << hex << CacheL1->nextLevel->cache_sets[scan][inScan].tag << (CacheL1->nextLevel->cache_sets[scan][inScan].dirty_bit ? " D  " : "    ");
		    }
            cout << endl << dec;
	    }
	}

    cout << "====== Simulation results (raw) ======\n";
    cout << setw(38) << "a. number of L1 reads:" << CacheL1->totalReads << endl;
    cout << setw(38) << "b. number of L1 read misses:" << CacheL1->missReads << endl;
    cout << setw(38) << "c. number of L1 writes:" << CacheL1->totalWrites << endl;
    cout << setw(38) << "d. number of L1 write misses:" << CacheL1->missWrites << endl;
	cout << setw(38) << "e. L1 miss rate:" << fixed << setprecision(4) << (CacheL1->missReads + CacheL1->missWrites) / (double)(CacheL1->totalReads + CacheL1->totalWrites) << endl;
    cout << setw(38) << "f. number of swaps:" << CacheL1->swap << endl;    
    cout << setw(38) << "g. number of victim cache writeback:" << CacheL1->writeBacks << endl;
    cout << setw(38) << "h. number of L2 reads:" << (CacheL1->nextLevel ? CacheL1->nextLevel->totalReads : 0) << endl;
	if (CacheL1->nextLevel == NULL) {
        cout << setw(38) << "i. number of L2 read misses:" << 0 << endl;
        cout << setw(38) << "j. number of L2 writes:" << 0 << endl;
        cout << setw(38) << "k. number of L2 write misses:" << 0 << endl;
        cout << setw(38) << "l. L2 miss rate:" << 0 << endl;
        cout << setw(38) << "m. number of L2 writebacks:" << 0 << endl;		
        cout << setw(38) << "n. total memory traffic:" << CacheL1->missReads + CacheL1->missWrites + CacheL1->writeBacks << endl;
	}
	else {
        cout << setw(38) << "i. number of L2 read misses:" << CacheL1->nextLevel->missReads << endl;
        cout << setw(38) << "j. number of L2 writes:" << CacheL1->nextLevel->totalWrites << endl;
        cout << setw(38) << "k. number of L2 write misses:" << CacheL1->nextLevel->missWrites << endl;
        cout << setw(38) << "l. L2 miss rate:" << fixed << setprecision(4) << CacheL1->nextLevel->missReads / (double)CacheL1->nextLevel->totalReads << endl;
        cout << setw(38) << "m. number of L2 writebacks:" << CacheL1->nextLevel->writeBacks << endl;
        cout << setw(38) << "n. total memory traffic:" << CacheL1->nextLevel->missReads + CacheL1->nextLevel->missWrites + CacheL1->nextLevel->writeBacks << endl;
	}

	// Average Access Time = Hit L1 + L1 Miss Rate * (Hit L2 + L2 Miss Rate * MissPenalty L2) 
	// Hit L1 (ns) =  0.25 + 2.5 * (L1 CacheSize / 512KB) + 0.025 * (L1 BlockSize / 16B) + 0.025 * assoc;
	double L1Hit = 0.25 + 2.5 * (parameters->L1_SIZE / (512.0 * 1024)) + 0.025 * (parameters->BLOCKSIZE / 16.0) + 0.025 * parameters->L1_ASSOC;
	double L1MissRate = (CacheL1->missReads + CacheL1->missWrites) / (double)(CacheL1->totalReads + CacheL1->totalWrites);
	double L2Hit, L2MissRate;
	if (CacheL1->nextLevel == NULL) {
        L2Hit      = 0;
		L2MissRate = 0;
	} else {
	    L2Hit      = 2.5 + 2.5 * (parameters->L2_SIZE / (512.0 * 1024)) + 0.025 * (parameters->BLOCKSIZE / 16.0) + 0.025 * parameters->L2_ASSOC;
	    L2MissRate = CacheL1->nextLevel->missReads / (double)CacheL1->nextLevel->totalReads;
	}
	double L2MissPenalty = 20 + 0.5 * (parameters->BLOCKSIZE / 16.0);
    cout << "==== Simulation results (performance) ====" << endl;
    if (CacheL1->nextLevel == NULL) {
	    cout << setw(32) << "1. average access time:" << setprecision(4) << L1Hit + (L1MissRate * L2MissPenalty) << " ns" << endl;	
	} else {
	    cout << setw(32) << "1. average access time:" << setprecision(4) << L1Hit + (L1MissRate * (L2Hit + L2MissRate * L2MissPenalty)) << " ns" << endl;
	}

}









/* For class Cache functions */
/* replace 0: LRU  replace 1: LFU */
/* write 0  : WBWA write   1: WTNA*/
Cache::Cache(int _cachesize, int _assoc, int _blocksize, string _trace_file, Cache* _nextLevel, int _victimsize) {
	cachesize  = _cachesize;
	assoc      = _assoc;
	blocksize  = _blocksize;
	trace_file = _trace_file;
	setnum     = cachesize / (blocksize * assoc);
	victimnum  = _victimsize / blocksize;
	totalReads = 0;
	missReads  = 0;
	totalWrites= 0;
	missWrites = 0;
	writeBacks = 0;

    // cout << setnum << endl;
	cache_sets = (Block**)malloc(sizeof(Block*) * setnum);
	
	for (int i = 0; i < setnum; ++i) {
		cache_sets[i] = (Block*)malloc(sizeof(Block) * assoc);
		for (int j = 0; j < assoc; ++j) {
			cache_sets[i][j].valid_bit   = 0;
			cache_sets[i][j].dirty_bit   = 0;
			cache_sets[i][j].tag         = -1;
			cache_sets[i][j].COUNT_BLOCK = -1;
		}
	}

    victim_cache = (Block*)malloc(sizeof(Block) * victimnum);
	for (int i=0; i < victimnum; ++i) {
		victim_cache[i].valid_bit   = 0;
		victim_cache[i].dirty_bit   = 0;
		victim_cache[i].tag         = -1;
		victim_cache[i].COUNT_BLOCK = 0;
	}

	swap = 0;
	nextLevel = _nextLevel;
	// cout << nextLevel << endl; // L2 and then L1
	// cout << "Create an object for Cache" << endl; // * 2
}
Cache::~Cache(void) {
	for (int i = 0; i < setnum; ++i) {
	    free(cache_sets[i]);
	}
	free(cache_sets);
	free(victim_cache);
	// cout << "OKK" << endl; 
}



bool Cache::readFromAddress(unsigned address) {
	totalReads++;
	// [          32 bit address         ] 
	// [tag      ][index     ][offset    ]
    unsigned tag = address / (blocksize * setnum);
	// [  address  xor  tag  ][   zero   ]
    unsigned index = (address ^ (tag * blocksize * setnum)) / blocksize;
    // cout << setnum << " " << blocksize << endl; // Case 8: 4 and 32
    // Specifically, for L2, find one valid otherwise replace
    for ( int way = 0; way < assoc; way++) {
        // valid and tag exists
		if (cache_sets[index][way].valid_bit && cache_sets[index][way].tag == tag) {
			// LRU Least Recently Used, Age Update
            for (int others=0; others < assoc; others++) {
				if( cache_sets[index][others].valid_bit && 
				    cache_sets[index][others].COUNT_BLOCK < cache_sets[index][way].COUNT_BLOCK) {
						cache_sets[index][others].COUNT_BLOCK++;
					}
			}
			cache_sets[index][way].COUNT_BLOCK = 0;
			cache_sets[index][way].dirty_bit   = cache_sets[index][way].dirty_bit;
			return true;
		}
	}
    // No before, Check Victim Block
	int vic; // NO Victim Cache Victimnum==0
	for (vic = 0; vic < victimnum; vic++) {
		// victim cache tag
		// [tag      ][index     ][offset    ]
		// [tag                  ][offset    ]
		if (victim_cache[vic].valid_bit && victim_cache[vic].tag == address / blocksize) {
			break;
		}
	}
	// Hit one in Victim Cache
	if (vic != victimnum) { // Swap L1 Cache and Victim Cache
	    swap++;
        Block L_to_swap;  
		unsigned max_age = -1;
		// cache_sets[index][i] is about to be swapped.
		int i = 0; 
		for (int k=0; k < assoc; k++) {
			if (int(cache_sets[index][k].COUNT_BLOCK) > int(max_age)) {
				i = k;
				max_age = cache_sets[index][i].COUNT_BLOCK;
			}
		}
		L_to_swap.valid_bit   = cache_sets[index][i].valid_bit;
		L_to_swap.dirty_bit   = cache_sets[index][i].dirty_bit;
		L_to_swap.tag         = cache_sets[index][i].tag;
		L_to_swap.COUNT_BLOCK = cache_sets[index][i].COUNT_BLOCK; 
        // All blocks in victim_cache, COUNT_BLOCK ++ if < hit_block
		for (int point = 0; point < victimnum; point++) {
			if (victim_cache[point].COUNT_BLOCK < victim_cache[vic].COUNT_BLOCK) {
                victim_cache[point].COUNT_BLOCK++;
			}
		}  
        // All blocks in Cache_sets, COUNT_BLOCK++
		for (int point = 0; point < assoc; point++) {
			cache_sets[index][point].COUNT_BLOCK++;
		}
		// SWAP: victim_hit_block and L1_swap_block COUNT_BLOCK reset
        cache_sets[index][i].valid_bit   = true;
		cache_sets[index][i].dirty_bit   = victim_cache[vic].dirty_bit;
		cache_sets[index][i].COUNT_BLOCK = 0;
		cache_sets[index][i].tag         = tag;
		victim_cache[vic].valid_bit   = true;
		victim_cache[vic].dirty_bit   = L_to_swap.dirty_bit;
		victim_cache[vic].tag         = L_to_swap.tag * (unsigned)setnum + index; // bits and tag recovery
		victim_cache[vic].COUNT_BLOCK = 0; 
		return true;
	}
	// L1 and Victim Miss
	missReads++;
	return false;
}





/* In replace_function, consider that Read or Write in Next Level CacheL2*/
void Cache::replace_block(unsigned address, bool IsW) {
	unsigned tag = address / (blocksize * setnum);
    unsigned index = (address ^ (tag * blocksize * setnum)) / blocksize;
    // Decide which one to replace
	int i;
	for (i = 0; i < assoc; i++) {
		if (cache_sets[index][i].valid_bit == 0) {
			// fetch one to Use. In Cold Phase
			break;
		}
	}
	// Cache full, find one to replace
    if ( i == assoc ) {
		// LRU Least Recently Used
        unsigned max_age = -1; 
		for (int k = 0; k < assoc; k++) {
			if (int(cache_sets[index][k].COUNT_BLOCK) > int(max_age)) {
                i = k;
				max_age = cache_sets[index][i].COUNT_BLOCK;
			}
		}
	}
	// Before replacing this block. Preserve
	Block L_to_replace;
	L_to_replace.COUNT_BLOCK = cache_sets[index][i].COUNT_BLOCK;
	L_to_replace.tag         = cache_sets[index][i].tag;
	L_to_replace.valid_bit   = cache_sets[index][i].valid_bit;
	L_to_replace.dirty_bit   = cache_sets[index][i].dirty_bit;
	// Update NO matter
	cache_sets[index][i].COUNT_BLOCK = 0;
	cache_sets[index][i].tag         = tag;
	cache_sets[index][i].valid_bit   = true;
	cache_sets[index][i].dirty_bit   = IsW;
    // Update age
	for (int others=0; others < assoc; others++) {
		if (others != i) {
		    cache_sets[index][others].COUNT_BLOCK++;
		}
	}
	// cout << "Breakpoint 1" << endl;
    // ONLY IF there is Level 2, Read
	if (L_to_replace.valid_bit == 0) {
		// Just Allocate, No Replace
		// For L2, do not need to send to next level
        if (nextLevel != NULL) {
			// cout << "Breakpoint 2" << endl;
			// nextLevel.function() 
			// error: request for member ‘replace_block’ in ‘((Cache*)this)->Cache::nextLevel’, which is of non-class type ‘Cache*
			bool result2 = nextLevel->readFromAddress(address); // Condition2, Before Writing, Read from next level 
			if (result2 == 0) {
				nextLevel->replace_block(address, false);
			}
		}
	}
	// L_to_replace has been used
	else if (L_to_replace.valid_bit == 1) {
		// Victim Cache Size == 0
		// Replace L_to_replace
		if (victimnum != 0) { // There is Victim Cache.
		// cout << "Breakpoint 3" << endl;
			// Use Vitim Cache to Replace, Similar to Common Cache
			int j;
			for (j = 0; j < victimnum; j++) {
                if (victim_cache[j].valid_bit == 0) {
					// Just replace this block
					break;
				}
 			}
			if (j == victimnum) { // LRU fine max age
				unsigned max_age = -1;
				for (int k = 0; k < victimnum; k++) {
					if (int(victim_cache[k].COUNT_BLOCK) > int(max_age)) {
						j = k;
                        max_age = victim_cache[j].COUNT_BLOCK;
					}
				}
			}
			// Preserve victim block to Replace and Swap
            Block victim_to_replace;
			victim_to_replace.COUNT_BLOCK = victim_cache[j].COUNT_BLOCK;
			victim_to_replace.tag         = victim_cache[j].tag;
			victim_to_replace.valid_bit   = victim_cache[j].valid_bit;
			victim_to_replace.dirty_bit   = victim_cache[j].dirty_bit;
            victim_cache[j].COUNT_BLOCK = 0;
			victim_cache[j].tag         = L_to_replace.tag * (unsigned)setnum + index;
			victim_cache[j].valid_bit   = true;
			victim_cache[j].dirty_bit   = L_to_replace.dirty_bit;
			for (int others = 0; others < victimnum; others++) {
				if (others != j) {
				    victim_cache[others].COUNT_BLOCK++;
				}
			}
            // 如果该LRU块是脏块，则将其写回下一级存储器，然后从下一级存储器读取所需的块
			if (victim_to_replace.dirty_bit == 1) { // Victim writeback
				writeBacks++; // Case 8 ends.
				if (nextLevel != NULL) {
					// address recovery
					bool result = nextLevel->writeToAddress(victim_to_replace.tag * blocksize);
				    if (result == 0) {
						nextLevel->replace_block(victim_to_replace.tag * blocksize, true);
					}
				}
			}
		} // Case 8, stop here.
		// NO Victim Cache
		// valid and dirty both 1
		else if (L_to_replace.dirty_bit == 1) { 
			// cout << "Breakpoint 4" << endl;
			if (nextLevel) {
			    bool result = nextLevel->writeToAddress((L_to_replace.tag * setnum + index) * blocksize);
			    if (result) {
					nextLevel->replace_block((L_to_replace.tag * setnum + index) * blocksize, true);
				}
			}
			// For L1 Cache, No Victim Cache, then write to L2 and replace L2
			// For L2 Cache,
			else {
                writeBacks++;
			}
		}
	}
	// ELSE IF END
}






bool Cache::writeToAddress(unsigned address) {
    totalWrites++;

    unsigned tag = address / (blocksize * setnum);
    unsigned index = (address ^ (tag * blocksize * setnum)) / blocksize;

    for ( int i=0; i < assoc; i++) {
        // valid and tag exists
		if (cache_sets[index][i].valid_bit && cache_sets[index][i].tag == tag) {
			// LRU Age Update
            for (int others=0; others < assoc; others++) {
				if( cache_sets[index][others].valid_bit && 
				    cache_sets[index][others].COUNT_BLOCK < cache_sets[index][i].COUNT_BLOCK ) {
						cache_sets[index][others].COUNT_BLOCK++;
					}
			}
			cache_sets[index][i].COUNT_BLOCK = 0;
			cache_sets[index][i].dirty_bit   = true;
			return true;
		}
	}

    // No before, Check Victim Block
	int vic;
	for (vic = 0; vic < victimnum; vic++) {
		if (victim_cache[vic].valid_bit && victim_cache[vic].tag == address / blocksize) {
			break;
		}
	}
	// Hit one in Victim Cache
	if (vic != victimnum) { // Swap L1 Cache and Victim Cache
	    swap++;
        Block L_to_swap;  
		unsigned max_age = -1;
		// cache_sets[index][i] is about to be swapped.
		int i = 0; 
		for (int k=0; k < assoc; k++) {
			if (int(cache_sets[index][k].COUNT_BLOCK) > int(max_age)) {
				i = k;
				max_age = cache_sets[index][i].COUNT_BLOCK;
			}
		}
		L_to_swap.valid_bit   = cache_sets[index][i].valid_bit;
		L_to_swap.dirty_bit   = cache_sets[index][i].dirty_bit;
		L_to_swap.tag         = cache_sets[index][i].tag;
		L_to_swap.COUNT_BLOCK = cache_sets[index][i].COUNT_BLOCK; 
        // All blocks in victim_cache, COUNT_BLOCK ++ if < hit_block
		for (int point=0; point < victimnum; point++) {
			if (victim_cache[point].COUNT_BLOCK < victim_cache[vic].COUNT_BLOCK) {
                victim_cache[point].COUNT_BLOCK++;
			}
		}  
        // All blocks in Cache_sets, COUNT_BLOCK++
		for (int point = 0; point < assoc; point++) {
			cache_sets[index][point].COUNT_BLOCK++;
		}
		// SWAP: victim_hit_block and L1_swap_block COUNT_BLOCK reset
        cache_sets[index][i].valid_bit   = true;
		cache_sets[index][i].dirty_bit   = true;
		cache_sets[index][i].COUNT_BLOCK = 0;
		cache_sets[index][i].tag         = tag;
		victim_cache[vic].valid_bit   = true;
		victim_cache[vic].dirty_bit   = L_to_swap.dirty_bit;
		victim_cache[vic].tag         = L_to_swap.tag * setnum + index; // bits and tag recovery
		victim_cache[vic].COUNT_BLOCK = 0; 
		return true;
	}

	missWrites++;
	return false;
}
