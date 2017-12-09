/* ********************************************************************************************* */
/* * Brandes Betweenness Algorithm Main Loop Thread Header File                                * */
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

#ifndef BRANDES_H
#define BRANDES_H

#include "graph.h"

/* Metadata data pack */
typedef struct {
	unsigned int n;
	graph_t *graph;
	double *cb;
	unsigned int chunkSize;
	unsigned int chunkSizeRem;
	unsigned int threadId;
} threadpack_t;

/**
 * @brief Main Brandes loop
 * @param threadPack Data pack containing metadata for processing
 * @return NULL
 */
void *brandes(void *threadPack);

#endif
