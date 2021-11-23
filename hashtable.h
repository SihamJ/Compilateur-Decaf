#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Ht_item Ht_item;
struct Ht_item
{
    char *key;
    char *value;
};

typedef struct LinkedList LinkedList;
struct LinkedList
{
    Ht_item *item;
    LinkedList *next;
};

typedef struct HashTable HashTable;
struct HashTable
{
    Ht_item **items;
    LinkedList **lists;
    int size;
    int count;
};

HashTable *create_table();
void ht_insert(HashTable *table, char *key, char *value);
char *ht_search(HashTable *table, char *key);
void ht_delete(HashTable *table, char *key);
void print_table(HashTable *table);
void free_table(HashTable *table);
