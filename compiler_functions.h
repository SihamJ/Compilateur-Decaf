#ifndef __METHOD_DECL_H__
#define __METHOD_DECL_H__



#include "intermediaire.h"
#include "hashtable.h"
#include "token.h"



char* add_to_tos(int type, char* name );
int verify_returns(list rtrn, int type);

char* end_func(char *name, int ctx_count, param p);
char* field_declare(declaration *dec, int type);
char* var_declare(declaration *dec, int type);
declaration get_declarations(char *name, declaration *next, int type, int size);
param push_param(char* name, int type, param next);
char* verify_aff_types(int type1, int type2, int oper, Ht_item *item);
void get_location(quadop* qo, quadop* q1, item_table* val, location l);
void bool_affectation(quadop op1, quadop op3, expr_val s, expr_val expr);
char* for_declare(char* counter_id, expr_val expr1, expr_val expr2);
expr_val get_max(char *counter_name, expr_val expr);
void gen_q_pop(int count);
void gen_test_counter(char *counter_name, expr_val expr);
void gen_increment_and_loopback(char* counter_name, int jump);
void get_write_string_args(char *label, char*value);
/* verifies if ctx_type type is a parent of the current context*/
int is_a_parent(ctx_type type);
/* verifies that list of parameters p1 corresponds to list p2 in number and types*/
int verify_param(param p1, param p2);
#endif