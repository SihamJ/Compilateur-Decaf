%{	
	#include "token.h"
	#include "intermediaire.h"
	#include "compiler_functions.h"
	#include "hashtable.h"
	#include "IOfunctions.h"
	#include "text_formating.h"
	#include "compiler.tab.h"
	
	int yylex();
	int warning;
	void yyerror( const char* );
	void yywarning( const char *);
	char* program_name;
%}

%union {
	long intval;
	char *stringval;
	expr_val expr;
	statement s;
	method_call m;
	list gt;
	literal l;
	declaration decl;
	location loc;
	param p;  
}
%define parse.error verbose
%token <intval> decimal_literal hex_literal char_literal bool_literal eq neq and or not leq geq aff_add aff_sub integer boolean voidtype '+' '-' '%' '/' '<' '>' '=' '!' '*'
%token <stringval> id string_literal ReadInt address
%token class If Else For Return Break Continue 

%type <expr> expr return_val Max 
%type <gt> G
%type <s> statement block S2 S ElseBlock
%type <m> method_call

%type <loc> location
%type <intval> int_literal assign_op type M oprel eq_op add_op mul_op
%type <decl> B glob_id 
%type <p> Param P Arg
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
						 	pushctx(CTX_GLOB); 	tmpCount = 0; 	
							glob_context = curr_context; 		
							gen_q_push();
							program_name = malloc(strlen($2)+1); 
							strcpy(program_name, $2);			// saving class name in a global variable
							add_libs_to_tos(); 					// Adding the I/O functions to the global symbol table
						}

						
				GLOBAL '}' { 	// verifying that we have a main method
								if( ht_search( glob_context,"main") == NULL ) {	yyerror("\nErreur: Pas de méthode main\n"); return 1; }
								global_code[glob_context->quad_index].op1.u.cst = curr_context->size;
								gen_q_pop(curr_context->size); popctx();
									return 0;
							}

M 		:	%empty 		{	$$ = nextquad; }

G 		:	%empty		{	$$ = crelist(nextquad); 	quadop qo;	 qo.type = QO_EMPTY; 	
							gencode(qo,qo,qo, Q_GOTO, NULL, -1, NULL); }

Max		:	%empty		{;}

GLOBAL 		: MD		{;}
			| FD MD 	{;}

FD 			: FD field_decl  {;}
			| field_decl	 {;}

field_decl	:	type glob_id ';' 		{ 	/* we push the global variables into the TOS
											returns NULL if successful, otherwise returns an error msg*/
											char *msg; if((msg = field_declare(&$2, $1)) != NULL) { 
												yyerror(msg); return 1;}
										}
														

glob_id	:	id '[' int_literal ']' ',' glob_id	{
													if($3 <= 0) { 
														yyerror ("\nErreur: Taille du tableau déclarée inférieure ou égale à 0\n");	
														return 1; }

													$$ = get_declarations($1, NULL, QO_TAB, $3*4);	}

		|	id '[' int_literal ']'				{
													if($3 <= 0) { 
														yyerror ("\nErreur: Taille du tableau déclarée inférieure ou égale à 0\n");	
														return 1; }

													$$ = get_declarations($1, NULL, QO_TAB, $3*4);	
												}

		|	id									{	$$ = get_declarations($1, NULL, QO_SCAL, 4); 	}

		|	id	','	glob_id						{	$$ = get_declarations($1, &$3, QO_SCAL, 4); 	}

MD			:	MD method_decl		{;}
			|	method_decl			{;}

