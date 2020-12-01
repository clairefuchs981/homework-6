#ifndef EDGE_H
#define EDGE_H
/* Function pointer for List; tests for equality between two edges, won't be used */ 
int edge_equals(void *key1, void *key2);
/* Function pointer for List; destroys an edge object. */
int edge_destroy(void *data);
/* Function pointer for List; translates an Edge into a string. */
char* edge_str(void *data);
/* Compare function for qsort on Edges */
int sort_edges(const void * a, const void * b);
#endif