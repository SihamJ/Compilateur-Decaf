#ifndef INTERMEDIATE_H_
#define INTERMEDIATE_H_

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "token.h"
#include "hashtable.h"

typedef struct list{
  int addr;
  struct list* suiv;
}*list;

typedef enum quadop_type{
    QO_CST, QO_ID, QO_TMP
  }quadop_type;

typedef enum quad_type{
    Q_ADD, Q_SUB, Q_MUL, Q_DIV, Q_MOD, Q_GOTO, Q_EQ, Q_NEQ, Q_LT, Q_GT, Q_LEQ, Q_GEQ, Q_AND, Q_OR, Q_NOT, Q_AFF, Q_AFFADD, Q_AFFSUB,  
  }quad_type;

typedef enum node_type{
	N_OP, N_VAL
}node_type;

typedef struct quadop {
  quadop_type type;
  union {
    int cst;
	char* temp;
    int offset;
  } u;
} quadop;

typedef struct quad{
  int addr;
  quad_type type;
  quadop op1, op2, op3;
  int label;
} quad;

typedef struct expr_val {
	quadop result;
	int type;
	list next;
	list t;
	list f;
} expr_val;

typedef struct ast_node {
	node_type n_type;
	expr_val expr;
	quad_type type;
	struct ast_node *children[2];
} ast_node;

typedef struct ast_stack {
	ast_node *node;
	struct ast_stack *next;
} ast_stack;

extern quad global_code[5000]; // code généré
extern size_t nextquad; // n° du prochain quad
extern size_t tmpCount; // n° de la prochaine variable temporaire dans la table des symboles
void gencode(quadop op1, quadop op2, quadop op3, quad_type type, int label); // écrie le quadruplet avec les paramètres spécifiés dans global_code[nextquad] et incrémente nextquad
quadop new_temp();
list crelist(int addr);
void complete(list n, int addr);
list concat(list n1, list n2);
void ast_print(ast_node *ast, int depth);
ast_node *ast_new_leaf(expr_val exprval);
ast_node *ast_new_node(ast_node *cl, quad_type qt, ast_node *cr);
ast_stack *ast_stack_push(ast_node *node, ast_stack *stack);
ast_stack *ast_stack_pop(ast_stack *stack);
void ast_stack_print(ast_stack *stack);
ast_stack *ast_to_stack(ast_node *ast, ast_stack *stack);
void print_globalcode();

#endif