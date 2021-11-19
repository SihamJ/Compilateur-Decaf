#ifndef TRANS_TOOL_H_
#define TRANS_TOOL_H_

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
 * \brief Save one Integer to the stack
 * \return The size occupied on the stack
 */
int mips_save_const_int(int val);

/**
 * @brief  Copy the value from stack(at offset) to the target register
 * 
 * @param target Load the value in stack(with offset) to the target register
 * @param offset 
 */
void mips_read_stack(char *target, int offset);

#endif