typedef struct min_heap_elem_ {
    int vertex;
    int weight;
} MinHeapElem;

typedef struct binary_min_heap_ {
    int size;
    int start_pt;
    int end_pt; 
    //MinHeapElem **elems;
    Vertex      **elems;
} BinaryMinHeap;

BinaryMinHeap* make_binary_min_heap(VertexList *v_list, int root_vertex);
void print_heap(BinaryMinHeap *min_heap);
bool is_empty(BinaryMinHeap *min_heap);
int del_min(BinaryMinHeap *min_heap);
void insert(BinaryMinHeap *min_heap, Vertex *vertex);