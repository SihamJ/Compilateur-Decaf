#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include "token.h"
#include "intermediaire.h"

/* Type de l'identificateur dans la TOS */
typedef enum id_type{
    ID_VAR, ID_METHOD, ID_TMP, ID_PARAM, ID_TAB
  }id_type;

/* Type du contexte */
typedef enum ctx_type{
    CTX_GLOB, CTX_METHOD, CTX_IF, CTX_FOR, CTX_BLOCK
  }ctx_type;

/* To store parameters of a method at declaration or call */
typedef struct param{
    int type;           // INT or BOOL or STRING;
    quadop arg;         // utilisé dans le cas d'un appel de méthode, l'argument est stocké dans un quadop
    struct param* next;
} *param;

typedef struct Ht_item {
    char *key;      // identificateur
    int id_type;    // variable (ID_VAR) ou méthode (ID_METHODE) ou temporaire (ID_TMP) ou paramètre de méthode (ID_PARAM) ou tableau (ID_TAB)
    int value;      // type (INT / BOOL / VOIDTYPE)
	int order;      // order d'insértion dans la TOS
    int size;       // si tableau, sinon 4
    param p;        // si l'item est une méthode, param est une liste des paramètres et leur type
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
    ctx_type type;                   // Type du contexte courrant: Méthode, boucle for, condition If, etc ...     
} HashTable;

/* Couple Item et sa TOS, utile dans les règles de syntaxes pour savoir si le contexte d'un identificateur retourné par lookup est global ou non, 
    et pour le calcul des offsets */
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

Ht_item* new_temp(int type);

void pushctx();
void popctx();
void newname(Ht_item *item);
item_table *lookup(char *key);
void print_ctx();
void print_stack();
void free_stack();
int verify_param(param p1, param p2);

/* Vérifier si le contexte type est un parent du contexte courant*/
int is_a_parent(ctx_type type);

int offset(item_table *item);

int table_size(HashTable *table);

void pop_tmp(); // dépile les var temporaires du contexte courant, appelée après chaque fin de statement d'affectation

extern HashTable* curr_context;
extern HashTable* glob_context;
extern HashTable* stack; // storing the symbol tables before popping 

#endif