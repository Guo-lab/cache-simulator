# Prepare  
Continue with Cache Simulator L1  
To Implement CacheL2 and VictimCache.  


# Do it  

# Work Step
After building the whole structure, debugs.  
1. Complete Case 8: L1 with Victim Cache.  Problem is caused by the assoc in the loop.  
2. Complete Case 6 and 7. L1 and L2 Cache without Victim Cache.  
   Problem is caused by two points.   
   - Read 2 Cache, no matter write L1 Cache or read La Cache (WBWA). 
   - writeToAddress result to decide whether to replace_block().   
