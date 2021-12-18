
#ifndef __INTERM_H__
#define __INTERM_H__

#include "token.h"

typedef struct param *param;

typedef struct list{
  int addr;
  struct list* suiv;
}*list;

typedef enum quadop_type{
    QO_CST, QO_ID, QO_TMP, QO_GLOBAL, QO_EMPTY
  }quadop_type;

typedef enum quad_type{
    Q_DECL, Q_ADD, Q_SUB, Q_MUL, Q_DIV, Q_MOD, Q_GOTO, Q_EQ, Q_NEQ, Q_LT, Q_GT, Q_LEQ, Q_GEQ, Q_AFF, Q_AFFADD, Q_AFFSUB, Q_LABEL, Q_SYSCALL, Q_ENDFUNC, Q_METHODCALL
  }quad_type;

typedef enum node_type{
	N_OP, N_VAL
}node_type;


typedef struct quadop {
  quadop_type type;     // Constante, identificateur, temporaire, globale
  union {
    int cst;
	  char* temp;  // a supprimer
    int offset;
    struct global{
      char *name;
      int size;
    } global;  // variable global
  } u;
} quadop;

typedef struct quad{
  int addr;
  quad_type type;
  quadop op1, op2, op3; 
  char *label; // label à placer avant le quad dans le mips (pour une méthode par exemple)
  int jump; // index du label dans le tableau des quad où effectuer le GOTO, -1 si pas de jump à effectuer
  param p;  // si method call
} quad;

extern quad global_code[5000]; // code généré
extern size_t nextquad; // n° du prochain quad
extern size_t tmpCount; // n° de la prochaine variable temporaire dans la table des symboles
extern size_t labelCount;
extern size_t glob_dec_count;

list crelist(int addr);
void complete(list n, int addr);
list concat(list n1, list n2);
void print_globalcode();
char *op_type(int type);
char* next_label_name();
char* new_endfunc_label(char *name);
void add_labels();

char *get_type_oper(int type);
char* new_label();

void gencode(quadop op1, quadop op2, quadop op3, quad_type type, char *label, int jump, param p); // écrie le quadruplet avec les paramètres spécifiés dans global_code[nextquad] et incrémente nextquad
/* HOW TO USE GENCODE : 

Opération binaire:  a = b op c       =>  op1 = a ; op2 = b ; op3 = c
Affectation:        a = b            =>  op1 = a ; op2 = b ; op3 = we don't care
Comparaison:        a rel_op b       =>  op1 = we don't care;  op2 = a ; op3 = b ;

*/

#endif