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

	param p;  // List for storing method parameters at the declaration or method call. Also pointed at by the Symbol Table (declaration), and the Quads (method_call).

}

%token <intval> decimal_literal hex_literal char_literal bool_literal eq neq and or not leq geq aff_add aff_sub integer boolean voidtype '+' '-' '%' '/' '<' '>' '=' '!' '*'
%token <stringval> id string_literal 
%token class If Else For Return Break Continue 

%type <expr> expr return_val E statement block S2 S Max
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

program	:  class id '{' { 	
								pushctx(CTX_GLOB); /* Pushing the global context*/
								program_name = malloc(strlen($2)+1); /* saving class name in a global variable*/
								strcpy(program_name, $2);
								add_libs_to_tos(); /* Adding the I/O functions to the global symbol table*/
								glob_context = curr_context; /* glob_context will point to this context throughout the execution of this program.*/
							}

						
				GLOBAL '}' { 
								if( ht_search( glob_context,"main") == NULL ){	/* verifying that we have a main method*/
									yyerror("\nErreur: Pas de méthode main\n");
									return 1;
								}

								quadop qo;
								popctx(); 
								return 0;
							}

M 		:	%empty 		{	$$ = nextquad; }

G 		:	%empty		{	$$ = crelist(nextquad);
							quadop qo;
							qo.type = QO_EMPTY;
							qo.u.cst = 0;
							gencode(qo,qo,qo, Q_GOTO, NULL, -1, NULL);
						}

Max		:	%empty		{;}

GLOBAL 		: MD		{;}
			| FD MD 	{;}

FD 			: FD field_decl  {;}
			| field_decl	 {;}

field_decl	:	type glob_id ';' 		{ 
											/* we push the global variables into the TOS
											returns NULL if successful, otherwise returns an error msg*/
											char *msg;
											if((msg = field_declare(&$2, $1)) != NULL) { yyerror(msg); return 1;}
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

method_decl	:		type id 	{	
								/* We add id to the TOS and push a new context for the parameters.
									Returns NULL if successful, otherwise returns an error msg. */
								char *msg;
								if((msg = add_to_tos($1, $2)) != NULL){
									yyerror(msg); return 1;}
								}

					'(' P ')' 	{	
									/* item_table is a structure that has a couple (Ht_item*, HashTable*),
									returned by the function lookup(char *id) */
									item_table* var = lookup($2);

									/* item is an Ht_item, it contains the attribute p for the items of type method where we store
									the types of parameters that the method takes.
									P is either a list of parameters or NULL if the method does not take any parameter */
									var->item->p = $5;}

					block		{
									// We verify that return exists
									if($8.rtrn == NULL) { yyerror("\nErreur: Méthode sans return\n"); return 1; }

									// verifying return types
									if(!verify_returns($8.rtrn, $1)) { yyerror("\nErreur: Méthode avec faux type de retour\n"); return 1; }

									complete($8.next,nextquad);	complete($8.rtrn, nextquad);

									char *msg; 
									if( (msg = end_func($2, curr_context->count, $5)) != NULL){ yyerror(msg); return 1; }	

									popctx(); 
								}
													

			/* Same as above, except the type is void. We are forced to separate them because of conflicts*/
			|	voidtype id		{
									/* We add id to the TOS and push a new context for the parameters
										Returns NULL if successful, otherwise returns an error msg.*/
									char *msg;
									if((msg = add_to_tos($1, $2)) != NULL){ yyerror(msg); return 1; }
								} 
							
			'(' P ')'  block	{ 		
									if(!verify_returns($7.rtrn, $1)){ yyerror("\nErreur: Méthode avec faux type de retour\n"); return 1; }

									item_table* var = lookup($2);
									var->item->p = $5;

									complete($7.next, nextquad); complete($7.rtrn, nextquad);	

									// next quad is here in END_FUNCTION. we also verify that P is NULL for main
									char *msg;
									if( (msg = end_func($2, curr_context->count, $5)) != NULL) { yyerror(msg); return 1; }	

									popctx();
								}

P 			:	Param 	{ $$ = $1;}
			|	%empty	{ $$ = NULL;}

/* Param is a list that retrieves the the names of declared parameters and their types */
Param		:	type id ',' Param	{ 	
										/* we verify that we don't declare two parameters with the same ID*/
										if( ht_search(curr_context, $2) != NULL ) { yyerror("\nErreur: Deux paramètres de méthodes de même nom\n"); return 1;}

										/* we store the parameter in the symbol table*/
										$$ = push_param($2, $1, $4);
									}
										
			|	type id				{	/* Idem*/
									 	if( ht_search(curr_context, $2) != NULL ) { yyerror("\nErreur: Deux paramètres de méthodes de même nom\n"); return 1;}
										$$ = push_param($2, $1, NULL);
									}

block 		:	'{' {  pushctx(CTX_BLOCK); } 

			V S2 '}' { 	
						/* we retrieve the adresses of the incomplete GOTOs if they exist*/
						$$ = $4;
						// If we declared any variables in this block, we should pop them from the stack and generate a Q_POP quad
						if(curr_context->count > 0) gen_q_pop(curr_context->count * 4);
						popctx();
					}

S2 			:	S 		{	$$ = $1;}
			|	%empty 	{	; }

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
								var.suiv = &$3; $$ = var; }
			|	id			{	$$.name = malloc((strlen($1)+1));   strcpy($$.name,$1); 
								$$.suiv = NULL; }


