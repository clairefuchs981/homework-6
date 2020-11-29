
#ifndef DIJKSTRA_H
#define DIJKSTRA_H
typedef struct vertex_ {
    int index;
    List *edges;
    int weight;
    bool visited;
    bool in_heap;
} Vertex;

typedef struct vertexlist_ {
    Vertex **vertices;
    int      size;
    int      root; 
} VertexList;

typedef struct edge_ {
    int start_vertex;
    int end_vertex;
    int weight;
} Edge;
#endif