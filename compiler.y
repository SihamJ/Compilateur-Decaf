%{	
	#include "token.h"
	#include "intermediaire.h"
	#include "compiler_functions.h"
	#include "hashtable.h"
	#include "IOfunctions.h"
	#include "text_formating.h"
	#include "compiler.tab.h"
	
	int yylex();
	void yyerror( char* );
	void yywarning(char *);
	char* program_name;
%}

%union {
	long intval;
	char *stringval;
	expr_val expr;
	block b;
	method_call m;
	list next;
	literal l;
	declaration decl;
	location loc;
	param p;  
}

%token <intval> decimal_literal hex_literal char_literal bool_literal eq neq and or not leq geq aff_add aff_sub integer boolean voidtype '+' '-' '%' '/' '<' '>' '=' '!' '*'
%token <stringval> id string_literal ReadInt
%token class If Else For Return Break Continue 

%type <expr> expr return_val E statement block S2 S Max ElseBlock
<next> G
<m> method_call
%type <loc> location
%type <intval> int_literal assign_op type M oprel eq_op add_op mul_op
%type <decl> B glob_id 
%type <p> Param P
%type <l> literal

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

program	:  class id '{' { 	pushctx(CTX_GLOB); 
							program_name = malloc(strlen($2)+1); 
							strcpy(program_name, $2);			/* saving class name in a global variable*/
							add_libs_to_tos(); 					/* Adding the I/O functions to the global symbol table*/
							glob_context = curr_context;		/* glob_context will point to this context throughout the execution of this program.*/
						}

						
				GLOBAL '}' { 	/* verifying that we have a main method*/
								if( ht_search( glob_context,"main") == NULL ) {	yyerror("\nErreur: Pas de méthode main\n"); return 1; }
								popctx();	return 0;
							}

M 		:	%empty 		{	$$ = nextquad; }

G 		:	%empty		{	$$ = crelist(nextquad); 	quadop qo;	 qo.type = QO_EMPTY; 	gencode(qo,qo,qo, Q_GOTO, NULL, -1, NULL); }

Max		:	%empty		{;}

GLOBAL 		: MD		{;}
			| FD MD 	{;}

FD 			: FD field_decl  {;}
			| field_decl	 {;}

field_decl	:	type glob_id ';' 		{ 	/* we push the global variables into the TOS
											returns NULL if successful, otherwise returns an error msg*/
											char *msg; if((msg = field_declare(&$2, $1)) != NULL) { yyerror(msg); return 1;}
										}
														

glob_id			:	id '[' int_literal ']' ',' glob_id	{	if($3 <= 0) { yyerror("\nErreur: Taille du tableau déclarée inférieure ou égale à 0\n"); return 1;}
															$$ = get_declarations($1, NULL, QO_TAB, $3*4);
														}
				|	id '[' int_literal ']'				{	if($3 <= 0) { yyerror("\nErreur: Taille du tableau déclarée inférieure ou égale à 0\n"); return 1;}
															$$ = get_declarations($1, NULL, QO_TAB, $3*4);
														}
				|	id									{	$$ = get_declarations($1, NULL, QO_SCAL, 4); }
				|	id	','	glob_id						{	$$ = get_declarations($1, &$3, QO_SCAL, 4); }

MD			:	MD method_decl		{;}
			|	method_decl			{;}

