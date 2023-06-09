# C based Memory Management
1. **Information**

   - Project Name: CSE374 HW6
   - Author Name: Yang Li
   - GitLab Repo: https://gitlab.cs.washington.edu/cse374-22au-students/yangli99-kangll/-/tree/main
   - Date: 12/08/2022

2. **Data Structure and Algorithms**

   - We modefied the FreeNode data structure to have 3 fields:
   - size: an uintptr_t integer that gives the size of the block excluding the node's own size(16 bytes)
   - next: the pointer to the next block on the freelist
   - addr: the address of the block
   - The getmem and freemem functions share the single data structure freelist.
   - freelist is a linked-list of free memory blocks that are available to satisfy memory allocation requests.
   - When a request for memory is made, the freelist will be scanned to check if there's available block whose size is ≥ the size requested.
   - If a block with suitable size is found, it will be removed from freelist and a pointer to it is returned.
   - If the block found is substantially larger than the memory requested, the block will be split up, yielding a block that will satisfy the request
     and leaving the remainder on the freelist.
   - If there isn't an available block found, extendSpace() will be called to acquire a good-sized block of storage from the underlying system,
     add it to the free list, split it up, yielding a block that will satisfy the request, and leaving the remainder on the freelist.
   - If the size requested is way larger than predefined EXTRA_SIZE, then block acquired is of 'size', else of EXTRA_SIZE.
   - When a request to free memory is made, groupAdjacent() will be called to scan the freelist, in which process blocks that are
     adjacent to the given block in their physical memory address will be grouped into a bigger one. If no adjacent blocks are found,
     just insert the given block into the list at location 'p'.

3. **Additional Features and Optimization**

   - In bench.c we seperated some part of the functions of main to a helper function that is called run_tests() for better modularization.
   - In mem_impl.h we introduced some helper functions as well that are used to create new nodes and thus initialize the list.

4. **Summary Of Benchmarking**

   - We started our test with 100 trials and with an increment of factor of 10 up to 1 millon trials.
   - In our test, in average, we found out that it roughly took 0 milliseconds before 10k trials, 2-3 milliseconds around 10k trials,
   - around hundreds of milliseconds to run 100k trials and around 5 seconds to run 1 million trials.
   - In terms of memory usage, after 10k tirals it resulted in around 200 free blocks and an average of 8000 bytes per block.
   - Generally, requests with more large memory calls require more CPU time and storage space.

5. **Summary of Resources**
   - Google (stackoverflow, geeksforgeeks), Lecture Slides, Panaptop Recordings, Ed discussion board and C documentation pages and tutorials.