method_decl	:		type id 	{	// We add id to the TOS and push a new context for the parameters.
									// Returns NULL if successful, otherwise returns an error msg.
									char *msg;  if( (msg = add_to_tos($1, $2)) != NULL) { yyerror(msg);	return 1;}
								}

					'(' P ')' 	{	// item_table is a structure that has a couple (Ht_item*, HashTable*),
									// returned by the function lookup(char *id) 
									item_table* var = lookup($2, curr_context);

									// item is an Ht_item, it contains the attribute p for the items of type method where we store
									// the types of parameters that the method takes.
									var->item->p = $5;
								}

					block		{	// We verify that return exists. Generates a warning if -Cafeine option is used
									 int is_returnval=0; 
									 if($8.rtrn == NULL) { 
										 yywarning("\nWarning: Méthode de type non Void sans Return");  }
									 else {	is_returnval = 1;	}

									// verifying return types
									if(is_returnval && !verify_returns($8.rtrn, $1)) { 
										yyerror("\nErreur: Méthode avec faux type de retour\n");  
										return 1; }
									complete($8.next,nextquad-1); 	complete($8.rtrn, nextquad-1);

									char* msg; 	
									if( (msg = end_func($2, curr_context->count, $5, is_returnval)) != NULL ) { 
										yyerror(msg); return 1; } 

									popctx(); 
								}
						
			/* Same as above, except the type is void. We are forced to separate them because of conflicts*/
			|	voidtype id		{
									char *msg; if((msg = add_to_tos($1, $2)) != NULL) { 
										yyerror(msg); return 1; } 
								} 
							
			'(' P ')'  			{	item_table* var = lookup($2, curr_context);		var->item->p = $5; }
			
				block			{
								 	if($8.rtrn != NULL && !verify_returns($8.rtrn, $1) ) { 
										yyerror("\nErreur: Méthode avec faux type de retour\n"); 
										return 1; }

									// nextquad-1 is always pop stack for the end of the block
									complete($8.next,nextquad-1);	complete($8.rtrn, nextquad-1);
									char* msg;	
									if( (msg = end_func($2, curr_context->count, $5, 0)) != NULL) { 
										yyerror(msg); return 1;	}

									popctx(); 	
								}

						// we store the parameters in the symbol table after verifications
P 			:	Param 	{
						 	$$ = $1; 	param p = $1;
							while(p) {	
								if( ht_search(curr_context, p->stringval) != NULL ) { 
									yyerror("\nErreur: Deux paramètres de méthodes de même nom\n");  
									return 1; }

								Ht_item *item = create_item(p->stringval, ID_PARAM, p->type);	
								newname(item);		p = p->next; 
							}
						}

			|	%empty	{ $$ = NULL;}

/* Param is a list that retrieves the names of declared parameters and their types */
Param		:	type id ',' Param	{ 	$$ = get_param($2, $1, $4); 	}
										
			|	type id				{	$$ = get_param($2, $1, NULL);	}

block 		:	'{' 	{ 	pushctx(CTX_BLOCK); 	 gen_q_push(); 		} 

			V S2 '}' 	{ 	$$ = $4; $$ = pop_block($$, $4);  popctx(); 		}

S2 			:	S 		{	$$ = $1;	}

			|	%empty 	{	
							$$.next = NULL; 	$$.rtrn = NULL; 	
							$$.brk = NULL; 		$$.cntu = NULL; 
						}

V 			:	V var_decl	{;}
			|	%empty;

var_decl 	:  	type B ';' {	// we push the B list of variables in the TOS
								// returns NULL if successful, error msg otherwise
								char *msg;  
								if ( (msg = var_declare(&$2, $1)) != NULL ) { 
										yyerror(msg); return 1; }
							}

B 			:	id ',' B  	{ 	
								declaration var;
								var.name = malloc((strlen($1)+1));   strcpy(var.name,$1);
								var.suiv = &$3;	 $$ = var; 
							}

			|	id			{
								$$.name = malloc((strlen($1)+1));   strcpy($$.name,$1); 
								$$.suiv = NULL; 
							}


type		:	integer 	{	$$ = $1;	}
			|	boolean 	{	$$ = $1;	}


S 			: 	S M statement 	{	
									complete($1.next, $2); 				$$.cntu = concat($$.cntu,$3.cntu); 		
									$$.brk = concat($$.brk,$3.brk); 	$$.rtrn = concat($$.rtrn,$3.rtrn);
									$$.next = $3.next; 	 
								}

			| 	statement		{
									$$.brk = $1.brk; 	$$.next = $1.next; 	
									$$.cntu = $1.cntu; 	$$.rtrn = $1.rtrn; 
								}

