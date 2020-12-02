# Homework 6

Claire Fuchs <br />
Homework 6 <br />
December 6 2020 <br /> 

## Repo Contents 

> binary_min_heap_parallel .c / .h - Parallel version of binary_min_heap (dijkstra_paralllel executable uses this version) <br/>
> binary_min_heap .c / .h - Contains binary min heap interface and implementation (dijkstra executable uses this version) <br/>
> dijkstra_parallel.c - Runs dijkstra's algorithm on all vertices in input file, using OpenMP <br/>
> dijkstra.c - Runs dijkstra's algorithm on all vertices in input file <br/>
> dijkstra.h - Contains structs for Vertices/Edges used in dijkstra algorithm and min heap <br/>
> edge_util .c / .h - Util files containing helper functions for Edges <br/>
> graph0.txt / graph1.txt / graph2.txt - Input files (smallest -> largest in this order) <br/>
> list.c / list.h - HW1 Code, unchanged <br/>
> output_1_parallel.txt / output_1.txt - Results for running dijkstra_parallel and dijkstra executables on graph0.txt <br/> 
> output_2_parallel.txt / output_2.txt - Results for running dijkstra_parallel and dijkstra executables on graph1.txt <br/> 
> output_3_parallel.txt / output_3.txt - Results for running dijkstra_parallel and dijkstra executables on graph2.txt <br/> 

## Performance Analysis 

All results come from running on a local Windows machine.<br/>

The results for graph0.txt are pretty trivial. Graph0 has only 9 vertices and 14 edges, so it's more of a correctness test than an indicator of efficiency. With or without parallelization, it takes well under a second to complete (see output_1 and output_1_parallel text files for exact numbers). According to our text files, the parallelization version ran about 5x slower, but that is expected since it's an awful lot of trouble to create all the extra threads/buffers when the amount of work is so small. We don't need to worry about these results too much; what's important is just that both were fast and we can see that they output the same longest edges, so that's good. <br/><br/>

For graph1.txt, you can reference output_2 and output_2_parallel.txt. Graph1.txt has 6301 vertices and 20777 edges. Here, parallelization about halves our runtime. Without parallelization, we take a total of 3.404 seconds, or 0.0005 seconds to process dijkstra on a single vertex. With parallelization, it takes a total of  1.866 seconds, so about 0.0003 per vertex. Note - the order of edges can vary between output.txt files and their output_parallel.txt counterpart due to threads switching up ordering. Graph1.txt was ran many times with both methods, but the numbers stayed very consistent, so we can consider our current results to be pretty fair averages. Of course, our 0.0005 and 0.0003 per vertex processing times (in seconds) don't take into account the amount of edges in the file. Since dijkstra doesn't result in any path information between two nodes (just the final weight between them), there's no super useful way to incorporate that # edges into our results. <br/><br/>

For graph2.txt, the results are in output_3 and output_3_parallel.txt. This graph was 265214 vertices and 420025 edges. In serial, it took about 2 hours 17 minutes, or about 8223.61 seconds. In parallel, it took about 1 hour 17 minutes, or 4649.172 seconds. Like graph1.txt, it maintains the same ratio of the parallel version being about twice as fast as the serial. Without parallelization, each vertex takes about 0.031 seconds to process. And with parallelization, it takes about 0.0175 seconds per vertex. (Note - output_3_parallel.txt has a small bug with its longest paths results that will be discussed below). Compared to graph1, the unparallelized results per vertex processing time goes from 0.0005 (graph1) to 0.031 (graph2). This is about 62x slower for graph2.txt. Likewise, for parallelization, we go from 0.0003 (graph1) to 0.0175 seconds, which is about 58x times slower. So, the slowdown was by a pretty consistent factor, regardless of parallelization, which is interesting. Why could this be true? For starters, this graph was about 42x the vertices and 20x the edges as graph1.txt. More importantly, the ratio of vertices to edges has changed as well. For graph1, it was about 1:3 (vertices:edges, as far as the totals went). For graph2, it was 1:1.5. Of course, this doesn't necessarily mean anything. In theory one might guess that graph2.txt might actually be faster, since there could potentially be less edges starting from each vertex, so maybe the heap tends to stay smaller throughout the algorithm. Judging from our results, what might have happened is that, while there could possibly be less edges per vertex in graph2.txt, there are many more vertices. The paths could branch throughout the graph and has the potential to create a much larger heap for each vertex. If that's true, it's only natural that the processing time would begin to suffer from the heap having extra work, and graph2.txt would take much longer per vertex. <br/>
Ultimately, dijkstra doesn't tell us much besides the final weight between two vertices. Because of that, we can't say much definitely after the paths without delving into the input files themselves and looking around. From these results, I'd just make a guess that, while graph2.txt might produce a sparser tree, the adjacent edges end up building far larger heaps to process than graph1 (due to all the extra vertices), and that would explain our increased processing time. 

## How to Run 

Clone the repo and run: 
```bash
make
```

This will build two executables; dijkstra and dijkstra_parallel. Commands are as follows: 

```bash 
./dijkstra <input_file_name>.txt
./dijkstra_parallel <input_file_name>.txt
// For example:
./dijkstra test.txt
./dijkstra graph1.txt
./dijkstra_parallel graph2.txt
```

