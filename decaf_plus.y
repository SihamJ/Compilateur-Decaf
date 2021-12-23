%{	
	#include "token.h"
	#include "intermediaire.h"
	#include "hashtable.h"
	#include "IOfunctions.h"
	#include "text_formating.h"
	#include "decaf_plus.tab.h"
	int yylex();
	void yyerror( char* );
	void yywarning(char *);
	char* program_name;
%}

%union {
	int intval;
	char *stringval;

	struct expr_val {
		quadop result;
		quadop offset;	// for accessing or writing in arrays	
		int type;		// type of result operand: INT / BOOL / STRING
		list next;		
		list t;  		// true
		list f;   		// false
		list brk; 		// break
		list cntu; 		// continue
		list rtrn;		// return
		int return_type; // if expr_val is a method
	} expr_val;

	struct literal{
		int intval;
		int type;
	}literal;

	struct decl{
		global_type type;
		char *name;
		struct decl *suiv;
		int size;
	} decl;

	param p;  // List for storing method parameters at the declaration or method call. Also pointed at by the Symbol Table (declaration), and the Quads (method_call).

}

%token <intval> decimal_literal hex_literal char_literal bool_literal eq neq and or not leq geq aff_add aff_sub integer boolean voidtype '+' '-' '%' '/' '<' '>' '=' '!' '*'
%token <stringval> id string_literal 
%token class If Else For Return Break Continue 

%type <expr_val> expr S S2 G block statement method_call location return_val 
%type <intval> int_literal assign_op type M oprel eq_op add_op mul_op
%type <decl> B glob_id 
%type <p> E Param P
%type <literal> literal

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
								/* pushing class name to global table*/
								program_name = malloc(strlen($2)+1);
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
								popctx(); /* End of the program, we pop the global context.*/
								return 0;
							}

/* Marks the address of the next quad to be generated */
M 		:	%empty 		{	$$ = nextquad; }


/* GOTO adress of next quad to be generated. Similar to the marker with an added jump.*/
G 		:	%empty		{	$$.next = crelist(nextquad);
							quadop qo;
							qo.type = QO_EMPTY;
							qo.u.cst = 0;
							gencode(qo,qo,qo, Q_GOTO, NULL, -1, NULL);
						}

/* The structure of the global context follows either of the two:
	MD: only contains method declarations
or	FD MD: starts with global variable declarations followed by method declarations  */
GLOBAL 		: MD		{;}
			| FD MD 	{;}

/* Block de déclarations de variables globales */
FD 			: FD field_decl  {;}
			| field_decl	 {;}

field_decl	:	type glob_id ';' 							{ 
															quadop qo,q1;
															struct decl *pt = &$2;
															qo.type = QO_GLOBAL;
															/* glob_id is a list of all arrays and global variables declared of same type, see rule below*/
															while(pt != NULL){

																/* verifying that the ID does not already exist in the current context*/
																if( ht_search(glob_context, pt->name) != NULL ) 
																	yyerror("\nErreur: Tableau déclarée deux fois\n");
																if(!strcmp(pt->name, next_label_name()))
																	labelCount++;
																
																/* declaration of the global id in the quad */
																qo.u.global.name = malloc(strlen(pt->name + 1));
																strcpy(qo.u.global.name, pt->name);
																qo.u.global.size = pt->size;
																qo.u.global.type = pt->type;
																gencode(qo, qo, qo, Q_DECL, NULL, -1, NULL);

																/* we increment the global variables counter, useful for MIPS*/
																glob_dec_count++;

																/* we store the id in the global symbol table*/
																int type;
																if(pt->type == QO_TAB)
																	type = ID_TAB;
																else if(pt->type == QO_SCAL)
																	type = ID_VAR;
																Ht_item *item = create_item(pt->name, type, $1);
																item->size = pt->size;
																newname(item);
																printf("%s: %d\n",item->key,item->id_type);
																pt = pt->suiv;
															}
														}

