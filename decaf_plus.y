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
	bool boolval;
	char *stringval;

	struct {
		quadop qop;
		int type;
	} literal;

	struct expr_val {
		quadop result;	
		int type;		// type de l'opérande result: INT / BOOL / STRING
		char *str; 		// si string_literal
		list next;	
		list t;  		// true
		list f;   		// false
		list brk; 		// break
		list cntu; 		// continue
	} expr_val;

	struct decl{
		char *name;
		struct decl *suiv;
		int size;
	} decl;

	param p;  // Liste pour récupérer les paramètres de méthodes à la déclaration et appel. Stocké aussi dans la TOS (déclaration) et les quadruplets (appel)

}

%token <intval> decimal_literal hex_literal char_literal eq neq and or not leq geq aff_add aff_sub integer boolean voidtype '+' '-' '%' '/' '<' '>' '=' '!' '*'
%token <boolval> bool_literal
%token <stringval> id string_literal
%token class Program If Else For Return comment Break Continue

%type <expr_val> expr S G block statement method_call
%type <intval> int_literal assign_op type M oprel eq_op add_op mul_op
%type <literal> literal
%type <decl> B Tab
%type <stringval> location
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

program	:  class Program '{' { 	pushctx(CTX_GLOB); /* Push le contexte global*/
								add_libs_to_tos(); /* Ajoute les fonctions de bibliothèques I/O à la TOS globale*/
								glob_context = curr_context; /* glob_context pointera sur ce contexte global tout au long de l'éxecution.*/
							}

						
				GLOBAL '}' { 
								if( ht_search( glob_context,"main") == NULL )	/* Vérifie qu'une méthode main existe bien dans la TOS globale*/
									yyerror("\nErreur: Pas de méthode main\n");
								popctx(); /* On arrive à la fin du programme, le contexte global et toutes les TOS sont dépilées*/
							}

/* Marqueur sur l'addresse du prochain quadruplet */
M 		:	%empty 		{	$$ = nextquad; }


/* GOTO à l'addresse du prochain quadruplet, même rôle que le marqueur avec un GOTO en plus*/
G 		:	%empty		{	$$.next = crelist(nextquad);
							quadop qo;
							qo.type = QO_EMPTY;
							qo.u.cst = 0;
							gencode(qo,qo,qo, Q_GOTO, global_code[nextquad].label, -1, NULL);
						}

/* La structure du contexte global soit:
	MD: contient uniquement des déclarations de méthodes globales, 
ou	FD MD: commence par des déclarations globales suivies de déclarations de méthodes  */
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
															
															/* Tab est une liste de tous les tableaux déclarés de même type, voir règle ci-dessous*/
															while(pt != NULL){

																/* vérifier que l'id du tableau n'existe pas déjà dans le contexte global*/
																if( ht_search(glob_context, pt->name) != NULL ) 
																	yyerror("\nErreur: Tableau déclarée deux fois\n");
																if(!strcmp(pt->name, next_label_name()))
																	labelCount++;
																
																/* déclaration du tableau */
																qo.u.global.name = malloc(strlen(pt->name + 1));
																strcpy(qo.u.global.name, pt->name);
																qo.u.global.size = pt->size * 4;
																gencode(qo, qo, qo, Q_DECL, global_code[nextquad].label, -1, NULL);

																/* on incrémente le nombre de variables globales, utile pour MIPS*/
																glob_dec_count++;

																/* on met le tableau dans la TOS globale*/
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
								/* Vérifier que l'id de cette méthode n'existe pas déjà dans la TOS*/
								if(ht_search(glob_context, $2) != NULL)
									yyerror("\nErreur: Méthode déjà déclarée avec ce nom\n");

								/* Vérifier que le main n'est pas déclaré avec une valeur de retour*/
								if(!strcmp($2,"main"))
									yyerror("\nErreur: La méthode main doit être de type void\n");

								if(!strcmp($2, next_label_name()))
									labelCount++;
								
								/* On met la méthode dans la TOS*/
								Ht_item *item = create_item($2, ID_METHOD, $1);
								newname(item);

								/* La déclaration des paramètres de la méthode a un contexte spécifique 
									qui peut être écrasé par le contexte de block.
								*/
								pushctx(CTX_METHOD);
								global_code[nextquad].label = malloc(strlen($2)+1);
								strcpy(global_code[nextquad].label,$2);

								} 

								'(' P ')'  block	{ 	
														/* item_table est une structure couple Identificateur et TOS où il se trouve
															retournée par lookup(char *id)
														*/
														item_table* var = lookup($2);
														/* item est un Ht_item, il contient un attribut p pour les item de type méthode
															où on stocke les types de paramètres que la méthode prend.
															P est soit une liste de paramètres soit NULL si aucun paramètre, voir règle ci-dessous.
														*/
														var->item->p = $5;
														/* on arrive à la fin de la déclaration d'une méthode. S'il y avait un GOTO quelque part dans le bloc
															pour sortir du bloc, maintenant on sait où se trouve la fin du bloc, c'est forcèment le quad suivant.
														*/
														complete($7.next,nextquad);	

														/* un gencode pour informer de la fin de la déclaration de cette méthode. Utile pour MIPS*/													
														quadop qo;
														qo.type = QO_EMPTY;
														qo.u.cst = 0;
														gencode(qo, qo, qo, Q_ENDFUNC, global_code[nextquad].label, -1, NULL);
														popctx(); }

			/* Idem que la règle précédente, mais de type retour void. Obligé de les séparer à cause de conflits Bison.*/
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
							
								'(' P ')'  block	{ 		item_table* var = lookup($2);
															var->item->p = $5;
															complete($7.next, nextquad);
															
															if(!strcmp($2,"main")){
																quadop qo; 
																qo.type = QO_CST; 
																qo.u.cst = 10; 
																gencode(qo,qo,qo, Q_SYSCALL, "end", -1, NULL);
															} 	

															else{
																quadop qo;
																qo.type = QO_EMPTY;
																qo.u.cst = 0;
																gencode(qo, qo, qo, Q_ENDFUNC, global_code[nextquad].label, -1, NULL);
															}							
															popctx();
														}

