CC=gcc
CFLAGS=-I. -lm -pedantic -Wall
OBJ = list.o dijkstra.o edge_util.o binary_min_heap.o

dijkstra: $(OBJ)
	$(CC) -o dijkstra $(OBJ)
