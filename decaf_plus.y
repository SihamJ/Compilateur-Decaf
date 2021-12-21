%{	
	#include "token.h"
	#include "intermediaire.h"
	#include "hashtable.h"
	#include "IOfunctions.h"
	int yylex();
	void yyerror(char*);
%}

%union {
	int intval;
	char *stringval;

	struct expr_val {
		quadop result;	
		int type;		// type of result operand: INT / BOOL / STRING
		list next;		
		list t;  		// true
		list f;   		// false
		list brk; 		// break
		list cntu; 		// continue
		list rtrn;		// return
		int return_type; // if expr_val is a method
	} expr_val;

	struct decl{
		char *name;
		struct decl *suiv;
		int size;
	} decl;

	param p;  // List for storing method parameters at the declaration or method call. Also pointed at by the Symbol Table (declaration), and the Quads (method_call).

}

%token <intval> decimal_literal hex_literal char_literal bool_literal eq neq and or not leq geq aff_add aff_sub integer boolean voidtype '+' '-' '%' '/' '<' '>' '=' '!' '*'
%token <stringval> id string_literal
%token class Program If Else For Return comment Break Continue

%type <expr_val> expr S S2 G block statement method_call location return_val literal
%type <intval> int_literal assign_op type M oprel eq_op add_op mul_op
%type <decl> B Tab
%type <p> E Param P

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

program	:  class Program '{' { 	;pushctx(CTX_GLOB); /* Pushing the global context*/
								add_libs_to_tos(); /* Adding the I/O functions to the global symbol table*/
								glob_context = curr_context; /* glob_context will point to this context throughout the execution of this program.*/
							}

						
				GLOBAL '}' { 
								if( ht_search( glob_context,"main") == NULL )	/* verifying that we have a main method*/
									yyerror("\nErreur: Pas de méthode main\n");
								popctx(); /* End of the program, we pop the global context.*/
							}

/* Marks the address of the next quad to be generated */
M 		:	%empty 		{	$$ = nextquad; }


/* GOTO adress of next quad to be generated. Similar to the marker with an added jump.*/
G 		:	%empty		{	$$.next = crelist(nextquad);
							quadop qo;
							qo.type = QO_EMPTY;
							qo.u.cst = 0;
							gencode(qo,qo,qo, Q_GOTO, global_code[nextquad].label, -1, NULL);
						}

/* The structure of the global context follows either of the two:
	MD: only contains method declarations
or	FD MD: starts with global variable declarations followed by method declarations  */
GLOBAL 		: MD		{;}
			| FD MD 	{;}

/* Block de déclarations de variables globales */
FD 			: FD field_decl  {;}
			| field_decl	 {;}

field_decl	:	var_decl	{;}
			|	tab_decl	{;}

