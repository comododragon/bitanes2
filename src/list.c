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

#include "list.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/* Maximum size of a single string inside a string list. This is used to avoid buffer overflows */
#define MAX_STR_SZ 256

/* Values used to identify of which type the list was created */
#define TYPE_D 0
#define TYPE_LF 1
#define TYPE_S 2

/* ********************************************************************************************* */
/* * Type-agnostic list functions (INTERNAL USAGE ONLY)                                        * */
/* ********************************************************************************************* */

/**
 * @brief ASSIGN macro. According to the list type, it will assign/copy the input value differently.
 * @param type The list type (TYPE_D, TYPE_LF or TYPE_S).
 * @param val The union variable to receive the input.
 * @param dval Integer value to be assigned. It is ignored if type != TYPE_D.
 * @param lfval Double value to be assigned. It is ignored if type != TYPE_LF.
 * @param sval String value to be copied. It is ignored if type != TYPE_S.
 */
#define ASSIGN(type, val, dval, lfval, sval) {\
	switch(type) {\
		case TYPE_LF:\
			val.lf = lfval;\
			break;\
		case TYPE_S:\
			if(sval) {\
				val.s = malloc(strnlen(sval, MAX_STR_SZ - 1) + 1);\
				strcpy(val.s, sval);\
			}\
			else {\
				val.s = NULL;\
			}\
			break;\
		default:\
			val.d = dval;\
			break;\
	}\
}

/**
 * @brief DEALLOC macro. According to the list type, it will free up resources if necessary.
 * @param type The list type (TYPE_D, TYPE_LF or TYPE_S).
 * @param val The union variable to be deallocated.
 */
#define DEALLOC(type, val) {\
	switch(type) {\
		case TYPE_S:\
			if(val.s)\
				free(val.s);\
			break;\
	}\
}

/**
 * @brief Create a list.
 * @note This is an internal function. Use the functions with defined types instead.
 */
list_t *_list_create(unsigned int type) {
	list_t *list = malloc(sizeof(list_t));
	list->size = 0;
	list->type = type;
	list->head = NULL;
#ifndef LIST_DISABLE_TAIL
	list->tail = NULL;
#endif
	return list;
}

/**
 * @brief Destroy a list; free up memory.
 * @note This is an internal function. Use the functions with defined types instead.
 */
void _list_destroy(list_t **list) {
	elem_t *tmpPointer = (*list)->head;
	elem_t *tmpPointerNext;

	/* Iterate through the list and delete everything */
	while(tmpPointer) {
		tmpPointerNext = tmpPointer->next;
		DEALLOC((*list)->type, tmpPointer->val);
		free(tmpPointer);
		tmpPointer = tmpPointerNext;
	}

	free(*list);
	*list = NULL;
}

/**
 * @brief Trim a list, leaving it with n elements.
 * @note This is an internal function. Use the functions with defined types instead.
 */
void _list_trim(list_t **list, unsigned int n) {
	/* Trim if the list has elements */
	if(n) {
		elem_t *tmpPointer = (*list)->head;
		elem_t *tmpPointerNext;

		/* Iterate through the list */
		int i;
		for(i = 0; tmpPointer; i++) {
			tmpPointerNext = tmpPointer->next;
			/* After passing through n elements, start deleting stuff */
			if(i >= n) {
				DEALLOC((*list)->type, tmpPointer->val);
				free(tmpPointer);
			}
			else if((n - 1) == i) {
				tmpPointer->next = NULL;
#ifndef LIST_DISABLE_TAIL
				(*list)->tail = tmpPointer;
#endif
			}

			tmpPointer = tmpPointerNext;
		}

		(*list)->size = n;
	}
	else
		_list_destroy(list);
} 

/**
 * @brief Insert an element at the end of the list.
 * @note This is an internal function. Use the functions with defined types instead.
 */
