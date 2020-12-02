#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h> 
#include "list.h"
#include "dijkstra.h"
#include "binary_min_heap_parallel.h"

/* Build binary min heap from list of vertices read in from input file */ 
BinaryMinHeap* make_binary_min_heap(DijkstraVertex **vertices, int size, int root_vertex) {
    BinaryMinHeap *min_heap = malloc(sizeof(BinaryMinHeap));
    // min heap will be stored as list with empty placeholder element at index 0
    // otherwise will contain pointers to vertices in the VertexList constructed from the input file
    min_heap->elems = malloc(sizeof(Vertex *) * size + 1);
    min_heap->size = 2; // for root and empty placeholder at 0
    min_heap->elems[1] = vertices[root_vertex];
    return min_heap;
}

/* Print current binary min heap */
void print_heap(BinaryMinHeap *min_heap) {
    printf("Elements in heap: %d\n", min_heap->size);
    printf("[placeholder@0]\n"); // 0 index placeholder
    for (int i = 1; i < min_heap->size; i++) {
        printf("[v:%d w:%d]\n", min_heap->elems[i]->vertex->index, min_heap->elems[i]->weight);
    }
}

/* Get heap size */
int size(BinaryMinHeap *min_heap) {
    return min_heap->size;
}

/* Check if heap is empty */
bool is_empty(BinaryMinHeap *min_heap) {
    // min heap is empty when only the placeholder element at index 0 exists
    return min_heap->size == 1;
}

/* Swap pointers for two vertices in min heap */
void swap_heap_elems(BinaryMinHeap *min_heap, int start, int end) {
    if (start != end) {
        DijkstraVertex *temp = min_heap->elems[start];
        min_heap->elems[start] = min_heap->elems[end];
        min_heap->elems[end] = temp;
    }
}  

/* Find the minimum child of a node in the heap. The children are located an 2i and 2i+1 where i is the index
   of the node */
int find_min_child(BinaryMinHeap *min_heap, int vertex) {
    if (vertex * 2 + 1 > min_heap->size) {
        return vertex * 2;
    } else if (min_heap->elems[vertex * 2]->weight < min_heap->elems[vertex * 2 + 1]->weight) {
        return vertex * 2;
    } else {
        return vertex * 2 + 1;
    }
}

/* Rebalance heap; Swap given vertex down the heap until swap fails, then break and terminate */
void percolate_down(BinaryMinHeap *min_heap, int vertex) {
    while (vertex * 2 < min_heap->size) {
        int min_child = find_min_child(min_heap, vertex);
        if (min_heap->elems[vertex]->weight > min_heap->elems[min_child]->weight) {
            swap_heap_elems(min_heap, vertex, min_child);
        } else {
            break;
        }
        vertex = min_child;
    }
}

/* Delete minimum value in heap (root), then rebalance heap */
int del_min(BinaryMinHeap *min_heap) {
    DijkstraVertex *res = min_heap->elems[1];
    min_heap->size--;
    // If heap is not empty, rebalance
    if (min_heap->size > 1) {
        min_heap->elems[1] = min_heap->elems[min_heap->size];
        percolate_down(min_heap, 1);
    }
    return res->vertex->index;
}

/* Rebalance heap; Swap vertex up the heap until swap fails, then break */
void percolate_up(BinaryMinHeap *min_heap, int vertex) {
    while (vertex / 2 > 0) {
        if (min_heap->elems[vertex]->weight < min_heap->elems[vertex / 2]->weight) {
            swap_heap_elems(min_heap, vertex, vertex / 2);
        } else {
            break;
        }
        vertex = vertex / 2;
    }
    return;
}

/* Insert new Vertex in the heap */
void insert(BinaryMinHeap *min_heap, DijkstraVertex *vertex) {
    min_heap->elems[min_heap->size] = vertex;
    percolate_up(min_heap, min_heap->size);
    min_heap->size++;
}