tab_decl	:	type Tab ';' 							{ 
															quadop qo,q1;
															struct decl *pt = &$2;
															qo.type = QO_GLOBAL;
															
															/* Tab is a list of all arrays declared of same type, see rule below*/
															while(pt != NULL){

																/* verifying that the ID of the declared array does not already exist in the current context*/
																if( ht_search(glob_context, pt->name) != NULL ) 
																	yyerror("\nErreur: Tableau déclarée deux fois\n");
																if(!strcmp(pt->name, next_label_name()))
																	labelCount++;
																
																/* declaration of the array in the quad */
																qo.u.global.name = malloc(strlen(pt->name + 1));
																strcpy(qo.u.global.name, pt->name);
																qo.u.global.size = pt->size * 4;
																qo.u.global.type = QO_TAB;
																gencode(qo, qo, qo, Q_DECL, global_code[nextquad].label, -1, NULL);

																/* we increment the global variables counter, useful for MIPS*/
																glob_dec_count++;

																/* we store the array in the global symbol table*/
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
															var.size = $3*4;
															var.suiv = &$6;
															$$ = var;
														}
			|	id '[' int_literal ']'					{
															struct decl *d;

															if($3 <= 0) yyerror("\nErreur: Taille du tableau déclarée inférieure ou égale à 0\n");															
															struct decl var;
															var.name = malloc((strlen($1)+1)); 
															strcpy(var.name,$1);
															var.size = $3*4;
															var.suiv = NULL;
															$$ = var;
														}

MD			:	MD method_decl		{;}
			|	method_decl			{;}

method_decl	:		type id {	
								/* verifying that ID does not already exist in the global context*/
								if(ht_search(glob_context, $2) != NULL)
									yyerror("\nErreur: Méthode déjà déclarée avec ce nom\n");

								/* Verifying that main is not declared with a return type other than void*/
								if(!strcmp($2,"main"))
									yyerror("\nErreur: La méthode main doit être de type void\n");

								if(!strcmp($2, next_label_name()))
									labelCount++;
								
								/* we store the method in the symbol table*/
								Ht_item *item = create_item($2, ID_METHOD, $1);
								newname(item);

								/* The parameters declaration within a method has its specific context
								which can be overwritten by new declarations in the method block.
								*/
								pushctx(CTX_METHOD);
								global_code[nextquad].label = malloc(strlen($2)+1);
								strcpy(global_code[nextquad].label,$2);

								} 

									'(' P ')' 			{	
															/* item_table is a structure that has a couple (Ht_item*, HashTable*),
															returned by the function lookup(char *id)
															*/
															item_table* var = lookup($2);
															/* 
																item is an Ht_item, it contains the attribute p for the items of type method where we store
																the types of parameters that the method takes.
																P is either a list of parameters or NULL if the method does not take any parameter, see rule P below.
															*/
															var->item->p = $5;}

								block					{
														 	/* We verify that return exists, and is of the same type*/
															if($8.rtrn == NULL)
																yyerror("\nErreur: Méthode sans return\n");
															/* In the Return rule below (~line 586), we generated a new quad for the Return and we 
																stored its index in $$.rtrn. We also stored the Return type in op2. Now we can access 
																this incomplete quad to verify the return type. 
															*/
															else if(global_code[$8.rtrn->addr].op2.u.cst != $1)
																yyerror("\nErreur: Méthode avec faux type de retour\n");
															/*
																we get to the end of the method declaration. If there was a GOTO somewhere in the block to jump out of the
																block, now we know where the end of the block is, it's necessarily the adress of the next quad of end function
																because this block is at the end of a method declaration. Same thing for the return jumps.
															*/
															complete($8.next,nextquad);	
															complete($8.rtrn, nextquad);
															/* 
															We generate a quad taht indicates the end of the method declaration, this is useful for MIPS
															for retrieving registers
															*/													
															quadop qo;
															qo.type = QO_EMPTY;
															qo.u.cst = 0;
															
															gencode(qo, qo, qo, Q_ENDFUNC, new_endfunc_label($2), -1, NULL);
															popctx(); }

			/* Same as above, except the type is void method. We are forced to separete them because of Bison conflicts with types of variable declarations*/
			|	voidtype id {	if(ht_search(glob_context, $2) != NULL)
									yyerror("\nErreur: Méthode déjà déclarée avec ce nom\n");
								if(!strcmp($2, next_label_name()))
									labelCount++;
								Ht_item *item = create_item($2, ID_METHOD, $1);
								newname(item);
								pushctx(CTX_METHOD); 
								global_code[nextquad].label = malloc(strlen($2)+1);
								strcpy(global_code[nextquad].label,$2);
								} 
							
								'(' P ')'  block	{ 		
															if($7.rtrn == NULL)
																yyerror("\nErreur: Méthode sans return\n");

															if(global_code[$7.rtrn->addr].op2.u.cst != $1)
																yyerror("\nErreur: Méthode avec faux type de retour\n");

															item_table* var = lookup($2);
															var->item->p = $5;
															complete($7.next, nextquad);
															complete($7.rtrn, nextquad);	

															if(!strcmp($2,"main")){
																if($5 != NULL) 
																	yyerror("\nErreur: Main ne prends pas de paramètres\n");
																quadop qo; 
																qo.type = QO_CST; 
																qo.u.cst = 10; 
																gencode(qo,qo,qo, Q_SYSCALL, "end", -1, NULL);
															} 	

															else {													
																quadop qo;
																qo.type = QO_EMPTY;
																qo.u.cst = 0;
																gencode(qo, qo, qo, Q_ENDFUNC, new_endfunc_label($2), -1, NULL);
															}
																				
															popctx();
														}

/* Param is a non empty list of method parameters*/
P 			:	Param 	{ $$ = $1;}
			|	%empty	{ $$ = NULL;}


/* Param is a list that retrieves the the names of declared parameters and their types */
Param		:	type id ',' Param	{ 	
										/* we verify that we don't declare two parameters with the same ID*/
										if( ht_search(curr_context, $2) != NULL ) 
											yyerror("\nErreur: Deux paramètres de méthodes de même nom\n");
										
										/* we store the parameter in the symbol table*/
										Ht_item *item = create_item($2, ID_PARAM, $1);
										newname(item);
										param p = (param) malloc(sizeof(struct param));
										p->type = $1;
										p->next = $4;
										$$ = p;
										}
			/* Idem*/
			|	type id				{ 	if( ht_search(curr_context, $2) != NULL ) 
											yyerror("\nErreur: Deux paramètres de méthodes de même nom\n");
										Ht_item *item = create_item($2, ID_PARAM, $1);
										newname(item);
										$$ = (param) malloc(sizeof(struct param));
										$$->type = $1;
										$$->next = NULL;
										
										}

/* each block starts with variable declarations followed by instructions, or instructions only, or variable declarations only (though last one is useless) */
block 		:	'{' { 
						pushctx(CTX_BLOCK); } 
			V S2 '}' 
					{ 	
						/* we retrieve the adresses of the incomplete GOTOs if they exist*/
						$$.next = $4.next; 
						$$.brk = $4.brk;
						$$.cntu = $4.cntu;
						$$.rtrn = concat($$.rtrn,$4.rtrn);
						popctx();
					}

S2 			:	S 	{$$=$1;}
			|	%empty {;}

			
/* a block of variable declarations */
V 			:	V var_decl 	{;}
			|	%empty		{;}


/* a variable declaration */
var_decl 	:  	type B ';' {
								/* if we are in the global context, the IDs are of type QO_GLOBAL */
								if(curr_context == glob_context){
									quadop qo;
									struct decl *pt = &$2;
									qo.type = QO_GLOBAL;
									
									/* We browse the list B of IDs declared */
									while(pt != NULL){
										/* We verify for each ID that it is not already declared in the symbol table*/
										if( ht_search(glob_context, pt->name) != NULL ) 
											yyerror("\nErreur: Variable déclarée deux fois\n");
										/* to avoid having global variables that have the same name as an auto-generated label*/
										if(!strcmp(pt->name, next_label_name()))
											labelCount++;

										/* we store the ID in a quadop and we generate a declaration quad with gencode*/
										qo.u.global.name = malloc(strlen(pt->name + 1));
										strcpy(qo.u.global.name, pt->name);
										qo.u.global.size = 4;
										gencode(qo, qo, qo, Q_DECL, global_code[nextquad].label, -1, NULL);

										/* we increment the global variables counter, useful for MIPS*/
										glob_dec_count++;

										/* we store the ID in the symbol table*/
										Ht_item *item = create_item(pt->name, ID_VAR, $1);
										newname(item);
										pt = pt->suiv;
									}
								}

								/* If we are not in the global context, IDs are of type QO_ID */
								else {

									quadop qo;
									/* pointer to browse the B list of IDs*/
									struct decl *pt = &$2;
									qo.type = QO_ID;
									
									/* Idem as above, with the addition of an offset for the stack*/
									while(pt != NULL){
										if( ht_search(curr_context, pt->name) != NULL ) 
											yyerror("\nErreur: Variable déclarée deux fois\n");
										if(!strcmp(pt->name, next_label_name()))
											labelCount++;
										qo.u.offset = 0;
										gencode(qo, qo, qo, Q_DECL, global_code[nextquad].label, -1, NULL);
										Ht_item *item = create_item(pt->name, ID_VAR, $1);
										newname(item);
										pt = pt->suiv;
									}
								}
							}

/* 
	We are forced to split this section from the previous section so we can declare multiple variables of same type in one line.
	We can't have a recursive grammar with "type" because it is not repeated.
	At this stage, we retrieve only the IDs et store them in the B list.
*/
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

/* 
	A series of statements
*/
S 			: 	S M statement 	{	
									/* At this point, we can complete the S GOTOs because we know the adress of the next statement*/
									complete($1.next, $2);

									/* We retrieve the incomplete GOTOs of statement to compete it later. */
									$$.next = $3.next;
									$$.cntu = concat($$.cntu,$3.cntu);
									$$.brk = concat($$.brk,$3.brk);
									$$.rtrn = concat($$.rtrn,$3.rtrn);
								}
			| 	statement		{	
									/* We retrieve the incomplete GOTOs of statement to complete it later. */
									$$.next = $1.next;	$$.cntu = $1.cntu; $$.brk = $1.brk; $$.rtrn = $1.rtrn;
								}



statement 	:	id assign_op expr ';' {				/* Affectation */
													/* We look for id in the symbol table and return the couple (Ht_item*, HashTable*) */
													item_table *val = lookup($1);
													if (!val)
														yyerror("\nErreur: Variable non déclarée\n");
													
													/* Verifying types*/
													if(val->item->value != $3.type)
														yyerror("\nErreur: Type de valeur incorrecte\n");
													if (($2 == Q_AFFADD || $2 == Q_AFFSUB) && (val->item->value == BOOL || $3.type == BOOL))
														yyerror("\nErreur: Type de valeur incorrecte\n");

													quadop q1;

													/* If we are in the global context*/
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
														/*
															If the affectation is of bOOL type, we have to create two quads. One is for false affectation (0) and
															the other one for true (1). The rules on expr already generate incomplete GOTOs. At this stage we complete
															them with the address of the 2 quads we generated. The true attribute of expr is completed with the adress
															of the true affectation, and the false attribute is completed with the address of the false affectation.
														*/
														quadop qo;

														/* True affectation */
														qo.type = QO_CST;
														qo.u.cst = true;
														complete($3.t, nextquad);
														gencode(q1, qo, qo, Q_AFF, global_code[nextquad].label, -1, NULL); 	

														/* To skip false affectation. Incomplete GOTO because we don't know yet where to skip.
															We add it to $$.next
														*/
														qo.type = QO_EMPTY;
														$$.next = crelist(nextquad);
														gencode(qo, qo, qo, Q_GOTO, global_code[nextquad].label, -1, NULL);

														/* False affectation*/
														qo.type = QO_CST;	
														qo.u.cst = false;		
														complete($3.f, nextquad);											
														gencode(q1, qo, qo, Q_AFF, global_code[nextquad].label, -1, NULL);	
													}
												
													else {
														/* If it's an affectation of type INT, we simply generate the corresponding quad*/

														/* Normal affectation */
														if($2 == Q_AFF)
															gencode(q1,$3.result,$3.result,$2, global_code[nextquad].label,-1, NULL);

														/* += is just an addition with op2 = op1*/
														else if($2 == Q_AFFADD)
															gencode(q1,q1,$3.result,Q_ADD, global_code[nextquad].label,-1, NULL);

														/* -= is just a subtraction with op2 = op1*/
														else if($2 == Q_AFFSUB)
															gencode(q1,q1,$3.result,Q_SUB, global_code[nextquad].label,-1, NULL);
													}
													/* we pop the temporary variables from the stack at the end of the expression evaluation*/
													pop_tmp();
												}

			/* The method call rules are defined below not here.*/
			|	method_call	';'								{ $$=$1; }

			|	If '(' expr ')' M block G Else M block	{	
															/* Verifying types*/
															if($3.type != BOOL) 
																yyerror("Erreur: Test avec expression non booléene\n");
															
															/* If expr is true, we jump to block1, the adress of its first quad is marked by M1*/
															complete($3.t, $5);

															/* If expr is false, we jump to block2, the adress of its first quad is marked by M2*/
															complete($3.f, $9);

															/* 
																If expr is true, there is a GOTO after block1, and if it is false there is a GOTO after block2
																that we don't need to mark. But in both cases we still don't know where to do the jump.
																We store the adress of the incomplete GOTOs in statement ($$). It will be completed in the above
																rule ( S )
															*/
															$$.next = $7.next;
															$$.next = concat($$.next, $10.next);

															/* 
																If our IF statement is within a FOR loop, we might have "break" and "continue" instructions
																that produce GOTOs. However, we don't know yet where the start and the end of this FOR loop is,
																this will be resolved in the FOR loop rules for the "continue" statement, and in the S rules for
																the "break" statement.
															*/
															$$.cntu = concat($6.cntu, $10.cntu);
															$$.brk = concat($6.brk, $10.brk);
														}
			|	If '(' expr ')' M block 				{
															/* IDEM but without ELSE*/
															if($3.type != BOOL) yyerror("Erreur: Test avec expression non booléene\n");
															complete($3.t, $5);
															$$.next = concat($3.f, $6.next);
															$$.cntu = $6.cntu;
															$$.brk = $6.brk;
															$$.rtrn = concat($$.rtrn,$6.rtrn);
														}
												
			|	For id 	'=' expr ',' expr				{  															
															/* The loop counter has its own context*/
															pushctx(CTX_FOR);
															/* Verifying types */
															if($4.type != INT || $6.type != INT) yyerror("Erreur: le compteur de boucle doit être de type INT\n");
															/* To avoid having variables and labels holding the same name*/
															if(!strcmp($2, next_label_name()))
																labelCount++;

															/* We declare the loop counter id, push it to the symbol table, and store the initial value expr1 in it*/
															quadop qo;
															qo.type = QO_ID;
															qo.u.offset = 0;
															gencode(qo, qo, qo, Q_DECL, global_code[nextquad].label, -1, NULL);
															Ht_item *item = create_item($2, ID_VAR, INT);
															newname(item);
															qo.type = QO_ID;
															qo.u.offset = 0;
															gencode(qo, $4.result, $4.result, Q_AFF, global_code[nextquad].label, -1, NULL); 
														} 
					
					M  									 {	
															/* 
																This marker is useful to complete the GOTOs at the end of the for block to go back to the
																start of the loop and compare id to its max value (expr2)
															*/
															item_table* val = lookup($2);
															quadop qo;
															qo.type = QO_ID;
															qo.u.offset = offset(val);

															/* this jump out of the for loop should be completed in a more global context (L)*/
															gencode(qo, qo, $6.result, Q_GT, global_code[nextquad].label, -1 , NULL); } 
					
					block 								{   /* This is happening at the end of the loop block,
																next instruction of the block is the incrementation of the loop counter 
																that's also where continue should take us, so we complete both.
															*/
															complete($10.next, nextquad);
															complete($10.cntu, nextquad);
															printf("before1\n");
															/* gencode to increment the loop counter*/
															quadop qo, q1;
															qo.type = QO_ID;
															q1.type = QO_CST;
															q1.u.cst = 1;

															/* offset of id is 0 because it's the only variable in the context at this point */
															qo.u.offset = 0;
															gencode(qo, qo, q1, Q_ADD, global_code[nextquad].label, -1, NULL);
															qo.type = QO_EMPTY;
															printf("before2\n");
															/* gencode to jump back to the Marker to test if the counter reached its max value */
															gencode(qo, qo, qo, Q_GOTO, global_code[nextquad].label, $8, NULL);

															/* we save the incomplete GOTOs of the comparison of the loop counter, and the break instructions. 
																They do the jump ouy of the loop, but we don't know hwere that is yet. 
																It is completed in a more global context (L) above. 
																We might also have incomplete returns, we retrieve there address aswell.
															 */
															 
															$$.next = crelist($8);
															$$.next = concat($$.next, $10.brk);
															$$.rtrn = concat($$.rtrn, $10.rtrn);
															popctx(); 
														 }

			|	Return return_val ';'					 {															
															/* Incomplete GOTO to jump to end of function*/
															$$.rtrn = crelist(nextquad);
															quadop q1;
															q1.type = QO_CST;
															q1.u.cst = $2.type;
															
															gencode($2.result, q1, q1, Q_RETURN, global_code[nextquad].label, -1, NULL);
															
														}
			|	Break ';'								 { 
															if(!is_a_parent(CTX_FOR)) 
																yyerror("\nERREUR: Break; doit être au sein d'une boucle FOR\n");
															quadop qo;
															qo.type = QO_EMPTY;
															/* Incomplete GOTO to jump out of the for loop*/
															$$.brk = crelist(nextquad);
															gencode(qo,qo,qo,Q_GOTO,global_code[nextquad].label,-1,NULL);
														 }
			|	Continue ';'							 { 
															if(!is_a_parent(CTX_FOR)) 
																yyerror("\nERREUR: Continue; doit être au sein d'une boucle FOR\n");
															quadop qo;
															qo.type = QO_EMPTY;
															/* Incomplete GOTO to jump to the start of the loop*/
															$$.cntu = crelist(nextquad);
															gencode(qo,qo,qo,Q_GOTO,global_code[nextquad].label,-1,NULL);
			
														 }
			|	block									 { $$.next = $1.next ;}

