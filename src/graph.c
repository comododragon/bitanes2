/* ********************************************************************************************* */
/* * Simple library for undirected unweighted graphs: libgraph                                 * */
/* * Author: André Bannwart Perina                                                             * */
/* ********************************************************************************************* */
/* * Copyright (c) 2017 André B. Perina                                                        * */
/* *                                                                                           * */
/* * libgraph is free software: you can redistribute it and/or modify it under the terms of    * */
/* * the GNU General Public License as published by the Free Software Foundation, either       * */
/* * version 3 of the License, or (at your option) any later version.                          * */
/* *                                                                                           * */
/* * libgraph is distributed in the hope that it will be useful, but WITHOUT ANY               * */
/* * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A           * */
/* * PARTICULAR PURPOSE.  See the GNU General Public License for more details.                 * */
/* *                                                                                           * */
/* * You should have received a copy of the GNU General Public License along with libgraph.    * */
/* * If not, see <http://www.gnu.org/licenses/>.                                               * */
/* ********************************************************************************************* */

#include "graph.h"

#include <math.h>
#include <stdlib.h>

/**
 * @brief Create a graph with unconnected nodes.
 */
void graph_create(graph_t **graph, unsigned int n, unsigned int m) {
	int i;

	if(!graph)
		return;

	*graph = malloc(sizeof(graph_t));
	(*graph)->n = n;
	(*graph)->adj = malloc(n * sizeof(int *));

#ifdef GRAPH_USE_ADJ_MATRIX
	/* Create adjacency matrix for a totally disconnected graph */
	for(i = 0; i < n; i++)
		(*graph)->adj[i] = calloc(n, sizeof(int));
#else
	/* Adjacency lists for each node are allocated on the fly */
	for(i = 0; i < n; i++)
		(*graph)->adj[i] = NULL;

	/* Chunk size of the adjacency lists: mallocs on the adjacency lists are made by chunks */
	/* Here we assume that every node will have a similar degree */
	(*graph)->chunkSz = (int) ceilf(m / (float) n);
#endif
}

/**
 * @brief Connect two nodes.
 */
void graph_putEdge(graph_t *graph, unsigned int orig, unsigned int dest) {
#ifdef GRAPH_USE_ADJ_MATRIX
	/* If adjacency matrix is used, connecting two nodes is done simply by putting 1 on the respective matrix element */
	if(graph)
		graph->adj[orig][dest] = 1;
#else
	if(graph) {
		/* Adjacency list for this node has already been created */
		if(graph->adj[orig]) {
			/* Element [orig][0] holds how many elements were already allocated in this list */
			/* Element [orig][1] holds how many elements of this allocated list was already used */
			/* If both are equal, it means that all chunks are filled. We need to reallocate */
			if(graph->adj[orig][0] == graph->adj[orig][1]) {
				/* Increase the size of this list in chunkSz elements */
				graph->adj[orig][0] += graph->chunkSz;
				graph->adj[orig] = realloc(graph->adj[orig], (graph->adj[orig][0] + 2) * sizeof(int));
			}
		}
		/* No adjacency list created yet */
		else {
			/* Allocate a list of size chunkSz and define that no space was used yet */
			graph->adj[orig] = malloc((graph->chunkSz + 2) * sizeof(int));
			graph->adj[orig][0] = graph->chunkSz;
			graph->adj[orig][1] = 0;
		}

		/* Add dest to the adjacency list and increase the number of filled elements */
		graph->adj[orig][graph->adj[orig][1] + 2] = dest;
		(graph->adj[orig][1])++;
	}
#endif
}

/**
 * @brief Check if two nodes are connected.
 */
int graph_getEdge(graph_t *graph, unsigned int orig, unsigned int dest) {
#ifdef GRAPH_USE_ADJ_MATRIX
	/* Piece of cake for adjacency matrix */
	return graph? graph->adj[orig][dest] : 0;
#else
	int i;

	/* Iterate through the orig's adjacency list trying to find if the dest node is present */
	if(graph && graph->adj[orig]) {
		for(i = 0; i < graph->adj[orig][1]; i++) {
			if(dest == graph->adj[orig][i + 2])
				return 1;
		}

		return 0;
	}
	else {
		return 0;
	}
#endif
}

#ifndef GRAPH_USE_ADJ_MATRIX
/**
 * @brief Get the adjacency list for a given node.
 */
int *graph_getAdjacents(graph_t *graph, unsigned int orig, unsigned int *noOfAdjacents) {
	/* Graph exists and orig node has adjacents */
	if(graph && graph->adj[orig]) {
		*noOfAdjacents = graph->adj[orig][1];
		/* Return the list starting from address 2 (since 0 and 1 are metadata) */
		return &(graph->adj[orig][2]);
	}

	*noOfAdjacents = 0;
	return NULL;
}
#endif

/**
 * @brief Destroy a graph; free memory.
 */
void graph_destroy(graph_t **graph) {
	int i;

	if(graph && *graph) {
		for(i = 0; i < (*graph)->n; i++)
			free((*graph)->adj[i]);
		free((*graph)->adj);

		free(*graph);
	}
}
