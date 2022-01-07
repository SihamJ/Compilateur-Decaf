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


/**
 * @brief Get the param object
 * 
 * @param name 
 * @param type 
 * @param next 
 * @return param 
 */
param get_param(char* name, int type, param next);


/**
 * @brief 
 * 
 * @param type1 
 * @param type2 
 * @param oper 
 * @param item 
 * @return char* 
 */
char* verify_aff_types(int type1, int type2, int oper, Ht_item *item);


/**
 * @brief Get the location object
 * 
 * @param qo 
 * @param q1 
 * @param val 
 * @param l 
 */
void get_location(quadop* qo, quadop* q1, item_table* val, location l);


/**
 * @brief 
 * 
 * @param op1 
 * @param op3 
 * @param s 
 * @param expr 
 */
void bool_affectation(quadop op1, quadop op3, expr_val *s, expr_val *expr);


/**
 * @brief 
 * 
 * @param counter_id 
 * @param expr1 
 * @param expr2 
 * @return char* 
 */
char* for_declare(char* counter_id, expr_val expr1, expr_val expr2);


/**
 * @brief we copy the max value of the for loop counter in a temporary variable
 * 
 * @param counter_name ID of the loop counter
 * @param expr the max value
 * @return the temporary variable as an expr_val
 */
expr_val get_max(char *counter_name, expr_val expr);


/**
 * @brief for popping the stack
 * 
 * @param count number of bits to pop
 */
void gen_q_pop(int count);


/**
 * @brief 
 * 
 */
void gen_q_push();


/**
 * 
 * @brief 
 * 
 * @param counter_name 
 * @param expr 
 */
void gen_test_counter(char *counter_name, expr_val expr);


/**
 * @brief 
 * 
 * @param counter_name 
 * @param jump 
 */
void gen_increment_and_loopback(char* counter_name, int jump);


/**
 * @brief Get the write string args object
 * 
 * @param label 
 * @param value 
 */
void get_write_string_args(char *label, char*value);

/* verifies if ctx_type type is a parent of the current context*/

/**
 * @brief 
 * 
 * @param type 
 * @return int 
 */
int is_a_parent(ctx_type type);

/* verifies that list of parameters p1 corresponds to list p2 in number and types*/

/**
 * @brief 
 * 
 * @param p1 
 * @param p2 
 * @return int 
 */
int verify_param(param p1, param p2);


/**
 * @brief 
 * 
 * @param id 
 * @param p 
 * @param m 
 * @return char* 
 */
char* verify_and_get_type_call(char *id, param p, method_call *m);


/**
 * 
 * @brief 
 * 
 * @param id 
 * @param par 
 * @param m 
 */
void gen_method_call(char *id, param par, method_call *m);


/**
 * @brief Get the literal object
 * 
 * @param l 
 * @return expr_val 
 */
expr_val get_literal(literal l);


/**
 * @brief 
 * 
 * @param l 
 * @param val 
 * @return char* 
 */
char* verify_location_access(location l, item_table* val);


/**
 * @brief 
 * 
 * @param l 
 * @param val 
 * @return expr_val 
 */
expr_val gen_global_scalar(location l, item_table* val);


/**
 * 
 * @brief generates a TAB ACCESS quad
 * 
 * @param l is a structure where we keep the Array identifier and the index to access
 * @param val Array Item and its symbol table
 * @return expr_val, a temporary variable where we store the value of the array element we accessed
 */
expr_val gen_access_tab(location l, item_table *val);


/**
 * @brief Gets the location of an ID
 */
expr_val get_local_id(location l, item_table *val);


/**
 * @brief Unary minus operation
 */
expr_val unaire(expr_val expr);


/**
 * @brief < =< > => == != operations
 */
expr_val oprel(expr_val expr1, int op, expr_val expr2);


/**
 * @brief MUL operation
 */
expr_val mul(expr_val expr1, int op, expr_val expr2);


/**
 * @brief ADD operation
 */
expr_val add(expr_val expr1, int op, expr_val expr2);


/**
 * @brief 
 * 
 * @param res 
 * @param name 
 * @param index 
 */
void gen_index_access_tab(location *res, char* name, expr_val index);


expr_val get_string_literal(char* str);


/**
 * @brief Creates a copy of a method call argument
 * 
 * @param expr 
 * @param list 
 * @param marker 
 * @return param 
 */
param copy_method_call_arg(expr_val expr, param list, int marker);


/**
 * @brief Retrieves the location/address of a method call argument. Used for ReadInt
 * 
 * @param str 
 * @param list 
 * @param val 
 * @return param 
 */
param get_arg_by_address(char* str, param list, item_table* val);


/**
 * @brief Transforms a boolean value to a couple of true/false lists. Used in Or/And operations and within an if statement
 */
expr_val val_to_goto(expr_val expr);


/**
 * @brief Backpatching of a true/false couple into a boolean value stored in a temporary variable. It also creates a new true list that we need to complete.
 */
expr_val goto_to_val(expr_val expr);


/**
 * @brief ! operation
 */
expr_val not_op(expr_val expr);


/**
 * @brief 
 * 
 * @param block 
 * @param s 
 * @return statement 
 */
statement pop_block(statement block, statement s);


/**
 * @brief initialises the Return, Break, Continue, and Next lists of a statement to NULL
 */
void initialise_lists(statement *s);

#endif