return_val	:	expr 									{	$$ = $1;}
			|	%empty									{	$$.type = VOIDTYPE; $$.result.type = QO_EMPTY;}


method_call :	id '(' E ')' 					{
													item_table *val = lookup($1);
													if(val == NULL)
														yyerror("Erreur: Méthode non déclarée\n");
													if(val->item->id_type != ID_METHOD)
														yyerror("\nErreur: l'ID utilisé n'est pas celui d'une méthode\n");
													/* retrieving return type */
													 $$.type = val->item->value; 

													/* verifiying type and number of parameters */
													if(!verify_param(val->item->p, $3))
														yyerror("Erreur: Appel de méthode avec paramètres incorrectes\n");

													quadop qo;
													qo.type = QO_EMPTY;
													gencode(qo,qo,qo, Q_METHODCALL, global_code[nextquad].label, -1, $3);
												}
			|	id '(' ')'						{
													item_table *val = lookup($1);
													if(val == NULL)
														yyerror("Erreur: Méthode non déclarée\n");

													/* retrieving return type */
													 $$.type = val->item->value; 

													/* verifiying parameters */
													if(!verify_param(val->item->p, NULL))
														yyerror("Erreur: Appel de méthode avec paramètres incorrectes\n");
													quadop qo;
													qo.type = QO_EMPTY;
													gencode(qo,qo,qo, Q_METHODCALL, global_code[nextquad].label, -1, NULL);
													
												}

