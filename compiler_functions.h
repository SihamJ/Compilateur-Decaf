#ifndef __METHOD_DECL_H__
#define __METHOD_DECL_H__



#include "intermediaire.h"
#include "hashtable.h"
#include "token.h"

/** @brief Functions used in the syntax rules "compiler.y" 
 * For code refactoring.
**/


/** @brief Push a new method to the global context after the necessary verifications
 *  @param type is the return type
 *  @param name is the identifier of the method
 */
char* add_to_tos(int type, char* name );


/** @brief used to verify that all the returns inside a method are of the correct return type
 *  @param rtrn is a list of the Return quads
 *  @param type is the correct return type of the method
 *  @return 1 if successful, 0 otherwise
 */
int verify_returns(list rtrn, int type);


/** @brief generates a quad for the end of a method. If the method is main, syscall to exit.
 *  @param name name of the method (useful for generating a unique label)
 *  @param ctx_count number of variables to pop from the stack
 *  @param param is a list of the method parameters 
 *  @return NULL if successful, or an error msg otherwise
 */
char* end_func(char *name, int ctx_count, param p, int is_returnval);


/** @brief used to get the next variable declaration and add it to the list of declarations
 *  @param name is the Identifier of the variable to be added to the list
 *  @param next is the existing list of declarations
 *  @param type is the type of the variable Array or Scalar
 *  @param size is the size in bytes of the next variable to be stored
 *  @return the list of declarations
 */
declaration get_declarations(char *name, declaration *next, int type, int size);


/** @brief used to verify a line of global declarations and push the variables in the TOS
 *  @param dec is a list of variables to declare
 *  @param type is the type of of the variables
 *  @return NULL if successful, or an error msg otherwise
 */
char* field_declare(declaration *dec, int type);


/** @brief used to verify a line of local declarations and push the variables in the TOS
 *  @param dec is a list of variables to declare
 *  @param type is the type of of the variables
 *  @return NULL if successful, or an error msg otherwise
 */
char* var_declare(declaration *dec, int type);



param push_param(char* name, int type, param next);
char* verify_aff_types(int type1, int type2, int oper, Ht_item *item);
void get_location(quadop* qo, quadop* q1, item_table* val, location l);
void bool_affectation(quadop op1, quadop op3, expr_val *s, expr_val *expr);
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
char* verify_and_get_type_call(char *id, param p, method_call *m);
void gen_method_call(char *id, expr_val *E, method_call *m);
#endif