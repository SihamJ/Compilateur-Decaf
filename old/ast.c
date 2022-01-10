#include "ast.h"

void ast_print(ast_node *ast, int depth){
	for(int i = 0; i < depth; i++){
		printf("  ");
	}
	if(ast->n_type == N_VAL){
		printf("%s\n",(ast->expr.type==INT)?"INT":"BOOL");
	} else {
		printf("%d\n",ast->type);
		ast_print(ast->children[0], depth+1);
		ast_print(ast->children[1], depth+1);
	}
}

ast_node *ast_new_leaf(expr_val exprval){
	ast_node *out = malloc(sizeof(struct ast_node));
	out->n_type = N_VAL;
	out->expr = exprval;
	out->children[0] = NULL;
	out->children[1] = NULL;
	return out;
}

ast_node *ast_new_node(ast_node *cl, quad_type qt, ast_node *cr){
	ast_node *out = malloc(sizeof(struct ast_node));
	out->n_type = N_OP;
	out->type = qt;
	out->children[0] = cl;
	out->children[1] = cr;
	return out;
}

void ast_node_free(ast_node *node){
	node->children[0] = NULL;
	node->children[1] = NULL;
	free(node);
}

ast_stack *ast_stack_push(ast_node *node, ast_stack *stack){
	ast_stack *out = malloc(sizeof(struct ast_stack));
	out->node = node;
	if (!stack){
		out->next = NULL;
	} else {
		out->next = stack;
	}
	return out;
}

ast_stack *ast_stack_pop(ast_stack *stack){
	ast_stack *out = stack->next;
	ast_stack *aux = stack;
	aux->next = NULL;
	free(aux);
	return out;
}

ast_node *ast_stack_top(ast_stack *stack){
	return stack->node;
}

void ast_stack_print(ast_stack *stack){
	ast_stack *temp = stack;
	while(temp){
		if(temp->node->n_type == N_OP)
			printf("%d ", temp->node->type);
		else
			printf("%s ",(temp->node->expr.type==INT)?"INT":"BOOL");
		temp = temp->next;
	}
	printf("\n");
}

ast_stack *ast_to_stack(ast_node *ast, ast_stack *stack){
	if(ast->n_type == N_VAL)
		return stack;
	stack = ast_stack_push(ast, stack);
	if(ast->children[0]->n_type == N_OP){
		quadop q;
		q.type = QO_TMP;
		q.u.temp = malloc(4*sizeof(char));
		sprintf(q.u.temp, "$t0");
		ast->children[0]->expr.result = q;
	}
	if(ast->children[1]->n_type == N_OP){
		quadop q;
		q.type = QO_TMP;
		q.u.temp = malloc(4*sizeof(char));
		sprintf(q.u.temp, "$t1");
		ast->children[1]->expr.result = q;
	}
	stack = ast_to_stack(ast->children[0],stack);
	stack = ast_to_stack(ast->children[1],stack);
	return stack;
}

void stack_to_code(ast_stack *stack){
	ast_stack *temp = stack;
	ast_node *node;
	while(temp){
		node = ast_stack_top(temp);
		if(node->expr.type == BOOL){
			if((node->type == Q_OR) || (node->type == Q_AND) || (node->type == Q_NOT)){
				
			}
		} else {
			gencode(node->expr.result, node->children[0]->expr.result, node->children[1]->expr.result, node->type, -1);
		}	
		temp = ast_stack_pop(temp);
	}
}