#include "graph.h"

#include <stdlib.h>

#ifdef TODO
#else
void graph_create(graph_t **graph, unsigned int n, unsigned int m) {
	int i, j;

	if(!graph)
		return;

	*graph = malloc(sizeof(graph_t));
	(*graph)->n = n;
	(*graph)->adj = malloc(n * sizeof(int *));

	for(i = 0; i < n; i++) {
		(*graph)->adj[i] = malloc(n * sizeof(int));

		for(j = 0; j < n; j++)
			(*graph)->adj[i][j] = 0;
		//(*graph)->adj[i][i] = 0;
	}
}

void graph_putEdge(graph_t *graph, unsigned int orig, unsigned int dest) {
	if(graph)
		graph->adj[orig][dest] = 1;
}

int graph_getEdge(graph_t *graph, unsigned int orig, unsigned int dest) {
	if(graph)
		return graph->adj[orig][dest];
	else
		return -1;
}

void graph_destroy(graph_t **graph) {
	int i;

	if(graph && *graph) {
		for(i = 0; i < (*graph)->n; i++)
			free((*graph)->adj[i]);
		free((*graph)->adj);

		free(*graph);
	}
}
#endif
