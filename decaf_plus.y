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
		int size;
	} decl;
}

%token <intval> decimal_literal hex_literal char_literal eq neq and or not leq geq aff_add aff_sub integer boolean voidtype '+' '-' '%' '/' '<' '>' '=' '!' '*'
%token <boolval> bool_literal
%token <stringval> id 
%token class Program If Else For Return Continue Break comment

%type <exprval> expr S G block statement
%type <intval> int_literal assign_op type method_call M oprel eq_op add_op mul_op
%type <literal> literal
%type <decl> B Tab
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

program	:  class Program '{' { pushctx(); glob_context = curr_context; } 
						
							GLOBAL '}' { if( ht_search( glob_context,"main") == NULL )
											yyerror("\nErreur: Pas de méthode main\n");
										 popctx() ;}

M 		:	%empty 		{	$$ = nextquad;}

G 		:	%empty		{	$$.next = crelist(nextquad);
							quadop qo;
							qo.type = QO_EMPTY;
							gencode(qo,qo,qo, Q_GOTO, global_code[nextquad].label, -1);
						}

GLOBAL 		: MD		{;}
			| FD 		{;}
			| FD MD 	{;}
			| %empty	{;}


FD 			: FD field_decl  {;}
			| field_decl	 {;}

field_decl	:	var_decl	{;}
			|	tab_decl	{;}

tab_decl	:	type Tab ';' 							{ 
															quadop qo,q1;
															struct decl *pt = &$2;
															qo.type = QO_GLOBAL;
															
															while(pt != NULL){
																if( ht_search(glob_context, pt->name) != NULL ) 
																	yyerror("\nErreur: Tableau déclarée deux fois\n");

																qo.u.global.name = malloc(strlen(pt->name + 1));
																strcpy(qo.u.global.name, pt->name);
																qo.u.global.size = pt->size;
																gencode(qo, qo, qo, Q_DECL, global_code[nextquad].label, -1);
																glob_dec_count++;
																Ht_item *item = create_item(pt->name, ID_TAB, $1);
																item->size = pt->size;
																newname(item);
																pt = pt->suiv;
															}
														}

Tab			:	id '[' int_literal ']' ',' Tab			{	if($3 <= 0) yyerror("\nErreur: Taille du tableau déclarée inférieure ou égale à 0\n");
															struct decl var;
															var.name = malloc((strlen($1)+1)); 
															strcpy(var.name,$1);
															var.size = $3;
															var.suiv = &$6;
															$$ = var;
														}
			|	id '[' int_literal ']'					{
															struct decl *d;

															if($3 <= 0) yyerror("\nErreur: Taille du tableau déclarée inférieure ou égale à 0\n");															
															struct decl var;
															var.name = malloc((strlen($1)+1)); 
															strcpy(var.name,$1);
															var.size = $3;
															var.suiv = NULL;
															$$ = var;
														}

MD			:	MD method_decl		{;}
			|	method_decl			{;}

method_decl	:		type id {	if(ht_search(glob_context, $2) != NULL)
									yyerror("\nErreur: Méthode déjà déclarée avec ce nom\n");
								if(!strcmp($2,"main"))
									yyerror("\nErreur: La méthode main doit être de type void\n");
								Ht_item *item = create_item($2, ID_METHOD, $1);
								newname(item);
								pushctx(); } 

								'(' Param ')'  block	{ popctx(); }

			|	voidtype id {	if(ht_search(glob_context, $2) != NULL)
									yyerror("\nErreur: Méthode déjà déclarée avec ce nom\n");
								Ht_item *item = create_item($2, ID_METHOD, $1);
								newname(item);
								pushctx(); } 
							
								'(' Param ')'  block	{ popctx(); }

Param		:	Param ',' type id 	{ 	if( ht_search(curr_context, $4) != NULL ) 
											yyerror("\nErreur: Deux paramètres de méthodes de même nom\n");	
											Ht_item *item = create_item($4, ID_PARAM, $3);
											newname(item);
										}
			|	type id				{ 	if( ht_search(curr_context, $2) != NULL ) 
											yyerror("\nErreur: Deux paramètres de méthodes de même nom\n");	
											Ht_item *item = create_item($2, ID_PARAM, $1);
											newname(item);}
			|	%empty				{ ;}


block 		:	'{' { pushctx(); } V S '}' {  popctx()  ;}

 V 			:	V var_decl 	{;}
			|	%empty		{;}

