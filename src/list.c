#include "list.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_SZ 256

#define TYPE_D 0
#define TYPE_LF 1
#define TYPE_S 2

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

#define DEALLOC(type, val) {\
	switch(type) {\
		case TYPE_S:\
			if(val.s)\
				free(val.s);\
			break;\
	}\
}

list_t *_list_create(unsigned int type) {
	list_t *list = malloc(sizeof(list_t));
	list->size = 0;
	list->type = type;
	list->head = NULL;

	return list;
}

void _list_destroy(list_t **list) {
	elem_t *tmpPointer = (*list)->head;
	elem_t *tmpPointerNext;

	while(tmpPointer) {
		tmpPointerNext = tmpPointer->next;
		DEALLOC((*list)->type, tmpPointer->val);
		free(tmpPointer);
		tmpPointer = tmpPointerNext;
	}

	free(*list);
	*list = NULL;
}

void _list_trim(list_t **list, unsigned int n) {
	if(n) {
		elem_t *tmpPointer = (*list)->head;
		elem_t *tmpPointerNext;

		int i;
		for(i = 0; tmpPointer; i++) {
			tmpPointerNext = tmpPointer->next;
			if(i >= n) {
				DEALLOC((*list)->type, tmpPointer->val);
				free(tmpPointer);
			}
			else if((n - 1) == i)
				tmpPointer->next = NULL;
			tmpPointer = tmpPointerNext;
		}

		(*list)->size = n;
	}
	else
		_list_destroy(list);
} 

void _list_pushBack(list_t *list, int dval, double lfval, char *sval) {
	if(list->head) {
		elem_t *tmpPointer = list->head;

		while(tmpPointer->next)
			tmpPointer = tmpPointer->next;

		tmpPointer->next = malloc(sizeof(elem_t));
		tmpPointer->next->next = NULL;
		ASSIGN(list->type, tmpPointer->next->val, dval, lfval, sval);
	}
	else {
		list->head = malloc(sizeof(elem_t));
		list->head->next = NULL;
		ASSIGN(list->type, list->head->val, dval, lfval, sval);
	}

	(list->size)++;
}

void _list_popFront(list_t *list) {
	if(list->head) {
		elem_t *tmpPointer = list->head->next;
		DEALLOC(list->type, list->head->val);
		free(list->head);
		list->head = tmpPointer;
		(list->size)--;
	}
}

void _list_pushFront(list_t *list, int dval, double lfval, char *sval) {
	elem_t *tmpPointer = list->head;

	list->head = malloc(sizeof(elem_t));
	list->head->next = tmpPointer;
	ASSIGN(list->type, list->head->val, dval, lfval, sval);

	(list->size)++;
}

elem_u _list_front(list_t *list) {
	return list->head->val;
}

elem_u _list_back(list_t *list) {
	elem_t *tmpPointer = list->head;

	while(tmpPointer->next)
		tmpPointer = tmpPointer->next;

	return tmpPointer->val;
}

elem_u _list_get(list_t *list, unsigned int pos) {
	elem_t *tmpPointer = list->head;

	int i;
	for(i = 0; i < pos; i++)
		tmpPointer = tmpPointer->next;

	return tmpPointer->val;
}

void _list_insert(list_t *list, unsigned int pos, int dval, double lfval, char *sval) {
	if(list->head) {
		elem_t *tmpCurr = NULL;
		elem_t *tmpNext = list->head;

		int i;
		for(i = 0; i < pos; i++) {
			tmpCurr = tmpNext;
			tmpNext = tmpNext->next;
		}

		elem_t *tmpElem = malloc(sizeof(elem_t));
		tmpElem->next = tmpNext;
		ASSIGN(list->type, tmpElem->val, dval, lfval, sval);
		if(tmpCurr)
			tmpCurr->next = tmpElem;
		else
			list->head = tmpElem;
	}
	else {
		list->head = malloc(sizeof(elem_t));
		list->head->next = NULL;
		ASSIGN(list->type, list->head->val, dval, lfval, sval);
	}

	(list->size)++;
}

