/* ********************************************************************************************* */
/* * Simple library for a linked list implementation: liblist                                  * */
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

#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

/* Different lists may hold different types of elements */
typedef union {
	int d;
	double lf;
	char *s;
} elem_u;

/* Structure of a list element: the value itself and pointer to next node */
typedef struct elem_t {
	elem_u val;
	struct elem_t *next; 
} elem_t;

/* Structure of a list */
typedef struct {
	unsigned int size;
	/* Type of list: tells which part of the elem_u union to use */
	unsigned int type;
	/* Head and tail (may be deactivated) pointers. Tail pointer is useful in queues */
	elem_t *head;
#ifndef LIST_DISABLE_TAIL
	elem_t *tail;
#endif
} list_t;

/* ********************************************************************************************* */
/* * List functions for integers                                                                * */
/* ********************************************************************************************* */

/**
 * @brief Create a list of integers.
 * @return A pointer to this new list.
 */
list_t *dlist_create(void);

/**
 * @brief Destroy a list of integers; free up memory.
 * @param list Pointer to a pointer to the list to be destroyed.
 */
void dlist_destroy(list_t **list);

/**
 * @brief Trim a list of integers, leaving it with n elements.
 * @param list Pointer to a pointer to the list to be trimmed.
 * @param n Size that the list should have after trimming. If the list is smaller than this, nothing happens.
 */
void dlist_trim(list_t **list, unsigned int n);

/**
 * @brief Insert an integer at the end of the list.
 * @param list Pointer to the list.
 * @param val Integer value to be inserted.
 */
void dlist_pushBack(list_t *list, int val);

/**
 * @brief Discard the first element of the list of integers.
 * @param list Pointer to the list.
 */
void dlist_popFront(list_t *list);

/**
 * @brief Insert an integer at the beginning of the list.
 * @param list Pointer to the list.
 * @param val Integer value to be inserted.
 */
void dlist_pushFront(list_t *list, int val);

/**
 * @brief Retrieves the first integer in this list.
 * @param list Pointer to the list.
 * @return The head of this list.
 */
int dlist_front(list_t *list);

/**
 * @brief Retrieves the last integer in this list.
 * @param list Pointer to the list.
 * @return The tail of this list.
 */
int dlist_back(list_t *list);

/**
 * @brief Retrieves an integer from this list.
 * @param list Pointer to the list.
 * @param pos The position of the list to be retrieved.
 * @return The pos-th element of this list.
 */
int dlist_get(list_t *list, unsigned int pos);

/**
 * @brief Inserts an integer in this list.
 * @param list Pointer to the list.
 * @param pos The position of the list to receive a new node. All other nodes after pos are moved.
 * @param val Integer to be added.
 */
void dlist_insert(list_t *list, unsigned int pos, int val);

/**
 * @brief Swap a position in this list with a new integer.
 * @param list Pointer to the list.
 * @param pos The position of the list to receive a new value.
 * @param val Integer to be swapped.
 */
void dlist_swap(list_t *list, unsigned int pos, int val);

/**
 * @brief Check if this list of integers is empty.
 * @param list Pointer to the list.
 * @return true if this list is empty, false otherwise.
 */
bool dlist_isEmpty(list_t *list);

/**
 * @brief Retrieves the size of this list.
 * @param list Pointer to the list.
 * @return The number of members inside the list.
 */
unsigned int dlist_size(list_t *list);

/* ********************************************************************************************* */
/* * List functions for doubles                                                                 * */
/* ********************************************************************************************* */

/**
 * @brief Similar logic of @f dlist_create but for double values.
 */
list_t *lflist_create(void);

/**
 * @brief Similar logic of @f dlist_destroy but for double values.
 */
void lflist_destroy(list_t **list);

/**
 * @brief Similar logic of @f dlist_trim but for double values.
 */
void lflist_trim(list_t **list, unsigned int n);

/**
 * @brief Similar logic of @f dlist_pushBack but for double values.
 */
void lflist_pushBack(list_t *list, double val);

/**
 * @brief Similar logic of @f dlist_popFront but for double values.
 */
void lflist_popFront(list_t *list);

/**
 * @brief Similar logic of @f dlist_pushFront but for double values.
 */
void lflist_pushFront(list_t *list, double val);

/**
 * @brief Similar logic of @f dlist_front but for double values.
 */
double lflist_front(list_t *list);

/**
 * @brief Similar logic of @f dlist_back but for double values.
 */
double lflist_back(list_t *list);

/**
 * @brief Similar logic of @f dlist_get but for double values.
 */
double lflist_get(list_t *list, unsigned int pos);

/**
 * @brief Similar logic of @f dlist_insert but for double values.
 */
void lflist_insert(list_t *list, unsigned int pos, double val);

/**
 * @brief Similar logic of @f dlist_swap but for double values.
 */
void lflist_swap(list_t *list, unsigned int pos, double val);

/**
 * @brief Similar logic of @f dlist_isEmpty but for double values.
 */
bool lflist_isEmpty(list_t *list);

/**
 * @brief Similar logic of @f dlist_size but for double values.
 */
unsigned int lflist_size(list_t *list);

/* ********************************************************************************************* */
/* * List functions for strings                                                                 * */
/* ********************************************************************************************* */

/**
 * @brief Similar logic of @f dlist_create but for string values.
 */
list_t *slist_create(void);

/**
 * @brief Similar logic of @f dlist_destroy but for string values.
 */
void slist_destroy(list_t **list);

/**
 * @brief Similar logic of @f dlist_trim but for string values.
 */
void slist_trim(list_t **list, unsigned int n);

/**
 * @brief Similar logic of @f dlist_pushBack but for string values.
 * @note A copy of the string is made internally, therefore no need to keep the string externally.
 */
void slist_pushBack(list_t *list, char *val);

/**
 * @brief Similar logic of @f dlist_popFront but for string values.
 */
void slist_popFront(list_t *list);

/**
 * @brief Similar logic of @f dlist_pushFront but for string values.
 * @note A copy of the string is made internally, therefore no need to keep the string externally.
 */
void slist_pushFront(list_t *list, char *val);

/**
 * @brief Similar logic of @f dlist_front but for string values.
 * @note A reference to the string will be provided: DEVELOPER SHOULD NOT FREE THIS STRING!
 */
char *slist_front(list_t *list);

/**
 * @brief Similar logic of @f dlist_back but for string values.
 * @note A reference to the string will be provided: DEVELOPER SHOULD NOT FREE THIS STRING!
 */
char *slist_back(list_t *list);

/**
 * @brief Similar logic of @f dlist_get but for string values.
 * @note A reference to the string will be provided: DEVELOPER SHOULD NOT FREE THIS STRING!
 */
char *slist_get(list_t *list, unsigned int pos);

/**
 * @brief Similar logic of @f dlist_insert but for string values.
 * @note A copy of the string is made internally, therefore no need to keep the string externally.
 */
void slist_insert(list_t *list, unsigned int pos, char *val);

/**
 * @brief Similar logic of @f dlist_swap but for string values.
 * @note A copy of the string is made internally, therefore no need to keep the string externally.
 */
void slist_swap(list_t *list, unsigned int pos, char *val);

/**
 * @brief Similar logic of @f dlist_isEmpty but for string values.
 */
bool slist_isEmpty(list_t *list);

/**
 * @brief Similar logic of @f dlist_size but for string values.
 */
unsigned int slist_size(list_t *list);

#endif
