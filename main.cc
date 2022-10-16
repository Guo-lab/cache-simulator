#include "cache.h"

int main(int argc, char *argv[]) {

	struct Param parameters;
    get_input(argc, argv, &parameters);
    
    parse_trace_and_run(&parameters);
}