statement 	:	location assign_op expr ';' {
												initialise_lists(&$$);
												item_table* val = lookup($1.stringval, curr_context);

												// Verifying types. returns NULL if successful, error msg otherwise
												char *msg; 
												if( (msg = verify_aff_types($1.type, $3.type, $2, val->item)) != NULL) { 
													yyerror(msg); 	return 1; 	}

												// qo contains the type of location: Array or Scalar
												// if location is an element in an array, q1 contains the index
												quadop qo,q1,q2;	get_location(&qo, &q1, val, $1);

												// from true/false list to value
												 if($3.result.type == QO_GOTO) { $3 = goto_to_val($3);	complete($3.t, nextquad); }
												
												// in the mips translater, we have to differentiate between | = | += | -= |  
												 gencode(qo,$3.result,q1,$2, NULL,-1, NULL);	
											}

											
 		|	method_call	';'					{ 	initialise_lists(&$$); }

		|	For id '=' expr ',' expr Max	{  	// this function verifies types, declares ID, and the necessary affectations and copy of values
												char* msg;
												if( (msg = for_declare($2, $4, $6)) != NULL) { yyerror(msg); return 1;}
												// Max is %empty. it is an expression that only serves us for doing a copy of the max counter value
												$7 = get_max($2, $6);
											} 
				M  							{	gen_test_counter($2, $7); } 
					
				block 						{
											    complete($11.next, nextquad);		complete($11.cntu, nextquad);
												$$.next = crelist($9); 				$$.next = concat($$.next, $11.brk);	
												$$.rtrn = $11.rtrn; 		 		gen_increment_and_loopback($2, $9); 
												complete($$.next, nextquad);		gen_q_pop(curr_context->size); 
												$$.next = crelist( nextquad);		quadop qo; qo.type = QO_EMPTY;	
												$$.brk = NULL;
												gencode(qo, qo, qo, Q_GOTO, NULL, -1, NULL); 

												// Now that we know the context size, we go back to PUSH_CTX quad and update its value
												global_code[curr_context->quad_index].op1.u.cst = curr_context->size; 
												  
												popctx(); 
											}

			|	Return return_val ';'		{
												initialise_lists(&$$);
												// we return $2.result and we store the return type in qo for later verification of types
												$$.rtrn = crelist(nextquad); 	quadop qo,q1;		qo.type = QO_CST;
												qo.u.cst = $2.type; 			q1.type = QO_EMPTY;
												if($2.type == QO_GOTO) { 
													$2 = goto_to_val($2);	complete($2.t, nextquad);}
												gencode($2.result, qo, q1, Q_RETURN, NULL, -1, NULL); 
											}

			|	Break ';'					{
											 	initialise_lists(&$$);
												// we verify that this break statement is within a FOR loop
												if(!is_a_parent(CTX_FOR)) { 
													yyerror("\nErreur: Break; doit être au sein d'une boucle FOR\n"); 
													return 1; }
												quadop qo;	 qo.type = QO_EMPTY; 					
												$$.brk = crelist(nextquad);
												gencode(qo,qo,qo,Q_GOTO,NULL,-1,NULL); 
											}

			|	Continue ';'				{
												initialise_lists(&$$);
												// we verify that this continue statement is within a FOR loop
												if(!is_a_parent(CTX_FOR)) { 
													yyerror("\nErreur: Continue; doit être au sein d'une boucle FOR\n");
													return 1;	}
												quadop qo; 		qo.type = QO_EMPTY;		 
												$$.cntu = crelist(nextquad);  
												gencode(qo,qo,qo,Q_GOTO,NULL,-1,NULL); 
											}

			|	block						{
											 	$$.brk = $1.brk;	 $$.next = $1.next; 	
												$$.cntu = $1.cntu; 	 $$.rtrn = $1.rtrn;
											}

			|	If '(' expr ')' 	      	{ 
												if ($3.type != BOOL) { 
													yyerror("\nErreur: Test avec expression non booléene\n"); 
													return 1; } 
											
												if ($3.result.type == QO_CST) { 
													quadop qo, q1; 				qo.type = QO_EMPTY; 
													q1.type = QO_CST; 			q1.u.cst = true;

													$3.t = crelist(nextquad);
													gencode(qo, $3.result, q1, Q_EQ, NULL, -1, NULL);

													$3.f = crelist(nextquad); 
													gencode(qo, qo, qo, Q_GOTO, NULL, -1, NULL); 
												}

												else if (	$3.result.type == QO_ID 
															|| $3.result.type == QO_TMP 
															|| $3.result.type == QO_GLOBAL) { 
													$3 = val_to_goto($3); 
												}	
											} 
			
				M ElseBlock					{
											 	complete($3.t, $6); 	$$.cntu = $7.cntu; 		
												$$.brk = $7.brk; 		$$.rtrn = $7.rtrn;
												// completion of GOTOs will be different if there is an Else Block
												if(!$7.isElseBlock) { 
													$$.next = concat($3.f, $7.next); 
												}
												else { 
													complete($3.f, $7.isElseBlock); 
													$$.next = concat($7.elseGoto, $7.next); 
												}
											}