/*
	 E is a list of parameters of a method call 
*/
E 			:	expr ',' E 						{ 
													param p = (param) malloc(sizeof(struct param)); 
													p->type = $1.type; 
													p->arg = $1.result; 
													p->next = $3; 
													$$ = p;
													if($1.type == BOOL){
														quadop qo;

														qo.type = QO_CST;
														qo.u.cst = true;
														complete($1.t, nextquad);
														gencode($$->arg, qo, qo, Q_AFF, global_code[nextquad].label, -1, NULL);

														qo.type = QO_EMPTY;
														gencode(qo, qo, qo, Q_GOTO, global_code[nextquad].label, nextquad+2, NULL);

														qo.type = QO_CST;
														qo.u.cst = false;
														complete($1.f, nextquad); 
														gencode($$->arg, qo, qo, Q_AFF, global_code[nextquad].label, -1, NULL);
													}
												}
			|	expr 							{ 
													$$ = (param) malloc(sizeof(struct param)); 
													$$->type = $1.type; 
													$$->arg = $1.result; 
													$$->next = NULL;

													if($1.type == BOOL){
														quadop qo;

														qo.type = QO_CST;
														qo.u.cst = true;
														complete($1.t, nextquad);
														gencode($$->arg, qo, qo, Q_AFF, global_code[nextquad].label, -1, NULL);
													
														qo.type = QO_EMPTY;
														gencode(qo, qo, qo, Q_GOTO, global_code[nextquad].label, nextquad+2, NULL);

														qo.type = QO_CST;
														qo.u.cst = false;
														complete($1.f, nextquad); 
														gencode($$->arg, qo, qo, Q_AFF, global_code[nextquad].label, -1, NULL);
													}
												}

