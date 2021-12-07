%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>
	#include "intermediaire.h"
	#include "hashtable.h"
	#include "token.h"
	int yylex();
	void yyerror(char*);
%}

%union {
	int intval;
	bool boolval;
	char *stringval;
	char **stringlist;
	struct {
		quadop qop;
		int type;
	} literal;
	expr_val exprval;
	ast_node* ast;
	struct decl{
		char *name;
		struct decl *suiv;
	} decl;
}

%token <intval> decimal_literal hex_literal char_literal eq neq and or not leq geq aff_add aff_sub integer boolean voidtype '+' '-' '%' '/' '<' '>' '=' '!' '*'
%token <boolval> bool_literal
%token <stringval> id 
%token class Program If Else For Return Continue Break

%type <exprval> S
%type <ast> expr
%type <intval> int_literal assign_op type
%type <literal> literal
%type <decl> B
%type <stringval> location

%left or
%left and
%left eq neq
%left '<' '>' leq geq
%left '+' '-'
%left '*' '/' '%'
%left '!'
%left NEG


%%

program	: class Program '{' {pushctx(); glob_context = curr_context;} P '}' { /* popctx()*/ ;}

P 		: MD	{;}
		| FD 	{;}
		| FD MD {;}
		| %empty	{;}

 FD 	: FD field_decl  {;}
		| field_decl	{;}

field_decl	:	var_decl	{;}
			|	tab_decl	{;}

MD	:	MD method_decl		{;}
	|	method_decl		{;}

method_decl	:	type id  '(' Param ')'  block	{;}
			|	voidtype id '(' Param ')'  block	{;}


Param	:	Param ',' type id 	{;}
		|	type id				{;}
		|	%empty				{;}

 tab_decl	:	type id '[' int_literal ']' ',' tab_decl ';' 	{;}
			|	type id '[' int_literal ']'	';'					{;}

block 		:	'{' { pushctx(); glob_context = curr_context; } V S '}' { /* popctx()*/  ;}

 V 			:	V var_decl 	{;}
			|	%empty		{;}

var_decl 	:  	type B ';' {
								quadop qo,q1;
								if($1 == INT)
									q1.u.cst = 0;
								else if($1 == BOOL)
									q1.u.cst = false;
								struct decl *pt = &$2;
								q1.type = QO_CST;
								qo.type = QO_ID;
								while(pt != NULL){
									Ht_item *item = (Ht_item*) malloc(sizeof(Ht_item));
									qo.u.offset = -4;
									gencode(qo,q1,q1,Q_AFF,-1);
									item->value = $1;
									item->key = malloc(strlen(pt->name)+1);
									strcpy(item->key, pt->name);
									newname(item);
									pt = pt->suiv;
								}
							 }

B 			:	id ',' B  	{ 	struct decl var;
								var.name = malloc((strlen($1)+1)); 
								strcpy(var.name,$1);var.suiv = &$3;
								$$ = var;
								}
			|	id			{	$$.name = malloc((strlen($1)+1)); 
								strcpy($$.name,$1); 
								$$.suiv = NULL;}

type		:	integer {$$=$1;}
			|	boolean {$$=$1;}

S 			: 	S statement 	{;}
			| 	%empty			{;}

statement 	:	id assign_op expr ';' {				
													Ht_item *val = lookup($1);
													if (!val)
														yyerror("Erreur: Variable non déclarée\n");
													if(val->value != $3->expr.type)
														yyerror("Erreur: Type de valeur incorrecte\n");
													if (($2 == Q_AFFADD || $2 == Q_AFFSUB) && (val->value == BOOL || $3->expr.type == BOOL))
														yyerror("Erreur: Type de valeur incorrecte\n");
													ast_print($3, 0);
													quadop q0, q1;
													q0.type = QO_TMP;
													q0.u.temp = malloc(4*sizeof(char));
													sprintf(q0.u.temp, "$t0");
													$3->expr.result = q0;
													ast_stack *stack = ast_to_stack($3, NULL);
													ast_stack_print(stack);
													stack_to_code(stack);
													q1.u.offset = offset(val);
													q1.type = QO_ID;
													if($2 == Q_AFF)
														gencode(q1,$3->expr.result, $3->expr.result,$2,-1);
													else
														gencode(q1,q1,$3->expr.result,$2,-1);

												}
			|	method_call	';'					{;}
			|	If '(' expr ')' block elseblock	{;}
			|	For id '=' expr ',' expr block	{;}
			|	Return return_val ';'			{;}
			|	Break ';'						{;}
			|	Continue ';'					{;}
			|	block							{;}

elseblock	:	Else block						{;}
			|	%empty							{;}

return_val	:	expr 							{;}
			|	%empty							{;}


method_call :	id '(' E ')' 	{;}

E 			:	E ',' expr 		{;}
			|	expr 			{;}
			| 	%empty			{;}

location	:	id	{;}
			|	id '[' expr ']'	{;}