ElseBlock	:	block 						{
												$$.elseGoto = NULL; 	$$.isElseBlock = 0; 
												$$.next = $1.next; 		$$.cntu = $1.cntu; 
												$$.brk = $1.brk; 		$$.rtrn = $1.rtrn; 
											}

			|	block G Else M block		{
												$$.isElseBlock = $4; 					$$.elseGoto = $2;	
												$$.next = concat($1.next, $5.next); 	$$.rtrn = concat($1.rtrn, $5.rtrn);
												$$.cntu = concat($1.cntu, $5.cntu); 	$$.brk = concat($1.brk, $5.brk);
											}

return_val	:	expr 						{ 	$$ = $1; }
			|	%empty						{ 	$$.type = VOIDTYPE; 	$$.result.type = QO_EMPTY; }


											//  we verifiy the parameters of the method call and generate the quad								
method_call :	id '(' Arg ')' 				{
												char *msg;
												if((msg = verify_and_get_type_call($1, $3, &$$)) != NULL) { 
													yyerror(msg); return 1; }

												if(!strcmp($1,"WriteString")) {
													get_write_string_args($3->arg.u.string_literal.label, 
																		  $3->arg.u.string_literal.value); }

												gen_method_call($1, $3, &$$); 
											}

			|	id '(' ')'					{
												char *msg;
												if((msg = verify_and_get_type_call($1, NULL, &$$)) != NULL) { 
													yyerror(msg); return 1; }

												gen_method_call($1, NULL, &$$); 
											}

										//	 E is a list of parameters of a method call 
Arg 		:	expr 					{ 	if ($1.result.type == QO_GOTO) { $1 = goto_to_val($1); }  }

			',' M Arg 					{ 	$$ = copy_method_call_arg($1, $5, $4);	}

			|	expr 					{
										 	if ($1.result.type == QO_GOTO) { $1 = goto_to_val($1); }

											$$ = copy_method_call_arg($1, NULL, -1); 
										}

			|	address ','	M Arg 		{	// address is an id starting with '&' 
											item_table* val = lookup($1+1, curr_context);
											if( val == NULL) {
												yyerror("\nErreur: Variable non déclarée\n"); 	return 1;  }
											
											$$ = get_arg_by_address($1, $4, val);  
										}

			|	address					{	
											item_table* val = lookup($1+1, curr_context);
											if( val == NULL) {	
												yyerror("\nErreur: Variable non déclarée\n"); 	return 1; }
											
											$$ = get_arg_by_address($1, NULL, val);
										}


location	:	id					{	$$.type = ID_VAR; 	$$.stringval = malloc(strlen($1)+1); 	strcpy($$.stringval, $1); }

			|	id '[' expr ']'		{ 	
										if($3.type != INT) {
												yyerror("\nErreur: L'ndice d'accès à un tableau doit être de type INT\n"); 
												return 1;	}

										gen_index_access_tab(&$$, $1, $3);				
									}