location	:	id				{
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
									if(val->item->value == BOOL) {
										quadop qo;
										qo.type = QO_CST;
										qo.u.cst = true;
										$$.t = crelist(nextquad);
										gencode($$.result, $$.result, qo, Q_EQ, global_code[nextquad].label, -1, NULL);
										$$.f = crelist(nextquad);
										qo.type = QO_EMPTY;
										qo.u.cst = 0;
										gencode(qo,qo,qo, Q_GOTO, global_code[nextquad].label, -1, NULL); 
									}	
								}
			|	id '[' expr ']'			{ 	/* expr de type int */ 
											if($3.type != INT)
												yyerror("\nErreur: indice de tableau doit être de type INT\n");

											/* on cherche id dans le contexte global*/
											item_table *val = lookup($1);
											if(val == NULL)
												yyerror("\nErreur: Accès à un tableau non déclaré\n");

											else if(val->item->id_type != ID_TAB )
												yyerror("\nErreur: Accès à l'indice d'un identificateur qui n'est pas un tableau\n");
											/*	TO DO: vérification dynamique:
												yyerror("\nErreur: Indice d'accès dépace la taille du tableau\n");*/

											else {
												$$.type = val->item->value; 
												$$.result.type = QO_GLOBAL;
												$$.result.u.global.name = malloc(strlen($1)+1);
												strcpy($$.result.u.global.name, $1); 
												quadop q1;
												q1.type = QO_CST;
												/* TO DO: Vérification dynamique offset*/
												q1.u.cst = 4*2;
												gencode($$.result, q1, q1, Q_ACCESTAB, global_code[nextquad].label, -1, NULL);
											}
												
										}

