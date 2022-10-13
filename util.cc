#include <stdio.h>
#include "util.h"
#include <string.h>
#include <typeinfo>
#include <fstream>
#include <sstream>

void get_input(int argc, char *argv[], struct Param* parameters) {
	printf("\nReceive the Input!\n\n");

	for (unsigned int i = 0; i < (unsigned int)argc; i++) {
	   printf("argument #%d = %s\n", i, argv[i]);
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

	printf("\n");

    return;
}


void parse_trace(struct Param* parameters) {
	string path = "../traces/";
	//cout << typeid(path).name() <<endl;
	//cout << typeid(parameters->trace_file).name() << endl;
	string file = path + parameters->trace_file;
	//cout << typeid(file).name() << endl;
    fstream f(file.c_str());

    string line;
	while(getline(f, line)) {
        char r_w;
        unsigned address; 

		istringstream separate(line);
		separate >> r_w;
		separate >> hex >> address; // convert hex address to dec
		// cout << r_w << " " << address << endl;

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
bool Cache::readFromAddress(unsigned int add) {

}

bool Cache::writeToAddress(unsigned int add) {

}