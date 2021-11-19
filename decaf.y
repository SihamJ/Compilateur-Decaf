%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>
	#include "intermediaire.h"
	int yylex();
	void yyerror(char*);
%}

%union {
	int intval;
	bool boolval;
	char *stringval;
	char **stringlist;
	quadop qop;
	struct {
		quadop result;
		list next;
		list t;
		list f;
	} exprval;
	struct decl{
		char *name;
		struct decl *suiv;
	} decl;
}

%token <intval> decimal_literal hex_literal char_literal eq neq add sub divide mul mod and or not lt gt leq geq aff aff_add aff_sub opar cpar oac cac instr integer boolean sep
%token <boolval> bool_literal
%token <stringval> id
%token newline

%type <exprval> expr S
%type <intval> int_literal assign_op type
%type <qop> literal
%type <decl> B

%left or
%left and
%left eq neq
%left lt gt leq geq
%left add sub
%left mul divide mod
%left not
%left NEG


%%
program		:	oac	A cac	{;}

A 			:	A S	{;}
			| 	%empty {;}

var_decl 	:  	type B instr {	
								quadop qo,q1;
								if($1 == integer)
									q1.u.cst = 0;
								else if($1 == boolean)
									q1.u.cst = false;
								struct decl *pt = &$2;
								q1.type = QO_CST;
								q1.u.cst = false;
								qo.type = QO_ID;
								while(pt != NULL){
									qo.u.name = malloc(strlen(pt->name));
									strcpy(qo.u.name, pt->name);
									gencode(qo,q1,q1,Q_AFF,-1);
									pt = pt->suiv;
								}
							 }

type		:	integer {$$=$1;}
			|	boolean {$$=$1;}

B 			:	B sep id 	{ 	struct decl var; var.name = malloc(strlen($3)); strcpy(var.name,$3);var.suiv = &$$; $$ = var;}
			|	id			{	$$.name = malloc(strlen($1)); strcpy($$.name,$1); $$.suiv = NULL;}

S 			:	id assign_op expr instr {
											quadop q1, q2;
											q1.u.name = malloc(strlen($1));
											strcpy(q1.u.name, $1);
											q1.type = QO_ID;
											gencode(q1,$3.result,$3.result,$2,-1);}
			|	var_decl {;}

expr		:	expr add expr			{$2 = Q_ADD; quadop qo; qo = new_temp() ;gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr sub expr			{$2 = Q_SUB;quadop qo; qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr mul expr			{$2 = Q_MUL;quadop qo;  qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr divide expr		{$2 = Q_DIV; quadop qo; qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr mod expr			{$2 = Q_MOD; quadop qo; qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr and expr			{$2 = Q_AND; quadop qo; qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr or expr			{$2 = Q_OR; quadop qo; qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr lt expr			{$2 = Q_LT; quadop qo; qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr gt expr			{$2 = Q_GT; quadop qo; qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr geq expr			{$2 = Q_GEQ; quadop qo; qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr leq expr			{$2 = Q_LEQ; quadop qo; qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr eq expr			{$2 = Q_EQ; quadop qo; qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr neq expr			{$2 = Q_NEQ; quadop qo; qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			| 	literal 				{
											$$.result = $1;}
			|	id 						{
											$$.result.u.name = malloc(strlen($1));
											strcpy($$.result.u.name,$1);
											$$.result.type = QO_ID;
											}
			|	sub expr %prec NEG 		{
											//TO_DO
											;
										}
			|	not expr %prec NEG 		{
											$$.t = $2.f;
											$$.f = $2.t; }
			|	opar expr cpar 			{
											$$.result = $2.result; 	}


literal		:	int_literal		{
									struct quadop q;
									q.u.cst = $1;
									q.type = QO_CST;
									$$ = q;
								}
			|	char_literal	{
									struct quadop q;
									q.u.cst = $1;
									q.type = QO_CST;
									$$ = q;
								}
			|	bool_literal	{
									struct quadop q;
									q.u.cst = $1;
									q.type = QO_CST;
									$$ = q;
								}

int_literal	: 	decimal_literal {$$ = $1;}
			| 	hex_literal	{$$ = $1;}

assign_op	:	aff		{$$ = Q_AFF;}
			|	aff_add	{$$ = Q_AFFADD;}
			|	aff_sub	{$$ = Q_AFFSUB;}

%%

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}
