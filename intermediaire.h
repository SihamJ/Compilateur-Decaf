
#ifndef __INTERM_H__
#define __INTERM_H__

#include "token.h"
#include "text_formating.h"

typedef struct HashTable HashTable;
extern HashTable* curr_context;

typedef enum quadop_type{
    QO_CST, QO_ID, QO_TMP, QO_GLOBAL, QO_EMPTY, QO_CSTSTR, QO_GOTO
  } quadop_type;

/**
 * @brief if the quadop is global, we need to know if it is a scalar or an array
 */
typedef enum global_type{
    QO_SCAL, QO_TAB
  } global_type;

typedef enum quad_type{
    Q_DECL, Q_ADD, Q_SUB, Q_MUL, Q_DIV, Q_MOD, Q_GOTO, Q_EQ, Q_NEQ, Q_LT, Q_GT, Q_LEQ, Q_GEQ, Q_SEQ, Q_SNE, Q_AFF, Q_AFFADD, Q_AFFSUB, Q_FUNC, Q_SYSCALL, Q_ENDFUNC, Q_METHODCALL, Q_ACCESTAB, Q_RETURN, Q_POP, Q_PUSH, Q_DELETED
  } quad_type;

/**
 * @brief a list of strings to declare in mips .data section
 */
typedef struct string_labels{
  char *label;
  char *value;
} string_labels; 

/**
 * @brief a quad operand
 * @param type Constant, Local Variable, Temporary Variable, Global Variable, String
 * @param union__int_cst If the type is constant, we store the value in cst
 * @param union__int_offset If the type is a local or temporary variable, we store its offset in offset
 * @param union__struct_global If the type is global, we need a structure where we can store the name of the operand, the type (scalar or array), and the size for the array
 * @param union__struct_string_literal If the type is String, we need a structure to store its value and give it a label to declare it in mips 
 */
typedef struct quadop {
  quadop_type type;     // Constante, identificateur, temporaire, globale, constante str, tableau
  union {
    int cst;
    int offset;         // si ID ou TMP
    char* name;         // si ID ou TMP
    struct global{
      char *name;
      int size;
      global_type type;         // TABLEAU ou SCALAIRE
    } global;  // variable global
    struct string_literal{
      char *label;
      char *value;
    } string_literal;  // string literal pour WriteString
  } u;
} quadop;

/**
 * @brief a list containing indexes of incomplete GOTOs
 */
typedef struct list{
  int addr;
  struct list* suiv;
}*list;

/**
 * @brief a list to store parameters of a method at the declaration or call
 * @param type INT BOOL or STRING
 * @param quadop_arg This is used in case of a method call. The argument is stored in a quad operand
 * @param list if the argument is of type BOOL, we need to retrieve the true and false lists to complete them
 * @param stringval so we can store the name of the temporary variable and retrieve the offset. This is needed when temporary variables are created to evaluate arguments of a method call
 * @param byAddress a boolean indicating if this a copy or a pointer, useful for ReadInt
 * @param next is the next parameter in the list
 */
typedef struct param{
    int type;           // INT or BOOL or STRING;
    quadop arg;         // utilisé dans le cas d'un appel de méthode, l'argument est stocké dans un quadop
    list t;
    list f;
    struct param* next;
    char *stringval;
    int byAddress;    // passage par adresse ou par valeur
} *param;

/**
 * @brief quad for three address instruction
 * @param type is the type of operation (ADD, MUL, DECLARATION, etc)
 * @param op_1_2_3 op1 op2 op3 are the three operands 
 * @param label If there is a jump to this quad, it needs to have a label name for mips. This is added at the end of 3 address code generation.
 * @param jump Index of jump in the global code array
 * @param p list of arguments if this quad is a method call
 */
typedef struct quad{
  quad_type type;
  quadop op1, op2, op3;
  char *label; // label à placer avant le quad dans le mips (pour une méthode par exemple)
  int jump; // index du label dans le tableau des quad où effectuer le GOTO, -1 si pas de jump à effectuer
  param p;  // si method call
  struct HashTable* ctx;
} quad;


typedef struct expr_val {
    quadop result;
    char *stringval;
    int type;		// type of result operand: INT / BOOL / STRING
    list t;  		// true
    list f;   	// false
  } expr_val;


typedef struct statement {
  list next;		
  list brk; 		// break
  list cntu; 		// continue
  list rtrn;		// return
  list elseGoto;
  int isElseBlock; 
} statement;

typedef struct method_call {
  quadop result;
  int return_type;
  char* result_id; // a temporary variable where we store the return value if any
} method_call;

typedef struct literal{
		int intval;
		int type;
}literal;

typedef struct location {
		char *stringval;
    char *index_name;
		quadop index;	// in case it is an array, we need the index
		int type;		// scalaire ou TAB
	} location;
  
typedef struct declaration{
    global_type type;
    char *name;
    struct declaration *suiv;
    int size;
} declaration;

extern quad global_code[5000]; // code généré
extern size_t nextquad; // n° du prochain quad
extern size_t tmpCount; // n° de la prochaine variable temporaire dans la table des symboles
extern size_t labelCount; // counter of the number of auto-generated labels
extern size_t glob_dec_count; // counter of global variables declared
extern size_t str_count; // counter of number of string_literals to be declared in MIPS .data, used only for generating labels
extern string_labels str_labels[100];

list crelist(int addr);
void complete(list n, int addr);
list concat(list n1, list n2);

void print_globalcode();

char *op_type(int type); //used for printing intermediary code

/* used for comparing an id with the supposed next auto-generated label to avoid conflicts*/
char* next_label_name();

/* generates a label for the end of a function*/
char* new_endfunc_label(char *name);

/* Called at the end in the main, it adds labels for all the quad adresses where we do a jump*/
void add_labels();

/* used for printing intermediary code*/
char *get_type_oper(int type);

/* generates a new_label*/
char* new_label();

/* generates a new label for string declarations*/
char* new_str();

void gencode(quadop op1, quadop op2, quadop op3, quad_type type, char *label, int jump, param p); 

int delete_quad(int index);

int replace_quad(int index, quadop op1, quadop op2, quadop op3, quad_type t, char *label, int jump, param p);

void delete_quad_list(list n);

void insert_quad(int index, quadop op1, quadop op2, quadop op3, quad_type t, char *label, int jump, param p);

void free_global_code();

#endif