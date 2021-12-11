#ifndef TOS_H_
#define TOS_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "token.h"

typedef enum id_type{
    ID_VAR, ID_METHOD, ID_TMP
  }id_type;

typedef struct Ht_item {
    char *key;      // identificateur
    int id_type;    // variable (ID_VAR) ou méthode (ID_METHODE) ou temporaire (ID_TMP)
    int value;      // type (INT / BOOL / VOIDTYPE)
	int order;      // order d'insértion dans la TOS
} Ht_item;

typedef struct LinkedList {
    Ht_item *item;
    struct LinkedList *next;
} LinkedList;


typedef struct HashTable {
    Ht_item **items;
    LinkedList **lists;
    int max_size;                // Taille max de la TOS
    int count;                  // Nombre d'élément dans la table
    int nb_var;                 // Nombre de variables (not method) dans la table (facilite le calcul des offsets)
	struct HashTable *next;     
    bool isGlobal;               // si context global alors = true
} HashTable;

typedef struct item_table {
    Ht_item *item;
    HashTable *table;
} item_table;


unsigned long hash_function(char *str);
static LinkedList *allocate_list();
static LinkedList *linkedlist_insert(LinkedList *list, Ht_item *item);
static Ht_item *linkedlist_remove(LinkedList *list);
static void free_linkedlist(LinkedList *list);
static LinkedList **create_lists(HashTable *table);
static void free_lists(HashTable *table);
Ht_item *create_item(char *key, int id_type, int value);
HashTable *create_table();
void free_item(Ht_item *item);
void free_table(HashTable *table);
void handle_collision(HashTable *table, unsigned long index, Ht_item *item);
void ht_insert(HashTable *table, Ht_item *item);
Ht_item *ht_search(HashTable *table, char *key);
void ht_delete(HashTable *table, char *key);
void print_table(HashTable *table);

void pushctx();
void popctx();
void newname(Ht_item *item);
item_table *lookup(char *key);
void print_ctx();

int offset(item_table *item);

int table_size(HashTable *table);

extern HashTable* curr_context;
extern HashTable* glob_context;

#endif