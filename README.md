# Homework 6

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
