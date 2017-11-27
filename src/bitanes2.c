/* ********************************************************************************************* */
/* * Simple implementation for Brandes Betweenness Algorithm: bitanes2                         * */
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

#include <errno.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/common.h"
#include "graph.h"
#include "list.h"

#define MAX_STR_SZ 256

/**
 * @brief Swap the extension of a file (or add it if the file has none.
 * @param inputFilename The input filename.
 * @param extesion The new extension, without initial dot.
 * @return The filename with the new extension, developer should free it after use.
 */
char *swapOrAddExtension(char *inputFilename, char *extension) {
	int i;
	char *outputFilename = NULL;

	/* Find for the dot in the input filename */
	for(i = strnlen(inputFilename, MAX_STR_SZ); i >= 0; i--) {
		/* Dot found, make swap */
		if('.' == inputFilename[i]) {
			outputFilename = calloc(i + strnlen(extension, MAX_STR_SZ) + 2, sizeof(char));
			strncpy(outputFilename, inputFilename, i + 1);
			strcpy(&outputFilename[i + 1], extension);
			break;
		}
	}

	/* No dot found, just add the extension */
	if(-1 == i) {
		outputFilename = calloc(strnlen(inputFilename, MAX_STR_SZ) + strnlen(extension, MAX_STR_SZ) + 2, sizeof(char));
		strcpy(outputFilename, inputFilename);
		outputFilename[strnlen(inputFilename, MAX_STR_SZ)] = '.';
		strcpy(&outputFilename[strnlen(inputFilename, MAX_STR_SZ) + 1], extension);
	}

	return outputFilename;
}

int main(int argc, char *argv[]) {
	/* Auxiliary variables */
	int i;
	char *inputFilename;
	char *outputFilename = NULL;
	int numThreads = omp_get_num_procs();
	FILE *inputFile = NULL;
	FILE *outputFile = NULL;
	/* Variables named according to the algorithm in Brandes Algorithm */
	unsigned int n, m;
	graph_t *graph = NULL;
	double *cb = NULL;

	/* Check if command line arguments were passed correctly */
	ASSERT_CALL((2 == argc) || (3 == argc), fprintf(stderr, "Usage: %s INPUTFILE [NUMTHREADS]\n", argv[0]));
	inputFilename = argv[1];
	outputFilename = swapOrAddExtension(inputFilename, "btw");
	if(3 == argc)
		numThreads = atoi(argv[2]);

	/* Open input and output files and check their existence */
	inputFile = fopen(inputFilename, "r");
	ASSERT_CALL(inputFile, fprintf(stderr, "Error: %s: %s\n", strerror(errno), inputFilename));
	outputFile = fopen(outputFilename, "w");
	ASSERT_CALL(outputFile, fprintf(stderr, "Error: %s: %s\n", strerror(errno), outputFilename));

	/* Read file header and allocate stuff */
	fscanf(inputFile, "%d", &n);
	fscanf(inputFile, "%d", &m);
	graph_create(&graph, n, m);
	cb = calloc(n, sizeof(double));

	/* Read edges from file */
	unsigned int orig, dest;
	for(i = 0; i < m; i++) {
		fscanf(inputFile, "%d %d", &orig, &dest);
		graph_putEdge(graph, orig, dest);
		graph_putEdge(graph, dest, orig);
	}

	/* Beginning of Brandes Algorithm */

#pragma omp parallel default(none) private(i) shared(n, graph, cb) num_threads(numThreads)
	{
		int t, s, v, w;
		int *sigma = malloc(n * sizeof(int));
		int *d = malloc(n * sizeof(int));
		double *delta = malloc(n * sizeof(double));
		list_t *S = NULL;
		list_t *Q = NULL;
		unsigned int noOfAdjacents;
		int *adjacents;

		list_t **P = calloc(n, sizeof(list_t *));
#pragma omp for
		for(s = 0; s < n; s++) {
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
	
				if(w != s) {
#pragma omp atomic
					cb[w] = cb[w] + delta[w];
				}
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
	}

	/* At last, print results */
	for(i = 0; i < n; i++)
		fprintf(outputFile, "%lf\n", cb[i] / 2.0);

_err:

	if(cb)
		free(cb);

	if(graph)
		graph_destroy(&graph);

	if(outputFile)
		fclose(outputFile);

	if(inputFile)
		fclose(inputFile);

	if(outputFilename)
		free(outputFilename);

	return EXIT_SUCCESS;
}