glob_id			:	id '[' int_literal ']' ',' glob_id	{	if($3 <= 0) 
																yyerror("\nErreur: Taille du tableau déclarée inférieure ou égale à 0\n");
															struct decl var;
															var.name = malloc((strlen($1)+1)); 
															strcpy(var.name,$1);
															var.size = $3*4;
															var.suiv = &$6;
															var.type = QO_TAB;
															$$ = var;
															
														}
				|	id '[' int_literal ']'					{
															if($3 <= 0) 
																yyerror("\nErreur: Taille du tableau déclarée inférieure ou égale à 0\n");															
															struct decl var;
															var.name = malloc((strlen($1)+1)); 
															strcpy(var.name,$1);
															var.size = $3*4;
															var.suiv = NULL;
															var.type = QO_TAB;
															$$ = var;
														}
				|	id									{	
															struct decl var;
															var.name = malloc(strlen($1)+1);
															strcpy(var.name, $1);
															var.size = 4;
															var.suiv = NULL;
															var.type = QO_SCAL;
															$$ = var;
														}
				|	id	','	glob_id						{
															struct decl var;
															var.name = malloc((strlen($1)+1)); 
															strcpy(var.name,$1);
															var.size = 4;
															var.suiv = &$3;
															var.type = QO_SCAL;
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
															else if(global_code[$8.rtrn->addr].op2.u.cst != $1){
																yyerror("\nErreur: Méthode avec faux type de retour\n");
																return 1;
															}
															/*
																we get to the end of the method declaration. If there was a GOTO somewhere in the block to jump out of the
																block, now we know where the end of the block is, it's necessarily the adress of the next quad of end function
																because this block is at the end of a method declaration. Same thing for the return jumps.
															*/
															complete($8.next,nextquad);	
															complete($8.rtrn, nextquad);
															/* 
															We generate a quad that indicates the end of the method declaration, this is useful for MIPS
															for popping the stack and retrieving register ra. Notice that we store the number of variables to pop
															in qo.
															*/
																										
															quadop qo;
															qo.type = QO_CST;
															qo.u.cst = curr_context->count*4;
															gencode(qo, qo, qo, Q_ENDFUNC, new_endfunc_label($2), -1, $5);
															popctx(); }

			/* Same as above, except the type is void method. We are forced to separete them because of Bison conflicts with types of variable declarations*/
			|	voidtype id {	if(ht_search(glob_context, $2) != NULL){
									yyerror("\nErreur: Méthode déjà déclarée avec ce nom\n");
									return 1;
								}
								if(!strcmp($2, next_label_name()))
									labelCount++;
								Ht_item *item = create_item($2, ID_METHOD, $1);
								newname(item);
								pushctx(CTX_METHOD); 
								global_code[nextquad].label = malloc(strlen($2)+1);
								strcpy(global_code[nextquad].label,$2);
								} 
							
								'(' P ')'  block	{ 		

															if($7.rtrn != NULL && global_code[$7.rtrn->addr].op2.u.cst != $1){
																yyerror("\nErreur: Méthode avec faux type de retour\n");
																return 1;
															}

															item_table* var = lookup($2);
															var->item->p = $5;
															complete($7.next, nextquad);
															complete($7.rtrn, nextquad);	

															if(!strcmp($2,"main")){
																if($5 != NULL){
																	yyerror("\nErreur: Main ne prends pas de paramètres\n");
																	return 1;
																}
																quadop qo; 
																qo.type = QO_CST; 
																qo.u.cst = 10; 
																gencode(qo,qo,qo, Q_SYSCALL, "end", -1, NULL);
															} 	

															else {													
																quadop qo;
															qo.type = QO_CST;
															qo.u.cst = curr_context->count;
																gencode(qo, qo, qo, Q_ENDFUNC, new_endfunc_label($2), -1, $5);
															}					
															popctx();
														}

/* Param is a non empty list of method parameters*/
P 			:	Param 	{ $$ = $1;}
			|	%empty	{ $$ = NULL;}


