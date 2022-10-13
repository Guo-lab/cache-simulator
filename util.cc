#include <stdio.h>
#include "util.h"
#include <string>


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