void _list_swap(list_t *list, unsigned int pos, int dval, double lfval, char *sval) {
	if(list->head) {
		elem_t *tmpNext = list->head;

		int i;
		for(i = 0; i < pos; i++)
			tmpNext = tmpNext->next;

		DEALLOC(list->type, tmpNext->val);
		ASSIGN(list->type, tmpNext->val, dval, lfval, sval);
	}
}

bool _list_isEmpty(list_t *list) {
	return (0 == list->size);
}

unsigned int _list_size(list_t *list) {
	return list->size;
}

list_t *dlist_create(void) {
	return _list_create(TYPE_D);
}

void dlist_destroy(list_t **list) {
	_list_destroy(list);
}

void dlist_trim(list_t **list, unsigned int n) {
	_list_trim(list, n);
} 

void dlist_pushBack(list_t *list, int val) {
	_list_pushBack(list, val, -1, NULL);
}

void dlist_popFront(list_t *list) {
	_list_popFront(list);
}

void dlist_pushFront(list_t *list, int val) {
	_list_pushFront(list, val, -1, NULL);
}

int dlist_front(list_t *list) {
	return _list_front(list).d;
}

int dlist_back(list_t *list) {
	return _list_back(list).d;
}

int dlist_get(list_t *list, unsigned int pos) {
	return _list_get(list, pos).d;
}

void dlist_insert(list_t *list, unsigned int pos, int val) {
	_list_insert(list, pos, val, -1, NULL);
}

void dlist_swap(list_t *list, unsigned int pos, int val) {
	_list_swap(list, pos, val, -1, NULL);
}

bool dlist_isEmpty(list_t *list) {
	return _list_isEmpty(list);
}

unsigned int dlist_size(list_t *list) {
	return _list_size(list);
}

list_t *lflist_create(void) {
	return _list_create(TYPE_LF);
}

void lflist_destroy(list_t **list) {
	_list_destroy(list);
}

void lflist_trim(list_t **list, unsigned int n) {
	_list_trim(list, n);
} 

void lflist_pushBack(list_t *list, double val) {
	_list_pushBack(list, -1, val, NULL);
}

void lflist_popFront(list_t *list) {
	_list_popFront(list);
}

void lflist_pushFront(list_t *list, double val) {
	_list_pushFront(list, -1, val, NULL);
}

double lflist_front(list_t *list) {
	return _list_front(list).lf;
}

double lflist_back(list_t *list) {
	return _list_back(list).lf;
}

double lflist_get(list_t *list, unsigned int pos) {
	return _list_get(list, pos).lf;
}

void lflist_insert(list_t *list, unsigned int pos, double val) {
	_list_insert(list, pos, -1, val, NULL);
}

void lflist_swap(list_t *list, unsigned int pos, double val) {
	_list_swap(list, pos, -1, val, NULL);
}

bool lflist_isEmpty(list_t *list) {
	return _list_isEmpty(list);
}

unsigned int lflist_size(list_t *list) {
	return _list_size(list);
}

list_t *slist_create(void) {
	return _list_create(TYPE_S);
}

void slist_destroy(list_t **list) {
	_list_destroy(list);
}

void slist_trim(list_t **list, unsigned int n) {
	_list_trim(list, n);
} 

void slist_pushBack(list_t *list, char *val) {
	_list_pushBack(list, -1, -1, val);
}

void slist_popFront(list_t *list) {
	_list_popFront(list);
}

void slist_pushFront(list_t *list, char *val) {
	_list_pushFront(list, -1, -1, val);
}

char *slist_front(list_t *list) {
	return _list_front(list).s;
}

char *slist_back(list_t *list) {
	return _list_back(list).s;
}

char *slist_get(list_t *list, unsigned int pos) {
	return _list_get(list, pos).s;
}

void slist_insert(list_t *list, unsigned int pos, char *val) {
	_list_insert(list, pos, -1, -1, val);
}

void slist_swap(list_t *list, unsigned int pos, char *val) {
	_list_swap(list, pos, -1, -1, val);
}

bool slist_isEmpty(list_t *list) {
	return _list_isEmpty(list);
}

unsigned int slist_size(list_t *list) {
	return _list_size(list);
}
