# Homework 6

Claire Fuchs <br />
Homework 6 <br />
December 6 2020 <br /> 

## Repo Contents 

> binary_min_heap_parallel .c / .h - Parallel version of binary_min_heap (goes with dijkstra_parallel) <br/>
> binary_min_heap .c / .h - Contains binary min heap interface and implementation (goes with dijkstra.c) <br/>
> dijkstra_parallel.c - Runs dijkstra's algorithm on all vertices in input file, using OpenMP <br/>
> dijkstra.c - Runs dijkstra's algorithm on all vertices in input file <br/>
> dijkstra.h - Contains structs for Vertices/Edges used in dijkstra algorithm and min heap <br/>
> edge_util .c / .h - Util files containing helper functions for Edges <br/>
> graph0.txt / graph1.txt / graph2.txt - Input files (smallest -> largest in this order) <br/>
> list.c / list.h - HW1 Code, can be ignored <br/>
> output_1_parallel.txt / output_1.txt - Results for running dijkstra_parallel and dijkstra executables on graph0.txt <br/> 
> output_2_parallel.txt / output_2.txt - Results for running dijkstra_parallel and dijkstra executables on graph1.txt <br/> 
> output_3_parallel.txt / output_3.txt - Results for running dijkstra_parallel and dijkstra executables on graph2.txt <br/> 

## Performance Analysis 

Well, the first time I ran dijkstra.c on graph2.txt, it was all set to take 9000 hours to complete, which was great (all from reading in directed edges as bidirectional,
who would've thought). Thankfully that turned out to not be the case. I'll mention this again below, but I coded dijkstra.c to maximize efficiency for a serial implementation.
The parallel version (dijkstra_parallel.c) also ended up with pretty great runtime (even if the code isn't nearly so nice). <br/>
The results for graph0 are pretty trivial. I'll discuss its purpose below in implementation details. With so few vertices to process, it takes well under a second for both dijkstra
and dijkstra_parallel to complete; though, the parallel version is still about 5x faster. This is likely just because the overhead needed to start all the threads is complete overkill
when there is only 9 vertices. <br/>
Graph1.txt is more interesting (see output_2.txt and output_2_parallel.txt). It swaps the relationship graph0.txt results have - parallelization about exactly halves the average time taken to process dijkstra on a single vertex. Note - the order of edges can vary in output files since the introduction of threads can switch things up. Either way, on several runs of the program, these
timings remained pretty flat. Graph 1 has 6301 vertices and 20777 edges. So without parallelization, a vertex is processed in about 0.0005 seconds. With parallelization, it takes 0.0003 seconds. This number doesn't necessarily represent the number of edges at all, so we'll create a new metric. Our total time from output_2.txt is 3.404 seconds. If divided by # vertices and # edges, we get about ~0.000000026 seconds. For output_2_parallel - ~0.0000000143 seconds. This can be seen as a per-vertex, per-edge processing time. Might not be a hugely interesting number, but can be compared against graph2.txt for fun. <br/>
Graph2.txt runtime is pretty good, serial or not. Serial takes about 2 hr 17 minutes, whereas parallel takes 1 hr 17 minutes (ran both twice and got the same results, so these numbers seem pretty stable). Considering the amount of extra memory/frees that dijkstra_parallel has to build up, this is pretty surprising (to me, anyway).   For output_3 (no parallelization), we have a processing time per vertex of 0.03 seconds. This is about 60x slower than graph1.txt. For output_3_parallel, our processing time per vertex is 0.017 seconds (which again, is about 1/2. The parallelization seems pretty darn good at cutting down the time in 2). Compared to output_2_parallel, this is about ~57x slower (or 60x, if we just want to round to make it uniform with the other half). So again, we see pretty consistent results, which is good. Graph2.txt has 265214 vertices and 420045 edges. Compared to graph1, this is about 42x the vertices and 20x the edges. We can do the same per-vertex per-edge processing numbers we did for graph1.txt. For output_3, we take a total of 8223.62 seconds, so ~0.000000074 seconds (about 3x the results from graph1). For output_3_parallel, it took a total of 4649.17 seconds, so 0.000000042 (again about 3x graph1 parallelized). The ratios are actually pretty close, which I guess you could expect. The code isn't doing anything fundamentally different regardless of how many vertices you add - the ratios should stay about the same. To account for the small differences we do see, we can consider that the vertices and edges did not increase by the same ratio between graph1 and graph2. Many more vertices were added in this case - it would make sense for the average time of processing to be pulled upwards slightly, since dijkstra will need to be run from scratch that many more times. Likewise, the heaps constructed for these vertices may very well be bigger, since there are more nodes they can reach (assuming the paths that come from vertices are equally varied as those from graph1.txt). Even if technically, the vertices in graph2 might have less immediate adjacent edges (judging only by the fact that the vertices are more plentiful compared to edges in graph2. compared to graph1), the paths can likely branch much farther. Unfortunately, this algorithm doesn't tell us  much about the actual paths these nodes take, so none of that can be very explored here. 

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

The input file should be in the following format: the first line contains two integers separated by a space, containing the number of vertices and number of edges. There will then be n lines where n = number of edges, each line having three numbers representing the from-vertex, to-vertex, and then weight. Any other format, or an invalid file name, will cause an error. The three graph.txt files in the repo all adhere to this. There is no stdout output from running the executable (for the sake of speed). You can uncomment the "print_dijkstra" function in the main loop for either dijkstra or dijkstra_parallel to see table results from running the algorithm, but it's not hugely useful. The algorithm currently dumps results into "output.txt", where it will print the 10 longest paths as well as runtime for the algorithm.<br/>

Implementation notes: <br/>
* dijkstra_parallel.c is experimental, and has at least one bug. I was satisfied with the runtime of dijkstra.c, but was curious how much faster it could get, so wrote dijkstra_parallel as well. dijkstra_parallel.c consumes massive amounts of memory (due to all the extra private arrays each thread needs to create - dijkstra.c relied heavily on one master vertice array to build all its work on, which did not extend to threads nicely). Due to all the extra allocations/objects in the code, it also just doesn't look as nice. The bug is something within "merge_longest_paths". Somehow, duplicates are sneaking their way in as each thread merges its longest paths to the main list. This could be from several things (and it only sometimes occurs), but it does get the correct weights in the output files (just sometimes the start-end vertices are duplicated). Either way, dijkstra_parallel was just a speed experiment, and it's already ugly, so I'm leaving the bug be. <br/>
* For correctness testing, I simply took a lot of known examples from tutorial pages ([like this](https://www.geeksforgeeks.org/dijkstras-shortest-path-algorithm-greedy-algo-7/)) and put them in text files. These served as unit tests during initial development. I left graph0.txt in the repo as an example; it corresponds to the above url.

## Code Shortcomings 
The main shortcoming is mentioned above in implementation notes: dijkstra_parallel does have a bug. The merging algorithm for the longest paths was very hastily done on my end, so I'm not surprised. Since the output files are still 99% correct, I'm fine with it (and it's not like the algorithm is skipping work; and since that's true and I'm only worried about timing, it's all good). When I first coded this assignment, my goal was to make the fastest serial program possible. I'm pretty satisfied all around with dijkstra.c because of that. That being said, the way I structured dijkstra.c, as a consequence, did not go well with parallelization. The amount of extra memory allocated is disgusting (though I wonder, realistically, if much better was possible). I think dijkstra.c is a lot prettier and more representative of what I wanted to do - though I am very happy with the runtime of both. Otherwise, I actually don't have any beef with my code, for once. 


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
