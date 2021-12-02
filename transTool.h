#ifndef TRANS_TOOL_H_
#define TRANS_TOOL_H_

#include "instructions.h"
#include "intermediaire.h"

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

void mips_load_1args(quad q);

void mips_load_2args(quad q);

void mips_beqz(char *target);

void mips_sum(char *target, char *addL, char *addR);

void mips_sub(char *target, char *subL, char *subR);

void mips_div(char *target, char *subL, char *subR);

void mips_mod(char *target, char *subL, char *subR);

void mips_mult(char *target, char *subL, char *subR);

void mips_eq(char *target, char *eqL, char *eqR);

void mips_neq(char *target, char *neqL, char *neqR);

void mips_lt(char *target, char *geqL, char *geqR);

void mips_gt(char *target, char *geqL, char *geqR);

void mips_leq(char *target, char *geqL, char *geqR);

void mips_geq(char *target, char *geqL, char *geqR);

#endif