void _list_pushBack(list_t *list, int dval, double lfval, char *sval) {
	/* List has elements */
	if(list->head) {
#ifndef LIST_DISABLE_TAIL
		/* With tail pointer, there's no need to iterate through the whole list to pushBack */

		elem_t *tmpPointer = malloc(sizeof(elem_t));
		tmpPointer->next = NULL;
		ASSIGN(list->type, tmpPointer->val, dval, lfval, sval);

		list->tail->next = tmpPointer;
		list->tail = tmpPointer;
#else
		elem_t *tmpPointer = list->head;

		/* Iterate 'till the end of the list */
		while(tmpPointer->next)
			tmpPointer = tmpPointer->next;

		tmpPointer->next = malloc(sizeof(elem_t));
		tmpPointer->next->next = NULL;
		ASSIGN(list->type, tmpPointer->next->val, dval, lfval, sval);
#endif
	}
	/* List has no elements */
	else {
		list->head = malloc(sizeof(elem_t));
		list->head->next = NULL;
#ifndef LIST_DISABLE_TAIL
		list->tail = list->head;
#endif
		ASSIGN(list->type, list->head->val, dval, lfval, sval);
	}

	(list->size)++;
}

/**
 * @brief Discard the first element of the list.
 * @note This is an internal function. Use the functions with defined types instead.
 */
void _list_popFront(list_t *list) {
	if(list->head) {
		elem_t *tmpPointer = list->head->next;
		DEALLOC(list->type, list->head->val);
		free(list->head);
		list->head = tmpPointer;
		(list->size)--;

#ifndef LIST_DISABLE_TAIL
		if(!(list->size))
			list->tail = NULL;
#endif
	}
}

/**
 * @brief Insert an element at the beginning of the list.
 * @note This is an internal function. Use the functions with defined types instead.
 */
void _list_pushFront(list_t *list, int dval, double lfval, char *sval) {
	elem_t *tmpPointer = list->head;

	list->head = malloc(sizeof(elem_t));
	list->head->next = tmpPointer;
	ASSIGN(list->type, list->head->val, dval, lfval, sval);

	(list->size)++;

#ifndef LIST_DISABLE_TAIL
	if(1 == list->size)
		list->tail = list->head;
#endif
}

/**
 * @brief Retrieves the first element in this list.
 * @note This is an internal function. Use the functions with defined types instead.
 */
elem_u _list_front(list_t *list) {
	return list->head->val;
}

/**
 * @brief Retrieves the last element in this list.
 * @note This is an internal function. Use the functions with defined types instead.
 */
elem_u _list_back(list_t *list) {
#ifndef LIST_DISABLE_TAIL
	/* No need to iterate the whole list if we have a tail pointer */
	return list->tail->val;
#else
	elem_t *tmpPointer = list->head;

	/* Iterate through the whole list */
	while(tmpPointer->next)
		tmpPointer = tmpPointer->next;

	return tmpPointer->val;
#endif
}

/**
 * @brief Retrieves an element from this list.
 * @note This is an internal function. Use the functions with defined types instead.
 */
elem_u _list_get(list_t *list, unsigned int pos) {
	elem_t *tmpPointer = list->head;

	int i;
	for(i = 0; i < pos; i++)
		tmpPointer = tmpPointer->next;

	return tmpPointer->val;
}

/**
 * @brief Inserts an element in this list.
 * @note This is an internal function. Use the functions with defined types instead.
 */
void _list_insert(list_t *list, unsigned int pos, int dval, double lfval, char *sval) {
	/* List has elements */
	if(list->head) {
		elem_t *tmpCurr = NULL;
		elem_t *tmpNext = list->head;

		/* Iterate 'till the pos-th position */
		int i;
		for(i = 0; i < pos; i++) {
			tmpCurr = tmpNext;
			tmpNext = tmpNext->next;
		}

		/* Malloc a new element, assign it and refresh pointers */
		elem_t *tmpElem = malloc(sizeof(elem_t));
		tmpElem->next = tmpNext;
		ASSIGN(list->type, tmpElem->val, dval, lfval, sval);
		if(tmpCurr)
			tmpCurr->next = tmpElem;
		else
			list->head = tmpElem;

#ifndef LIST_DISABLE_TAIL
		if(pos == list->size)
			list->tail = tmpElem;
#endif
	}
	/* List has no elements */
	else {
		list->head = malloc(sizeof(elem_t));
		list->head->next = NULL;
#ifndef LIST_DISABLE_TAIL
		list->tail = list->head;
#endif
		ASSIGN(list->type, list->head->val, dval, lfval, sval);
	}

	(list->size)++;
}

