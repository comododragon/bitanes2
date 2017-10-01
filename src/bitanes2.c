#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/common.h"
#include "graph.h"
#include "list.h"

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
	list_t *S = NULL;
	list_t **P = NULL;
	int *sigma = NULL;
	int *d = NULL;
	double *delta = NULL;
	list_t *Q = NULL;

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
	printf("%d %d\n", n, m);
	for(i = 0; i < n; i++) {
		for(j = 0; j < n; j++)
			printf("%d ", graph_getEdge(graph, i, j));
		putchar('\n');
	}
#endif

	P = calloc(n, sizeof(list_t *));
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

		//dlist_pushBack(Q, s);
		qsdlist_pushBack(Q, s);

		while(!dlist_isEmpty(Q)) {
			v = dlist_front(Q);
			//dlist_popFront(Q);
			qdlist_popFront(Q);
			dlist_pushFront(S, v);

			for(w = 0; w < n; w++) {
				if(graph_getEdge(graph, v, w)) {
					if(d[w] < 0) {
						//dlist_pushBack(Q, w);
						qwdlist_pushBack(Q, w);
						d[w] = d[v] + 1;
					}

					if((d[v] + 1) == d[w]) {
						sigma[w] = sigma[w] + sigma[v];
						pvdlist_pushBack(P[w], v);
					}
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
				//dlist_popFront(P[w]);
				pdlist_popFront(P[w]);

				delta[v] = delta[v] + ((sigma[v] / ((double) sigma[w])) * (1 + delta[w]));
			}

			if(w != s)
				cb[w] = cb[w] + delta[w];
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

	for(v = 0; v < n; v++)
		printf("cb[%d] = %lf\n", v, cb[v] / 2.0);

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
		for(i = 0; i < n; i++) {
			if(P[i])
				dlist_destroy(&P[i]);
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
