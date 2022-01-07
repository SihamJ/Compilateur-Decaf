
#include "compiler_functions.h"

/** @brief Functions used in the syntax rules "compiler.y" 
 * For code refactoring.
**/


char* add_to_tos(int type, char *name ){
    /* verifying that ID does not already exist in the global context*/
    if(ht_search(glob_context, name) != NULL)
        return "\nErreur: Méthode déjà déclarée avec ce nom\n";

    /* Verifying that main is not declared with a return type other than void*/
    if(type != VOIDTYPE && !strcmp(name,"main"))
        return "\nErreur: La méthode main doit être de type void\n";

    if(!strcmp(name, next_label_name()))
        labelCount++;
    
    /* we store the method in the symbol table*/
    Ht_item *item = create_item(name, ID_METHOD, type);
    item->size = 0;
    newname(item);

    /* The parameters declaration within a method has its specific context
    which can be overwritten by new declarations in the method block.
    */
    pushctx(CTX_METHOD);
    quadop qo; qo.type = QO_EMPTY;
    global_code[nextquad].ctx = curr_context;
    gencode(qo, qo, qo, Q_FUNC, name, -1, NULL);

    return NULL;
}


int verify_returns(list rtrn, int type){
  while(rtrn){
    if(global_code[rtrn->addr].op2.u.cst != type)
      return 0;
    rtrn = rtrn->suiv;
  }
  return 1;
}

char* end_func(char *name, int ctx_count, param p, int is_returnval){

  if(!strcmp(name, "main")){
    if(p != NULL){
      return "\nErreur: Main ne prends pas de paramètres\n";
    }
    quadop qo; 
    qo.type = QO_CST; 
    qo.u.cst = 10; 
    gencode(qo,qo,qo, Q_SYSCALL, "end", -1, NULL);
  }

  else{
    quadop qo;
    qo.type = QO_CST;
    qo.u.cst = is_returnval;
    char *label = new_endfunc_label(name);
    if(lookup(label, curr_context)!=NULL)
      label = new_label();
    global_code[nextquad-1].label = label;
    global_code[nextquad-1].type = Q_ENDFUNC;
    global_code[nextquad-1].op2 = qo;
  }
  return NULL;
}

char* field_declare(declaration *dec, int type){

  quadop qo,q1;
  declaration *pt = dec;
  qo.type = QO_GLOBAL;
  /* dec is a list of all arrays and global variables declared of same type*/
  while(pt != NULL){

    /* verifying that the ID does not already exist in the current context*/
    if( ht_search(glob_context, pt->name) != NULL ) 
      return "\nErreur: Identificateur déjà déclaré\n";

    if(!strcmp(pt->name, next_label_name()))
      labelCount++;
    
    /* declaration of the global id in the quad */
    qo.u.global.name = pt->name;
    qo.u.global.size = pt->size;
    qo.u.global.type = pt->type;
    quadop q; q.type = QO_EMPTY;
    gencode(qo, q, q, Q_DECL, NULL, -1, NULL);

    /* we increment the global variables counter, useful for MIPS*/
    glob_dec_count++;

    /* we store the id in the global symbol table*/
    int id_type;
    if(pt->type == QO_TAB)
      id_type = ID_TAB;
    else if(pt->type == QO_SCAL)
      id_type = ID_VAR;
    Ht_item *item = create_item(pt->name, id_type, type);
    item->size = pt->size;
    newname(item);
    pt = pt->suiv;
	}

  free(pt);
  return NULL;
}

char* var_declare(declaration *dec, int type){

  quadop qo;
  declaration *pt = dec;
  qo.type = QO_ID;
  
  while(pt != NULL){

    if( ht_search(curr_context, pt->name) != NULL ) 
      return "\nErreur: Identificateur déjà déclaré\n";
    
    if(!strcmp(pt->name, next_label_name()))
      labelCount++;
    Ht_item *item = create_item(pt->name, ID_VAR, type); 

    item->size = 4;
    newname(item);
    pt = pt->suiv;
  }

  free(pt);
  return NULL;
}
                  
declaration get_declarations(char *name, declaration *next, int type, int size){
  declaration var;
  var.name = name;
  var.size = size;
  var.suiv = next;
  var.type = type;
  return var;
}