var_decl 	:  	type B ';' {
								if(curr_context == glob_context){
									quadop qo;
									struct decl *pt = &$2;
									qo.type = QO_GLOBAL;
									
									while(pt != NULL){
										if( ht_search(glob_context, pt->name) != NULL ) 
											yyerror("\nErreur: Variable déclarée deux fois\n");

										qo.u.global.name = malloc(strlen(pt->name + 1));
										strcpy(qo.u.global.name, pt->name);
										qo.u.global.size = 4;
										gencode(qo, qo, qo, Q_DECL, global_code[nextquad].label, -1);
										glob_dec_count++;
										Ht_item *item = create_item(pt->name, ID_VAR, $1);
										newname(item);
										pt = pt->suiv;
									}
								}
								else {
									quadop qo;
									struct decl *pt = &$2;
									qo.type = QO_ID;
									
									while(pt != NULL){
										if( ht_search(curr_context, pt->name) != NULL ) 
											yyerror("\nErreur: Variable déclarée deux fois\n");
										qo.u.offset = 0;
										gencode(qo, qo, qo, Q_DECL, global_code[nextquad].label, -1);
										Ht_item *item = create_item(pt->name, ID_VAR, $1);
										newname(item);
										pt = pt->suiv;
									}
								}
							}

B 			:	id ',' B  	{ 	struct decl var;
								var.name = malloc((strlen($1)+1)); 
								strcpy(var.name,$1);
								var.suiv = &$3;
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
														yyerror("\nErreur: Variable non déclarée\n");
													if(val->item->value != $3.type)
														yyerror("\nErreur: Type de valeur incorrecte\n");
													if (($2 == Q_AFFADD || $2 == Q_AFFSUB) && (val->item->value == BOOL || $3.type == BOOL))
														yyerror("\nErreur: Type de valeur incorrecte\n");
												
													quadop q1;
													if (val->table == glob_context) {
														q1.type = QO_GLOBAL;
														q1.u.global.name = malloc(strlen(val->item->key)+1);
														strcpy(q1.u.global.name, val->item->key);
														q1.u.global.size = 4;
													}
													else {
														q1.u.offset = offset(val);
														q1.type = QO_ID;
													}

													if($3.type == BOOL) {

														quadop qo;
														qo.type = QO_CST;
														qo.u.cst = true;
														complete($3.t, nextquad);
														gencode(q1, qo, qo, Q_AFF, global_code[nextquad].label, -1); 	
														qo.type = QO_EMPTY;
														gencode(qo, qo, qo, Q_GOTO, global_code[nextquad].label, nextquad+2);

														qo.type = QO_CST;	
														qo.u.cst = false;
														char *label = new_label();		
														complete($3.f, nextquad);											
														gencode(q1, qo, qo, Q_AFF, global_code[nextquad].label, -1);					

														qo.type = QO_EMPTY;
														gencode(qo, qo, qo, Q_LABEL, new_label(), -1);
													}
													else {
													if($2 == Q_AFF)
														gencode(q1,$3.result,$3.result,$2,global_code[nextquad].label,-1);

													else if($2 == Q_AFFADD)
														gencode(q1,q1,$3.result,Q_ADD,global_code[nextquad].label,-1);

													else if($2 == Q_AFFSUB)
														gencode(q1,q1,$3.result,Q_SUB,global_code[nextquad].label,-1);
													}
													pop_tmp();
												}

			|	method_call	';'								{;}

			|	If '(' expr ')' M block G Else M block	{	
															if($3.type != BOOL) yyerror("Erreur: Test avec expression non booléene\n");
															complete($3.t, $5);
															complete($3.f, $9);
															$$.next = concat($6.next, $7.next);
															$$.next = concat($$.next,$10.next);
														}
			|	If '(' expr ')' M block 				{
															if($3.type != BOOL) yyerror("Erreur: Test avec expression non booléene\n");
															complete($3.t, $5);
															$$.next = concat($3.f, $6.next);
														}
												
			|	For id '=' expr ',' expr block	{   /* compteur de boucle for déclaré implicitement et expr1 de type int*/ 	
													if($4.type != INT)
														yyerror("Erreur: le compteur de boucle doit être de type INT\n");
													
													
												}
			|	Return return_val ';'			{;}
			|	Break ';'						{;}
			|	Continue ';'					{;}
			|	block							{;}


return_val	:	expr 							{;}
			|	%empty							{;}


method_call :	id '(' E ')' 					{
													item_table *val = lookup($1);
													if(val == NULL)
														yyerror("Erreur: Méthode non déclarée\n");
													$$ = val->item->value;
													/* Vérifier les paramètres E */
												;}

E 			:	E ',' expr 		{;}
			|	expr 			{;}
			| 	%empty			{;}

location	:	id	{;}
			|	id '[' expr ']'			{ /* expr de type int */ 
											if($3.result.type != INT)
												yyerror("Erreur: indice de tableau doit être de type INT\n");
										}

