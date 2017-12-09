/* ********************************************************************************************* */
/* * Simple implementation for Brandes Betweenness Algorithm: bitanes2: PThreads Version       * */
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
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "brandes.h"
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
	/* If no number of threads is passed as argument, we will use omp_get_num_procs() */
	int numThreads = 2;
	FILE *inputFile = NULL;
	FILE *outputFile = NULL;
	unsigned int chunkSize, chunkSizeRem;
	threadpack_t *packs = NULL;
	pthread_t *threads = NULL;
	/* Variables named according to the algorithm in Brandes Algorithm */
	unsigned int n, m;
	graph_t *graph = NULL;
	double *cb = NULL;
	double *cbReduced = NULL;

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
	cb = calloc(n * numThreads, sizeof(double));
	cbReduced = calloc(n,  sizeof(double));

	/* Read edges from file */
	unsigned int orig, dest;
	for(i = 0; i < m; i++) {
		fscanf(inputFile, "%d %d", &orig, &dest);
		graph_putEdge(graph, orig, dest);
		graph_putEdge(graph, dest, orig);
	}

	/* Calculate chunk sizes for each thread and allocate threads stuff */
	chunkSize = n / numThreads;
	chunkSizeRem = n % numThreads;
	packs = malloc(numThreads * sizeof(threadpack_t));
	threads = malloc(numThreads * sizeof(pthread_t));

	/* Prepare and dispatch threads */
	for(i = 0; i < numThreads; i++) {
		/* Prepare the data pack for each thread */
		packs[i].n = n;
		packs[i].graph = graph;
		packs[i].cb = cb;
		packs[i].chunkSize = chunkSize;
		packs[i].chunkSizeRem = chunkSizeRem;
		packs[i].threadId = i;

		/* Create i-th thread */
		int threadRet = pthread_create(&threads[i], NULL, brandes, &packs[i]);
		ASSERT_CALL(0 == threadRet, fprintf(stderr, "Error: pthread_create() failed\n"));
	}

	/* Wait for all threads to finish */
	for(i = 0; i < numThreads; i++)
		pthread_join(threads[i], NULL);

	/* At last, reduce and print results */
	for(i = 0; i < n; i++) {
		int j;
		for(j = 0; j < numThreads; j++)
			cbReduced[i] += cb[i + (n * j)];

		fprintf(outputFile, "%lf\n", cbReduced[i] / 2.0);
	}

_err:

	if(threads)
		free(threads);

	if(packs)
		free(packs);

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

	return EXIT_SUCCESS;
}