param get_param(char* name, int type, param next){

  param p = (param) malloc(sizeof(struct param));
  p->type = type;
  p->next = next;
  p->stringval = name;
  return p;
}

char* verify_aff_types(int type1, int type2, int oper, Ht_item *item){
  
  if(item->value != type2)
    return "\nErreur: Type différents à l'affectation\n";

  if ((oper == Q_AFFADD || oper == Q_AFFSUB) && (item->value == BOOL || type2 == BOOL)) 
    return "\nErreur: Affectation += ou -= avec un type booléen\n";

  if(type1 == ID_TAB && item->id_type == ID_VAR) 
    return "\nErreur: Accès à l'indice d'une variable qui n'est pas un tableau\n";

  if( type1 == ID_VAR && item->id_type == ID_TAB) 
    return "\nErreur: Affectation scalaire comme tableau\n";

  return NULL;
}

void get_location(quadop* qo, quadop* q1, item_table* val, location l){

/* If location is an element in an array, we need to retrieve the offset from 
  the attribute 'quadop offset' of l and store it in q1. If not, we use q1 as an empty quadop in the AFF below. */

  if(val->table == glob_context) {
    qo->type = QO_GLOBAL;
    qo->u.global.name = l.stringval;

    if(l.type == ID_VAR) {
      qo->u.global.type = QO_SCAL;
      q1->type = QO_EMPTY;
    }
    else if(l.type == ID_TAB) {
      qo->u.global.type = QO_TAB;
      if(l.index.type == QO_ID || l.index.type == QO_TMP){
        l.index.u.name = l.index_name;
      }
      *q1 = l.index;
    }
  }
  // If location is a local variable, we retrieve the name
  else {
    qo->type = QO_ID;
    qo->u.name = val->item->key;
    q1->type = QO_EMPTY;
  }
}

char* for_declare(char* counter_id, expr_val expr1, expr_val expr2){

  /* The loop counter has its own context*/
  pushctx(CTX_FOR);     gen_q_push();

  if(expr1.type != INT || expr2.type != INT)
    return "\nErreur: le compteur de boucle doit être de type INT\n";

  /* To avoid having variables and labels holding the same name*/
  if(!strcmp(counter_id, next_label_name()))
    labelCount++;

  /* We declare the loop counter id and store the initial value expr1 in it*/

  Ht_item *item = create_item(counter_id, ID_VAR, INT);
  item->size = 4;
  newname(item);

  quadop qo;
  qo.type = QO_ID;
  qo.u.name = item->key; quadop q; q.type = QO_EMPTY;

  gencode(qo, expr1.result, expr1.result, Q_AFF, NULL, -1, NULL); 
  return NULL;
}


expr_val get_max(char *counter_name, expr_val expr){
  
  char *name = malloc(strlen(counter_name)+7);
  snprintf(name, strlen(counter_name)+7, "0_%s%s",counter_name, "_max");

  Ht_item *item = create_item(name, ID_VAR, INT);
  item->size = 4;             newname(item);
  quadop qo, q;               qo.type = QO_TMP;
  qo.u.name = item->key;      q.type = QO_EMPTY;

  gencode(qo, expr.result, expr.result, Q_AFF, NULL, -1, NULL); 

  expr_val max;     max.result = qo;    max.stringval = item->key;

  return max;
}


void gen_q_pop(int count){
  quadop q, qo; qo.type = QO_EMPTY;
  q.type = QO_CST;
  q.u.cst = count;
  gencode(q, qo, qo, Q_POP, NULL, -1, NULL);
}

void gen_test_counter(char *counter_name, expr_val max){
  quadop qo,q1,q2;
  q1.type = QO_EMPTY;
  qo.type = QO_ID;
  qo.u.name = counter_name;

  max.result.u.name = max.stringval;
  gencode(q1, qo, max.result, Q_GT, NULL, -1 , NULL);  
}

void gen_increment_and_loopback(char* counter_name, int jump){
  /* gencode to increment the loop counter*/
  quadop qo, q1;
  qo.type = QO_ID;
  q1.type = QO_CST;
  q1.u.cst = 1;
  /* offset of id is 0 because it's the only variable in the context at this point */
  qo.u.name = counter_name;
  gencode(qo, qo, q1, Q_ADD, NULL, -1, NULL);

  qo.type = QO_EMPTY;
  /* gencode to jump back to the Marker to test if the counter reached its max value */
  gencode(qo, qo, qo, Q_GOTO, NULL, jump, NULL);
}

