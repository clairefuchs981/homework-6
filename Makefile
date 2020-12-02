CC=gcc
CFLAGS=-I. -lm -pedantic -Wall -fopenmp
OBJ = list.o dijkstra.o edge_util.o binary_min_heap.o
OBJP = list.o dijkstra_parallel.o edge_util.o binary_min_heap_parallel.o

default: dijkstra dijkstra_parallel

dijkstra: $(OBJ)
	$(CC) -o dijkstra $(OBJ)

dijkstra_parallel: $(OBJ)
	$(CC) -o dijkstra $(OBJ)