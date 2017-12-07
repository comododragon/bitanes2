/* ********************************************************************************************* */
/* * Simple implementation for Brandes Betweenness Algorithm: bitanes2: MPI Version 2          * */
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
#define BATCH_SIZE 100

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
	MPI_Status mpiStatus;

	/* Auxiliary variables */
	int i;
	int mpiSlaveNode, mpiBatchStart, mpiBatchEnd;
	char *inputFilename;
	char *outputFilename = NULL;
	FILE *inputFile = NULL;
	FILE *outputFile = NULL;
	unsigned int graphSizes[2];
	graph_t *graph = NULL;
	unsigned int taskCount;
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

	/* Master node */
	if(!mpiRank) {
		/* Check if command line arguments were passed correctly */
		ASSERT_CALL(2 == argc, fprintf(stderr, "Usage: %s INPUTFILE\n", argv[0]));
		inputFilename = argv[1];
		outputFilename = swapOrAddExtension(inputFilename, "btw");

		/* Since this is a master-slave approach, at least two nodes are needed */
		ASSERT_CALL(mpiTotal > 1, fprintf(stderr, "Error: run with at least two MPI processes.\n"));

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

		/* Wait for a request from one of the slave nodes */
		for(taskCount = 0; taskCount < graphSizes[0]; taskCount += BATCH_SIZE) {
			MPI_Recv(&mpiSlaveNode, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &mpiStatus);
			/* Send a task of size BATCH_SIZE */
			MPI_Send(&taskCount, 1, MPI_INT, mpiSlaveNode, 2, MPI_COMM_WORLD);
		}

		/* All tasks are over. Send -1 to all slave nodes. This will finish their execution */
		taskCount = -1;
		for(i = 1; i < mpiTotal; i++)
			MPI_Send(&taskCount, 1, MPI_INT, i, 2, MPI_COMM_WORLD);

		cb = calloc(graphSizes[0], sizeof(double));
	}
	/* Slave nodes */
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

		cb = calloc(graphSizes[0], sizeof(double));
		sigma = malloc(graphSizes[0] * sizeof(int));
		d = malloc(graphSizes[0] * sizeof(int));
		delta = malloc(graphSizes[0] * sizeof(double));

		/* Execute until no more tasks are available */
		while(1) {
			/* Send this node's ID to master node */
			MPI_Send(&mpiRank, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
			/* Wait for an answer with a batch to process */
			MPI_Recv(&mpiBatchStart, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &mpiStatus);

			/* Received task is -1. It's time to go home and rest */
			if(-1 == mpiBatchStart)
				break;

			/* Beginning of Brandes Algorithm */

			P = calloc(graphSizes[0], sizeof(list_t *));

			/* If this is the last batch, it may be smaller than BATCH_SIZE */
			mpiBatchEnd = ((mpiBatchStart + BATCH_SIZE) >= graphSizes[0])? graphSizes[0] : (mpiBatchStart + BATCH_SIZE);

			/* Process received batch */
			for(s = mpiBatchStart; s < mpiBatchEnd; s++) {
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
		}
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
