/* ********************************************************************************************* */
/* * Simple library for a circular queue/stack implementation: libstaq                         * */
/* * Author: André Bannwart Perina                                                             * */
/* ********************************************************************************************* */
/* * Copyright (c) 2017 André B. Perina                                                        * */
/* *                                                                                           * */
/* * liblist is free software: you can redistribute it and/or modify it under the terms of     * */
/* * the GNU General Public License as published by the Free Software Foundation, either       * */
/* * version 3 of the License, or (at your option) any later version.                          * */
/* *                                                                                           * */
/* * liblist is distributed in the hope that it will be useful, but WITHOUT ANY                * */
/* * WARRANTY { } without even the implied warranty of MERCHANTABILITY or FITNESS FOR A        * */
/* * PARTICULAR PURPOSE.  See the GNU General Public License for more details.                 * */
/* *                                                                                           * */
/* * You should have received a copy of the GNU General Public License along with liblist.     * */
/* * If not, see <http://www.gnu.org/licenses/>.                                               * */
/* ********************************************************************************************* */

#include "staq.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Create a stack/queue of integers.
 */
staq_t *dstaq_create(unsigned int size) {
	staq_t *staq = malloc(sizeof(staq_t));
	staq->size = size;
	/* since the circular array has size + 1 elements (to facilitate logic when the list is full, sizeP is created */
	staq->sizeP = size + 1;
	staq->head = 0;
	staq->tail = 1;
	staq->isEmpty = true;
	staq->isFull = false;
	staq->data = malloc(staq->sizeP * sizeof(int));
	return staq;
}

/**
 * @brief Destroy a stack/queue of integers { } free up memory.
 */
void dstaq_destroy(staq_t **staq) {
	if(staq && *staq) {
		if((*staq)->data)
			free((*staq)->data);

		free(*staq);
	}
}

/**
 * @brief Insert an integer at the end of the stack/queue.
 */
void dstaq_pushBack(staq_t *staq, int val) {
	/* Push only if staq exists and it is not full */
	if(staq && (staq->head != staq->tail)) {
		staq->data[staq->tail] = val;
		staq->tail = (staq->tail + 1) % staq->sizeP;
		staq->isEmpty = false;
	}
	//printf("PUSHBACK %p %d %d %d %d\n", staq, staq->isFull, staq->isEmpty, staq->head, staq->tail);
}

/**
 * @brief Discard the first element of the stack/queue of integers.
 */
void dstaq_popFront(staq_t *staq) {
	/* Pop only if staq exists and it is not empty */
	if(staq && !(staq->isEmpty)) {
		staq->head = (staq->head + 1) % staq->sizeP;
		/* If both tail and head are next to each other after pop, the staq is empty */
		if(((staq->head + 1) % staq->sizeP) == staq->tail)
			staq->isEmpty = true;
	}
	//printf("POPFRONT %p %d %d %d %d\n", staq, staq->isFull, staq->isEmpty, staq->head, staq->tail);
}

/**
 * @brief Insert an integer at the beginning of the stack/queue.
 */
void dstaq_pushFront(staq_t *staq, int val) {
	/* Push only if staq exists and it is not full */
	if(staq && (staq->head != staq->tail)) {
		staq->data[staq->head] = val;
		staq->head = (staq->head)? ((staq->head - 1) % staq->sizeP) : (staq->sizeP - 1);
		staq->isEmpty = false;
	}
	//printf("PUSHFRONT %p %d %d %d %d\n", staq, staq->isFull, staq->isEmpty, staq->head, staq->tail);
}

/**
 * @brief Retrieves the first integer in this stack/queue.
 */
int dstaq_front(staq_t *staq) {
	//printf("FRONT %p %d\n", staq, staq->data[(staq->head + 1) % staq->sizeP]);
	return staq->data[(staq->head + 1) % staq->sizeP];
}

/**
 * @brief Retrieves the last integer in this stack/queue.
 */
int dstaq_back(staq_t *staq) {
	//printf("BACK %p %d\n", staq, staq->data[(staq->tail)? ((staq->tail - 1) % staq->sizeP) : (staq->sizeP - 1)]);
	return staq->data[(staq->tail)? ((staq->tail - 1) % staq->sizeP) : (staq->sizeP - 1)];
}

/**
 * @brief Check if this stack/queue of integers is empty.
 */
bool dstaq_isEmpty(staq_t *staq) {
	return staq->isEmpty;
}