expr		:	expr '+' expr			{
											if($1->expr.type != INT || $3->expr.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$2 = Q_ADD; $$ = ast_new_node($1,$2,$3); $$->expr.type = INT;}
			|	expr '-' expr			{	if($1->expr.type != INT || $3->expr.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$2 = Q_SUB; $$ = ast_new_node($1,$2,$3); $$->expr.type = INT;}
			|	expr '*' expr			{	if($1->expr.type != INT || $3->expr.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$2 = Q_MUL; $$ = ast_new_node($1,$2,$3); $$->expr.type = INT;}
			|	expr '/' expr			{	if($1->expr.type != INT || $3->expr.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$2 = Q_DIV; $$ = ast_new_node($1,$2,$3); $$->expr.type = INT;}
			|	expr '%' expr			{	if($1->expr.type != INT || $3->expr.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$2 = Q_MOD; $$ = ast_new_node($1,$2,$3); $$->expr.type = INT;}
			|	expr and expr			{	if($1->expr.type != BOOL || $3->expr.type != BOOL)
												yyerror("Erreur: AND operator with non boolean value");
											$2 = Q_AND; $$ = ast_new_node($1,$2,$3); $$->expr.type = BOOL;}
			|	expr or expr			{	if($1->expr.type != BOOL || $3->expr.type != BOOL)
												yyerror("Erreur: OR operator with non boolean value");
											$2 = Q_OR; $$ = ast_new_node($1,$2,$3);}
			|	expr '<' expr			{	if($1->expr.type != INT || $3->expr.type != INT)
												yyerror("Erreur: REL OP non entière");
											$2 = Q_LT; $$ = ast_new_node($1,$2,$3); $$->expr.type = BOOL;}
			|	expr '>' expr			{	if($1->expr.type != INT || $3->expr.type != INT)
												yyerror("Erreur: REL OP non entière");
											$2 = Q_GT; $$ = ast_new_node($1,$2,$3); $$->expr.type = BOOL;}
			|	expr geq expr			{	if($1->expr.type != INT || $3->expr.type != INT)
												yyerror("Erreur: REL OP non entière");
											$2 = Q_GEQ; $$ = ast_new_node($1,$2,$3); $$->expr.type = BOOL;}
			|	expr leq expr			{	if($1->expr.type != INT || $3->expr.type != INT)
												yyerror("Erreur: REL OP non entière");
											$2 = Q_LEQ; $$ = ast_new_node($1,$2,$3); $$->expr.type = BOOL;}
			|	expr eq expr			{	if($1->expr.type != $3->expr.type )
												yyerror("Erreur: Comparaison de types différents");
											$2 = Q_EQ; $$ = ast_new_node($1,$2,$3); $$->expr.type = BOOL;}
			|	expr neq expr			{	if($1->expr.type != $3->expr.type )
												yyerror("Erreur: Comparaison de types différents");
											$2 = Q_NEQ; $$ = ast_new_node($1,$2,$3); $$->expr.type = BOOL;}
			| 	literal 				{
											expr_val eval;
											eval.result = $1.qop;
											eval.type = $1.type;
											$$ = ast_new_leaf(eval);
											}
			|	location 				{
											Ht_item *val = lookup($1);
											expr_val eval;
											if(!val)
												yyerror("Erreur: Variable non déclarée\n");
											eval.result.u.offset = offset(val);
											eval.result.type = QO_ID;
											eval.type = val->value;
											$$ = ast_new_leaf(eval);			
											}
			|	'-' expr %prec NEG 		{
											if($2->expr.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											quadop q1;
											q1.type = QO_CST;
											q1.u.cst = 0;
											expr_val eval;
											eval.type = INT;
											eval.result = q1;
											$$ = ast_new_node(ast_new_leaf(eval),Q_SUB, $2);
											$$->expr.type = INT;
										}
			|	'!' expr %prec NEG 		{	if($2->expr.type != BOOL)
												yyerror("Erreur: NOT operator with non boolean value");
											$$ = ast_new_node($2, Q_NOT, $2);
											$$->expr.type = BOOL;
										}
			|	'(' expr ')' 			{
											$$ = $2; 	}


literal		:	int_literal		{
									struct quadop q;
									q.u.cst = $1;
									q.type = QO_CST;
									$$.qop = q;
									$$.type = INT;
								}
			|	char_literal	{
									struct quadop q;
									q.u.cst = $1;
									q.type = QO_CST;
									$$.qop = q;
									$$.type = INT;
								}
			|	bool_literal	{
									struct quadop q;
									q.u.cst = $1;
									q.type = QO_CST;
									$$.qop = q;
									$$.type = BOOL;
								}

int_literal	: 	decimal_literal {$$ = $1;}
			| 	hex_literal	{$$ = $1;}

assign_op	:	'='		{$$ = Q_AFF;}
			|	aff_add	{$$ = Q_AFFADD;}
			|	aff_sub	{$$ = Q_AFFSUB;}

%%

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
	exit(0);
}