void get_write_string_args(char *label, char*value){
  str_labels[str_count-1].label = label;
  str_labels[str_count-1].value = value;
}

/* Utile pour savoir si un break ou un continue est bien au sein d'une boucle for */
int is_a_parent(ctx_type type){
    HashTable* pt = curr_context;
    while(pt != NULL){
        if(pt->type == type)
            return true;
        pt = pt->next;
    }
    free(pt);
    return false;
}


int verify_param(param p1, param p2){
    while(p1 != NULL && p2 != NULL){
        if(p1->type != p2->type || p1->byAddress != p2->byAddress){
            return false;
        }
        p1=p1->next;
        p2=p2->next;
    }
    if(p1 != NULL || p2 != NULL){
        return false;
    }
    return true;
}

char* verify_and_get_type_call(char *id, param p, method_call *m){

  item_table *val = lookup(id, curr_context);

  if(val == NULL) 
    return "\nErreur: Méthode non déclarée\n"; 
  if(val->item->id_type != ID_METHOD) 
    return "\nErreur: l'ID utilisé n'est pas celui d'une méthode\n";
  if(!verify_param(val->item->p, p)) 
    return "\nErreur: Appel de méthode avec paramètres incorrectes\n";

  m->return_type = val->item->value; 
  return NULL;
}


/**
 * @brief we store in qo the name of the method to call, in q1 the return type and q2 is a temporary variable where we store the return value
 * if there is any
 * 
 * @param id name of the method to call
 * @param E arguments passed to the method
 * @param m this structure has a result quadop where we will store the return value (q2), to pass it to expr in case there is an affectation
 */
void gen_method_call(char *id, param par, method_call *m){

  quadop qo,q1,q2; 
  
  qo.type = QO_CSTSTR;
  qo.u.string_literal.label = id;

  if(m->return_type != VOIDTYPE) { 
    
    Ht_item* tmp = new_temp(INT);
    q1.type = QO_CST; q1.u.cst = m->return_type; 
    q2.type = QO_TMP; q2.u.name = tmp->key;
    m->result_id = tmp->key;
   }
  
  else { q1.type = QO_EMPTY; q2.type = QO_EMPTY; }

  if(par != NULL){

    param p = par;
    while(p){
      if( p->arg.type == QO_ID || p->arg.type == QO_TMP){
        p->arg.u.name = p->stringval;
        }
      if(p->t && global_code[p->t->addr].jump == -1) complete(p->t, nextquad);
      p = p->next;
    }
    gencode(qo,q1,q2, Q_METHODCALL, NULL, -1, par);
  } else {
    gencode(qo,q1,q2, Q_METHODCALL, NULL, -1, NULL);
  }

  m->result = q2;

}


expr_val get_literal(literal l){
  expr_val res;
  res.result.type = QO_CST;	res.type = l.type;	res.result.u.cst = l.intval; 

  if(l.type == BOOL){ res.t = NULL; res.f = NULL; }
  return res;								
}

expr_val gen_global_scalar(location l, item_table* val) {
  expr_val res;
    quadop q1,q2; 	q1.type = QO_GLOBAL; 	q1.u.global.type = QO_SCAL;
    q1.u.global.name = l.stringval;	 q1.u.global.size = 4;
    q2.type = QO_EMPTY;
    
    Ht_item* item = new_temp(val->item->value);
    quadop qo; qo.type = QO_TMP; qo.u.name = item->key;
    gencode(qo, q1, q1, Q_AFF, NULL, -1, NULL);
    res.stringval = item->key;
    res.type = val->item->value;
    res.result = qo;
    return res;

}

char* verify_location_access(location l, item_table* val){
  if(l.type == ID_VAR && val->item->id_type == ID_TAB) 
      return "\nErreur: Accès tableau comme scalaire\n"; 
  if(l.type == ID_TAB && val->item->id_type == ID_VAR) 
      return "\nErreur: Scalaire utilisé comme tableau\n"; 
  return NULL;
}

