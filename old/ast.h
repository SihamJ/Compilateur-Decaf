#include "intermediaire.h"

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

void ast_print(ast_node *ast, int depth);
ast_node *ast_new_leaf(expr_val exprval);
ast_node *ast_new_node(ast_node *cl, quad_type qt, ast_node *cr);
ast_stack *ast_stack_push(ast_node *node, ast_stack *stack);
ast_stack *ast_stack_pop(ast_stack *stack);
void ast_stack_print(ast_stack *stack);
ast_stack *ast_to_stack(ast_node *ast, ast_stack *stack);
void stack_to_code(ast_stack *stack);