method_decl	:		type id 	{	/* We add id to the TOS and push a new context for the parameters.
									Returns NULL if successful, otherwise returns an error msg. */
									char *msg;
									if((msg = add_to_tos($1, $2)) != NULL){ yyerror(msg); return 1;}
								}

					'(' P ')' 	{	/* item_table is a structure that has a couple (Ht_item*, HashTable*),
									returned by the function lookup(char *id) */
									item_table* var = lookup($2);

									/* item is an Ht_item, it contains the attribute p for the items of type method where we store
									the types of parameters that the method takes.
									P is either a list of parameters or NULL if the method does not take any parameter */
									
									
									var->item->p = $5;
									}

					block		{	// ( We verify that return exists ) !! UPDATE !! THIS IS NOT AN ERROR
									 int is_returnval=0; 
									 if($8.rtrn == NULL) yywarning("\nWarning: Méthode de type non Void sans Return");  
									 else 	is_returnval = 1;

									// verifying return types
									if(!verify_returns($8.rtrn, $1)) { yyerror("\nErreur: Méthode avec faux type de retour\n"); return 1; }
										
									complete($8.next,nextquad-1); complete($8.rtrn, nextquad-1);
									char* msg; 	if( (msg = end_func($2, curr_context->count, $5, is_returnval)) != NULL) { yyerror(msg); return 1;}
									popctx(); 
								}
													

			/* Same as above, except the type is void. We are forced to separate them because of conflicts*/
			|	voidtype id		{	char *msg; if((msg = add_to_tos($1, $2)) != NULL){ yyerror(msg); return 1; } } 
							
			'(' P ')'  block	{ 	if(!verify_returns($7.rtrn, $1)){ yyerror("\nErreur: Méthode avec faux type de retour\n"); return 1; }
									item_table* var = lookup($2); var->item->p = $5;
									complete($7.next,nextquad-1); complete($7.rtrn, nextquad-1);
									char* msg; if( (msg = end_func($2, curr_context->count, $5, 0)) != NULL) { yyerror(msg); return 1;}
									popctx(); 	}

P 			:	Param 	{ $$ = $1; param p = $1;
							while(p){	
								Ht_item *item = create_item(p->stringval, ID_PARAM, p->type);
								newname(item); p = p->next; }
						}
			|	%empty	{ $$ = NULL;}

/* Param is a list that retrieves the the names of declared parameters and their types */
Param		:	type id ',' Param	{ 	
										/* we verify that we don't declare two parameters with the same ID*/
										if( ht_search(curr_context, $2) != NULL ) { yyerror("\nErreur: Deux paramètres de méthodes de même nom\n"); return 1;}

										/* we store the parameter in the symbol table*/
										$$ = push_param($2, $1, $4);
										// printf("\nPARAM: %s\n", $2);
									}
										
			|	type id				{	/* Idem*/
									 	if( ht_search(curr_context, $2) != NULL ) { yyerror("\nErreur: Deux paramètres de méthodes de même nom\n"); return 1;}
										$$ = push_param($2, $1, NULL);
									//	printf("\nPARAM: %s\n", $2);
									}

block 		:	'{' { pushctx(CTX_BLOCK); } 

			V S2 '}' { 	
						/* we retrieve the adresses of the incomplete GOTOs if they exist*/
						$$ = $4;
						// If we declared any variables in this block, we should pop them from the stack and generate a Q_POP quad
						gen_q_pop(curr_context->count * 4);
						popctx();
					}

S2 			:	S 		{	$$ = $1;}
			|	%empty 	{	$$.next = NULL; 	$$.rtrn = NULL; 	$$.brk = NULL; 	$$.cntu = NULL; }

V 			:	V var_decl	{;}
			|	%empty;

var_decl 	:  	type B ';' {
								/* we push the B list of variables in the TOS
								returns NULL if successful, otherwise returns an error msg */
								char *msg;
								if ( (msg = var_declare(&$2, $1)) != NULL ) { yyerror(msg); return 1; }
							}

B 			:	id ',' B  	{ 	declaration var;
								var.name = malloc((strlen($1)+1));   strcpy(var.name,$1);
								var.suiv = &$3;	 $$ = var; }

			|	id			{	$$.name = malloc((strlen($1)+1));   strcpy($$.name,$1); 
								$$.suiv = NULL; }


type		:	integer {$$=$1;}
			|	boolean {$$=$1;}


S 			: 	S M statement 	{	complete($1.next, $2); 						
									$$.cntu = concat($$.cntu,$3.cntu); 	$$.next = $3.next; 		
									$$.brk = concat($$.brk,$3.brk); 	$$.rtrn = concat($$.rtrn,$3.rtrn); }

			| 	statement		{	$$.brk = $1.brk; 	$$.next = $1.next; 	$$.cntu = $1.cntu; 	$$.rtrn = $1.rtrn; }

