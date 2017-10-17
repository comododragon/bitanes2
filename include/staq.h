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
/* * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A           * */
/* * PARTICULAR PURPOSE.  See the GNU General Public License for more details.                 * */
/* *                                                                                           * */
/* * You should have received a copy of the GNU General Public License along with liblist.     * */
/* * If not, see <http://www.gnu.org/licenses/>.                                               * */
/* ********************************************************************************************* */

#ifndef STAQ_H
#define STAQ_H

#include <stdbool.h>

/* Structure of the stack/queue */
typedef struct {
	unsigned int size;
	/* Head and tail pointers */
	unsigned int head;
	unsigned int tail;
	/* Helper variables */
	bool isEmpty;
	bool isFull;
	/* The stack/queue itself */
	int *data;
} staq_t;

/**
 * @brief Create a stack/queue of integers.
 * @brief size Size of this stack/queue.
 * @return A pointer to this new stack/queue.
 */
staq_t *dstaq_create(unsigned int size);

/**
 * @brief Destroy a stack/queue of integers; free up memory.
 * @param staq Pointer to a pointer to the stack/queue to be destroyed.
 */
void dstaq_destroy(staq_t **staq);

/**
 * @brief Insert an integer at the end of the stack/queue.
 * @param staq Pointer to the stack/queue.
 * @param val Integer value to be inserted.
 */
void dstaq_pushBack(staq_t *staq, int val);

/**
 * @brief Discard the first element of the stack/queue of integers.
 * @param staq Pointer to the stack/queue.
 */
void dstaq_popFront(staq_t *staq);

/**
 * @brief Insert an integer at the beginning of the stack/queue.
 * @param staq Pointer to the stack/queue.
 * @param val Integer value to be inserted.
 */
void dstaq_pushFront(staq_t *staq, int val);

/**
 * @brief Retrieves the first integer in this stack/queue.
 * @param staq Pointer to the stack/queue.
 * @return The head of this stack/queue.
 */
int dstaq_front(staq_t *staq);

/**
 * @brief Retrieves the last integer in this stack/queue.
 * @param staq Pointer to the stack/queue.
 * @return The tail of this stack/queue.
 */
int dstaq_back(staq_t *staq);

/**
 * @brief Check if this stack/queue of integers is empty.
 * @param staq Pointer to the stack/queue.
 * @return true if this stack/queue is empty, false otherwise.
 */
bool dstaq_isEmpty(staq_t *staq);

#endif