/**
 * @brief Swap a position in this list with a new element.
 * @note This is an internal function. Use the functions with defined types instead.
 */
void _list_swap(list_t *list, unsigned int pos, int dval, double lfval, char *sval) {
	if(list->head) {
		elem_t *tmpNext = list->head;

		int i;
		for(i = 0; i < pos; i++)
			tmpNext = tmpNext->next;

		/* Just dealloc and reassign solves this problem */
		DEALLOC(list->type, tmpNext->val);
		ASSIGN(list->type, tmpNext->val, dval, lfval, sval);
	}
}

/**
 * @brief Check if this list is empty.
 * @note This is an internal function. Use the functions with defined types instead.
 */
bool _list_isEmpty(list_t *list) {
	return (0 == list->size);
}

/**
 * @brief Retrieves the size of this list.
 * @note This is an internal function. Use the functions with defined types instead.
 */
unsigned int _list_size(list_t *list) {
	return list->size;
}

/* ********************************************************************************************* */
/* * List functions for integers                                                                * */
/* ********************************************************************************************* */

/**
 * @brief Create a list of integers.
 */
list_t *dlist_create(void) {
	return _list_create(TYPE_D);
}

/**
 * @brief Destroy a list of integers; free up memory.
 */
void dlist_destroy(list_t **list) {
	_list_destroy(list);
}

/**
 * @brief Trim a list of integers, leaving it with n elements.
 */
void dlist_trim(list_t **list, unsigned int n) {
	_list_trim(list, n);
} 

/**
 * @brief Insert an integer at the end of the list.
 */
void dlist_pushBack(list_t *list, int val) {
	_list_pushBack(list, val, -1, NULL);
}

/**
 * @brief Discard the first element of the list of integers.
 */
void dlist_popFront(list_t *list) {
	_list_popFront(list);
}

/**
 * @brief Insert an integer at the beginning of the list.
 */
void dlist_pushFront(list_t *list, int val) {
	_list_pushFront(list, val, -1, NULL);
}

/**
 * @brief Retrieves the first integer in this list.
 */
int dlist_front(list_t *list) {
	return _list_front(list).d;
}

/**
 * @brief Retrieves the last integer in this list.
 */
int dlist_back(list_t *list) {
	return _list_back(list).d;
}

/**
 * @brief Retrieves an integer from this list.
 */
int dlist_get(list_t *list, unsigned int pos) {
	return _list_get(list, pos).d;
}

/**
 * @brief Inserts an integer in this list.
 */
void dlist_insert(list_t *list, unsigned int pos, int val) {
	_list_insert(list, pos, val, -1, NULL);
}

/**
 * @brief Swap a position in this list with a new integer.
 */
void dlist_swap(list_t *list, unsigned int pos, int val) {
	_list_swap(list, pos, val, -1, NULL);
}

/**
 * @brief Check if this list of integers is empty.
 */
bool dlist_isEmpty(list_t *list) {
	return _list_isEmpty(list);
}

/**
 * @brief Retrieves the size of this list.
 */
unsigned int dlist_size(list_t *list) {
	return _list_size(list);
}

/* ********************************************************************************************* */
/* * List functions for doubles                                                                 * */
/* ********************************************************************************************* */

/**
 * @brief Similar logic of @f dlist_create but for double values.
 */
list_t *lflist_create(void) {
	return _list_create(TYPE_LF);
}

/**
 * @brief Similar logic of @f dlist_destroy but for double values.
 */
void lflist_destroy(list_t **list) {
	_list_destroy(list);
}

/**
 * @brief Similar logic of @f dlist_trim but for double values.
 */
void lflist_trim(list_t **list, unsigned int n) {
	_list_trim(list, n);
} 

