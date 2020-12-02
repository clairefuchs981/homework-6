#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h> 
#include <limits.h>
#include <omp.h>
#include "list.h"
#include "dijkstra.h"
#include "binary_min_heap_parallel.h"
#include "edge_util.h"

#define MAX_LINE_LENGTH 100 // max line length in input file
#define SAVED_PATHS 10 // number of longest paths we save & write to output file
#define NUM_THREADS 20 // number of threads to use for computing dijkstra on each node

/* For each vertex, print its id and all edges starting from it. */
void print_vertices(VertexList *v_list) {
    for (int i = 0; i < v_list->size; i++) {
        printf("Vertex: %d\n", v_list->vertices[i]->index);
        printf("Adjacent edges:\n");
        list_print(v_list->vertices[i]->edges);
    }
}

/* Print dijkstra results for given start node. */
void print_dijkstra_results(VertexList *v_list, int start_node) {
    printf("Dijkstra results for node: %d\n", start_node);
    for (int i = 0; i < v_list->size; i++) {
        printf("Node: %d. In Heap: %d. Path length: %d.\n", i, v_list->vertices[i]->in_heap, v_list->vertices[i]->weight);
    }
}

/* Initialize vertex list with an empty vertex at each index, each with an empty edge list */
void initialize_vertices(VertexList *v_list) {
    for (int i = 0; i < v_list->size; i++) {
        Vertex *curr = malloc(sizeof(Vertex));
        curr->index = i; 
        curr->edges = list_create(edge_str, edge_destroy, edge_equals);
        v_list->vertices[i] = curr;
    }
}

/* Read in input file. Expects first line of input file to be two digits (number of vertices, number of edges). Each
   following line should represent an edge with three digits (from-vertex, to-vertex, weight). Edges are directed. */
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

/* Keeps a list of longest paths generated by dijkstra; called every time a node is removed
   from the min heap. */
void update_longest_paths(DijkstraVertex **vertices, int start, int end, Edge **paths) {
    if (vertices[end]->weight > paths[0]->weight) { // is edge longer than our current smallest edge
        // swap values into existing smallest edge and resort list
        paths[0]->start_vertex = start;
        paths[0]->end_vertex = end;
        paths[0]->weight = vertices[end]->weight;
        qsort(paths, SAVED_PATHS, sizeof(Edge *), sort_edges);
    }
    return;
}

/* Called for each dijkstra iteration; prepares a list of vertices with initial state (infinite weight and
   marked as not in the heap) */
DijkstraVertex** prep_dijkstra_vertices(VertexList *v_list) {
    DijkstraVertex **vertices = malloc(sizeof(DijkstraVertex *) * v_list->size);
    for (int i = 0; i < v_list->size; i++) {
        DijkstraVertex *curr = malloc(sizeof(DijkstraVertex));
        curr->vertex = v_list->vertices[i];
        curr->weight = INT_MAX;
        curr->in_heap = false;
        vertices[i] = curr;
    }
    return vertices;
}

/* Creates a list of fake edges to be used as a placeholder for our longest edges list. This list will be
   gradually updated by the threads to contain SAVED_PATHS longest edges */
Edge** prep_longest_paths() {
    Edge **longest_paths = malloc(sizeof(Edge *) * SAVED_PATHS); // will store longest edges generated
    for (int i = 0; i < SAVED_PATHS; i++) {
        longest_paths[i] = malloc(sizeof(Edge));
        longest_paths[i]->weight = -1;
        longest_paths[i]->start_vertex = -1;
        longest_paths[i]->end_vertex = -1;
    }
    return longest_paths;
}

/* Helper to free a list of Edges */
void free_edge_list(Edge **paths, int size) {
    for (int i = 0; i < size; i++) {
        free(paths[i]);
    }
    free(paths);
}

/* Free array of DijksraVertices */
void free_dijkstra_vertices(DijkstraVertex **vertices, int size) {
    for (int i = 0; i < size; i++) {
        free(vertices[i]);
    }
    free(vertices);
}

