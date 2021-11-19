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
	quadop qop;
	struct {
		quadop result;
		list next;
		list t;
		list f;
	} exprval;
}

%token <intval> decimal_literal hex_literal char_literal
%token <boolval> bool_literal
%token <stringval> id
%token eq add sub divide mul mod and or not lt gt leq geq aff aff_add aff_sub opar cpar oac cac instr 
%token newline

%type <exprval> expr S operand
%type <intval> int_literal arith_op rel_op cond_op eq_op assign_op bin_op
%type <qop> literal


%left and or
%left eq neq
%left lt gt leq geq
%left add sub
%left divide
%left mul
%left aff aff_add aff_moins
%left NEG


%%
program		:	oac	A cac	{;}

A 			:	A S	{;}
			| 	%empty {;}

S 			:	id assign_op expr instr {	
											quadop q1, q2;
											q1 = quadop_name($1);											
											gencode(q1,$3.result,$3.result,$2,-1);}

expr		:	expr bin_op operand  	{
											quadop qo = new_temp();
											gencode(qo, $1.result, $3.result, $2, -1);
											$$.result = qo;
										}
			| 	literal 				{
											$$.result = $1;}
			|	id 						{
											$$.result = quadop_name($1);}
			|	sub expr %prec NEG 		{
											//TO_DO
											;
										}
			|	not expr %prec NEG 		{
											$$.t = $2.f;
											$$.f = $2.t; }
			|	opar expr cpar 			{
											$$.result = $2.result; 	}

operand		:	expr 					{	$$.result = $1.result;	}


bin_op 		: 	arith_op	{$$ = $1;}
			|	rel_op		{$$ = $1;}
			|	eq_op		{$$ = $1;}
			|	cond_op		{$$ = $1;}

literal		:	int_literal		{
									struct quadop q;
									q.type = QO_INTCST;
									q.u.cst = $1;
									$$ = q;
								}
			|	char_literal	{
									struct quadop q;
									q.type = QO_INTCST;
									q.u.cst = $1;
									$$ = q;
								}
			|	bool_literal	{
									struct quadop q;
									q.type = QO_BOOLCST;
									q.u.cst = $1;
									$$ = q;
								}

int_literal	: 	decimal_literal {$$ = $1;}
			| 	hex_literal	{$$ = $1;}

arith_op	:	add	{$$ = Q_ADD;}
			|	sub	{$$ = Q_SUB;}
			|	divide	{$$ = Q_DIV;}
			|	mul	{$$ = Q_MUL;}
			|	mod {$$ = Q_MOD;}

rel_op		:	lt	{$$ = Q_LT;}
			|	gt	{$$ = Q_GT;}
			|	leq {$$ = Q_LEQ;}
			|	geq {$$ = Q_GEQ;}

eq_op		:	eq	{$$ = Q_EQ;}
			|	neq	{$$ = Q_NEQ;}

cond_op		:	and	{$$ = Q_AND;}
			|	or	{$$ = Q_OR;}

assign_op	:	aff		{$$ = Q_AFF;}
			|	aff_add	{$$ = Q_AFFADD;}
			|	aff_sub	{$$ = Q_AFFSUB;}

%%

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}