/* Param is a list that retrieves the the names of declared parameters and their types */
Param		:	type id ',' Param	{ 	
										/* we verify that we don't declare two parameters with the same ID*/
										if( ht_search(curr_context, $2) != NULL ) {
											yyerror("\nErreur: Deux paramètres de méthodes de même nom\n");
											return 1;
										}
										
										/* we store the parameter in the symbol table*/
										Ht_item *item = create_item($2, ID_PARAM, $1);
										newname(item);
										param p = (param) malloc(sizeof(struct param));
										p->type = $1;
										p->next = $4;
										$$ = p;
										}
			/* Idem*/
			|	type id				{ 	if( ht_search(curr_context, $2) != NULL ) {
											yyerror("\nErreur: Deux paramètres de méthodes de même nom\n");
											return 1;
										}
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

						/* If we declared any variables in this block, we should pop them from the stack,
							and tell MIPS to do the same.
						*/
						if(curr_context->count > 0){
							quadop qo;
							qo.type = QO_CST;
							qo.u.cst = curr_context->count*4;
							gencode(qo, qo, qo, Q_POP, NULL, -1, NULL);
						}
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
										if( ht_search(glob_context, pt->name) != NULL ){
											yyerror("\nErreur: Variable déclarée deux fois\n");
											return 1;
										}
										/* to avoid having global variables that have the same name as an auto-generated label*/
										if(!strcmp(pt->name, next_label_name()))
											labelCount++;

										/* we store the ID in a quadop and we generate a declaration quad with gencode*/
										qo.u.global.name = malloc(strlen(pt->name + 1));
										strcpy(qo.u.global.name, pt->name);
										qo.u.global.size = 4;
										gencode(qo, qo, qo, Q_DECL, NULL, -1, NULL);

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
										if( ht_search(curr_context, pt->name) != NULL ) {
											yyerror("\nErreur: Variable déclarée deux fois\n");
											return 1;
										}
										if(!strcmp(pt->name, next_label_name()))
											labelCount++;
										qo.u.offset = 0;
										gencode(qo, qo, qo, Q_DECL, NULL, -1, NULL);
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


statement 	:	location assign_op expr ';' {		/* Affectation */
													/* Verifying types*/
													if($1.type != $3.type){
														yyerror("\nErreur: Type de valeur incorrecte\n");
														return 1;
													}
													if (($2 == Q_AFFADD || $2 == Q_AFFSUB) && ($1.type == BOOL || $3.type == BOOL)){
														yyerror("\nErreur: Type de valeur incorrecte\n");
														return 1;
													}

													quadop qo,q1;

													/* If location is an element in an array, we need to retrieve the offset from 
														the attribute 'quadop offset' and store it in q1. If not, we use q1 as an empty quadop
														in the AFF below.
													*/
													if($1.result.type == QO_TAB)
														q1 = $1.offset;
													else{
														q1.type = QO_EMPTY;
													}
															
													/* we have to update the offsets after generating new tmp*/
													if(curr_context!=glob_context)
														$1.result.u.offset += 4*tmpCount;

													if($3.type == BOOL) {
														/*
															If the affectation is of bOOL type, we have to create two quads. One is for false affectation (0) and
															the other one for true (1). The rules on expr already generate incomplete GOTOs. At this stage we complete
															them with the address of the 2 quads we generated. The true attribute of expr is completed with the adress
															of the true affectation, and the false attribute is completed with the address of the false affectation.
														*/

														/* True affectation */
														qo.type = QO_CST;
														qo.u.cst = true;
														complete($3.t, nextquad);
														
														gencode($1.result, qo, q1, Q_AFF, NULL, -1, NULL); 	

														/* To skip false affectation. Incomplete GOTO because we don't know yet where to skip.
															We add it to $$.next
														*/
														qo.type = QO_EMPTY;
														$$.next = crelist(nextquad);
														gencode(qo, qo, qo, Q_GOTO, NULL, -1, NULL);

														/* False affectation*/
														qo.type = QO_CST;	
														qo.u.cst = false;		
														complete($3.f, nextquad);											
														gencode($1.result, qo, q1, Q_AFF, NULL, -1, NULL);	
													}
												
													else {
														/* If it's an affectation of type INT, we simply generate the corresponding quad*/
														/* in the translater, we have to differentiate between = += and -=
															If $1 is an array, the offset is q1. If not, q1 is empty.
														 */
														gencode($1.result,$3.result,q1,$2, NULL,-1, NULL);
													}
													/* we pop the temporary variables from the stack at the end of the expression evaluation if we have any.
														we store the number of temporary variables to pop in qo. The counter is initialized in 
														pop_tmp().
													*/			
													if(tmpCount > 0){
														quadop qo;
														qo.type = QO_CST;
														qo.u.cst = tmpCount*4;
														gencode(qo, qo, qo, Q_POP, NULL, -1, NULL);
													}
													pop_tmp();
												}

			/* The method call rules are defined below not here.*/
			|	method_call	';'								{ $$=$1; }

			|	If '(' expr ')' M block G Else M block	{	
															/* Verifying types*/
															if($3.type != BOOL) {
																yyerror("\nErreur: Test avec expression non booléene\n");
																return 1;
															}
															
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
															$$.next = concat($7.next, $10.next);
															$$.next = concat($$.next, $6.next);

															/* 
																If our IF statement is within a FOR loop, we might have "break" and "continue" instructions
																that produce GOTOs. However, we don't know yet where the start and the end of this FOR loop is,
																this will be resolved in the FOR loop rules for the "continue" statement, and in the S rules for
																the "break" statement.
															*/
															$$.cntu = concat($6.cntu, $10.cntu);
															$$.brk = concat($6.brk, $10.brk);
															$$.rtrn = concat($6.rtrn,$10.rtrn);
														}
			|	If '(' expr ')' M block 				{
															/* IDEM but without ELSE*/
															if($3.type != BOOL) {
																yyerror("\nErreur: Test avec expression non booléene\n");
																return 1;
															}
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
															if($4.type != INT || $6.type != INT){ 
																yyerror("\nErreur: le compteur de boucle doit être de type INT\n");
																return 1;
															}
															/* To avoid having variables and labels holding the same name*/
															if(!strcmp($2, next_label_name()))
																labelCount++;

															/* We declare the loop counter id, push it to the symbol table, and store the initial value expr1 in it*/
															quadop qo;
															qo.type = QO_ID;
															qo.u.offset = 0;
															gencode(qo, qo, qo, Q_DECL, NULL, -1, NULL);
															Ht_item *item = create_item($2, ID_VAR, INT);
															newname(item);
															qo.type = QO_ID;
															qo.u.offset = 0;
															gencode(qo, $4.result, $4.result, Q_AFF, NULL, -1, NULL); 
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
															gencode(qo, qo, $6.result, Q_GT, NULL, -1 , NULL); } 
					
					block 								{   /* This is happening at the end of the loop block,
																next instruction of the block is the incrementation of the loop counter 
																that's also where continue should take us, so we complete both.
															*/
															complete($10.next, nextquad);
															complete($10.cntu, nextquad);
															/* gencode to increment the loop counter*/
															quadop qo, q1;
															qo.type = QO_ID;
															q1.type = QO_CST;
															q1.u.cst = 1;

															/* offset of id is 0 because it's the only variable in the context at this point */
															qo.u.offset = 0;
															gencode(qo, qo, q1, Q_ADD, NULL, -1, NULL);
															qo.type = QO_EMPTY;
															/* gencode to jump back to the Marker to test if the counter reached its max value */
															gencode(qo, qo, qo, Q_GOTO, NULL, $8, NULL);

															/* we save the incomplete GOTOs of the comparison of the loop counter, and the break instructions. 
																They do the jump ouy of the loop, but we don't know hwere that is yet. 
																It is completed in a more global context (L) above. 
																We might also have incomplete returns, we retrieve there address aswell.
															 */
															 
															$$.next = crelist($8);
															$$.next = concat($$.next, $10.brk);
															$$.rtrn = concat($$.rtrn, $10.rtrn);

															/* We pop the FOR counter context (i.e the loop counter variable only)
															*/
															qo.type = QO_CST;
															qo.u.cst = curr_context->count*4; 		// should be equal to 1
															gencode(qo,qo,qo, Q_POP, NULL, -1, NULL);
															popctx(); 
														 }

			|	Return return_val ';'					 {															
															/* Incomplete GOTO to jump to end of function*/
															$$.rtrn = crelist(nextquad);

															quadop q1;
															q1.type = QO_CST;
															q1.u.cst = $2.type;
															quadop q2;
															q2.type = QO_CST;
															q2.u.cst = curr_context->count*4;
															gencode($2.result, q1, q2, Q_RETURN, NULL, -1, NULL);
															
														}
			|	Break ';'								 { 
															if(!is_a_parent(CTX_FOR)) {
																yyerror("\nErreur: Break; doit être au sein d'une boucle FOR\n");
																return 1;
															}
															quadop qo;
															qo.type = QO_EMPTY;
															/* Incomplete GOTO to jump out of the for loop*/
															$$.brk = crelist(nextquad);
															gencode(qo,qo,qo,Q_GOTO,NULL,-1,NULL);
														 }
			|	Continue ';'							 { 
															if(!is_a_parent(CTX_FOR)) {
																yyerror("\nErreur: Continue; doit être au sein d'une boucle FOR\n");
																return 1;
															}
															quadop qo;
															qo.type = QO_EMPTY;
															/* Incomplete GOTO to jump to the start of the loop*/
															$$.cntu = crelist(nextquad);
															gencode(qo,qo,qo,Q_GOTO,NULL,-1,NULL);
			
														 }
			|	block									 { $$.next = $1.next ;}

return_val	:	expr 									{	$$ = $1;}
			|	%empty									{	$$.type = VOIDTYPE; 
															$$.result.type = QO_EMPTY;
														}


method_call :	id '(' E ')' 					{
													item_table *val = lookup($1);
													if(val == NULL){
														yyerror("\nErreur: Méthode non déclarée\n");
														return 1;
													}
													if(val->item->id_type != ID_METHOD) {
														yyerror("\nErreur: l'ID utilisé n'est pas celui d'une méthode\n");
														return 1;
													}
													
													/* retrieving return type */
													 $$.type = val->item->value; 

													/* verifiying type and number of parameters */
													if(!verify_param(val->item->p, $3)){
														yyerror("\nErreur: Appel de méthode avec paramètres incorrectes\n");
														return 1;
													}

													if(!strcmp($1,"WriteString")){
														str_labels[str_count-1].label = malloc(strlen($3->arg.u.string_literal.label)+1);
														strcpy(str_labels[str_count-1].label,$3->arg.u.string_literal.label);
														str_labels[str_count-1].value = malloc(strlen($3->arg.u.string_literal.value)+1);
														strcpy(str_labels[str_count-1].value,$3->arg.u.string_literal.value);
													}

													quadop qo;
													qo.type = QO_CSTSTR;
													qo.u.string_literal.label = malloc(strlen($1)+1);
													strcpy(qo.u.string_literal.label,$1);
													gencode(qo,qo,qo, Q_METHODCALL, NULL, -1, $3);
												}
			|	id '(' ')'						{
													item_table *val = lookup($1);
													if(val == NULL){
														yyerror("\nErreur: Méthode non déclarée\n");
														return 1;
													}
													if(val->item->id_type != ID_METHOD) {
														yyerror("\nErreur: l'ID utilisé n'est pas celui d'une méthode\n");
														return 1;
													}
													/* retrieving return type */
													 $$.type = val->item->value; 

													/* verifiying parameters */
													if(!verify_param(val->item->p, NULL)){
														yyerror("\nErreur: Appel de méthode avec paramètres incorrectes\n");
														return 1;
													}
													quadop qo;
													qo.type = QO_CSTSTR;
													qo.u.string_literal.label = malloc(strlen($1)+1);
													strcpy(qo.u.string_literal.label,$1);
													gencode(qo,qo,qo, Q_METHODCALL, NULL, -1, NULL);
													
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
														gencode($$->arg, qo, qo, Q_AFF, NULL, -1, NULL);

														qo.type = QO_EMPTY;
														gencode(qo, qo, qo, Q_GOTO, NULL, nextquad+2, NULL);

														qo.type = QO_CST;
														qo.u.cst = false;
														complete($1.f, nextquad); 
														gencode($$->arg, qo, qo, Q_AFF, NULL, -1, NULL);
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
														gencode($$->arg, qo, qo, Q_AFF, NULL, -1, NULL);
													
														qo.type = QO_EMPTY;
														gencode(qo, qo, qo, Q_GOTO, NULL, nextquad+2, NULL);

														qo.type = QO_CST;
														qo.u.cst = false;
														complete($1.f, nextquad); 
														gencode($$->arg, qo, qo, Q_AFF, NULL, -1, NULL);
													}
												}