/* Run dijkstra's algorithm on the vertex at the given index. */ 
Edge** run_dijkstra(VertexList *v_list, int start_node) {
    int original_node = start_node;
    // Each thread will need a private DijkstraVertex array to hold its traversal results,
    // as well as this thread's list of longest seen edges. 
    DijkstraVertex **vertices = prep_dijkstra_vertices(v_list);
    Edge **longest_paths = prep_longest_paths();
    BinaryMinHeap *min_heap = make_binary_min_heap(vertices, v_list->size, start_node);
    vertices[start_node]->weight = 0;

    while (!is_empty(min_heap)) {
        // Pop the front; see if it needs to be included in our longest edges list
        start_node = del_min(min_heap);
        vertices[start_node]->in_heap = false;
        update_longest_paths(vertices, original_node, start_node, longest_paths);

        // Check if adjacent edges can have their weights updated to smaller values
        List *edges = vertices[start_node]->vertex->edges;
        List_element *head = edges->head;
        while (head != NULL) {
            Edge *curr = (Edge *) head->data;
            int end = curr->end_vertex;
            int weight = curr->weight;
            if (vertices[end]->weight > (vertices[start_node]->weight + weight)) {
                vertices[end]->weight = vertices[start_node]->weight + weight;
                if (!vertices[end]->in_heap) { // add vertex to min heap if not already in it
                    insert(min_heap, vertices[end]);
                    vertices[end]->in_heap = true;
                }
            }
            head = head->next;
        }
    }
    free(min_heap->elems);
    free(min_heap);
    free_dijkstra_vertices(vertices, v_list->size);
    return longest_paths;
}

/* Each thread generates **paths, which is a list of the longest edges it found. This function is called
   to merge **paths into the main list of longest paths; called for each thread */ 
void merge_longest_paths(Edge **paths, Edge **longest_paths) {
    // Brute force; create an array of size SAVED_PATHS * 2 and fill it with 
    // the current main list of longest paths and those local to the calling thread.
    // Sort and take the top 10; this is our new longest paths list.
    // NOTE: there is some bug here where occassionally duplicate edges get added. 
    int union_size = SAVED_PATHS * 2;
    Edge **temp = malloc(sizeof(Edge *) * union_size);
    for (int i = 0; i < SAVED_PATHS; i++) {
        temp[i] = longest_paths[i];
    }
    for (int i = SAVED_PATHS; i < union_size; i++) {
            temp[i] = paths[i - SAVED_PATHS];
    }
    qsort(temp, union_size, sizeof(Edge *), sort_edges);
    int start = union_size - 10 >= 0 ? union_size - 10 : 0;
    for (int i = start; i < union_size; i++) {
        longest_paths[i - start]->weight = temp[i]->weight;
        longest_paths[i - start]->start_vertex = temp[i]->start_vertex;
        longest_paths[i - start]->end_vertex = temp[i]->end_vertex;
    }
    // Free extra arrays
    free_edge_list(paths, SAVED_PATHS);
    free(temp);
}

/* Given an input file with n vertices and m edges, run dijkstra's algorithm n times, starting from each of the vertices.
   Output to a file the 10 longest paths generated. */
int main(const int argc, const char** argv) {
    int n_nodes;                    // number of nodes in input file
    int n_edges;                    // number of edges in input file
    clock_t start, end;             // to store start/end time of algorithm
    if (argc != 2) {
        printf("Usage: ./dijksta <input_file>.txt. Aborting.\n");
        return 1;
    }
    /******** read in all vertices/edges *********/
    VertexList *v_list = malloc(sizeof(VertexList));
    read_in_file(v_list, argv[1], &n_nodes, &n_edges); // initialize vertex list
    Edge **longest_paths = prep_longest_paths();

    start = clock();
    /********** do dijkstra starting from each vertex **********/
    // parallelize dijkstra on the n vertices
    int start_node;
    omp_set_num_threads(NUM_THREADS); 
 #pragma omp parallel \
    private(start_node)
    {
    #pragma omp for schedule(static)
    for (start_node = 0; start_node < v_list->size; start_node++) {
        Edge **paths = run_dijkstra(v_list, start_node);
        # pragma omp critical
        {
        merge_longest_paths(paths, longest_paths); // threads must do this individually
        }
    }
    }
    end = clock();
    double total_time = (double)(end-start)/ CLOCKS_PER_SEC;
    
    /******** print 10 longest to file ********/
    FILE *output_file = fopen("output.txt", "w");
    for (int i = 0; i < SAVED_PATHS; i++) {
        fprintf(output_file, "start vertex %d, end vertex %d, distance %d\n", longest_paths[i]->start_vertex, longest_paths[i]->end_vertex, longest_paths[i]->weight);
    }
    fprintf(output_file, "Input file: %s. Total time: %lf seconds (%lf minutes). Average time per Dijkstra run: %lf.\n", argv[1], total_time, total_time/60, total_time/v_list->size);
    fclose(output_file);

    /******** free everything *************/
    free_edge_list(longest_paths, SAVED_PATHS);
    for (int i = 0; i < v_list->size; i++) {
        list_clear(v_list->vertices[i]->edges);
        free(v_list->vertices[i]->edges);
        free(v_list->vertices[i]);
    }
    free(v_list->vertices);
    free(v_list);
}
