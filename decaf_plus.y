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
	struct decl{
		char *name;
		struct decl *suiv;
	} decl;
}

%token <intval> decimal_literal hex_literal char_literal eq neq and or not leq geq aff_add aff_sub integer boolean voidtype '+' '-' '%' '/' '<' '>' '=' '!' '*'
%token <boolval> bool_literal
%token <stringval> id 
%token class Program If Else For Return Continue Break comment

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

%start program
%%

program	:  class Program '{' {pushctx(); glob_context = curr_context; } P '}' { print_ctx(); popctx() ;}


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

method_decl	:	type id  '(' Param ')'  block	{ /* Vérifier que id n'est pas le nom d'une méthode déjà déclarée*/ ;}
			|	voidtype id '(' Param ')'  block	{ /* Vérifier que id n'est pas le nom d'une méthode déjà déclarée*/ ;}


Param	:	Param ',' type id 	{;}
		|	type id				{;}
		|	%empty				{;}

tab_decl	:	type id '[' int_literal ']' ',' tab_decl ';' 	{ /* Vérifier que int_literal supérieur à 0*/ ;}
			|	type id '[' int_literal ']' ';'				{  /* Vérifier que int_literal supérieur à 0*/  ;}

block 		:	'{' { pushctx(); glob_context = curr_context; } V S '}' { print_ctx(); popctx()  ;}

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
									if( ht_search(curr_context, pt->name) != NULL ) 
										yyerror("Erreur: Variable déclarée deux fois\n");

									qo.u.offset = -4;
									gencode(qo, qo, qo, Q_DECL, NULL, -1);
									qo.u.offset = 0;
									gencode(qo, q1, q1, Q_AFF, NULL, -1);

									Ht_item *item = create_item(pt->name, ID_VAR, $1);
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
													item_table *val = lookup($1);
													if (!val)
														yyerror("Erreur: Variable non déclarée\n");
													if(val->item->value != $3.type)
														yyerror("Erreur: Type de valeur incorrecte\n");
													if (($2 == Q_AFFADD || $2 == Q_AFFSUB) && (val->item->value == BOOL || $3.type == BOOL))
														yyerror("Erreur: Type de valeur incorrecte\n");
													quadop q1, q2;
													q1.u.offset = offset(val);
													q1.type = QO_ID;
													if($2 == Q_AFF)
														gencode(q1,$3.result,$3.result,$2,NULL,-1);
													else
														gencode(q1,q1,$3.result,$2,NULL,-1);
													pop_tmp();

												}
			|	method_call	';'					{;}
			|	If '(' expr ')' block elseblock	{	/* Vérifier que expr est de type boolean*/ ;}
			|	For id '=' expr ',' expr block	{   /* compteur de boucle for déclaré implicitement et expr1 de type int*/ ;}
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
			|	id '[' expr ']'	{ /* expr de type int */ ;}

