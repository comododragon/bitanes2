/* ********************************************************************************************* */
/* * Simple implementation for Brandes Betweenness Algorithm: bitanes2: MPI Version            * */
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
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

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
	MPI_Init(&argc, &argv);
	int mpiTotal, mpiRank;

	/* Auxiliary variables */
	int i;
	int mpiChunkStart, mpiChunkStop;
	char *inputFilename;
	char *outputFilename = NULL;
	FILE *inputFile = NULL;
	FILE *outputFile = NULL;
	unsigned int graphSizes[2];
	graph_t *graph = NULL;
	/* Variables named according to the algorithm in Brandes Algorithm */
	double *cb = NULL;
	double *cbReduced = NULL;
	int t, s, v, w;
	list_t *S = NULL;
	list_t **P = NULL;
	int *sigma = NULL;
	int *d = NULL;
	double *delta = NULL;
	list_t *Q = NULL;
	unsigned int noOfAdjacents;
	int *adjacents;

	/* Get number of nodes and this node's number */
	ASSERT_CALL(MPI_SUCCESS == MPI_Comm_size(MPI_COMM_WORLD, &mpiTotal), fprintf(stderr, "Error: MPI error\n"));
	ASSERT_CALL(MPI_SUCCESS == MPI_Comm_rank(MPI_COMM_WORLD, &mpiRank), fprintf(stderr, "Error: MPI error\n"));

	/* Execute on root node only */
	if(!mpiRank) {
		/* Check if command line arguments were passed correctly */
		ASSERT_CALL(2 == argc, fprintf(stderr, "Usage: %s INPUTFILE\n", argv[0]));
		inputFilename = argv[1];
		outputFilename = swapOrAddExtension(inputFilename, "btw");

		/* Open input and output files and check their existence */
		inputFile = fopen(inputFilename, "r");
		ASSERT_CALL(inputFile, fprintf(stderr, "Error: %s: %s\n", strerror(errno), inputFilename));
		outputFile = fopen(outputFilename, "w");
		ASSERT_CALL(outputFile, fprintf(stderr, "Error: %s: %s\n", strerror(errno), outputFilename));

		/* Read file header and allocate stuff */
		fscanf(inputFile, "%d", &graphSizes[0]);
		fscanf(inputFile, "%d", &graphSizes[1]);

		/* Broadcast the graph size and number of edges */
		MPI_Bcast(graphSizes, 2, MPI_INT, 0, MPI_COMM_WORLD);
		graph_create(&graph, graphSizes[0], graphSizes[1]);

		/* Read edges from file */
		unsigned int orig, dest;
		for(i = 0; i < graphSizes[1]; i++) {
			fscanf(inputFile, "%d %d", &orig, &dest);
			/* Broadcast this edge */
			MPI_Bcast(&orig, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast(&dest, 1, MPI_INT, 0, MPI_COMM_WORLD);
			graph_putEdge(graph, orig, dest);
			graph_putEdge(graph, dest, orig);
		}
	}
	/* Execute on non-root nodes */
	else {
		/* Read graph sizes coming through broadcast */
		MPI_Bcast(graphSizes, 2, MPI_INT, 0, MPI_COMM_WORLD);
		graph_create(&graph, graphSizes[0], graphSizes[1]);

		/* Read edges coming through broadcast */
		unsigned int orig, dest;
		for(i = 0; i < graphSizes[1]; i++) {
			MPI_Bcast(&orig, 1, MPI_INT, 0, MPI_COMM_WORLD);
			MPI_Bcast(&dest, 1, MPI_INT, 0, MPI_COMM_WORLD);
			graph_putEdge(graph, orig, dest);
			graph_putEdge(graph, dest, orig);
		}
	}

	cb = calloc(graphSizes[0], sizeof(double));
	sigma = malloc(graphSizes[0] * sizeof(int));
	d = malloc(graphSizes[0] * sizeof(int));
	delta = malloc(graphSizes[0] * sizeof(double));

	/* Partition the problem into chunks of graph nodes of similar sizes */
	int chunkSize = graphSizes[0] / mpiTotal;
	int chunkSizeRem = graphSizes[0] % mpiTotal;
	if(mpiRank < chunkSizeRem) {
		mpiChunkStart = mpiRank * (chunkSize + 1);
		mpiChunkStop = mpiChunkStart + chunkSize + 1;
	}
	else {
		mpiChunkStart = (mpiRank * chunkSize) + chunkSizeRem;
		mpiChunkStop = mpiChunkStart + chunkSize;
	}

	/* Beginning of Brandes Algorithm */

	P = calloc(graphSizes[0], sizeof(list_t *));

	/* Each node will work on its chunk */
	for(s = mpiChunkStart; s < mpiChunkStop; s++) {
		S = dlist_create();
		for(w = 0; w < graphSizes[0]; w++)
			P[w] = dlist_create();
		for(t = 0; t < graphSizes[0]; t++) {
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

		for(v = 0; v < graphSizes[0]; v++)
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
				cb[w] = cb[w] + delta[w];
		}

		dlist_destroy(&Q);
		Q = NULL;
		for(w = 0; w < graphSizes[0]; w++) {
			dlist_destroy(&P[w]);
			P[w] = NULL;
		}
		dlist_destroy(&S);
		S = NULL;
	}

	/* Reduce the cb[] array of all nodes. Result is the final betweenness value */
	cbReduced = malloc(graphSizes[0] * sizeof(double));
	MPI_Reduce(cb, cbReduced, graphSizes[0], MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if(!mpiRank) {
		/* At last, print results */
		for(v = 0; v < graphSizes[0]; v++)
			fprintf(outputFile, "%lf\n", cbReduced[v] / 2.0);
	}

_err:

	if(delta)
		free(delta);

	if(Q)
		dlist_destroy(&Q);

	if(d)
		free(d);

	if(sigma)
		free(sigma);

	if(P) {
		for(i = 0; i < graphSizes[0]; i++) {
			if(P[i])
				dlist_destroy(&P[i]);
		}

		free(P);
	}

	if(cbReduced)
		free(cbReduced);

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

	MPI_Finalize();

	return EXIT_SUCCESS;
}