statement 	:	location assign_op expr ';' {		
												item_table* val = lookup($1.stringval);

												/* Verifying types. returns NULL if successful, error msg otherwise*/
												char *msg;
												if( (msg = verify_aff_types($1.type, $3.type, $2, val->item)) != NULL) { yyerror(msg); return 1; }

												quadop qo,q1,q2;

												// if location is an element in array, q1 contains the index
												// qo contains the type of location (element in array or not)
												get_location(&qo, &q1, val, $1);

												// bool affectation is different from int
												 if($3.type == BOOL) { bool_affectation(qo, q1, &$$, &$3);}
												
												// in the mips translater, we have to differentiate between | = | += | -= |  
												 else { gencode(qo,$3.result,q1,$2, NULL,-1, NULL);}

												// generates a Q_POP quad	
												int nb = 0; if((nb = pop_tmp())>0) { gen_q_pop(nb*4);}
											}

			/* The method call rules are defined below not here.*/
 			|	method_call	';'				{ 	; }

			|	method_call_by_address ';'	{;}


		|	For id '=' expr ',' expr Max	{  	/* verifying types, declaration of ID, and affectation*/
												char* msg;
												if( (msg = for_declare($2, $4, $6)) != NULL) { yyerror(msg); return 1;}
												$7 = get_max($2, $6);
											} 
				M  							{	gen_test_counter($2, $7); } 
					
				block 						{   complete($11.next, nextquad);	complete($11.cntu, nextquad);
												$$.next = crelist($9); 		  	$$.next = concat($$.next, $11.brk);
												$$.rtrn = $11.rtrn; 		 	gen_increment_and_loopback($2, $9);
												complete($$.next, nextquad);	gen_q_pop(curr_context->count*4);
												$$.next = crelist( nextquad);
												quadop qo; qo.type = QO_EMPTY; gencode(qo, qo, qo, Q_GOTO, NULL, -1, NULL); 
												popctx(); }

			|	Return return_val ';'		{	$$.rtrn = crelist(nextquad); 
												// we return $2.result and we store the return type in qo for later verification of types
												
												quadop qo,q1;	 qo.type = QO_CST;	 qo.u.cst = $2.type; q1.type = QO_EMPTY;
												if($2.type == BOOL) { complete($2.t, nextquad); complete($2.f, nextquad);}
												gencode($2.result, qo, q1, Q_RETURN, NULL, -1, NULL); 
												
												}

			|	Break ';'					{ 	if(!is_a_parent(CTX_FOR)) { yyerror("\nErreur: Break; doit être au sein d'une boucle FOR\n"); return 1; }
												quadop qo;	 	qo.type = QO_EMPTY; 	$$.brk = crelist(nextquad);
												gencode(qo,qo,qo,Q_GOTO,NULL,-1,NULL); }

			|	Continue ';'				{  if(!is_a_parent(CTX_FOR)) { yyerror("\nErreur: Continue; doit être au sein d'une boucle FOR\n");return 1;}
												quadop qo; 		qo.type = QO_EMPTY;		 $$.cntu = crelist(nextquad);  
												gencode(qo,qo,qo,Q_GOTO,NULL,-1,NULL); }

			|	block						{ 	$$.brk = $1.brk;	 $$.next = $1.next; 	$$.cntu = $1.cntu; 		$$.rtrn = $1.rtrn;}

			|	If '(' expr ')' 	      	{ 	if($3.type != BOOL) { yyerror("\nErreur: Test avec expression non booléene\n"); return 1; } 
												int nb = 0; if((nb = pop_tmp()) > 0) {
													quadop qo; qo.type = QO_EMPTY;

													complete($3.t, nextquad); gen_q_pop(nb*4); 
													$3.t = crelist(nextquad); gencode(qo, qo, qo, Q_GOTO, NULL, -1, NULL);
													
													complete($3.f, nextquad); gen_q_pop(nb*4); 
													$3.f = crelist(nextquad); gencode(qo, qo, qo, Q_GOTO, NULL, -1, NULL);
												} 
											} 
			
				M ElseBlock					{ 	complete($3.t, $6); $$.cntu = $7.cntu; $$.brk = $7.brk; $$.rtrn = $7.rtrn;
												if(!$7.isElseBlock) {
													$$.next = concat($3.f, $7.next); 
												}
												else {
													complete($3.f, $7.isElseBlock); $$.next = concat($7.elseGoto, $7.next);
												}
											}