type		:	integer {$$=$1;}
			|	boolean {$$=$1;}


S 			: 	S M statement 	{	 complete($1.next, $2); 						
									$$.cntu = concat($$.cntu,$3.cntu); 	$$.next = $3.next; 		
									$$.brk = concat($$.brk,$3.brk); 	$$.rtrn = concat($$.rtrn,$3.rtrn); }

			| 	statement		{	$$.brk = $1.brk; $$.next = $1.next; $$.cntu = $1.cntu; $$.rtrn = $1.rtrn; }

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
												 if($3.type == BOOL) { bool_affectation(qo, q1, $$, $3);}
												
												// in the mips translater, we have to differentiate between | = | += | -= |  
												 else { gencode(qo,$3.result,q1,$2, NULL,-1, NULL);}

												// generates a Q_POP quad	
												if(tmpCount>0) { gen_q_pop(tmpCount*4);}
												pop_tmp();
											}

			/* The method call rules are defined below not here.*/
 			|	method_call	';'				{ 	; }

			|	If '(' expr ')' M block 
				G Else M block				{	
												/* Verifying types*/
												if($3.type != BOOL) { yyerror("\nErreur: Test avec expression non booléene\n"); return 1; }
												complete($3.t, $5); complete($3.f, $9); $$.next = concat($7, $10.next);
												$$.next = concat($$.next, $6.next); $$.cntu = concat($6.cntu, $10.cntu);
												$$.brk = concat($6.brk, $10.brk); $$.rtrn = concat($6.rtrn,$10.rtrn);
												gen_q_pop(curr_context->count*4); 
											}
			|	If '(' expr ')' M block 	{	
												if($3.type != BOOL) { yyerror("\nErreur: Test avec expression non booléene\n"); return 1; }
												complete($3.t, $5); $$.next = concat($3.f, $6.next);
												$$.cntu = $6.cntu; $$.brk = $6.brk; $$.rtrn = $6.rtrn;
												gen_q_pop(curr_context->count*4); 
											}
												
			|	For id '=' expr ',' expr Max{  	/* verifying types, declaration of ID, and affectation*/
												char* msg;
												if( (msg = for_declare($2, $4, $6)) != NULL) { yyerror(msg); return 1;}
												$7 = get_max($2, $6);
											} 
					M  							{	gen_test_counter($2, $7); } 
						
					block 						{   complete($11.next, nextquad); complete($11.cntu, nextquad);
													$$.next = crelist($9); $$.next = concat($$.next, $11.brk);
													$$.rtrn = $11.rtrn;
													gen_increment_and_loopback($2, $9);

													complete($$.next, nextquad);
													gen_q_pop(curr_context->count*4); 
													popctx(); }

			|	Return return_val ';'		{	$$.rtrn = crelist(nextquad); 
												/* we return $2.result and we store the return type in qo for later verification of types*/
												quadop qo; qo.type = QO_CST; qo.u.cst = $2.type;
												gencode($2.result, qo, qo, Q_RETURN, NULL, -1, NULL); }

			|	Break ';'					{ 	if(!is_a_parent(CTX_FOR)) { yyerror("\nErreur: Break; doit être au sein d'une boucle FOR\n"); return 1; }
												quadop qo; qo.type = QO_EMPTY;
												$$.brk = crelist(nextquad);
												gencode(qo,qo,qo,Q_GOTO,NULL,-1,NULL); }

			|	Continue ';'				{  if(!is_a_parent(CTX_FOR)) { yyerror("\nErreur: Continue; doit être au sein d'une boucle FOR\n");return 1;}
												quadop qo; qo.type = QO_EMPTY;
												$$.cntu = crelist(nextquad);  
												gencode(qo,qo,qo,Q_GOTO,NULL,-1,NULL); }

			|	block						{ 	$$.brk = $1.brk; $$.next = $1.next; $$.cntu = $1.cntu; $$.rtrn = $1.rtrn;}

