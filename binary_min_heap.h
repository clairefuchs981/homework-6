#ifndef MIN_HEAP_H
#define MIN_HEAP_H
/* Binary Min Heap; minimum element will be at root */
typedef struct binary_min_heap_ {
    int size;        // current size of heap
    Vertex **elems;  // list of all elements (vertices) in heap
} BinaryMinHeap;

/* Build binary min heap from list of vertices read in from input file */ 
BinaryMinHeap* make_binary_min_heap(VertexList *v_list, int root_vertex);
/* Print current binary min heap */
void print_heap(BinaryMinHeap *min_heap);
/* Check if heap is empty */
bool is_empty(BinaryMinHeap *min_heap);
/* Delete minimum value in heap (root), then rebalance heap */
int del_min(BinaryMinHeap *min_heap);
/* Insert new Vertex in the heap */
void insert(BinaryMinHeap *min_heap, Vertex *vertex);
#endif