expr_val gen_access_tab(location l, item_table *val){

  expr_val res;     res.type = val->item->value;
  quadop qo; 		    qo.type = QO_GLOBAL; 	        qo.u.global.type = QO_TAB;
  qo.u.global.name = l.stringval;   	            qo.u.global.size = val->item->size;

  Ht_item* item = new_temp(INT);
  quadop q1, q2;    q2.type = QO_EMPTY;	    q1.type = QO_TMP; 		q1.u.name = item->key;


  gencode(q1, qo, l.index, Q_ACCESTAB, NULL, -1, NULL); 	res.result = q1; 
  res.stringval = item->key;

  return res;
}

expr_val get_local_id(location l, item_table *val){
  expr_val res;
  quadop qo; 	qo.type = QO_ID; 	qo.u.name =  l.stringval; 	res.result = qo;
  res.type = val->item->value; 	res.stringval = l.stringval;
  if(res.type == BOOL){  res.t = NULL; res.f = NULL; }
  return res;
}

expr_val add(expr_val expr1, int op, expr_val expr2){
  expr_val res;
  res.type = INT; 	Ht_item* item = new_temp(INT); 	quadop qo; 	qo.type = QO_TMP; 	qo.u.name = item->key;

  res.stringval = item->key;

  quadop q1; q1.type = QO_EMPTY;
  gencode(qo, expr1.result, expr2.result, op, NULL, -1, NULL); res.result = qo;
  return res;
}

expr_val mul(expr_val expr1, int op, expr_val expr2){
  expr_val res;
  res.type = INT;	 Ht_item* item = new_temp(INT); 	quadop qo; 	qo.type = QO_TMP; 	qo.u.name = item->key;

  res.stringval = item->key;
  
  quadop q1; q1.type = QO_EMPTY;
  gencode(qo, expr1.result, expr2.result, op, NULL, -1, NULL); 	res.result = qo;
  return res;
}

expr_val unaire(expr_val expr){
  expr_val res;
  res.type = INT; 	Ht_item* item = new_temp(INT); 	quadop qo;	 qo.type = QO_TMP;	 qo.u.name = item->key;
  quadop q1; q1.type = QO_EMPTY;

  res.stringval = item->key;

  q1.type = QO_CST; 	q1.u.cst = 0;
  gencode(qo, q1, expr.result, Q_SUB, NULL,-1, NULL); 	res.result = qo;
  return res;
}


expr_val oprel(expr_val expr1, int op, expr_val expr2){

  expr_val res; res.type = BOOL; res.result.type = QO_GOTO;

  quadop qo; 	qo.type = QO_EMPTY;

  res.t = crelist(nextquad); 	gencode(qo, expr1.result, expr2.result, op, NULL, -1, NULL);
  res.f = crelist(nextquad); 	gencode(qo, qo, qo, Q_GOTO, NULL, -1, NULL);
  return res;
}


void gen_index_access_tab(location *res, char* name, expr_val index){

  res->type = ID_TAB; res->stringval = name; res->index = index.result; 

  if(index.result.type == QO_TMP) {
    index.result.u.name = index.stringval;
    quadop qo; qo.type = QO_CST; qo.u.cst = 4;
    gencode(index.result, index.result, qo, Q_MUL, NULL, -1, NULL);
    res->index.type = QO_TMP;
    res->index_name = index.stringval; 
    res->index = index.result;
  }
  else if(index.result.type == QO_ID){
    Ht_item *item = new_temp(INT);
    quadop qo;	qo.type = QO_TMP;	qo.u.name = item->key;
    quadop q1;	q1.type = QO_CST; q1.u.cst = 4;

    index.result.u.name = index.stringval;
    gencode(qo, index.result, q1, Q_MUL, NULL, -1, NULL);
    res->index.type = QO_TMP;
    res->index_name = item->key; 
    res->index = qo;
  }
  else if(index.result.type == QO_CST){
    res->index.type = QO_CST;
    res->index.u.cst = 4*index.result.u.cst;
  }						
}

expr_val get_string_literal(char* str){
  expr_val res;
  res.type = STRING; 	res.result.type = QO_CSTSTR;
  res.result.u.string_literal.label = new_str();
  res.result.u.string_literal.value = str;
  return res;
}