return_val	:	expr 						{ 	$$ = $1; }
			|	%empty						{ 	$$.type = VOIDTYPE; $$.result.type = QO_EMPTY; }


method_call :	id '(' E ')' 				{	item_table *val = lookup($1);
												if(val == NULL) { yyerror("\nErreur: Méthode non déclarée\n"); return 1; }
												if(val->item->id_type != ID_METHOD) { yyerror("\nErreur: l'ID utilisé n'est pas celui d'une méthode\n"); return 1;}
												if(!verify_param(val->item->p, $3.p)) { yyerror("\nErreur: Appel de méthode avec paramètres incorrectes\n");return 1;}

												if(!strcmp($1,"WriteString")) {
													get_write_string_args($3.p->arg.u.string_literal.label, $3.p->arg.u.string_literal.value); }

												$$.return_type = val->item->value; 

												// we store in qo the label of the method to call and in q1 the return value
												quadop qo,q2; q2.type = QO_EMPTY; qo.type = QO_CSTSTR;
												qo.u.string_literal.label = malloc(strlen($1)+1);
												strcpy(qo.u.string_literal.label,$1);
												quadop q1; if($$.return_type != VOIDTYPE) { q1.type = QO_CST; q1.u.cst = $$.return_type; } else { q1.type = QO_EMPTY;}
												complete($3.t,nextquad); complete($3.f,nextquad);
												gencode(qo,q1,q2, Q_METHODCALL, NULL, -1, $3.p);
												$$.result = q1; }

			|	id '(' ')'					{
												item_table *val = lookup($1);
												if(val == NULL) { yyerror("\nErreur: Méthode non déclarée\n"); return 1; }
												if(val->item->id_type != ID_METHOD) { yyerror("\nErreur: l'ID utilisé n'est pas celui d'une méthode\n"); return 1;}
												if(!verify_param(val->item->p, NULL)) { yyerror("\nErreur: Appel de méthode avec paramètres incorrectes\n"); return 1;}
												$$.return_type = val->item->value;

												// we store in qo the label of the method to call
												quadop qo;
												qo.type = QO_CSTSTR;
												qo.u.string_literal.label = malloc(strlen($1)+1);
												strcpy(qo.u.string_literal.label,$1);
												gencode(qo,qo,qo, Q_METHODCALL, NULL, -1, NULL); }

/*
	 E is a list of parameters of a method call 
*/
E 			:	expr ',' E 						{ 
													param p = (param) malloc(sizeof(struct param)); 
													p->type = $1.type; 
													p->arg = $1.result;
													p->next = $3.p; 
													$$.p = p;
													if($1.type == BOOL){
														$$.t = concat($$.t,$1.t);
														$$.f = concat($$.f,$1.f);
													}
												}
			|	expr 							{ 
													$$.p = (param) malloc(sizeof(struct param)); 
													$$.p->type = $1.type; 
													$$.p->arg = $1.result;
													$$.p->next = NULL;
													if($1.type == BOOL){
														$$.t = $1.t;
														$$.f = $1.f;
													}
													
												}

location	:	id				{
										
										$$.type = ID_VAR;
										$$.stringval = malloc(strlen($1)+1);
										strcpy($$.stringval, $1);
											
									}
			|	id '[' expr ']'		{ 	
										$$.type = ID_TAB;
										$$.stringval = malloc(strlen($1)+1);
										strcpy($$.stringval, $1);
										$$.index = $3.result;
								}