location	:	id				{
									item_table *val = lookup($1);
									if(!val){
										yyerror("\nErreur: Variable non déclarée\n");
										return 1;
									}
									if(val->item->id_type == ID_TAB) {
										yyerror("\nErreur: Accès tableau comme scalaire\n");
										return 1;
									}
									if(val->table == glob_context) {
										$$.result.type = QO_GLOBAL;
										$$.result.u.global.type = QO_SCAL;
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
			|	id '[' expr ']'			{ 	/* expr de type not int */ 
											if($3.type != INT){
												yyerror("\nErreur: indice de tableau doit être de type INT\n");
												return 1;
											}

											/* on cherche id dans la TOS*/
											item_table *val = lookup($1);
											if(val == NULL) {
												yyerror("\nErreur: Accès à un tableau non déclaré\n");
												return 1;
											}
											printf("%s: %d\n",$1,val->item->id_type);
											 if(val->item->id_type != ID_TAB ) {
												yyerror("\nErreur: Accès à l'indice d'un identificateur qui n'est pas un tableau\n");
												return 1;
											}

											$$.type = val->item->value; 
											$$.result.type = QO_GLOBAL;
											$$.result.u.global.type = QO_TAB;
											$$.result.u.global.name = malloc(strlen($1)+1);
											strcpy($$.result.u.global.name, $1); 
											$$.offset = $3.result;
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
												gencode(qo,qo,qo,Q_DECL,NULL,-1, NULL);

												if($1.result.type == QO_ID || $1.result.type == QO_TMP)
													$1.result.u.offset += 4;
												if($3.result.type == QO_ID || $3.result.type == QO_TMP)
													$3.result.u.offset += 4;

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
												gencode(qo,qo,qo,Q_DECL,NULL,-1, NULL);

												if($1.result.type == QO_ID || $1.result.type == QO_TMP)
													$1.result.u.offset += 4;
												if($3.result.type == QO_ID || $3.result.type == QO_TMP)
													$3.result.u.offset += 4;

												gencode(qo,$1.result,$3.result,$2,NULL,-1, NULL);
												$$.result = qo;
											}
			|	expr and M expr				{	if($1.type != BOOL || $4.type != BOOL){
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
												$$.t = crelist(nextquad);
												quadop qo;
												qo.type = QO_EMPTY;
												qo.u.cst = 0;
												gencode(qo, $1.result, $3.result, $2, NULL, -1, NULL);
												$$.f = crelist(nextquad);
												gencode(qo, qo, qo, Q_GOTO, NULL, -1, NULL);
											}
			|	expr eq_op expr	%prec eq	{	
												if($1.type != $3.type ){
													yyerror("\nErreur: Comparaison de types différents");
													return 1;
												}

												if($1.type == BOOL){
													complete($1.t, nextquad+1);
													complete($1.f, nextquad+1);
												}
												if($3.type == BOOL){
													complete($3.t, nextquad+1);
													complete($3.f, nextquad+1);
												}

												quadop qo;
												Ht_item* item = new_temp(INT);
												qo.type = QO_TMP;
												qo.u.offset = 0;
												gencode(qo,qo,qo,Q_DECL,NULL,-1, NULL);

												if($1.result.type == QO_ID || $1.result.type == QO_TMP)
													$1.result.u.offset += 4;
												if($3.result.type == QO_ID || $3.result.type == QO_TMP)
													$3.result.u.offset += 4;


												$$.type = BOOL; 
												qo.type = QO_EMPTY;									
												gencode(qo,$1.result,$3.result,$2,NULL, nextquad+2, NULL);

												quadop q1;
												q1.type = QO_EMPTY;
												$$.t = crelist(nextquad);		
												gencode(q1,q1,q1, Q_GOTO, NULL,nextquad+3, NULL);

												q1.type = QO_CST;
												q1.u.cst = 1;
												gencode(qo, q1, q1, Q_AFF, NULL, -1, NULL);
												
												q1.type = QO_EMPTY;
												$$.t = crelist(nextquad);		
												gencode(q1,q1,q1, Q_GOTO, NULL, -1, NULL);

												q1.type = QO_CST;
												q1.u.cst = 0;
												gencode(qo, q1, q1, Q_AFF, NULL, -1, NULL);

												$$.result = qo;

												q1.type = QO_EMPTY;
												$$.f = crelist(nextquad);
												gencode(q1, q1, q1, Q_GOTO, NULL, -1, NULL);
												
											}
			| 	literal 					{
												$$.type = $1.type;
												$$.result.type = QO_CST;
												$$.result.u.cst = $1.intval;
											}
											
			|	location 					{
												if($1.result.type == QO_GLOBAL && $1.result.u.global.type == QO_TAB){
													$$.type = $1.type;
													Ht_item* item = new_temp(INT);
													quadop qo;
													qo.type = QO_TMP;
													qo.u.offset = 0;
													gencode(qo,qo,qo,Q_DECL,NULL,-1, NULL);
													$1.offset.u.offset += 4;
													gencode(qo, $1.result, $1.offset, Q_ACCESTAB, NULL, -1, NULL);
													$$.result = qo;
												}
												else 
													$$ = $1;
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

												if($2.result.type == QO_ID || $2.result.type == QO_TMP)
													$2.result.u.offset +=4 ;											
												gencode(qo, $2.result, $2.result, Q_SUB, NULL,-1, NULL);
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
												if($1.type == VOIDTYPE){
													yyerror("\nErreur: méthode de type de retour void utilisée comme expression\n");
													return 1;
												}
												$$ = $1;
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