ElseBlock	:	block 						{	$$.elseGoto = NULL; $$.isElseBlock = 0; $$.next = $1.next; 
												$$.cntu = $1.cntu; $$.brk = $1.brk; $$.rtrn = $1.rtrn; }

			|	block G Else M block		{	$$.isElseBlock = $4; $$.elseGoto = $2;	
												$$.next = concat($1.next, $5.next); $$.rtrn = concat($1.rtrn, $5.rtrn);
												$$.cntu = concat($1.cntu, $5.cntu); $$.brk = concat($1.cntu, $5.cntu);
											}

return_val	:	expr 						{ 	$$ = $1; }
			|	%empty						{ 	$$.type = VOIDTYPE; $$.result.type = QO_EMPTY; }

method_call_by_address	:	ReadInt '(' location ')' { 
														quadop qo,q1,q2; qo.type = QO_CSTSTR;printf("here\n"); qo.u.string_literal.label = malloc(strlen($1)+1); 
														strcpy(qo.u.string_literal.label, $1);
														q1.type = QO_ID; item_table* val = lookup($3.stringval); q1.u.offset = offset(val);
														q2.type = QO_EMPTY;
														gencode(qo, q1, q2, Q_METHODCALL, NULL, -1, NULL);
													}

											

method_call :	id '(' E ')' 				{	char *msg;
												 if((msg = verify_and_get_type_call($1, $3.p, &$$)) != NULL) { yyerror(msg); return 1; }
												param p = $3.p;
												if(!strcmp($1,"WriteString")) {
													get_write_string_args($3.p->arg.u.string_literal.label, $3.p->arg.u.string_literal.value); }

												gen_method_call($1, &$3, &$$); }

			|	id '(' ')'					{	char *msg;
												if((msg = verify_and_get_type_call($1, NULL, &$$)) != NULL) { yyerror(msg); return 1; }

												gen_method_call($1, NULL, &$$); }

//	 E is a list of parameters of a method call 
E 			:	expr ',' E 						{ 
													param p = (param) malloc(sizeof(struct param));

													 if($1.result.type == QO_ID || $1.result.type == QO_TMP){
														item_table* val = lookup($1.stringval); 
														$1.result.u.offset = offset(val);
														p->stringval = malloc(strlen($1.stringval)+1);
														strcpy(p->stringval, $1.stringval);
													}
													
													p->type = $1.type; p->arg = $1.result; p->next = $3.p; 	
												 	if($1.type == BOOL) { p->t = $1.t; p->f = $1.f;}
													$$.p = p;
													
												}
			|	expr 							{
													$$.p = (param) malloc(sizeof(struct param));
													 if($1.result.type == QO_ID || $1.result.type == QO_TMP){
														item_table* val = lookup($1.stringval); 
														$1.result.u.offset = offset(val);
														$$.p->stringval = malloc(strlen($1.stringval)+1);
														strcpy($$.p->stringval, $1.stringval);
													}
													
													$$.p->type = $1.type;	$$.p->arg = $1.result; 	$$.p->next = NULL;
													if($1.type == BOOL) { $$.p->t = $1.t; $$.p->f = $1.f; }	
													
												}