The input file should be in the following format: the first line contains two integers separated by a space, containing the number of vertices and number of edges. There will then be n lines where n = number of edges, each line having three numbers representing the from-vertex, to-vertex, and then weight. Any other format, or an invalid file name, will cause an error. The three graph.txt files in the repo all adhere to this. There is no stdout output from running the executable (for the sake of speed). You can uncomment the "print_dijkstra" function in the main loop for either dijkstra or dijkstra_parallel to see table results from running the algorithm, but it's not hugely useful. The algorithm currently dumps results into "output.txt", where it will print the 10 longest paths as well as runtime for the algorithm. The 6 output.txt files in the repo correspond to running dijkstra & dijkstra_parallel against graph0, graph1, and graph2.<br/>

Implementation notes: <br/>
* dijkstra_parallel.c has a minor bug. I haven't fully tracked it, but I'm pretty confident that it is either in the function "merge_longest_paths" or "update_longest_paths". The finalized list of 10 longest paths sometimes contains duplicates after dijkstra_parallel.c executes. The behavior is flaky (though it should not be a race condition, since merge_longest_paths is a critical section). Output_3_parallel.txt shows this bug. The program always gets the correct weights (e.g. output_3_parallel should have four edges weighing 101, four weighing 102, and 2 weighting 103, and it does (and as does output_3.txt)). However, sometimes the start-vertices and end-vertices are duplicated. I'm assuming this is because the merge_longest_paths method is pretty sloppy, and is probably letting duplicates through now and then dependent on what local longest paths each thread creates and the order that the lists merge together. Not hugely worried about this; it's still getting the "right" answer as far as weights go, and it's not skipping any work so our time results should be unaffected. Will remove note if I fix bug.   
* For correctness testing, I simply took a lot of known examples from tutorial pages ([like this](https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/)) and put them in text files. These served as unit tests during initial development. I left graph0.txt in the repo as an example; it corresponds to the above url. Also, I used the brute force dijkstra algorithm originally provided on the hw README (still attached at the very bottom of this one) to generate answers for graph0.txt and graph1.txt. I used those to validate the correctness of the algorithm before running graph2.txt (of course, my computer couldn't handle running graph2.txt with the brute force method, so I'm more just trusting those results as correct).
* The valgrind report for dijkstra_parallel.c won't be clean, since valgrind and openmp don't really understand each other. If you comment out the OpenMP code (all the pragmas and the omp_set_threads), you'll see that the valgrind report is clean, so just ignore anything that comes up from dijkstra_parallel. 
* Since the timer just uses the clock() method, the results might not be quite right on the linux cluster. Will remove bullet when fixed.


## Code Shortcomings 
The main shortcoming is what was just mentioned in the implementation notes; dijkstra_parallel does have some small bug allowing duplicate longest edges in our final output. Beyond that, dijkstra_parallel consumes an awfully huge amount of memory. I originally designed dijkstra.c and binary_min_heap.c to optimize a serial run, and it gets away with no massive memory allocations except for the initial VertexList that stores all our input Vertices/Edges. Since that is reused for every iteration of dijkstra, a lot of overhead is shaved off. Unfortunately, doing it this way didn't allow for a super clean port to parallelization. Each dijkstra iteration needs to create huge local copies to store vertex and longest path data. Some overhead is still shaved off by using pointers to the VertexList, and the binary heap code doesn't worsen at all, but it's still an annoying amount of extra memory considering the original managed to get around all that. Obviously dijkstra_parallel still produced a good speedup so it's not the end of the world, but I don't think it's nearly as nice as the serial version, as far as code-prettiness and acceptable memory management goes. 


### Old REAMDE contents

Write a C program that solves the all source shortest path problem by applying Dijkstra n = |V| times –– that is, once for each vertex in the input graph G, as described in the recorded lectures and outlined in the code snipped below. An important detail not described in typical Dijkstra pseudocode is the implementation of the next-closest vertex search. To get full credit, and to ensure the algorithm runs in reasonable time for the large input file provided, the search must be done using a binary min-heap priority queue*. Rather than save all of the path data, output only the 10 longest paths in a file with format:

start vertex 1, end vertex 1, distance 1<br>
start vertex 2, end vertex 2, distance 2<br>
...<br>
...<br>
start vertex 10, end vertex 10, distance 10

In addition to devising and running your own correctness tests, carry out a performance anaysis on the two graphs provided. No credit will be given for extremely inefficient implementations (several hours is the expected execution time in serial for large graph). To speed up execution of the large graph, OpenMP is strongly recommended, but not required for credit.

*An excellent reference for building a min-heap priority queue can be found here: https://bradfieldcs.com/algos/trees/priority-queues-with-binary-heaps/

*Dijkstra Shortest Path Main Loop*
```
while (processed[v] == false) {
    processed[v] = true;
    p = g->edges[v];
    while ( p != NULL) {
      w = p->y;
      weight = p->weight;
      if (distance[w] > (distance[v] + weight)) {
        distance[w] = (distance[v] + weight);
      }
      p = p->next;
    }
    /* find new min: naive approach, replace with min-heap priority queue */
    for (i=1,v=1,dist=INT_MAX; i<=g->nvertices; i++) {
      if ((processed[i] == false) && (dist > distance[i])) {
        dist = distance[i];
        v = i;
      }
    }
  }
```