/* Param est une liste non vide de paramètres de méthodes*/
P 			:	Param 	{ $$ = $1;}
			|	%empty	{ $$ = NULL;}


/* Param est une liste qui récupère les paramètres d'une méthode déclarée */
Param		:	type id ',' Param	{ 	
										/* on vérifie qu'on ne défini pas deux paramètres de même noms au sein de la même méthode*/
										if( ht_search(curr_context, $2) != NULL ) 
											yyerror("\nErreur: Deux paramètres de méthodes de même nom\n");
										
										/* On met le paramètre id dans la TOS*/
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

/* Chaque block est composé de déclarations de variables en premier (V), suivi d'instructions (S) */
block 		:	'{' { 
						pushctx(CTX_BLOCK); } 
			V S '}' 
					{ 	
						/* on récupère l'adresse des GOTO incomplets s'ils existent*/
						$$.next = $4.next; 
						$$.brk = $4.brk;
						$$.cntu = $4.cntu;
						popctx();
					}

/* Block de déclaration de variables */
V 			:	V var_decl 	{;}
			|	%empty		{;}


/* Une déclaration de variable */
var_decl 	:  	type B ';' {
								/* Si on se trouve dans le contexte globale, les variables dont de type QO_GLOBAL*/
								if(curr_context == glob_context){
									quadop qo;
									struct decl *pt = &$2;
									qo.type = QO_GLOBAL;
									
									/* On parcours la liste des déclarations B*/
									while(pt != NULL){
										/* On vérifie que l'id ne se trouve pas dans la TOS*/
										if( ht_search(glob_context, pt->name) != NULL ) 
											yyerror("\nErreur: Variable déclarée deux fois\n");
										/* Pour éviter qu'un ID ne porte le même nom qu'un label auto-généré*/
										if(!strcmp(pt->name, next_label_name()))
											labelCount++;

										/* on met l'id dans un quadop pour ajouter un quad DECL avec gencode*/
										qo.u.global.name = malloc(strlen(pt->name + 1));
										strcpy(qo.u.global.name, pt->name);
										qo.u.global.size = 4;
										gencode(qo, qo, qo, Q_DECL, global_code[nextquad].label, -1, NULL);

										/* On incrémente le nombre de variables globales, utiles pour MIPS*/
										glob_dec_count++;

										/* On met l'ID dans la TOS globale*/
										Ht_item *item = create_item(pt->name, ID_VAR, $1);
										newname(item);
										pt = pt->suiv;
									}
								}
								/* Si pas contexte globale, les ID sont de type QO_ID */
								else {

									quadop qo;
									/* pointeur de parcours de B*/
									struct decl *pt = &$2;
									qo.type = QO_ID;
									
									/* Idem que plus haut avec l'ajout d'un offset pour le stack*/
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

/* On est obligé de séparer cette section de la section précédente pour pouvoir déclarer des id séparés par des ',' 
	On peut pas avoir une syntaxe récursive avec type qui ne se répète pas.
	Ici, on récupère seulement les noms des ID et on les stock dans B.
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

/* une série de stament.

*/
S 			: 	S M statement 	{	
									/* à ce stade, on peut compléter les GOTO incomplets de S car on connait l'addresse prochain statement*/
									complete($1.next, $2);

									/* On récupère les GOTO incomplets de statement pour les compléter par la suite */
									$$.next = $3.next;
									$$.cntu = $3.cntu;
									$$.brk = $3.brk;
								}
			| 	statement		{	
									/* On récupère les GOTO incomplets de statement pour les compléter par la suite */
									$$.next = $1.next;	$$.cntu = $1.cntu; $$.brk = $1.brk; 
								}


statement 	:	id assign_op expr ';' {				/* Affectation */
													/* On cherche id dans la TOS et on le retourne avec sa TOS*/
													item_table *val = lookup($1);
													if (!val)
														yyerror("\nErreur: Variable non déclarée\n");
													
													/* Vérification des types*/
													if(val->item->value != $3.type)
														yyerror("\nErreur: Type de valeur incorrecte\n");
													if (($2 == Q_AFFADD || $2 == Q_AFFSUB) && (val->item->value == BOOL || $3.type == BOOL))
														yyerror("\nErreur: Type de valeur incorrecte\n");

													quadop q1;

													/* Si on se trouve dans le context globale*/
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
														/* Si affectation de type BOOL, il faut créér deux quadruplets, un pour false (0) et un pour true (1)
															les règles sur expr génèrent déjà des GOTO incomplets, ici on les complètent par les 2 quad générés.
															l'attribut true de expr est complété par l'addresse du quad d'affectation true, et l'attribut false
															par l'addresse du deuxième quadruplet.
														*/
														quadop qo;

														/* Affectation true*/
														qo.type = QO_CST;
														qo.u.cst = true;
														complete($3.t, nextquad);
														gencode(q1, qo, qo, Q_AFF, global_code[nextquad].label, -1, NULL); 	

														/* Pour sauter l'affectation false. Le quad d'adresse nextquad+2 est défini ici plus bas.*/
														qo.type = QO_EMPTY;
														gencode(qo, qo, qo, Q_GOTO, global_code[nextquad].label, nextquad+2, NULL);

														/* Affectation false*/
														qo.type = QO_CST;	
														qo.u.cst = false;		
														complete($3.f, nextquad);											
														gencode(q1, qo, qo, Q_AFF, global_code[nextquad].label, -1, NULL);					

														/* On créé un label pour nextquad+2 défini précédemment qui actuellement est juste nextquad. 
															NB: gencode avec Q_LABEL n'incrémente pas nextquad.
														*/
														qo.type = QO_EMPTY;
														qo.u.cst = 0;
														gencode(qo, qo, qo, Q_LABEL, new_label(), -1, NULL);
													}
												
													else {
														/* Si c'est une affectation de type INT, il suffit de générer le quad correspondant.*/

														/* affectation normale*/
														if($2 == Q_AFF)
															gencode(q1,$3.result,$3.result,$2, global_code[nextquad].label,-1, NULL);

														/* le += est just une addition avec op2 = op1*/
														else if($2 == Q_AFFADD)
															gencode(q1,q1,$3.result,Q_ADD, global_code[nextquad].label,-1, NULL);

														/* le -= est just une soustraction avec op2 = op1*/
														else if($2 == Q_AFFSUB)
															gencode(q1,q1,$3.result,Q_SUB, global_code[nextquad].label,-1, NULL);
													}
													/* On dépile les variables temporaires du Stack à la fin de l'évaluation d'une expression*/
													pop_tmp();
												}

			/* Les règles pour les appels de méthodes sont définies dans method_call plus bas.*/
			|	method_call	';'								{;}

			|	If '(' expr ')' M block G Else M block	{	
															/* vérifications des types*/
															if($3.type != BOOL) 
																yyerror("Erreur: Test avec expression non booléene\n");
															
															/* Si expr est vrai, on rentre dans block*/
															complete($3.t, $5);

															/* Si expr est fausse, on va au block du else marqué par M*/
															complete($3.f, $9);

															/* Si expr est vrai, il y a un GOTO après block1, et si elle est fausse il y a un GOTO après block2
																mais dans les deux cas on ne sait pas encore où on jump.
																On garde l'adresse des GOTO incomplets dans le statement. Ils seront résolus dans la 
																règle plus haut ( L )
															*/
															$$.next = $7.next;
															$$.next = concat($$.next, $10.next);

															/* Si ce IF est au sein d'une boucle FOR, on peut avoir des break et continue qui 
															produisent des GOTO. Mais on ne sait pas encore où se trouve le début du FOR et sa fin
															pour effectuer les jump. Ce sera résolu dans les règles de la boucle FOR pour les continue,
															et dans L pour break;
															*/
															$$.cntu = concat($6.cntu, $10.cntu);
															$$.brk = concat($6.brk, $10.brk);
														}
			|	If '(' expr ')' M block 				{
															/* IDEM mais pas de ELSE*/
															if($3.type != BOOL) yyerror("Erreur: Test avec expression non booléene\n");
															complete($3.t, $5);
															$$.next = concat($3.f, $6.next);
															$$.cntu = $6.cntu;
															$$.brk = $6.brk;
														}
												
			|	For id 	'=' expr ',' expr				{  															
															/* Le compteur de boucle à son propre contexte*/
															pushctx(CTX_FOR);
															/* Vérification des types*/
															if($4.type != INT || $6.type != INT) yyerror("Erreur: le compteur de boucle doit être de type INT\n");
															/* Pour éviter d'avoir des variables et label de même nom*/
															if(!strcmp($2, next_label_name()))
																labelCount++;

															/* On déclare le compteur de boucle ID, on le met dans la TOS et on lui affecte expr1*/
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

															/* gencode to increment the loop counter*/
															quadop qo, q1;
															qo.type = QO_ID;
															q1.type = QO_CST;
															q1.u.cst = 1;

															/* offset of id is 0 because it's the only variable in the context at this point */
															qo.u.offset = 0;
															gencode(qo, qo, q1, Q_ADD, global_code[nextquad].label, -1, NULL);
															qo.type = QO_EMPTY;

															/* gencode to jump back to the Marker to test if the counter reached its max value */
															gencode(qo, qo, qo, Q_GOTO, global_code[nextquad].label, $8, NULL);

															/* we save the incomplete GOTOs of the comparison of the loop counter, and the break instructions. 
																They do the jump our of the loop, but we don't know here that is yet. 
																It is completed in a more global context (L) above.
															 */
															$$.next = crelist($8);
															$$.next = concat($$.next, $10.brk);
															popctx(); 
														 }

			|	Return return_val ';'					 {;}
			|	Break ';'								 { 
															if(!is_for_a_parent()) 
																yyerror("\nERREUR: Break; doit être au sein d'une boucle FOR\n");
															quadop qo;
															qo.type = QO_EMPTY;
															/* Incomplete GOTO to jump out of the for loop*/
															$$.brk = crelist(nextquad);
															gencode(qo,qo,qo,Q_GOTO,global_code[nextquad].label,-1,NULL);
														 }
			|	Continue ';'							 { 
															if(!is_for_a_parent()) 
																yyerror("\nERREUR: Continue; doit être au sein d'une boucle FOR\n");
															quadop qo;
															qo.type = QO_EMPTY;
															/* Incomplete GOTO to jump to the start of the loop*/
															$$.cntu = crelist(nextquad);
															gencode(qo,qo,qo,Q_GOTO,global_code[nextquad].label,-1,NULL);
			
														 }
			|	block									 { $$.next = $1.next ;}


return_val	:	expr 							{;}
			|	%empty							{;}


method_call :	id '(' E ')' 					{
													item_table *val = lookup($1);
													if(val == NULL)
														yyerror("Erreur: Méthode non déclarée\n");
													// $$ = val->item->value;
													/* Vérifier les paramètres E */
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
													// $$ = val->item->value;
													/* Vérifier les paramètres E */
													if(!verify_param(val->item->p, NULL))
														yyerror("Erreur: Appel de méthode avec paramètres incorrectes\n");
													quadop qo;
													qo.type = QO_EMPTY;
													gencode(qo,qo,qo, Q_METHODCALL, global_code[nextquad].label, -1, NULL);
													
												}

E 			:	expr ',' E 		{ param p = (param) malloc(sizeof(struct param)); p->type = $1.type; p->arg = $1.result; p->next = $3; $$ = p;}
			|	expr 			{ $$ = (param) malloc(sizeof(struct param)); $$->type = $1.type; $$->arg = $1.result; $$->next = NULL;}

location	:	id	{;}
			|	id '[' expr ']'			{ /* expr de type int */ 
											if($3.result.type != INT)
												yyerror("Erreur: indice de tableau doit être de type INT\n");
											item_table *val = lookup($1);

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
											$$.result = qo;}
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
											gencode(qo, $1.result, $3.result, $2, global_code[nextquad].label, -1, NULL);
											$$.f = crelist(nextquad);
											gencode(qo, qo, qo, Q_GOTO, global_code[nextquad].label, -1, NULL);
											}
			|	expr eq_op expr	%prec eq	{	if($1.type != $3.type )
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
			| 	literal 				{
											$$.result = $1.qop;
											$$.type = $1.type;
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
											if(val->item->value == BOOL){
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
			|	'-' expr %prec NEG 		{
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
											if($1.type == VOIDTYPE)
												yyerror("Erreur: méthode de type de retour void utilisée comme expression\n");
											$$.result = $1.result;
										}
			|	string_literal			{	
											$$.type = STRING;
											$$.str = malloc(sizeof($1)+1);
											strcpy($$.str, $1);
										}

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
