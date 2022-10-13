#include "util.h"
#include "stdio.h"

int main(int argc, char *argv[]) {

	struct Param parameters;
    get_input(argc, argv, &parameters);
	//cout << parameters.L1_ASSOC << "\n";

    give_output(&parameters);
}
