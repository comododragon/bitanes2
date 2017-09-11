#ifndef GRAPH_H
#define GRAPH_H

#ifdef TODO
#else
typedef struct {
	int n;
	int **adj;
} graph_t;

void graph_create(graph_t **graph, unsigned int n, unsigned int m);
void graph_putEdge(graph_t *graph, unsigned int orig, unsigned int dest);
int graph_getEdge(graph_t *graph, unsigned int orig, unsigned int dest);
void graph_destroy(graph_t **graph);
#endif

#endif
