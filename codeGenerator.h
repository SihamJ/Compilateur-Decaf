#ifndef _CODE_GENERATOR_H
#define _CODE_GENERATOR_H

/**
 * @brief Generate constant instructions (instructions without parameters)
 * 
 * @param cstInstruct 
 */
void genCstInstruction(const char *cstInstruct);

/**
 *  \brief \brief Ser the value of a register to a const val
 *  @param reg The register to modify
 *  @param val The value to load
 */
void gen_load_immediate(const char *reg, int val);

/**
 * \brief Save one Integer to the stack
 * \return The size occupied on the stack
 */
int gen_save_int(int val);

/**
 * @brief  Copy the value from stack(at offset) to the target register
 * 
 * @param target 
 * @param offset 
 */
void gen_readFromStack(const char* target, int offset);

#endif