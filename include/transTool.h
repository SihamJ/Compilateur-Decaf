#ifndef __TRANSTOOL_H__
#define __TRANSTOOL_H__


#include "instructions.h"
#include "intermediaire.h"
#include "hashtable.h"

void mips_dec_global(quadop q);

void mips_init_array(quadop q);

void mips_store_word(char *reg, char *target);
/**
 * @brief Generate constant instructions (instructions without parameters)
 * 
 * @param cstInstruct 
 */
void mips_instruction(const char *cstInstruct);

/**
 *  \brief \brief Set the value of a register to a const val
 *  @param reg The register to modify
 *  @param val The value to load
 */
void mips_load_immediate(char *reg, int val);

/**
 * \brief Save one word to the stack
 * \return The size occupied on the stack
 */
int mips_push_word(char *val);

/**
 * \brief Pop one word from the stack to $v0
 */
void mips_pop_word();

/**
 * @brief  Copy the value from stack(at offset) to the target register
 * 
 * @param target Load the value in stack(with offset) to the target register
 * @param offset 
 */
void mips_read_stack(char *target, int offset);

void mips_write_stack(char *target, int offset);

void mips_read_tmp(char *tmp, char *target);

void mips_write_tmp(char *tmp, char *target);

void mips_load_1args(quadop op, HashTable *ctx);

void mips_load_2args( quadop op1, quadop op2, HashTable *ctx);

void mips_beqz(char *target);

void mips_sum(char *target, char *addL, char *addR);

void mips_sub(char *target, char *subL, char *subR);

void mips_div(char *target, char *subL, char *subR);

void mips_mod(char *target, char *subL, char *subR);

void mips_mult(char *target, char *subL, char *subR);

void mips_eq(char *target, char *eqL, char *eqR, char *label);

void mips_neq(char *target, char *neqL, char *neqR, char *label);

void mips_lt(char *target, char *geqL, char *geqR, char *label);

void mips_gt(char *target, char *geqL, char *geqR, char *label);

void mips_leq(char *target, char *geqL, char *geqR, char *label);

void mips_geq(char *target, char *geqL, char *geqR, char *label);

void mips_jump(char *label);

void mips_syscall(int num);

void mips_seq(char *target, char *eqL, char *eqR);

void mips_sne(char *target, char *eqL, char *eqR);

/** The abstraction of "For" and "While" should be the same.
* (Looping the content in its block: @param block_content
* based on the value of a boolean: @param condition_reg) 
*/
void mips_loop(char *condition_reg, const char *block_content); 

/**
 * Control structure if-else
 * 
 * @param condition_reg The condition register
 * @param blk_if_content The content in "if's block"
 * @param blk_el_content The content in "else's block", if this value is NULL, 
 * 							none "else" block will be generated
 */
void mips_if_else(char *condition_reg, const char* blk_if_content, const char * blk_el_content);

/**
 * @brief Declare a table
 */
void declare_tab(char *tab_name, int size);

/**
 * @brief Save the value in the buffer to the target table at a specified offset
 * @param buffer_reg The buffer register holding the value to save
 * @param tab_name The name of the target table
 * @param offset The offset
 */
void mips_tab_put(char *buffer_reg, char *tab_name, int offset);

/**
 * @brief Save the value in the buffer to the target table at a specified offset
 * @param buffer_reg The buffer register holding the value to save
 * @param tab_name The name of the target table
 * @param offset The register containing the offset
 */
void mips_tab_put_IdxByReg(char *buffer_reg, char *tab_name, char *offset_reg);

/**
 * @brief Save the value in the table at a specified offset to the target buffer
 * @param buffer_reg The target buffer register to save a value 
 * @param tab_name The name of the table
 * @param offset The offset
 */
void tab_get(char *buffer_reg, char *tab_name, int offset);

/**
 * @brief Save the value in the buffer to the target table at a specified offset
 * @param buffer_reg The buffer register holding the value to save
 * @param tab_name The name of the target table
 * @param offset The register containing the offset
 */
void tab_get_IdxByReg(char *buffer_reg, char *tab_name, char *offset_reg);

void mips_pop_stack(int offset);
void mips_method_call(quad q, HashTable *ctx);
int mips_push_args(param p, HashTable *ctx);
void mips_return(quad q, HashTable *ctx);
void mips_declare_strings();

void mips_tab_get(char *buffer_reg, char *tab_name, int offset);

/**
 * @brief Save the value in the table at a specified offset to the target buffer
 * @param buffer_reg The target buffer register to save a value 
 * @param tab_name The name of the table
 * @param offset The register containing the offset
 */
void mips_tab_get_IdxByReg(char *buffer_reg, char *tab_name, char *offset_reg);

void mips_push_stack(int size);
void mips_initialise_stack(int size);

char* add_namespace(char *name);

 #endif
