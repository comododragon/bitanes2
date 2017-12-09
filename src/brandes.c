/* ********************************************************************************************* */
/* * Brandes Betweenness Algorithm Main Loop Thread                                            * */
/* * Author: André Bannwart Perina                                                             * */
/* * Algorithm: Brandes, Ulrik. "A faster algorithm for betweenness centrality."               * */
/* *            Journal of mathematical sociology 25.2 (2001): 163-177.                        * */
/* ********************************************************************************************* */
/* * Copyright (c) 2017 André B. Perina                                                        * */
/* *                                                                                           * */
/* * bitanes2 is free software: you can redistribute it and/or modify it under the terms of    * */
/* * the GNU General Public License as published by the Free Software Foundation, either       * */
/* * version 3 of the License, or (at your option) any later version.                          * */
/* *                                                                                           * */
/* * bitanes2 is distributed in the hope that it will be useful, but WITHOUT ANY               * */
/* * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A           * */
/* * PARTICULAR PURPOSE.  See the GNU General Public License for more details.                 * */
/* *                                                                                           * */
/* * You should have received a copy of the GNU General Public License along with bitanes2.    * */
/* * If not, see <http://www.gnu.org/licenses/>.                                               * */
/* ********************************************************************************************* */

#include "brandes.h"

#include <stdlib.h>

#include "graph.h"
#include "list.h"

/**
 * @brief Main Brandes loop
 */
void *brandes(void *threadPack) {
	/* Read received data pack */
	threadpack_t *pack = (threadpack_t *) threadPack;
	unsigned int n = pack->n;
	graph_t *graph = pack->graph;
	double *cb = pack->cb;
	unsigned int chunkSize = pack->chunkSize;
	unsigned int chunkSizeRem = pack->chunkSizeRem;
	unsigned int threadId = pack->threadId;
	unsigned int chunkStart, chunkStop;

	int i, t, s, v, w;
	int *sigma = malloc(n * sizeof(int));
	int *d = malloc(n * sizeof(int));
	double *delta = malloc(n * sizeof(double));
	list_t *S = NULL;
	list_t *Q = NULL;
	unsigned int noOfAdjacents;
	int *adjacents;

	list_t **P = calloc(n, sizeof(list_t *));

	/* Calculate the chunk of data this thread should execute */
	if(threadId < chunkSizeRem) {
		chunkStart = threadId * (chunkSize + 1);
		chunkStop = chunkStart + chunkSize + 1;
	}
	else {
		chunkStart = (threadId * chunkSize) + chunkSizeRem;
		chunkStop = chunkStart + chunkSize;
	}

	/* Main Brandes loop (or part of it) */
	for(s = chunkStart; s < chunkStop; s++) {
		S = dlist_create();
		for(w = 0; w < n; w++)
			P[w] = dlist_create();
		for(t = 0; t < n; t++) {
			sigma[t] = 0;
			d[t] = -1;
		}
		sigma[s] = 1;
		d[s] = 0;
		Q = dlist_create();

		dlist_pushBack(Q, s);

		while(!dlist_isEmpty(Q)) {
			v = dlist_front(Q);
			dlist_popFront(Q);
			dlist_pushFront(S, v);

			/* Smarter way of getting node neighbours: get all nodes w which are neighbours of v, no checking necessary */
			adjacents = graph_getAdjacents(graph, v, &noOfAdjacents);
			for(i = 0; i < noOfAdjacents; i++) {
				w = adjacents[i];
				if(d[w] < 0) {
					dlist_pushBack(Q, w);
					d[w] = d[v] + 1;
				}

				if((d[v] + 1) == d[w]) {
					sigma[w] = sigma[w] + sigma[v];
					dlist_pushBack(P[w], v);
				}
			}
		}

		for(v = 0; v < n; v++)
			delta[v] = 0;

		while(!dlist_isEmpty(S)) {
			w = dlist_front(S);
			dlist_popFront(S);

			while(!dlist_isEmpty(P[w])) {
				v = dlist_front(P[w]);
				dlist_popFront(P[w]);

				delta[v] = delta[v] + ((sigma[v] / ((double) sigma[w])) * (1 + delta[w]));
			}

			if(w != s)
				cb[w + (threadId * n)] += delta[w];
		}

		dlist_destroy(&Q);
		Q = NULL;
		for(w = 0; w < n; w++) {
			dlist_destroy(&P[w]);
			P[w] = NULL;
		}
		dlist_destroy(&S);
		S = NULL;
	}

	if(Q)
		dlist_destroy(&Q);

	if(P) {
		for(i = 0; i < n; i++) {
			if(P[i])
				dlist_destroy(&P[i]);
		}
		free(P);
	}
	if(delta)
		free(delta);
	if(d)
		free(d);
	if(sigma)
		free(sigma);

	return NULL;
}
