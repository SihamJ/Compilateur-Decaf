#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct list{
  int addr;
  struct list* suiv;
}*list;


typedef enum quadop_type{
    QO_CST, QO_ID,
  }quadop_type;

typedef enum quad_type{
    Q_ADD, Q_SUB, Q_MUL, Q_DIV, Q_MOD, Q_GOTO, Q_EQ, Q_NEQ, Q_LT, Q_GT, Q_LEQ, Q_GEQ, Q_AND, Q_OR, Q_NOT, Q_AFF, Q_AFFADD, Q_AFFSUB,  
  }quad_type;

typedef struct quadop {
  quadop_type type;
  union {
    int cst;
    char* name;
  } u;
} quadop;

typedef struct quad{
  int addr;
  quad_type type;
  quadop op1, op2, op3;
  int label;
} quad;


quad global_code[5000]; // code généré
size_t nextquad; // n° du prochain quad
size_t tmpCount; // n° de la prochaine variable temporaire dans la table des symboles
void gencode(quadop op1, quadop op2, quadop op3, quad_type type, int label); // écrie le quadruplet avec les paramètres spécifiés dans global_code[nextquad] et incrémente nextquad
quadop new_temp();
list crelist(int addr);
void complete(list n, int addr);
list concat(list n1, list n2);
quadop quadop_name(char *name);
void print_globalcode();
