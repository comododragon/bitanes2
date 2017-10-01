#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

typedef union {
	int d;
	double lf;
	char *s;
} elem_u;

typedef struct elem_t {
	elem_u val;
	struct elem_t *next; 
} elem_t;

typedef struct {
	unsigned int size;
	unsigned int type;
	elem_t *head;
#ifndef LIST_DISABLE_TAIL
	elem_t *tail;
#endif

} list_t;

list_t *dlist_create(void);
void dlist_destroy(list_t **list);
void dlist_trim(list_t **list, unsigned int n);
void dlist_pushBack(list_t *list, int val);
void qsdlist_pushBack(list_t *list, int val);
void qwdlist_pushBack(list_t *list, int val);
void pvdlist_pushBack(list_t *list, int val);
void qdlist_popFront(list_t *list);
void pdlist_popFront(list_t *list);
void dlist_popFront(list_t *list);
void dlist_pushFront(list_t *list, int val);
int dlist_front(list_t *list);
int dlist_back(list_t *list);
int dlist_get(list_t *list, unsigned int pos);
void dlist_insert(list_t *list, unsigned int pos, int val);
void dlist_swap(list_t *list, unsigned int pos, int val);
bool dlist_isEmpty(list_t *list);
unsigned int dlist_size(list_t *list);

list_t *lflist_create(void);
void lflist_destroy(list_t **list);
void lflist_trim(list_t **list, unsigned int n);
void lflist_pushBack(list_t *list, double val);
void lflist_popFront(list_t *list);
void lflist_pushFront(list_t *list, double val);
double lflist_front(list_t *list);
double lflist_back(list_t *list);
double lflist_get(list_t *list, unsigned int pos);
void lflist_insert(list_t *list, unsigned int pos, double val);
void lflist_swap(list_t *list, unsigned int pos, double val);
bool lflist_isEmpty(list_t *list);
unsigned int lflist_size(list_t *list);

list_t *slist_create(void);
void slist_destroy(list_t **list);
void slist_trim(list_t **list, unsigned int n);
void slist_pushBack(list_t *list, char *val);
void slist_popFront(list_t *list);
void slist_pushFront(list_t *list, char *val);
char *slist_front(list_t *list);
char *slist_back(list_t *list);
char *slist_get(list_t *list, unsigned int pos);
void slist_insert(list_t *list, unsigned int pos, char *val);
void slist_swap(list_t *list, unsigned int pos, char *val);
bool slist_isEmpty(list_t *list);
unsigned int slist_size(list_t *list);

#endif
