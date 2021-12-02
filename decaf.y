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
	struct {
		quadop result;
		int type;
		list next;
		list t;
		list f;
	} exprval;
	struct decl{
		char *name;
		struct decl *suiv;
	} decl;
}

%token <intval> decimal_literal hex_literal char_literal eq neq and or not leq geq aff_add aff_sub integer boolean voidtype '+' '-' '%' '/' '<' '>' '=' '!' '*'
%token <boolval> bool_literal
%token <stringval> id 
%token class Program If Else For Return Continue Break

%type <exprval> expr S
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

program	: class Program '{' {pushctx(); glob_context = curr_context; }  FD MD '}' { /* popctx()*/ ;}

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

B 			:	B ',' id  	{ 	struct decl var; var.name = malloc((strlen($3)+1)); strcpy(var.name,$3);var.suiv = &$1; $$ = var;}
			|	id			{	$$.name = malloc((strlen($1)+1)); strcpy($$.name,$1); $$.suiv = NULL;}

type		:	integer {$$=$1;}
			|	boolean {$$=$1;}

S 			: 	S statement 	{;}
			| 	%empty			{;}

statement 	:	location assign_op expr ';' {				Ht_item *val = lookup($1);
													if (!val)
														yyerror("Erreur: Variable non déclarée\n");
													if(val->value != $3.type)
														yyerror("Erreur: Type de valeur incorrecte\n");
													if (($2 == Q_AFFADD || $2 == Q_AFFSUB) && (val->value == BOOL || $3.type == BOOL))
														yyerror("Erreur: Type de valeur incorrecte\n");
													quadop q1, q2;
													q1.u.offset = offset(val);
													q1.type = QO_ID;
													if($2 == Q_AFF)
														gencode(q1,$3.result,$3.result,$2,-1);
													else
														gencode(q1,q1,$3.result,$2,-1);
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
											if($1.type != INT || $3.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$$.type = INT; $2 = Q_ADD; quadop qo = new_temp() ;gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr '-' expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$$.type = INT; $2 = Q_SUB;quadop qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr '*' expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$$.type = INT; $2 = Q_MUL;quadop qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr '/' expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$$.type = INT; $2 = Q_DIV; quadop qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr '%' expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$$.type = INT; $2 = Q_MOD; quadop qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr and expr			{	if($1.type != BOOL || $3.type != BOOL)
												yyerror("Erreur: AND operator with non boolean value");
											$$.type = BOOL; $2 = Q_AND; quadop qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr or expr			{	if($1.type != BOOL || $3.type != BOOL)
												yyerror("Erreur: OR operator with non boolean value");
											$$.type = BOOL; $2 = Q_OR; quadop qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr '<' expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: REL OP non entière");
											$$.type = BOOL; $2 = Q_LT; quadop qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr '>' expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: REL OP non entière");
											$$.type = BOOL; $2 = Q_GT; quadop qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr geq expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: REL OP non entière");
											$$.type = BOOL; $2 = Q_GEQ; quadop qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr leq expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: REL OP non entière");
											$$.type = BOOL; $2 = Q_LEQ; quadop qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr eq expr			{	if($1.type != $3.type )
												yyerror("Erreur: Comparaison de types différents");
											$$.type = BOOL; $2 = Q_EQ; quadop qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			|	expr neq expr			{	if($1.type != $3.type )
												yyerror("Erreur: Comparaison de types différents");
											$$.type = BOOL; $2 = Q_NEQ; quadop qo = new_temp();gencode(qo,$1.result,$3.result,$2,-1);$$.result = qo;}
			| 	literal 				{
											$$.result = $1.qop;
											$$.type = $1.type;
											}
			|	location 						{
											Ht_item *val = lookup($1);
											if(!val)
												yyerror("Erreur: Variable non déclarée\n");
											$$.result.u.offset = offset(val);
											$$.result.type = QO_ID;
											$$.type = val->value;			
											}
			|	'-' expr %prec NEG 		{
											if($2.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$$.type = INT;
											quadop qo = new_temp();
											quadop q1;
											q1.type = QO_CST;
											q1.u.cst = 0;
											gencode(qo, q1, $2.result, Q_SUB, -1);
											$$.result = qo;
										}
			|	'!' expr %prec NEG 		{	if($2.type != BOOL)
												yyerror("Erreur: NOT operator with non boolean value");
											$$.type = BOOL; 
											quadop q0 = new_temp();
											gencode(q0, $2.result, $2.result, Q_NOT, -1);
											$$.result = q0;
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
