CC = g++-4.4
OPT = -O3 -m32
#OPT = -g -m32
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

# List all your .cc files here (source files, excluding header files)
SIM_SRC = main.cc cache.cc

# List corresponding compiled object files here (.o files)
SIM_OBJ = main.o cache.o
 
#################################

# default rule

all: sim_cache
	@echo "my work is done here..."


# rule for making sim_cache

sim_cache: $(SIM_OBJ)
	$(CC) -o sim_cache $(CFLAGS) $(SIM_OBJ) -lm
	@echo "-----------DONE WITH SIM_CACHE-----------"
	./sim_cache 32 2048 4 0 4096 8 gcc_trace.txt > Result6_PartB.txt && diff -iw Result6_PartB.txt ../validation/Validation6_PartB.txt
	./sim_cache 16 1024 8 0 8192 4 go_trace.txt  > Result7_PartB.txt && diff -iw Result7_PartB.txt ../validation/Validation7_PartB.txt
	./sim_cache 32 1024 8 256 0 0 perl_trace.txt > Result8_PartB.txt && diff -iw Result8_PartB.txt ../validation/Validation8_PartB.txt
	./sim_cache 128 1024 2 1024 4096 4 gcc_trace.txt > Result9_PartB.txt && diff -iw Result9_PartB.txt ../validation/Validation9_PartB.txt
	./sim_cache 64 8192 2 1024 16384 4 perl_trace.txt > Result10_PartB.txt && diff -iw Result10_PartB.txt ../validation/Validation10_PartB.txt
	

	
# generic rule for converting any .cc file to any .o file
 
.cc.o:
	$(CC) $(CFLAGS)  -c $*.cc


# type "make clean" to remove all .o files plus the sim_cache binary

clean:
	rm -f *.o sim_cache


