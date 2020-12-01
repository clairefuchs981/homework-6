#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h> 
#include "list.h"
#include "dijkstra.h"

#define LINE_LENGTH 100

/* Function pointer for List; tests for equality between two edges, won't be used */ 
int edge_equals(void *key1, void *key2) {
    Edge *e1 = (Edge *) key1;
    Edge *e2 = (Edge *) key2;
    return (e1->start_vertex == e2->start_vertex) && (e1->end_vertex == e2->end_vertex)
              && (e1->weight == e2->weight);
}

/* Function pointer for List; destroys an edge object. */
int edge_destroy(void *data) {
    Edge *e = (Edge *) data;
    free(e);
    return 0;
}

/* Function pointer for List; translates an Edge into a string. */
char* edge_str(void *data) {
    static char str[LINE_LENGTH * 2]; // *2 to hold extra characters below 
    Edge *curr = (Edge *) data; 
    sprintf(str, "[start_index = %d, end_index = %d, weight = %d]", curr->start_vertex, curr->end_vertex, curr->weight);
    return str;
}

/* Compare function for qsort on Edges */ 
int sort_edges(const void * a, const void * b) {
    Edge *e1 = *((Edge **) a);
    Edge *e2 = *((Edge **) b);
    if (e1->weight > e2->weight) return 1;
    else if (e1->weight < e2->weight) return -1;
    else return 0;  
}
