#ifndef TOS_H_
#define TOS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "token.h"

typedef struct Ht_item {
    char *key;
    int value;
	int order;
	struct Hashtable *tos;
} Ht_item;


typedef struct LinkedList {
    Ht_item *item;
    struct LinkedList *next;
} LinkedList;


typedef struct HashTable {
    Ht_item **items;
    LinkedList **lists;
    int size;
    int count;
	struct Hashtable *next;
} HashTable;




HashTable *create_table();
void ht_insert(HashTable *table, Ht_item *item);
Ht_item *ht_search(HashTable *table, char *key);
void ht_delete(HashTable *table, char *key);
void print_table(HashTable *table);
void free_table(HashTable *table);

void pushctx();
void popctx();
HashTable* currentctx();
void newname(Ht_item *item);
Ht_item *lookup(char *key);
void print_ctx();

HashTable* curr_context;

#endif