#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include "token.h"
#include "intermediaire.h"
#include "text_formating.h"

/* Type de l'identificateur dans la TOS */
typedef enum id_type{
    ID_VAR, ID_METHOD, ID_TMP, ID_PARAM, ID_TAB, ID_CLASS
  }id_type;

/* Type du contexte, utile pour les break et continue */
typedef enum ctx_type{
    CTX_GLOB, CTX_METHOD, CTX_IF, CTX_FOR, CTX_BLOCK, CTX_PARAM
  }ctx_type;

/**
 * @brief This represents an item in the TOS
 * @param key is the identifier 
 * @param id_type Variable, Method, Temporary Variable, Parameter of a method, or Array
 * @param value is the type INT, BOOL, or VOID
 * @param order is the order of insertion of this item in the TOS, this is useful for calculating the offset
 * @param size to store the size of an Array
 * @param p is a list of parameters if this item is a Method
 */
typedef struct Ht_item {
    char *key;      // identificateur
    int id_type;    // variable (ID_VAR) ou méthode (ID_METHODE) ou temporaire (ID_TMP) ou paramètre de méthode (ID_PARAM) ou tableau (ID_TAB)
    int value;      // type (INT / BOOL / VOIDTYPE)
	int order;      // ordre d'insértion dans la TOS
    int size;       // si tableau, sinon 4
    param p;        // si l'item est une méthode, param est une liste des paramètres et leurs types
} Ht_item;

typedef struct LinkedList {
    Ht_item *item;
    struct LinkedList *next;
} LinkedList;

/**
 * @brief this is the symbol table
 * @param count is the number of elements in the TOS
 * @param type is the type of context (FOR, METHOD, IF, etc). useful for semantic rules.
 */
typedef struct HashTable {
    Ht_item **items;
    LinkedList **lists;
    int max_size;                // Taille max de la TOS
    int count;                  // Nombre d'élément dans la table
	struct HashTable *next;
    ctx_type type;                   // Type du contexte courrant: Méthode, boucle for, condition If, etc ...     
} HashTable;

/**
 * @brief an item and the symbol table where it is located. This structure is returned by the lookup function, it serves to calculate the offset
 * of the item and to know if the item is in a global context or not.
 */
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


/**
 * @brief create a new temporary variable and adds it to the TOS
 * @param type INT or BOOL
 * @return a pointer to the item in the symbol table containing the temporary variable created
 */
Ht_item* new_temp(int type);

void pushctx(); 
void popctx();

/**
 * @brief inserts a enw item to the current context
 * @param item the item to insert to the TOS
 */
void newname(Ht_item *item);

/* looks for a variable with the name key in the symbol tables and returns the couple ( item, symbol table) */
/**
 * @brief Looks in the Symbol Tables for the first variable with this key 
 * @param key is the identifier to look for
 * @return a structure which has a pointer to the Item in the TOS, and a pointer to the TOS where the item is found.
 */
item_table *lookup(char *key);

/* prints the symbol table of the current context*/
void print_ctx();

/* prints all the symbol tables pushed into the stack*/
void print_stack();
void free_stack();

/**
 * @brief calculates the offset of an item in relation to the current context (curr_context is a global variable pointing to the current TOS)
 * @param item this is a structure containing a pointer to the item, and a pointer to its symbol table
 * @return the offset of the item
 */
int offset(item_table *item);

/**
 * @brief called after an affectation to pop the temporary variables used in the evaluation of the expression
 * @return the number of variable to pop
 */
int pop_tmp(); 
char* num_to_char(int nb);
void pop_nb_tmp(int nb);

param reverse_list(param p);    // not used anymore

char* get_type_global(int type); // used for printing the symbol table
char* get_type_id(int type);    // used for printing the symbol table

extern HashTable* curr_context; // pointer to the current context
extern HashTable* glob_context; // pointer to the global context
extern HashTable* stack;        // stack of symbol tables where the popped ones are stored, used for printing the TOS at the end of the program

#endif