expr		:	expr add_op expr %prec '+'	{
											if($1.type != INT || $3.type != INT)
												yyerror("\nErreur: Arithmètique non entière");
											$$.type = INT; 
											Ht_item* item = new_temp(INT);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,global_code[nextquad].label,-1);
											qo.u.offset = 0;

											if($1.result.type == QO_ID || $1.result.type == QO_TMP)
												$1.result.u.offset += 4;
											if($3.result.type == QO_ID || $3.result.type == QO_TMP)
												$3.result.u.offset += 4;

											gencode(qo,$1.result,$3.result,$2,global_code[nextquad].label,-1);
											$$.result = qo;}
			|	expr mul_op expr %prec '*'	{	if($1.type != INT || $3.type != INT)
												yyerror("\nErreur: Arithmètique non entière");
											$$.type = INT; 
											Ht_item* item = new_temp(INT);
											quadop qo;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo,qo,qo,Q_DECL,global_code[nextquad].label,-1);
											qo.u.offset = 0;

											if($1.result.type == QO_ID || $1.result.type == QO_TMP)
												$1.result.u.offset += 4;
											if($3.result.type == QO_ID || $3.result.type == QO_TMP)
												$3.result.u.offset += 4;

											gencode(qo,$1.result,$3.result,$2,global_code[nextquad].label,-1);
											$$.result = qo;}
			|	expr and M expr			{	if($1.type != BOOL || $4.type != BOOL)
												yyerror("\nErreur: AND operator with non boolean value");
											$$.type = BOOL;
											complete($1.t, $3);
											$$.f = concat($1.f, $4.f);
											$$.t = $4.t; 
											}

			|	expr or M expr			{	if($1.type != BOOL || $4.type != BOOL)
												yyerror("\nErreur: OR operator with non boolean value");
											$$.type = BOOL;
											complete($1.f, $3);
											$$.t = concat($1.t, $4.t);
											$$.f = $4.f;
											}
			|	expr oprel expr	%prec '<' {
											if($1.type != INT || $3.type != INT)
												yyerror("\nErreur: REL OP non entière");
											$$.type = BOOL;
											$$.t = crelist(nextquad);
											quadop qo;
											qo.type = QO_EMPTY;
											qo.u.cst = 0;
											gencode(qo, $1.result, $3.result, $2, global_code[nextquad].label, -1);
											$$.f = crelist(nextquad);
											gencode(qo, qo, qo, Q_GOTO, global_code[nextquad].label, -1);
											}
			|	expr eq_op expr	%prec eq	{	if($1.type != $3.type )
												yyerror("\nErreur: Comparaison de types différents");
											$$.type = BOOL; 
											quadop qo;
											qo.type = QO_EMPTY;
											qo.u.cst = 0;											
											$$.t = crelist(nextquad);
											gencode(qo,$1.result,$3.result,$2,global_code[nextquad].label, -1);
											$$.f = crelist(nextquad);
											gencode(qo, qo, qo, Q_GOTO, global_code[nextquad].label, -1);
										}
			| 	literal 				{
											$$.result = $1.qop;
											$$.type = $1.type;
											}
			|	location 				{
											item_table *val = lookup($1);
											if(!val)
												yyerror("\nErreur: Variable non déclarée\n");
											if(val->table == glob_context){
												$$.result.type = QO_GLOBAL;
												$$.result.u.global.name = malloc(strlen($1)+1);
												strcpy($$.result.u.global.name, $1);
												$$.result.u.global.size = 4;
												$$.type = val->item->value;
											}
											else {
												$$.result.u.offset = offset(val);
												$$.result.type = QO_ID;
												$$.type = val->item->value;			
											}
										}
			|	'-' expr %prec NEG 		{
											if($2.type != INT)
												yyerror("\nErreur: Arithmètique non entière");
											$$.type = INT;
											Ht_item* item = new_temp(INT);
											quadop qo;
											qo.type = QO_TMP;
											qo.type = QO_TMP;
											qo.u.offset = -4;
											gencode(qo, qo, qo, Q_DECL, global_code[nextquad].label,-1);
											qo.u.offset = 0;
											if($2.result.type == QO_ID || $2.result.type == QO_TMP)
												$2.result.u.offset +=4 ;											
											gencode(qo, $2.result, $2.result, Q_SUB, global_code[nextquad].label,-1);
											$$.result = qo;
											}
			|	'!' expr %prec NEG 		{	if($2.type != BOOL)
												yyerror("\nErreur: NOT operator with non boolean value");
											$$.type = BOOL; 
											$$.t = $2.f;
											$$.f = $2.t;
											}
			|	'(' expr ')' 			{
											$$ = $2;
											}
			|	method_call				{ 
											if($1 == VOIDTYPE)
												yyerror("Erreur: méthode de type de retour void utilisée comme expression\n");
											/* gencode*/
										;}

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

oprel		:	'<'		{$$ = Q_LT;}
			|	'>'		{$$ = Q_GT;}
			|	geq 	{$$ = Q_GEQ;}
			|	leq 	{$$ = Q_LEQ;}

eq_op		:	eq		{$$ = Q_EQ;}
			|	neq		{$$ = Q_NEQ;}

add_op		:	'+'		{$$ = Q_ADD;}
			|	'-'		{$$ = Q_SUB;}
mul_op		:	'*'		{$$ = Q_MUL;}
			|	'/'		{$$ = Q_DIV;}
			|	'%'		{$$ = Q_MOD;}
%%

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
	exit(0);
}