expr		:	expr '+' expr			{
											if($1.type != INT || $3.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$$.type = INT; 
											$2 = Q_ADD; 
											Ht_item* item = new_temp(INT);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											
											update_offsets(&$1.result,&$3.result);
											gencode(qo,$1.result,$3.result,$2,NULL,-1);
											$$.result = qo;}
			|	expr '-' expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$$.type = INT; 
											$2 = Q_SUB;
											Ht_item* item = new_temp(INT);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											
											update_offsets(&$1.result,&$3.result);
											gencode(qo,$1.result,$3.result,$2,NULL,-1);
											$$.result = qo;}
			|	expr '*' expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$$.type = INT; 
											$2 = Q_MUL;
											Ht_item* item = new_temp(INT);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											
											update_offsets(&$1.result,&$3.result);
											gencode(qo,$1.result,$3.result,$2,NULL,-1);
											$$.result = qo;}
			|	expr '/' expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$$.type = INT; 
											$2 = Q_DIV; 
											Ht_item* item = new_temp(INT);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											
											update_offsets(&$1.result,&$3.result);
											gencode(qo,$1.result,$3.result,$2,NULL,-1);
											$$.result = qo;}
			|	expr '%' expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$$.type = INT; 
											$2 = Q_MOD; 
											Ht_item* item = new_temp(INT);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											
											update_offsets(&$1.result,&$3.result);
											gencode(qo,$1.result,$3.result,$2,NULL,-1);
											$$.result = qo;}
			|	expr and expr			{	if($1.type != BOOL || $3.type != BOOL)
												yyerror("Erreur: AND operator with non boolean value");
											$$.type = BOOL; 
											$2 = Q_AND; 
											Ht_item* item = new_temp(BOOL);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											
											update_offsets(&$1.result,&$3.result);
											gencode(qo,$1.result,$3.result,$2,NULL,-1);
											$$.result = qo;}
			|	expr or expr			{	if($1.type != BOOL || $3.type != BOOL)
												yyerror("Erreur: OR operator with non boolean value");
											$$.type = BOOL; 
											$2 = Q_OR; 
											Ht_item* item = new_temp(BOOL);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											
											update_offsets(&$1.result,&$3.result);
											gencode(qo,$1.result,$3.result,$2,NULL,-1);
											$$.result = qo;}
			|	expr '<' expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: REL OP non entière");
											$$.type = BOOL; 
											$2 = Q_LT; 
											Ht_item* item = new_temp(INT);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											
											update_offsets(&$1.result,&$3.result);
											gencode(qo,$1.result,$3.result,$2,NULL,-1);
											$$.result = qo;}
			|	expr '>' expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: REL OP non entière");
											$$.type = BOOL; 
											$2 = Q_GT; 
											Ht_item* item = new_temp(INT);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											
											update_offsets(&$1.result,&$3.result);
											gencode(qo,$1.result,$3.result,$2,NULL,-1);
											$$.result = qo;}
			|	expr geq expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: REL OP non entière");
											$$.type = BOOL; 
											$2 = Q_GEQ; 
											Ht_item* item = new_temp(INT);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											
											update_offsets(&$1.result,&$3.result);
											gencode(qo,$1.result,$3.result,$2,NULL,-1);
											$$.result = qo;}
			|	expr leq expr			{	if($1.type != INT || $3.type != INT)
												yyerror("Erreur: REL OP non entière");
											$$.type = BOOL; 
											$2 = Q_LEQ; 
											Ht_item* item = new_temp(INT);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											
											update_offsets(&$1.result,&$3.result);
											gencode(qo,$1.result,$3.result,$2,NULL,-1);
											$$.result = qo;}
			|	expr eq expr			{	if($1.type != $3.type )
												yyerror("Erreur: Comparaison de types différents");
											$$.type = BOOL; 
											$2 = Q_EQ; 
											Ht_item* item = new_temp($1.type);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											
											update_offsets(&$1.result,&$3.result);
											gencode(qo,$1.result,$3.result,$2,NULL,-1);
											$$.result = qo;}
			|	expr neq expr			{	if($1.type != $3.type )
												yyerror("Erreur: Comparaison de types différents");
											$$.type = BOOL; 
											$2 = Q_NEQ; 
											Ht_item* item = new_temp($1.type);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											
											update_offsets(&$1.result,&$3.result);
											gencode(qo,$1.result,$3.result,$2,NULL,-1);
											$$.result = qo;}
			| 	literal 				{
											$$.result = $1.qop;
											$$.type = $1.type;
											}
			|	location 				{
											item_table *val = lookup($1);
											if(!val)
												yyerror("Erreur: Variable non déclarée\n");
											$$.result.u.offset = offset(val);
											$$.result.type = QO_ID;
											$$.type = val->item->value;			
											}
			|	'-' expr %prec NEG 		{
											if($2.type != INT)
												yyerror("Erreur: Arithmètique non entière");
											$$.type = INT;
											Ht_item* item = new_temp(INT);
											quadop qo;
											qo.type = QO_TMP;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo, qo, qo, Q_DECL, NULL,-1);
											qo.u.offset = 0;
											update_offset(&$2.result);											
											gencode(qo, $2.result, $2.result, Q_SUB, NULL,-1);
											$$.result = qo;
											}
			|	'!' expr %prec NEG 		{	if($2.type != BOOL)
												yyerror("Erreur: NOT operator with non boolean value");
											$$.type = BOOL; 
											Ht_item* item = new_temp(BOOL);
											quadop qo;
											qo.type = QO_TMP;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,NULL,-1);
											qo.u.offset = 0;
											update_offset(&$2.result);											
											gencode(qo, $2.result, $2.result, Q_NOT, NULL,-1);
											$$.result = qo;
											}
			|	'(' expr ')' 			{
											$$ = $2;
											}
			|	method_call				{ /* verifier que el type de la méthode n'est pas void */	;}

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