expr		:	expr add_op expr %prec '+'	{
												if($1.type != INT || $3.type != INT){
													yyerror("\nErreur: Arithmètique non entière");
													return 1;
												}
												$$.type = INT; 
												Ht_item* item = new_temp(INT);
												quadop qo;
												qo.type = QO_TMP;
												qo.u.offset = 0;

												$$.stringval = malloc(strlen(item->key)+1);
												strcpy($$.stringval, item->key);

												if($1.result.type == QO_ID || $1.result.type == QO_TMP) {
													item_table* val;
													val = lookup($1.stringval);
													$1.result.u.offset = offset(val);
												}
												if($3.result.type == QO_ID || $3.result.type == QO_TMP) {
													item_table* val;
													val = lookup($3.stringval);
													$3.result.u.offset = offset(val);
												}

												gencode(qo,qo,qo,Q_DECL,NULL,-1, NULL);


												gencode(qo,$1.result,$3.result,$2,NULL,-1, NULL);
												$$.result = qo;
											}
			|	expr mul_op expr %prec '*'	{	if($1.type != INT || $3.type != INT){
													yyerror("\nErreur: Arithmètique non entière");
													return 1;
												}
												$$.type = INT; 
												Ht_item* item = new_temp(INT);
												quadop qo;
												qo.type = QO_TMP;
												qo.u.offset = 0;

												$$.stringval = malloc(strlen(item->key)+1);
												strcpy($$.stringval, item->key);

												if($1.result.type == QO_ID || $1.result.type == QO_TMP) {
													item_table* val;
													val = lookup($1.stringval);
													$1.result.u.offset = offset(val);
												}
												if($3.result.type == QO_ID || $3.result.type == QO_TMP) {
													item_table* val;
													val = lookup($3.stringval);
													$3.result.u.offset = offset(val);
												}

												gencode(qo,qo,qo,Q_DECL,NULL,-1, NULL);

												gencode(qo,$1.result,$3.result,$2,NULL,-1, NULL);
												$$.result = qo;
											}
			|	expr and M expr				{
												if($1.type != BOOL || $4.type != BOOL){
													yyerror("\nErreur: AND operator with non boolean value");
													return 1;
												}			
												$$.type = BOOL;
												complete($1.t, $3);
												$$.f = concat($1.f, $4.f);
												$$.t = $4.t; 

											}

			|	expr or M expr				{	if($1.type != BOOL || $4.type != BOOL){
													yyerror("\nErreur: OR operator with non boolean value");
													return 1;
												}
												$$.type = BOOL;
												complete($1.f, $3);
												$$.t = concat($1.t, $4.t);
												$$.f = $4.f;
											}
			|	expr oprel expr	%prec '<' 	{
												
												if($1.type != INT || $3.type != INT){
													yyerror("\nErreur: REL OP non entière");
													return 1;
												}
												$$.type = BOOL;
												quadop qo;
												qo.type = QO_EMPTY;
												qo.u.cst = 0;

												if($1.result.type == QO_ID || $1.result.type == QO_TMP) {
													item_table* val = lookup($1.stringval);
													$1.result.u.offset = offset(val);
												}
												if($3.result.type == QO_ID || $3.result.type == QO_TMP) {
													item_table* val = lookup($3.stringval);
													$3.result.u.offset = offset(val);
												}

												$$.t = crelist(nextquad);
												gencode(qo, $1.result, $3.result, $2, NULL, -1, NULL);
												$$.f = crelist(nextquad);
												gencode(qo, qo, qo, Q_GOTO, NULL, -1, NULL);


											}
			|	expr eq_op expr	%prec eq	{	
												if($1.type != $3.type ){
													yyerror("\nErreur: Comparaison de types différents");
													return 1;
												}
												$$.type = BOOL; 
												quadop qo;
												qo.type = QO_EMPTY;
												qo.u.cst = 0;

												if($1.result.type == QO_ID || $1.result.type == QO_TMP) {
													item_table* val = lookup($1.stringval);
													$1.result.u.offset = offset(val);
												}
												if($3.result.type == QO_ID || $3.result.type == QO_TMP) {
													item_table* val = lookup($3.stringval);
													$3.result.u.offset = offset(val);
												}

												$$.t = crelist(nextquad);
												gencode(qo,$1.result,$3.result,$2,NULL, -1, NULL);
												$$.f = crelist(nextquad);
												gencode(qo, qo, qo, Q_GOTO, NULL, -1, NULL);

											}
			| 	literal 					{
												
												

												
													$$.result.type = QO_CST;
													$$.type = $1.type;
													$$.result.u.cst = $1.intval;
												
											}
											
			|	location 					{
												item_table *val = lookup($1.stringval);
												if(val == NULL){
													yyerror("\nErreur: Variable non déclarée\n");
													return 1;
												}
												if(val->table == glob_context) {
													if($1.type == ID_VAR && val->item->id_type == ID_TAB) {
														yyerror("\nErreur: Accès tableau comme scalaire\n");
														return 1;
													}
													if($1.type == ID_TAB && val->item->id_type == ID_VAR) {
														yyerror("\nErreur: Scalaire utilisé comme tableau\n");
														return 1;
													}

													if($1.type == ID_VAR) {

														quadop qo;
														qo.type = QO_GLOBAL;
														qo.u.global.type = QO_SCAL;
														qo.u.global.name = malloc(strlen($1.stringval)+1);
														strcpy(qo.u.global.name, $1.stringval);
														qo.u.global.size = 4;
														$$.type = val->item->value;
														$$.result = qo;
														
													}
													else if($1.type == ID_TAB) {
														$$.type = val->item->value;
														quadop qo;
														qo.type = QO_GLOBAL;
														qo.u.global.type = QO_TAB;
														qo.u.global.name = malloc(strlen($1.stringval)+1);
														strcpy(qo.u.global.name, $1.stringval);
														qo.u.global.size = 4;

														Ht_item* item = new_temp(INT);
														quadop q1;
														q1.type = QO_EMPTY;
														q1.u.offset = 0;
														gencode(q1,q1,q1,Q_DECL,NULL,-1, NULL);

														gencode(q1, qo, $1.index, Q_ACCESTAB, NULL, -1, NULL);
														$$.result = q1;
													}
												}
												else {
														
														quadop qo;
														/* location can only be an ID if we are not in a global context*/
														qo.type = QO_ID;
														qo.u.offset = offset(val);
														$$.result = qo;
														$$.type = val->item->value;
														$$.stringval = malloc(strlen($1.stringval)+1);
														strcpy($$.stringval, $1.stringval);
													}
												}
			|	'-' expr %prec NEG 			{
												if($2.type != INT){
													yyerror("\nErreur: Arithmètique non entière");
													return 1;
												}
												$$.type = INT;
												Ht_item* item = new_temp(INT);
												quadop qo;
												qo.type = QO_TMP;
												qo.u.offset = 0;
												gencode(qo, qo, qo, Q_DECL, NULL,-1, NULL);
											
												$$.stringval = malloc(strlen(item->key)+1);
												strcpy($$.stringval, item->key);

												if($2.result.type == QO_ID || $2.result.type == QO_TMP) {
													item_table* val;
													val = lookup($2.stringval);
													$2.result.u.offset = offset(val);
												}
												quadop q1;
												q1.type = QO_CST;
												q1.u.cst = 0;
												gencode(qo, q1, $2.result, Q_SUB, NULL,-1, NULL);
												$$.result = qo;
											}
			|	'!' expr %prec NEG 			{	
												if($2.type != BOOL){
													yyerror("\nErreur: NOT operator with non boolean value");
													return 1;
												}
												$$.type = BOOL; 
												$$.t = $2.f;
												$$.f = $2.t;
											}
			|	'(' expr ')' 				{
												$$ = $2;
											}
			|	method_call					{ 
												/* A method with void return type can't be used as an expression*/
												if($1.return_type == VOIDTYPE){ yyerror("\nErreur: méthode de type de retour void utilisée comme expression\n"); return 1;}
												$$.result = $1.result;
												$$.type = $1.return_type;
											}
			|	string_literal				{	
												/* This is only used for Write_String( string_literal ) */
												$$.type = STRING;
												$$.result.type = QO_CSTSTR;
												$$.result.u.string_literal.label = new_str();
												$$.result.u.string_literal.value = malloc(strlen($1)-2);
												strncpy($$.result.u.string_literal.value, $1+1, strlen($1)-2);
											}

literal		:	int_literal					{
												$$.intval = $1;
												$$.type = INT;
											}
			|	char_literal				{
												$$.intval = $1;
												$$.type = INT;
											}
			|	bool_literal				{
												$$.intval= $1;
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

void yyerror(char *msg) {
	fprintf(stderr, "%s%s\n",RED, msg);
	set_color(NORMAL);
	
}

void yywarning(char *msg) {
	fprintf(stderr, "%s%s\n",YELLOW, msg);
	set_color(NORMAL);
}