expr		:	expr add_op expr %prec '+'	{
												if($1.type != INT || $3.type != INT) { 
													yyerror("\nErreur: Arithmètique non entière"); 
													return 1; 	}

												$$ = add($1, $2, $3);
											}

			|	expr mul_op expr %prec '*'	{
												if($1.type != INT || $3.type != INT) { 
													yyerror("\nErreur: Arithmètique non entière"); 
													return 1;	 }
												$$ = mul($1, $2, $3);
											}

			|	expr and 		{
									if($1.type != BOOL) { 
										yyerror("\nErreur: AND operator with non boolean value"); 
										return 1; }

									if($1.result.type != QO_GOTO) { $1 = val_to_goto($1); }
								}
			
				M expr			{	if($5.type != BOOL) { 
										yyerror("\nErreur: AND operator with non boolean value"); 
										return 1; }		

									$$.type = BOOL;
									if($5.result.type != QO_GOTO) { $5 = val_to_goto($5);	}

									complete($1.t, $4); 	$$.f = concat($1.f, $5.f); 	
									$$.t = $5.t; 			$$.result.type = QO_GOTO; 
								}

			|	expr or 	 	{
									if($1.type != BOOL) { 
										yyerror("\nErreur: OR operator with non boolean value"); 
										return 1; }

									if($1.result.type != QO_GOTO) {  $1 = val_to_goto($1);	}
								}
			
				M expr			{
									if($5.type != BOOL) { 
										yyerror("\nErreur: OR operator with non boolean value"); 
										return 1; }

									$$.type = BOOL;
									if($5.result.type != QO_GOTO) { $5 = val_to_goto($5); }

									complete($1.f, $4); 	$$.t = concat($1.t, $5.t); 	
									$$.f = $5.f; 			$$.result.type = QO_GOTO;
								}

			|	expr oprel expr	%prec '<' 	{
												if($1.type != INT || $3.type != INT) { 
													yyerror("\nErreur: REL OP non entière"); 
													return 1; }

												$$ = oprel($1, $2, $3);
											}

			|	expr eq_op 					{	if($1.result.type == QO_GOTO) { $1 = goto_to_val($1); } }
			
				M expr %prec eq				{
												if($1.type != $5.type ) { 
													yyerror("\nErreur: Comparaison de types différents"); 
													return 1; }

												if($5.result.type == QO_GOTO) { $5 = goto_to_val($5); complete($5.t, nextquad); }

												if($1.type == BOOL && $1.t != NULL) { complete($1.t, $4); }

												$$ = oprel($1, $2, $5);
											}

			|	'-' expr %prec NEG 			{
												if($2.type != INT){ 
													yyerror("\nErreur: Arithmètique non entière"); 
													return 1; }
												$$ = unaire($2);
											}

			|	'!' expr %prec NEG 			{	if($2.type != BOOL) { 
													yyerror("\nErreur: NOT operator with non boolean value"); 
													return 1; }

												if($2.result.type == QO_CST) {
													$$.result.type = QO_CST; 
													$$.result.u.cst = !$2.result.u.cst; }

												else if($2.result.type == QO_GOTO) { 
													$$.t = $2.f; 	$$.f = $2.t;  
													$$.result.type = QO_GOTO; 	}

												else if($2.result.type == QO_ID || $2.result.type == QO_TMP || $2.result.type == QO_GLOBAL) {
												 	$$ = not_op($2); }

												$$.type = BOOL; 
											}

			|	'(' expr ')' 				{	$$ = $2; }

			|	method_call					{ 	if($1.return_type == VOIDTYPE) { 
													yyerror("\nErreur: méthode de type de retour void utilisée comme expression\n"); 
													return 1;	}
												$$.result = $1.result; 		$$.type = $1.return_type; 
												$$.stringval = malloc(strlen($1.result_id)+1); 	
												strcpy($$.stringval, $1.result_id);
											}		
						
			|	location 					{
												item_table *val = lookup($1.stringval, curr_context);
												if(val == NULL){ 
													yyerror("\nErreur: Variable non déclarée\n"); 
													return 1; }

												if(val->table == glob_context) {

													char *msg; 
													if( (msg = verify_location_access($1, val)) != NULL) { 
														yyerror(msg); return 1;	}

													if ($1.type == ID_VAR) 		 { $$ = gen_global_scalar($1,val); 	}
													else if ($1.type == ID_TAB)  { $$ = gen_access_tab($1, val);	}
												}

												else {	$$ = get_local_id($1, val); }
											}
											

			|	string_literal				{	$$ = get_string_literal($1); }

			| 	literal 					{	$$ = get_literal($1);	}
											


literal		:	int_literal					{	$$.intval = $1; $$.type = INT;	}

			|	char_literal				{	$$.intval = $1;	$$.type = INT;	}

			|	bool_literal				{	$$.intval = $1;	$$.type = BOOL;	}

int_literal	: 	decimal_literal 			{	$$ = $1;	}
			| 	hex_literal					{	$$ = $1;	}

assign_op	:	'='							{	$$ = Q_AFF;		}
			|	aff_add						{	$$ = Q_AFFADD;	}
			|	aff_sub						{	$$ = Q_AFFSUB;	}

oprel		:	'<'							{	$$ = Q_LT;	}
			|	'>'							{	$$ = Q_GT;	}
			|	geq 						{	$$ = Q_GEQ;	}
			|	leq 						{	$$ = Q_LEQ;	}

eq_op		:	eq							{	$$ = Q_EQ;	}
			|	neq							{	$$ = Q_NEQ;	}

add_op		:	'+'							{	$$ = Q_ADD;	}
			|	'-'							{	$$ = Q_SUB;	}
mul_op		:	'*'							{	$$ = Q_MUL;	}
			|	'/'							{	$$ = Q_DIV;	}
			|	'%'							{	$$ = Q_MOD;	}
%%


void yyerror(const char *msg) {
	fprintf(stderr, "%s%8s\n",RED, msg);
	set_color(NORMAL);
}

void yywarning(const char *msg) {
	if(warning == 1){
		fprintf(stderr, "\t%s%s\n%s",YELLOW, msg, NORMAL);
		set_color(NORMAL);
	}
}