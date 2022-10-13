#include "util.h"
#include "stdio.h"

int main(int argc, char *argv[]) {

	struct Param parameters;
    get_input(argc, argv, &parameters);
	//cout << parameters.L1_ASSOC << "\n";

    parse_trace(&parameters);
    give_output(&parameters);
}