/**
 * @brief Similar logic of @f dlist_pushBack but for double values.
 */
void lflist_pushBack(list_t *list, double val) {
	_list_pushBack(list, -1, val, NULL);
}

/**
 * @brief Similar logic of @f dlist_popFront but for double values.
 */
void lflist_popFront(list_t *list) {
	_list_popFront(list);
}

/**
 * @brief Similar logic of @f dlist_pushFront but for double values.
 */
void lflist_pushFront(list_t *list, double val) {
	_list_pushFront(list, -1, val, NULL);
}

/**
 * @brief Similar logic of @f dlist_front but for double values.
 */
double lflist_front(list_t *list) {
	return _list_front(list).lf;
}

/**
 * @brief Similar logic of @f dlist_back but for double values.
 */
double lflist_back(list_t *list) {
	return _list_back(list).lf;
}

/**
 * @brief Similar logic of @f dlist_get but for double values.
 */
double lflist_get(list_t *list, unsigned int pos) {
	return _list_get(list, pos).lf;
}

/**
 * @brief Similar logic of @f dlist_insert but for double values.
 */
void lflist_insert(list_t *list, unsigned int pos, double val) {
	_list_insert(list, pos, -1, val, NULL);
}

/**
 * @brief Similar logic of @f dlist_swap but for double values.
 */
void lflist_swap(list_t *list, unsigned int pos, double val) {
	_list_swap(list, pos, -1, val, NULL);
}

/**
 * @brief Similar logic of @f dlist_isEmpty but for double values.
 */
bool lflist_isEmpty(list_t *list) {
	return _list_isEmpty(list);
}

/**
 * @brief Similar logic of @f dlist_size but for double values.
 */
unsigned int lflist_size(list_t *list) {
	return _list_size(list);
}

/* ********************************************************************************************* */
/* * List functions for strings                                                                 * */
/* ********************************************************************************************* */

/**
 * @brief Similar logic of @f dlist_create but for string values.
 */
list_t *slist_create(void) {
	return _list_create(TYPE_S);
}

/**
 * @brief Similar logic of @f dlist_destroy but for string values.
 */
void slist_destroy(list_t **list) {
	_list_destroy(list);
}

/**
 * @brief Similar logic of @f dlist_trim but for string values.
 */
void slist_trim(list_t **list, unsigned int n) {
	_list_trim(list, n);
} 

/**
 * @brief Similar logic of @f dlist_pushBack but for string values.
 */
void slist_pushBack(list_t *list, char *val) {
	_list_pushBack(list, -1, -1, val);
}

/**
 * @brief Similar logic of @f dlist_popFront but for string values.
 */
void slist_popFront(list_t *list) {
	_list_popFront(list);
}

/**
 * @brief Similar logic of @f dlist_pushFront but for string values.
 */
void slist_pushFront(list_t *list, char *val) {
	_list_pushFront(list, -1, -1, val);
}

/**
 * @brief Similar logic of @f dlist_front but for string values.
 */
char *slist_front(list_t *list) {
	return _list_front(list).s;
}

/**
 * @brief Similar logic of @f dlist_back but for string values.
 */
char *slist_back(list_t *list) {
	return _list_back(list).s;
}

/**
 * @brief Similar logic of @f dlist_get but for string values.
 */
char *slist_get(list_t *list, unsigned int pos) {
	return _list_get(list, pos).s;
}

/**
 * @brief Similar logic of @f dlist_insert but for string values.
 */
void slist_insert(list_t *list, unsigned int pos, char *val) {
	_list_insert(list, pos, -1, -1, val);
}

/**
 * @brief Similar logic of @f dlist_swap but for string values.
 */
void slist_swap(list_t *list, unsigned int pos, char *val) {
	_list_swap(list, pos, -1, -1, val);
}

/**
 * @brief Similar logic of @f dlist_isEmpty but for string values.
 */
bool slist_isEmpty(list_t *list) {
	return _list_isEmpty(list);
}

/**
 * @brief Similar logic of @f dlist_size but for string values.
 */
unsigned int slist_size(list_t *list) {
	return _list_size(list);
}
