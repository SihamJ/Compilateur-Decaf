
#ifndef __INTERM_H__
#define __INTERM_H__

#include "token.h"

typedef enum quadop_type{
    QO_CST, QO_ID, QO_TMP, QO_GLOBAL, QO_EMPTY, QO_CSTSTR
  } quadop_type;

typedef enum global_type{
    QO_SCAL, QO_TAB
  } global_type;

typedef enum quad_type{
    Q_DECL, Q_ADD, Q_SUB, Q_MUL, Q_DIV, Q_MOD, Q_GOTO, Q_EQ, Q_NEQ, Q_LT, Q_GT, Q_LEQ, Q_GEQ, Q_AFF, Q_AFFADD, Q_AFFSUB, Q_FUNC, Q_SYSCALL, Q_ENDFUNC, Q_METHODCALL, Q_ACCESTAB, Q_RETURN, Q_POP
  } quad_type;

/* strings to declare for write string as labels in mips*/
typedef struct string_labels{
  char *label;
  char *value;
} string_labels; 

typedef struct quadop {
  quadop_type type;     // Constante, identificateur, temporaire, globale, constante str, tableau
  union {
    int cst;
    int offset;         // si ID ou TMP
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

/* To store parameters of a method at declaration or call */
typedef struct param{
    int type;           // INT or BOOL or STRING;
    quadop arg;         // utilisé dans le cas d'un appel de méthode, l'argument est stocké dans un quadop
    struct param* next;
} *param;

typedef struct quad{
  int addr;
  quad_type type;
  quadop op1, op2, op3;
  char *label; // label à placer avant le quad dans le mips (pour une méthode par exemple)
  int jump; // index du label dans le tableau des quad où effectuer le GOTO, -1 si pas de jump à effectuer
  param p;  // si method call
} quad;

/* list for storing address of incomplete quads */
typedef struct list{
  int addr;
  struct list* suiv;
}*list;

typedef struct expr_val {
    quadop result;
    char *stringval;
    int type;		// type of result operand: INT / BOOL / STRING
    list t;  		// true
    list f;   		// false
    list next;		
    list brk; 		// break
    list cntu; 		// continue
    list rtrn;		// return
    param p;
    } expr_val;

typedef struct block {
  list next;		
  list brk; 		// break
  list cntu; 		// continue
  list rtrn;		// return
} block;

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
extern size_t str_count;

list crelist(int addr);
void complete(list n, int addr);
list concat(list n1, list n2);

void print_globalcode();

/* used for printing global code*/
char *op_type(int type);

/* used for comparing a declared id with the supposed next auto-generated label*/
char* next_label_name();

/* generates a label for the end of a function.*/
char* new_endfunc_label(char *name);

/* Called at the end in the main, it adds labels for all the quad adresses where we do a jump*/
void add_labels();

/* used for printing global code*/
char *get_type_oper(int type);

/* generates a new_label*/
char* new_label();

/* generates a new label for string declaration*/
char* new_str();

void gencode(quadop op1, quadop op2, quadop op3, quad_type type, char *label, int jump, param p); // écrie le quadruplet avec les paramètres spécifiés dans global_code[nextquad] et incrémente nextquad
/* HOW TO USE GENCODE : 

Opération binaire:  a = b op c       =>  op1 = a ; op2 = b ; op3 = c
Affectation:        a = b            =>  op1 = a ; op2 = b ; op3 = we don't care
Comparaison:        a rel_op b       =>  op1 = we don't care;  op2 = a ; op3 = b ;
GOTO:               j next           =>  type = Q_GOTO, jump = next
*/

void update_offsets(quadop *op1, int offset);



#endif