location	:	id					{	$$.type = ID_VAR; $$.stringval = malloc(strlen($1)+1); strcpy($$.stringval, $1); }

			|	id '[' expr ']'		{ 	if($3.type != INT) {yyerror("\nErreur: L'ndice d'accès à un tableau doit être de type INT\n"); return 1;}
										$$.type = ID_TAB; $$.stringval = malloc(strlen($1)+1);
										strcpy($$.stringval, $1); $$.index = $3.result; 

										 if($3.result.type == QO_TMP) {
											item_table *val = lookup($3.stringval);
											$3.result.u.offset = offset(val); 
											quadop qo, q1; qo.type = QO_CST; qo.u.cst = 4;
											gencode($3.result, $3.result, qo, Q_MUL, NULL, -1, NULL);
											$$.index_name = malloc(strlen($3.stringval)+1);	strcpy($$.index_name, $3.stringval); 
											$$.index = $3.result;
										}
										else if($3.result.type == QO_ID){
											quadop qo;	qo.type = QO_TMP;	qo.u.offset = 0;	Ht_item *item = new_temp(INT);
											quadop q1;	q1.type = QO_EMPTY;
											gencode(qo, q1, q1, Q_DECL, NULL, -1, NULL);
											item_table *val = lookup($3.stringval);
											$3.result.u.offset = offset(val);
											gencode(qo, $3.result, q1, Q_AFF, NULL, -1, NULL);
											q1.type = QO_CST; q1.u.cst = 4;
											gencode(qo, qo, q1, Q_MUL, NULL, -1, NULL);
											$$.index_name = malloc(strlen(item->key)+1);	strcpy($$.index_name, item->key); 
											$$.index = qo;
										}
										else if($3.result.type == QO_CST){
											$$.index.u.cst = 4*$3.result.u.cst;
										}
										
										// TO DO : Accès tableau avec offset variable globale
										
									}