param copy_method_call_arg(expr_val expr, param list, int marker){

  param p = (param) malloc(sizeof(struct param));

  if(expr.result.type == QO_ID || expr.result.type == QO_TMP){
    p->stringval = expr.stringval;
  }
  if(expr.type == BOOL && expr.t)
    complete(expr.t, marker);

  p->type = expr.type; p->arg = expr.result; p->next = list; 	p->byAddress = 0;
  if(expr.type == BOOL) { p->t = expr.t; }
  return p;
  
}

param get_arg_by_address(char* str, param list, item_table* val){

  param p = (param) malloc(sizeof(struct param));
  
  quadop qo; 
  if(val->table == glob_context){
    qo.type = QO_GLOBAL; qo.u.global.name = str; qo.u.global.type = QO_SCAL;
  }
  else{
    qo.type = QO_ID; qo.u.name = str;
  }
  p->stringval = str;
  p->type = val->item->value; p->arg = qo; p->next = list; p->byAddress = 1;
  return p;
}

expr_val val_to_goto(expr_val expr){

  quadop qo, q; q.type = QO_EMPTY;  qo.type = QO_CST; qo.u.cst = true; 

  expr.t = crelist(nextquad);
  gencode(q, expr.result, qo, Q_EQ, NULL, -1, NULL);

  expr.f = crelist(nextquad);
  gencode(q, q, q, Q_GOTO, NULL, -1, NULL);

 

  expr.result.type = QO_GOTO;
  return expr;
}


expr_val goto_to_val (expr_val expr) {

  Ht_item* tmp = new_temp(expr.type);
  expr_val res;             quadop q, qo, q1;           q.type = QO_EMPTY;    
  q1.type = QO_CST;         q1.u.cst = true;            qo.type = QO_TMP;    
  qo.u.name = tmp->key;     res.stringval = tmp->key;

  
  complete(expr.t, nextquad);
  gencode(qo, q1, q, Q_AFF, NULL, -1, NULL);
  

  res.t = crelist(nextquad);
  gencode(q, q, q, Q_GOTO, NULL, -1, NULL);
  q1.u.cst = false;
  complete(expr.f, nextquad);
  gencode(qo, q1, q, Q_AFF, NULL, -1, NULL);

  res.result = qo; res.type = expr.type;

  return res;
}


expr_val not_op(expr_val expr){

  expr_val res; res.type = BOOL;
  Ht_item* item = new_temp(BOOL);

  quadop qo, q1, q2; q2.type = QO_EMPTY; qo.type = QO_TMP;  qo.u.name = item->key; 	q1.type = QO_CST; 	q1.u.cst = true;	
  
  gencode(qo, q1, expr.result, Q_SUB, NULL, -1, NULL);
  res.result = qo;

  res.stringval = item->key;
  res.t = NULL; res.f = NULL;
  return res;
}


void gen_q_push(){
  quadop qo, q; qo.type = QO_CST; q.type = QO_EMPTY;
  global_code[nextquad].ctx = curr_context;
  gencode(qo, q, q, Q_PUSH, NULL, -1, NULL);
}

statement pop_block(statement block, statement s){

  block = s; 

  if(curr_context->size > 0){
    if(block.brk != NULL) {
      complete(block.brk, nextquad);	gen_q_pop(curr_context->size); block.brk = crelist(nextquad); quadop q; q.type = QO_EMPTY;
      gencode(q, q, q, Q_GOTO, NULL, -1, NULL); }

    if(block.next!= NULL) {
      complete(block.next, nextquad); gen_q_pop(curr_context->size); block.next = crelist(nextquad); quadop q; q.type = QO_EMPTY;
      gencode(q, q, q, Q_GOTO, NULL, -1, NULL);}

    if(block.cntu != NULL){
      complete(block.cntu, nextquad); gen_q_pop(curr_context->size); block.cntu = crelist(nextquad); quadop q; q.type = QO_EMPTY;
      gencode(q, q, q, Q_GOTO, NULL, -1, NULL); }
  }
  gen_q_pop(curr_context->size);
  global_code[curr_context->quad_index].op1.u.cst = curr_context->size;
  return block;
}

void initialise_lists(statement *s){
  s->brk = NULL;  s->cntu = NULL; s->rtrn = NULL; s->next = NULL; s->elseGoto = NULL;
}