expr		:	expr add_op expr %prec '+'	{
												if($1.type != INT || $3.type != INT)
													yyerror("\nErreur: Arithmètique non entière");
												$$.type = INT; 
												Ht_item* item = new_temp(INT);
												quadop qo;
												qo.type = QO_TMP;
												qo.u.offset = 0;
												gencode(qo,qo,qo,Q_DECL,global_code[nextquad].label,-1, NULL);
												qo.u.offset = 0;

												if($1.result.type == QO_ID || $1.result.type == QO_TMP)
													$1.result.u.offset += 4;
												if($3.result.type == QO_ID || $3.result.type == QO_TMP)
													$3.result.u.offset += 4;

												gencode(qo,$1.result,$3.result,$2,global_code[nextquad].label,-1, NULL);
												$$.result = qo;
											}
			|	expr mul_op expr %prec '*'	{	if($1.type != INT || $3.type != INT)
													yyerror("\nErreur: Arithmètique non entière");
												$$.type = INT; 
												Ht_item* item = new_temp(INT);
												quadop qo;
												qo.type = QO_TMP;
												qo.u.offset = 0;
												gencode(qo,qo,qo,Q_DECL,global_code[nextquad].label,-1, NULL);
												qo.u.offset = 0;

												if($1.result.type == QO_ID || $1.result.type == QO_TMP)
													$1.result.u.offset += 4;
												if($3.result.type == QO_ID || $3.result.type == QO_TMP)
													$3.result.u.offset += 4;

												gencode(qo,$1.result,$3.result,$2,global_code[nextquad].label,-1, NULL);
												$$.result = qo;
											}
			|	expr and M expr				{	if($1.type != BOOL || $4.type != BOOL)
													yyerror("\nErreur: AND operator with non boolean value");
												$$.type = BOOL;
												complete($1.t, $3);
												$$.f = concat($1.f, $4.f);
												$$.t = $4.t; 
											}

			|	expr or M expr				{	if($1.type != BOOL || $4.type != BOOL)
													yyerror("\nErreur: OR operator with non boolean value");
												$$.type = BOOL;
												complete($1.f, $3);
												$$.t = concat($1.t, $4.t);
												$$.f = $4.f;
											}
			|	expr oprel expr	%prec '<' 	{
												if($1.type != INT || $3.type != INT)
													yyerror("\nErreur: REL OP non entière");
												$$.type = BOOL;
												$$.t = crelist(nextquad);
												quadop qo;
												qo.type = QO_EMPTY;
												qo.u.cst = 0;
												gencode(qo, $1.result, $3.result, $2, global_code[nextquad].label, -1, NULL);
												$$.f = crelist(nextquad);
												gencode(qo, qo, qo, Q_GOTO, global_code[nextquad].label, -1, NULL);
											}
			|	expr eq_op expr	%prec eq	{	
												if($1.type != $3.type )
													yyerror("\nErreur: Comparaison de types différents");
												$$.type = BOOL; 
												quadop qo;
												qo.type = QO_EMPTY;
												qo.u.cst = 0;											
												$$.t = crelist(nextquad);
												gencode(qo,$1.result,$3.result,$2,global_code[nextquad].label, -1, NULL);
												$$.f = crelist(nextquad);
												gencode(qo, qo, qo, Q_GOTO, global_code[nextquad].label, -1, NULL);
											}
			| 	literal 					{
												$$ = $1;
												
												if($1.type == BOOL){
													quadop qo;
													qo.type = QO_CST;
													qo.u.cst = true;
													$$.t = crelist(nextquad);
													gencode($$.result, $$.result, qo, Q_GEQ, global_code[nextquad].label, -1, NULL);
													$$.f = crelist(nextquad);
													qo.type = QO_EMPTY;
													qo.u.cst = 0;
													gencode(qo,qo,qo, Q_GOTO, global_code[nextquad].label, -1, NULL); 
												}
											}
			|	location 					{
												
												$$ = $1;
											}
			|	'-' expr %prec NEG 			{
												if($2.type != INT)
													yyerror("\nErreur: Arithmètique non entière");
												$$.type = INT;
												Ht_item* item = new_temp(INT);
												quadop qo;
												qo.type = QO_TMP;
												qo.type = QO_TMP;
												qo.u.offset = 0;
												gencode(qo, qo, qo, Q_DECL, global_code[nextquad].label,-1, NULL);
												qo.u.offset = 0;
												if($2.result.type == QO_ID || $2.result.type == QO_TMP)
													$2.result.u.offset +=4 ;											
												gencode(qo, $2.result, $2.result, Q_SUB, global_code[nextquad].label,-1, NULL);
												$$.result = qo;
											}
			|	'!' expr %prec NEG 			{	
												if($2.type != BOOL)
													yyerror("\nErreur: NOT operator with non boolean value");
												$$.type = BOOL; 
												$$.t = $2.f;
												$$.f = $2.t;
												}
			|	'(' expr ')' 				{
												$$ = $2;
											}
			|	method_call					{ 
												/* A method with void return type can't be used as an expression*/
												if($1.type == VOIDTYPE)
													yyerror("Erreur: méthode de type de retour void utilisée comme expression\n");
												$$ = $1;
											}
			|	string_literal				{	
												/* This is only used for Write_String( string_literal ) */
												$$.type = STRING;
												$$.result.type = QO_CSTSTR;
												$$.result.u.string_literal.label = new_str();
												$$.result.u.string_literal.value = malloc(strlen($1)+1);
												strcpy($$.result.u.string_literal.value, $1);
											}

literal		:	int_literal					{
												$$.result.type = QO_CST;
												$$.result.u.cst = $1;
												$$.type = INT;
											}
			|	char_literal				{
												$$.result.type = QO_CST;
												$$.result.u.cst = $1;
												$$.type = INT;
											}
			|	bool_literal				{
												$$.result.type = QO_CST;
												$$.result.u.cst = $1;
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
