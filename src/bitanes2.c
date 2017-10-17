#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/common.h"
#include "graph.h"

#ifdef USE_STAQ
#include "staq.h"
#define LIST_TYPE staq_t
#define LIST_CREATE(size) dstaq_create(size)
#define LIST_PUSHBACK dstaq_pushBack
#define LIST_ISEMPTY dstaq_isEmpty
#define LIST_FRONT dstaq_front
#define LIST_POPFRONT dstaq_popFront
#define LIST_PUSHFRONT dstaq_pushFront
#define LIST_DESTROY dstaq_destroy
#else
#include "list.h"
#define LIST_TYPE list_t
#define LIST_CREATE(size) dlist_create()
#define LIST_PUSHBACK dlist_pushBack
#define LIST_ISEMPTY dlist_isEmpty
#define LIST_FRONT dlist_front
#define LIST_POPFRONT dlist_popFront
#define LIST_PUSHFRONT dlist_pushFront
#define LIST_DESTROY dlist_destroy
#endif

#define MAX_STR_SZ 256

char *swapOrAddExtension(char *inputFilename, char *extension) {
	int i;
	char *outputFilename = NULL;

	for(i = strnlen(inputFilename, MAX_STR_SZ); i >= 0; i--) {
		if('.' == inputFilename[i]) {
			outputFilename = calloc(i + strnlen(extension, MAX_STR_SZ) + 2, sizeof(char));
			strncpy(outputFilename, inputFilename, i + 1);
			strcpy(&outputFilename[i + 1], extension);
			break;
		}
	}

	if(-1 == i) {
		outputFilename = calloc(strnlen(inputFilename, MAX_STR_SZ) + strnlen(extension, MAX_STR_SZ) + 2, sizeof(char));
		strcpy(outputFilename, inputFilename);
		outputFilename[strnlen(inputFilename, MAX_STR_SZ)] = '.';
		strcpy(&outputFilename[strnlen(inputFilename, MAX_STR_SZ) + 1], extension);
	}

	return outputFilename;
}

int main(int argc, char *argv[]) {
	int i;
	char *inputFilename;
	char *outputFilename = NULL;
	FILE *inputFile = NULL;
	FILE *outputFile = NULL;
	unsigned int n, m;
	graph_t *graph = NULL;
	double *cb = NULL;
	int t, s, v, w;
	LIST_TYPE *S = NULL;
	LIST_TYPE **P = NULL;
	int *sigma = NULL;
	int *d = NULL;
	double *delta = NULL;
	LIST_TYPE *Q = NULL;
#ifdef GRAPH_USE_GET_ADJACENTS
	unsigned int noOfAdjacents;
	int *adjacents;
#endif

	ASSERT_CALL(2 == argc, fprintf(stderr, "Usage: %s INPUTFILE\n", argv[0]));
	inputFilename = argv[1];
	outputFilename = swapOrAddExtension(inputFilename, "btw");

	inputFile = fopen(inputFilename, "r");
	ASSERT_CALL(inputFile, fprintf(stderr, "Error: %s: %s\n", strerror(errno), inputFilename));

	outputFile = fopen(outputFilename, "w");
	ASSERT_CALL(outputFile, fprintf(stderr, "Error: %s: %s\n", strerror(errno), outputFilename));

	fscanf(inputFile, "%d", &n);
	fscanf(inputFile, "%d", &m);
	graph_create(&graph, n, m);
	cb = calloc(n, sizeof(double));
	sigma = malloc(n * sizeof(int));
	d = malloc(n * sizeof(int));
	delta = malloc(n * sizeof(double));

	unsigned int orig, dest;
	for(i = 0; i < m; i++) {
		fscanf(inputFile, "%d %d", &orig, &dest);
		graph_putEdge(graph, orig, dest);
		graph_putEdge(graph, dest, orig);
	}

#if 0
	int j;
	printf("%d %d\n", n, m);
	for(i = 0; i < n; i++) {
		for(j = 0; j < n; j++)
			printf("%d ", graph_getEdge(graph, i, j));
		putchar('\n');
	}
#endif

	P = calloc(n, sizeof(LIST_TYPE *));
	for(s = 0; s < n; s++) {
		S = LIST_CREATE(n);
		for(w = 0; w < n; w++)
			P[w] = LIST_CREATE(n);
		for(t = 0; t < n; t++) {
			sigma[t] = 0;
			d[t] = -1;
		}
		sigma[s] = 1;
		d[s] = 0;
		Q = LIST_CREATE(n);

		LIST_PUSHBACK(Q, s);

		while(!LIST_ISEMPTY(Q)) {
			v = LIST_FRONT(Q);
			LIST_POPFRONT(Q);
			LIST_PUSHFRONT(S, v);

#ifdef GRAPH_USE_GET_ADJACENTS
			adjacents = graph_getAdjacents(graph, v, &noOfAdjacents);
			for(i = 0; i < noOfAdjacents; i++) {
				w = adjacents[i];

				{
#else
			for(w = 0; w < n; w++) {
				if(graph_getEdge(graph, v, w)) {
#endif
					if(d[w] < 0) {
						LIST_PUSHBACK(Q, w);
						d[w] = d[v] + 1;
					}

					if((d[v] + 1) == d[w]) {
						sigma[w] = sigma[w] + sigma[v];
						LIST_PUSHBACK(P[w], v);
					}
				}

			}
		}

		for(v = 0; v < n; v++)
			delta[v] = 0;

		while(!LIST_ISEMPTY(S)) {
			w = LIST_FRONT(S);
			LIST_POPFRONT(S);

			while(!LIST_ISEMPTY(P[w])) {
				v = LIST_FRONT(P[w]);
				LIST_POPFRONT(P[w]);

				delta[v] = delta[v] + ((sigma[v] / ((double) sigma[w])) * (1 + delta[w]));
			}

			if(w != s)
				cb[w] = cb[w] + delta[w];
		}

		LIST_DESTROY(&Q);
		Q = NULL;
		for(w = 0; w < n; w++) {
			LIST_DESTROY(&P[w]);
			P[w] = NULL;
		}
		LIST_DESTROY(&S);
		S = NULL;
	}

	for(v = 0; v < n; v++)
		fprintf(outputFile, "%lf\n", cb[v] / 2.0);

_err:

	if(delta)
		free(delta);

	if(Q)
		LIST_DESTROY(&Q);

	if(d)
		free(d);

	if(sigma)
		free(sigma);

	if(P) {
		for(i = 0; i < n; i++) {
			if(P[i])
				LIST_DESTROY(&P[i]);
		}

		free(P);
	}

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