expr		:	expr add_op expr %prec '+'	{	if($1.type != INT || $3.type != INT){ yyerror("\nErreur: Arithmètique non entière"); return 1; }
												$$.type = INT; 	Ht_item* item = new_temp(INT); 	quadop qo; 	qo.type = QO_TMP; 	qo.u.offset = 0;

												$$.stringval = malloc(strlen(item->key)+1); 	strcpy($$.stringval, item->key);

												if($1.result.type == QO_ID || $1.result.type == QO_TMP) {
													item_table* val; val = lookup($1.stringval); $1.result.u.offset = offset(val); }
												if($3.result.type == QO_ID || $3.result.type == QO_TMP) {
													item_table* val; val = lookup($3.stringval); $3.result.u.offset = offset(val); }

												gencode(qo,qo,qo,Q_DECL,NULL,-1, NULL);
												gencode(qo,$1.result,$3.result,$2,NULL,-1, NULL); $$.result = qo;
											}

			|	expr mul_op expr %prec '*'	{	if($1.type != INT || $3.type != INT){ yyerror("\nErreur: Arithmètique non entière"); return 1; }
												$$.type = INT;	 Ht_item* item = new_temp(INT); 	quadop qo; 	qo.type = QO_TMP; 	qo.u.offset = 0;

												$$.stringval = malloc(strlen(item->key)+1); 	strcpy($$.stringval, item->key);

												if($1.result.type == QO_ID || $1.result.type == QO_TMP) {
													item_table* val; val = lookup($1.stringval); $1.result.u.offset = offset(val); 	}
												if($3.result.type == QO_ID || $3.result.type == QO_TMP) {
													item_table* val; val = lookup($3.stringval); $3.result.u.offset = offset(val); }

												gencode(qo,qo,qo,Q_DECL,NULL,-1, NULL);
												gencode(qo,$1.result,$3.result,$2,NULL,-1, NULL); 	$$.result = qo;
											}

			|	expr and M expr				{	if($1.type != BOOL || $4.type != BOOL){ yyerror("\nErreur: AND operator with non boolean value"); return 1; }			
												$$.type = BOOL; 	complete($1.t, $3); 	$$.f = concat($1.f, $4.f); 	$$.t = $4.t; }

			|	expr or M expr				{	if($1.type != BOOL || $4.type != BOOL){ yyerror("\nErreur: OR operator with non boolean value"); return 1; }
												$$.type = BOOL; 	complete($1.f, $3); 	$$.t = concat($1.t, $4.t); 	$$.f = $4.f; }

			|	expr oprel expr	%prec '<' 	{	if($1.type != INT || $3.type != INT){ yyerror("\nErreur: REL OP non entière"); return 1; }
												$$.type = BOOL;
												quadop qo; 	qo.type = QO_EMPTY; 	qo.u.cst = 0;

												if($1.result.type == QO_ID || $1.result.type == QO_TMP) {
													item_table* val = lookup($1.stringval);
													$1.result.u.offset = offset(val); }
												if($3.result.type == QO_ID || $3.result.type == QO_TMP) {
													item_table* val = lookup($3.stringval);
													$3.result.u.offset = offset(val); }

												$$.t = crelist(nextquad); 	gencode(qo, $1.result, $3.result, $2, NULL, -1, NULL);
												$$.f = crelist(nextquad); 	gencode(qo, qo, qo, Q_GOTO, NULL, -1, NULL);
											}

			|	expr eq_op expr	%prec eq	{	
												if($1.type != $3.type ){ yyerror("\nErreur: Comparaison de types différents"); return 1; }
												$$.type = BOOL; 	quadop qo; 	qo.type = QO_EMPTY; 	qo.u.cst = 0;

												if($1.type == BOOL){
													complete($1.t, nextquad); 	complete($1.f, nextquad); 
													complete($3.t, nextquad); 	complete($3.f, nextquad);
												}

												if($1.result.type == QO_ID || $1.result.type == QO_TMP) {
													item_table* val = lookup($1.stringval);
													$1.result.u.offset = offset(val); }
												if($3.result.type == QO_ID || $3.result.type == QO_TMP) {
													item_table* val = lookup($3.stringval);
													$3.result.u.offset = offset(val); }

												$$.t = crelist(nextquad); 	gencode(qo,$1.result,$3.result,$2,NULL, -1, NULL);
												$$.f = crelist(nextquad); 	gencode(qo, qo, qo, Q_GOTO, NULL, -1, NULL);

											}

			|	'-' expr %prec NEG 			{	if($2.type != INT){ yyerror("\nErreur: Arithmètique non entière"); return 1; }
												$$.type = INT; 	Ht_item* item = new_temp(INT); 	quadop qo;	 qo.type = QO_TMP;	 qo.u.offset = 0;
												gencode(qo, qo, qo, Q_DECL, NULL,-1, NULL);
												$$.stringval = malloc(strlen(item->key)+1); 	strcpy($$.stringval, item->key);

												if($2.result.type == QO_ID || $2.result.type == QO_TMP) {
													item_table* val; val = lookup($2.stringval); $2.result.u.offset = offset(val); }

												quadop q1; 	q1.type = QO_CST; 	q1.u.cst = 0;
												gencode(qo, q1, $2.result, Q_SUB, NULL,-1, NULL); 	$$.result = qo;
											}

			|	'!' expr %prec NEG 			{	if($2.type != BOOL){ yyerror("\nErreur: NOT operator with non boolean value"); return 1; }
												$$.type = BOOL; 	$$.t = $2.f; 	$$.f = $2.t; }

			|	'(' expr ')' 				{	$$ = $2; }

			|	method_call					{ 	if($1.return_type == VOIDTYPE){ yyerror("\nErreur: méthode de type de retour void utilisée comme expression\n"); return 1;}
												$$.result = $1.result; 		$$.type = $1.return_type; 
												$$.stringval = malloc(strlen($1.result_id)+1); 	strcpy($$.stringval, $1.result_id);}		
						
			|	location 					{	item_table *val = lookup($1.stringval);
												if(val == NULL){ yyerror("\nErreur: Variable non déclarée\n"); return 1; }

												if(val->table == glob_context) {

													if($1.type == ID_VAR && val->item->id_type == ID_TAB) {
														yyerror("\nErreur: Accès tableau comme scalaire\n"); return 1; }
													if($1.type == ID_TAB && val->item->id_type == ID_VAR) {
														yyerror("\nErreur: Scalaire utilisé comme tableau\n"); return 1;}

													if($1.type == ID_VAR) {
														quadop q1; 	q1.type = QO_GLOBAL; 	q1.u.global.type = QO_SCAL;
														q1.u.global.name = malloc(strlen($1.stringval)+1);
														strcpy(q1.u.global.name, $1.stringval);	 q1.u.global.size = 4;

														quadop qo; qo.type = QO_TMP; qo.u.offset = 0; 
														Ht_item* item = new_temp(val->item->value);
														gencode(qo, qo, qo, Q_DECL, NULL, -1, NULL); 
														gencode(qo, q1, q1, Q_AFF, NULL, -1, NULL);
														$$.stringval = malloc(strlen(item->key)+1);
														strcpy($$.stringval, item->key);
														$$.type = val->item->value;
														$$.result = qo;

													}

													else if($1.type == ID_TAB) {
														$$.type = val->item->value;
														quadop qo; 		qo.type = QO_GLOBAL; 	qo.u.global.type = QO_TAB;
														qo.u.global.name = malloc(strlen($1.stringval)+1);
														strcpy(qo.u.global.name, $1.stringval); 	qo.u.global.size = val->item->size;

														Ht_item* item = new_temp(INT);
														quadop q1; 		q1.type = QO_TMP; 		q1.u.offset = 0;
														gencode(q1, q1, q1, Q_DECL, NULL, -1, NULL);

														 if($1.index.type == QO_ID || $1.index.type == QO_TMP){
															item_table *val = lookup($1.index_name);
															$1.index.u.offset = offset(val);}
														gencode(q1, qo, $1.index, Q_ACCESTAB, NULL, -1, NULL); 	$$.result = q1; 
														$$.stringval = malloc(strlen(item->key)+1); strcpy($$.stringval, item->key);}
												}
												else {	// location can only be an ID if we are not in a global context
														quadop qo; 	qo.type = QO_ID; 	qo.u.offset = offset(val); 	$$.result = qo;
														$$.type = val->item->value; 	$$.stringval = malloc(strlen($1.stringval)+1);
														strcpy($$.stringval, $1.stringval); 
														if($$.type == BOOL){
															quadop q1, q2; 	q1.type = QO_CST; 	q1.u.cst = 1; 	q2.type = QO_EMPTY;
															$$.t = crelist(nextquad); 	gencode(q2, qo, q1, Q_EQ, NULL, -1, NULL);
															$$.f = crelist(nextquad); 	gencode(q2, q2, q2, Q_GOTO, NULL, -1, NULL);
														}
														
														}
											}

			|	string_literal				{	$$.type = STRING; 	$$.result.type = QO_CSTSTR;
												$$.result.u.string_literal.label = new_str();
												$$.result.u.string_literal.value = malloc(strlen($1)+1);
												strcpy($$.result.u.string_literal.value, $1);
												$$.result.u.string_literal.value[strlen($1)] = '\0';
												}

			| 	literal 					{	$$.result.type = QO_CST;	$$.type = $1.type;	$$.result.u.cst = $1.intval; 

												if($1.type == BOOL){
													Ht_item* item = new_temp(BOOL);
													quadop q1; 	q1.type = QO_TMP; 	q1.u.offset = 0;
													gencode(q1,q1,q1,Q_DECL,NULL,-1, NULL);

													quadop qo,q2; qo.type = QO_EMPTY; 	q2.type = QO_CST; 	q2.u.cst = $1.intval;	
													gencode(q1, q2, q2, Q_AFF, NULL, -1, NULL);
													q2.u.cst = 1;
													$$.t = crelist(nextquad); 	gencode(qo, q1, q2, Q_EQ, NULL, -1, NULL);
													$$.f = crelist(nextquad); 	gencode(qo, qo, qo, Q_GOTO, NULL, -1, NULL);
												}														
											}
											


literal		:	int_literal					{	$$.intval = $1; $$.type = INT;	}

			|	char_literal				{	$$.intval = $1;	$$.type = INT;	}

			|	bool_literal				{	$$.intval= $1;	$$.type = BOOL;	}

int_literal	: 	decimal_literal {$$ = $1;}
			| 	hex_literal		{$$ = $1;}

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

void yyerror(char *msg) {
	fprintf(stderr, "\t%s%s\n",RED, msg);
	set_color(NORMAL);
}

void yywarning(char *msg) {
	fprintf(stderr, "\t%s%s\n%s",YELLOW, msg, NORMAL);
	set_color(NORMAL);
}