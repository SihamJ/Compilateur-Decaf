#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include "token.h"
#include "intermediaire.h"

/* Type de l'identificateur dans la TOS */
typedef enum id_type{
    ID_VAR, ID_METHOD, ID_TMP, ID_PARAM, ID_TAB, ID_CLASS
  }id_type;

/* Type du contexte */
typedef enum ctx_type{
    CTX_GLOB, CTX_METHOD, CTX_IF, CTX_FOR, CTX_BLOCK, CTX_PARAM
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
	int order;      // ordre d'insértion dans la TOS
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
	struct HashTable *next;
    ctx_type type;                   // Type du contexte courrant: Méthode, boucle for, condition If, etc ...     
} HashTable;

/* Couple Item et sa TOS, utile dans les règles de syntaxes pour savoir si le contexte d'un identificateur retourné par lookup est global ou non, 
    et pour le calcul des offsets */
typedef struct item_table {
    Ht_item *item;
    HashTable *table;
} item_table;



/* HashTable functions */
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


/* returns a new temporary variable*/
Ht_item* new_temp(int type);

void pushctx(); 
void popctx();

/* insert item in the current symbol table*/
void newname(Ht_item *item);

/* looks for a variable with the name key in the symbol tables and returns the couple ( item, symbol table) */
item_table *lookup(char *key);

/* prints the symbol table of the current context*/
void print_ctx();

/* prints all the symbol tables pushed into the stack*/
void print_stack();
void free_stack();

/* verifies that list of parameters p1 corresponds to list p2 in number and types*/
int verify_param(param p1, param p2);

/* verifies if ctx_type type is a parent of the current context*/
int is_a_parent(ctx_type type);

/* returns the offset of item in the symbol table*/
int offset(item_table *item);

/* pop the temporary variables from the current context, called at the end of the evaluation of an expression*/
void pop_tmp(); 


extern HashTable* curr_context; // points to the current context
extern HashTable* glob_context; // points to the global context
extern HashTable* stack;        // stack of symbol tables where the popped ones are stored

#endif