#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h> 
#include "list.h"
#include "dijkstra.h"
#include "binary_min_heap.h"

#define MAX_LINE_LENGTH 100
#define SAVED_PATHS 10

/**
TODO:
0. Parallelization? (might need some refactoring, think about it). Do as many efficiency fixes as possible. (marked)
2. add timers, test on all inputs (marked)
3. break into separate files, valgrind, makefile (day 2)
4. final cleanup, writeup, readme (day 2)
 */ 

/* Function pointer for List; tests for equality between two edges, won't be used */ 
int edge_equals(void *key1, void *key2) {
    Edge *e1 = (Edge *) key1;
    Edge *e2 = (Edge *) key2;
    return (e1->start_vertex == e2->start_vertex) && (e1->end_vertex == e2->end_vertex)
              && (e1->weight == e2->weight);
}

/* Function pointer for List; destroys an edge object. Nothing to destroy. */
int edge_destroy(void *data) {
    return 0;
}

/* Function pointer for List; translates an Edge into a string. */
char* edge_str(void *data) {
    static char str[MAX_LINE_LENGTH * 2]; // *2 to hold extra characters below 
    Edge *curr = (Edge *) data; 
    sprintf(str, "[start_index = %d, end_index = %d, weight = %d]", curr->start_vertex, curr->end_vertex, curr->weight);
    return str;
}

/* Helper function to print vertices and the directed edges starting from them. */
void print_vertices(VertexList *v_list) {
    for (int i = 0; i < v_list->size; i++) {
        Vertex *curr = v_list->vertices[i];
        printf("Vertex: %d\n", curr->index);
        printf("Adjacent edges:\n");
        list_print(curr->edges);
    }
}

/* Initialize vertex list with an empty vertex at each index, each with an empty edge list */
void initialize_vertices(VertexList *v_list) {
    for (int i = 0; i < v_list->size; i++) {
        Vertex *curr = malloc(sizeof(Vertex));
        curr->index = i; 
        curr->edges = malloc(sizeof(List));
        curr->edges = list_create(edge_str, edge_destroy, edge_equals);
        v_list->vertices[i] = curr;
    }
}

/* Read in input file. Expects each line to be of format v e w where v is starting vertex, e is
   ending vertex, and w is the edge weight. Expects that these are directional edges (e.g. v->e only
   would be represented by the line, not both v->e and e->v). */
void read_in_file(VertexList *v_list, const char* filename, int *n_vertices, int *n_edges) {
    FILE *input_file = fopen(filename, "r");
    char *buffer = malloc(MAX_LINE_LENGTH * sizeof(char) + 1);  
    fscanf(input_file, "%d %d", n_vertices, n_edges);
    v_list->vertices = malloc(sizeof(Vertex *) * (*n_vertices));
    v_list->size = *n_vertices;
    initialize_vertices(v_list);
    int v, e, w; // read in all edges and store adjacent ones
    while (fscanf(input_file, "%d %d %d", &v, &e, &w) != EOF) {  
        Vertex *s_curr = v_list->vertices[v];
        Edge *s_new_edge = malloc(sizeof(Edge));
        s_new_edge->start_vertex = v;
        s_new_edge->end_vertex = e;
        s_new_edge->weight = w;
        list_prepend(s_curr->edges, s_new_edge);
    }
    free(buffer);
    fclose(input_file);
}

/* Runs every time after dijkstra is completed for some node. Resets weights and history
   for each vertex so the next node can be processed with dijkstra. */ 
void reset_vertices(VertexList *v_list) {
    for (int i = 0; i < v_list->size; i++) {
        v_list->vertices[i]->weight = INT_MAX;
        v_list->vertices[i]->visited = false;
        v_list->vertices[i]->in_heap = false;
    }
}

/* Compare function for qsort on Edges */ 
static int sort_edges(const void * a, const void * b) {
    Edge *e1 = *((Edge **) a);
    Edge *e2 = *((Edge **) b);
    if (e1->weight > e2->weight) return 1;
    else if (e1->weight < e2->weight) return -1;
    else return 0;  
}

/* Keep a running storage of the 10 longest paths; ran each time a node is fully processed*/
void update_longest_paths(VertexList *v_list, int start, int end, Edge **paths) {
    // check if this edge should be in our top 10
    if (v_list->vertices[end]->weight > paths[0]->weight) {
        paths[0]->start_vertex = start;
        paths[0]->end_vertex = end;
        paths[0]->weight = v_list->vertices[end]->weight;
        qsort(paths, SAVED_PATHS, sizeof(Edge *), sort_edges);
    }
    return;
}

