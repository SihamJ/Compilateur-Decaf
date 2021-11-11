#ifndef OPERATIONS_H_
#define OPERATIONS_H_

/**
 * @param $a0 variable value
 * @param $a1 cariable size
*/
#define MIPS_PUSH "sub $sp $sp $a1\nsw $a0 ($sp)\n"

/**
 * @param $a0 Size of the variable to pop out
 * \return $v0 Value of the variable popped out
 */
#define MIPS_POP_TO_V0 "lw $v0 ($sp)\nadd $sp $sp $a0\n"


/**
 * @param $a0 Size of the stack to pop out
 */
#define MIPS_ELIM_STACK "addiu $sp $sp $a0\n"

/**
 * @brief Read value from stack at offset
 * @attention Always offer offset when using this instruction
 * \return $v0 Result value of reading
 */
#define MIPS_READ_STACK "lw $v0 %d($sp)\n"

/**
 * @brief By convention, a MIPS function's return value is stored in $v0
 *         this instruction copies the value to another register
 */
#define MIPS_RETURN_TO_REG "la %s ($v0)\n"

/**
 * @brief Calculate the sum of $a0 and $a1, return the sum value to $v0
 * 
 */
#define MIPS_INT_SUM "la $t0 ($zero)\nadd $t0 $a0 $a1\nla $v0 ($t0)\n"

/**
 * @brief @brief Calculate the diferance between $a0 and $a1 ($a0 - $a1), return the sum value to $v0
 * 
 */
#define MIPS_INT_SUB "la $t0 ($zero)\nadd $t0 $a0 $a1\nla $v0 ($t0)\n"


#endif