#include "intermediaire.h"

quad global_code[5000];
size_t nextquad; 
size_t tmpCount; 

quadop new_temp(){
  quadop op;
  int a = tmpCount;
  int cpt = 1;
  while(a){
    a=a/10;
    cpt++;
  }
  op.u.temp = malloc(cpt+3);
  op.type = QO_TMP;
  sprintf(op.u.temp, "$t%ld",tmpCount);
  // TO DO: mettre op dans la table des symboles
  tmpCount++;
  return op;
}

void gencode(quadop op1, quadop op2, quadop op3, quad_type t, int label){
  global_code[nextquad].type = t;
  global_code[nextquad].op1 = op1;
  global_code[nextquad].op2 = op2;
  global_code[nextquad].op3 = op3;
  global_code[nextquad].label = label;
  nextquad++;
}

list crelist(int addr){
  list n = (list) malloc(sizeof(struct list));
  n->addr = addr;
  n->suiv = NULL;
  return n;
}

list concat(list n1, list n2){
  list pt = n1;
  while(pt->suiv != NULL)
    pt = pt->suiv;
  pt->suiv = n2;
  return n1;
}


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
		gencode(node->expr.result, node->children[0]->expr.result, node->children[1]->expr.result, node->type, -1);
		temp = ast_stack_pop(temp);	
	}
}

void print_globalcode(){
  printf("\nCode Intermediaire:\n__________________________\n    op1   op2   op3   oper\n__________________________\n");
  for (int i=0; i<nextquad; i++){
    printf("%d: ",i);
    if(global_code[i].op1.type == QO_ID )
      printf(" %d   ", global_code[i].op1.u.offset);
    else if(global_code[i].op1.type == QO_CST)
      printf(" %d   ",global_code[i].op1.u.cst);
	else
	  printf(" %s   ",global_code[i].op1.u.temp);
    if(global_code[i].op2.type == QO_ID)
      printf(" %d   ", global_code[i].op2.u.offset);
    else if(global_code[i].op2.type == QO_CST)
      printf(" %d   ",global_code[i].op2.u.cst);
	else
	  printf(" %s   ",global_code[i].op2.u.temp);
    if(global_code[i].op3.type == QO_ID)
      printf(" %d   ", global_code[i].op3.u.offset);
    else if(global_code[i].op3.type == QO_CST)
      printf(" %d   ",global_code[i].op3.u.cst);
	else
	  printf(" %s   ",global_code[i].op3.u.temp);

    printf("op[%d] \n",global_code[i].type);
  }
  printf("\n");
}
