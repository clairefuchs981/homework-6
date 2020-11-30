
#ifndef DIJKSTRA_H
#define DIJKSTRA_H
// Vertex struct
typedef struct vertex_ {
    int   index;     // integer id for vertex
    List *edges;     // list of Edges starting from this vertex
    int   weight;    // weight of this vertex
    bool  in_heap;   // true if vertex is currently in min_heap 
} Vertex;

// Struct holding list of vertices 
typedef struct vertexlist_ {
    Vertex **vertices;  // list of vertices
    int      size;      // number of vertices in list
} VertexList;

// Struct for directed Edge of Vertex
typedef struct edge_ {
    int start_vertex;  // from-vertex of edge
    int end_vertex;    // to-vertex of edge
    int weight;        // weight of edge
} Edge;
#endif