/* Run dijkstra's algorithm on the vertex at the given index. Note that some additional helper arrays (weight, visited,
   in_heap) are used to prevent additional checks in the min_heap. This saves time and memory. */ 
void run_dijkstra(VertexList *v_list, int start_node, Edge **longest_paths) {
    int original_node = start_node;
    BinaryMinHeap *min_heap = make_binary_min_heap(v_list, start_node);
    v_list->vertices[start_node]->weight = 0;
    while (!is_empty(min_heap)) {
        // Pop the front; mark as processed and having exited the heap
        start_node = del_min(min_heap);
        v_list->vertices[start_node]->visited = true;
        v_list->vertices[start_node]->in_heap = false;
        update_longest_paths(v_list, original_node, start_node, longest_paths);

        // for edge edge of our current vertex, see if weights need to be updated. If the weight is updated but that edge
        // is not yet in the heap, insert it
        List *edges = v_list->vertices[start_node]->edges;
        List_element *head = edges->head;
        while (head != NULL) {
            Edge *curr = (Edge *) head->data;
            int end = curr->end_vertex;
            int weight = curr->weight;
            if (v_list->vertices[end]->weight > (v_list->vertices[start_node]->weight + weight) && !v_list->vertices[end]->visited) {
                v_list->vertices[end]->weight = v_list->vertices[start_node]->weight + weight;
                if (!v_list->vertices[end]->in_heap) {
                    insert(min_heap, v_list->vertices[end]);
                    v_list->vertices[end]->in_heap = true;
                }
            }
            head = head->next;
        }
    }
    free(min_heap->elems);
    free(min_heap);
}

/* Helper function to print dijkstra data after completion */
void print_dijkstra_results(VertexList *v_list, int start_node) {
    printf("Dijkstra results for node: %d\n", start_node);
    for (int i = 0; i < v_list->size; i++) {
        printf("Node: %d. Processed: %d. Path length: %d.\n", i, v_list->vertices[i]->visited, v_list->vertices[i]->weight);
    }
}

/* Helper function to print our 10 current longest edges. */
void print_longest_edges(Edge **paths) {
    printf("Longest paths:\n");
    for (int i = 0; i < SAVED_PATHS; i++) {
        printf("start vertex %d, end vertex %d, distance %d\n", paths[i]->start_vertex, paths[i]->end_vertex, paths[i]->weight);
    }
}

/* Given an input file with some n vertices and m edges, run dijkstra's algorithm n times, starting from each of the vertices.
   Output to a file the 10 longest paths overall. */
int main(const int argc, const char** argv) {
    int n_nodes;                    // number of nodes in input file
    int n_edges;                    // number of edges in input file
    if (argc != 2) {
        printf("Usage: ./dijksta <input_file>.txt. Aborting.\n");
        return 1;
    }
    /******** read in all vertices/edges *********/
    VertexList *v_list = malloc(sizeof(VertexList));
    read_in_file(v_list, argv[1], &n_nodes, &n_edges);
    Edge **longest_paths = malloc(sizeof(Edge) * SAVED_PATHS); // stores 10 longest paths
    for (int i = 0; i < SAVED_PATHS; i++) {
        longest_paths[i] = malloc(sizeof(Edge));
        longest_paths[i]->weight = -1;
    }
    reset_vertices(v_list); // prep vertex list 

    /********** do dijkstra starting from each vertex **********/
    for (int start_node = 0; start_node < v_list->size; start_node++) {
        printf("Starting dijkstra for vertex %d.\n", start_node);
        run_dijkstra(v_list, start_node, longest_paths);
        //print_dijkstra_results(v_list, start_node); // uncomment to see dijkstra output
        reset_vertices(v_list);
    }

    /******** print 10 longest to file ********/
    FILE *output_file = fopen("output.txt", "w");
    for (int i = 0; i < SAVED_PATHS; i++) {
        fprintf(output_file, "start vertex %d, end vertex %d, distance %d\n", longest_paths[i]->start_vertex, longest_paths[i]->end_vertex, longest_paths[i]->weight);
    }
    fclose(output_file);

    /******** free everything *************/
    for (int i = 0; i < SAVED_PATHS; i++) {
        free(longest_paths[i]);
    }
    free(longest_paths);
    for (int i = 0; i < v_list->size; i++) {
        list_clear(v_list->vertices[i]->edges);
        free(v_list->vertices[i]->edges);
        free(v_list->vertices[i]);
    }
    free(v_list->vertices);
    free(v_list);
}