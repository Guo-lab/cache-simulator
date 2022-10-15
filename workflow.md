# Prepare  
- `make` to get 
```
g++-4.4 -o sim_cache -O3 -m32 -Wall   main.o world.o -lm
-----------DONE WITH SIM_CACHE-----------
my work is done here...
```
- `./sim_cache 32 8192 4 0 1 gcc_trace.txt` to get  
```
Receive the Input!

argument #0 = ./sim_cache
argument #1 = 32
argument #2 = 8192
argument #3 = 4
argument #4 = 0
argument #5 = 1
argument #6 = gcc_trace.txt
```


# Do it  
To implement L1 cache, L2 cache and victim cache.
---

--- 
- Last, to test all validation, dir structure is as follow:  

# Work Step
1. ### deal with input
2. ### parse the trace file, get index and tag
3. ### Read and then Write (follow timeline)
  - #### First, Consider LFU Least Frequently Used
    - Write-back Write-Allocate (WBWA)
    - Write-through Write-Not-Allocate (WTNA) 
    - Order
      First, block not exsists, try to replace,  
      Then, replace valid block, otherwise in terms of LFU to find one to replace.  
      Last, block exsists.
    - Cache Variables to update:
      - While reading,  
      COUNT_BLOCK, Reads and missReads,  
      writeBacks,   
      COUNT_SET,   
      tag, dirty_bit, valid_bit.

  - #### Then, LRU Least Recently Used
    - Write-back Write-Allocate (WBWA)
    - Write-through Write-Not-Allocate (WTNA) 
    - Implement the rest of the order of LFU.  
4. ### Replace the block if fetching fails (follow timeline)
5. ### output the results.

