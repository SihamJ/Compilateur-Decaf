#ifndef OPERATIONS_H_
#define OPERATIONS_H_

#define MIPS_TRUE "li $v0 1\n"

#define MIPS_FALSE "li $v0 0\n"

/**
 * @param $a0 variable value
 * @param $a1 cariable size
*/
#define MIPS_PUSH "sub $sp $sp 4\nsw $a0 ($sp)\n"

/**
 * @param $a0 Size of the variable to pop out
 * \return $v0 Value of the variable popped out
 */
#define MIPS_POP_TO_V0 "lw $v0 ($sp)\nadd $sp $sp 4\n"


/**
 * @param $a0 Size of the stack to pop out
 */
#define MIPS_ELIM_STACK "addiu $sp $sp $a0\n"

/**
 * @brief Calculate the sum of $a0 and $a1, return the sum value to $v0
 * 
 */
#define MIPS_INT_SUM "add $v0 $a0 $a1\n"

/**
 * @brief @brief Calculate the diferance between $a0 and $a1 ($a0 - $a1), return the sum value to $v0
 * 
 */
#define MIPS_INT_SUB "sub $v0 $a0 $a1\n"

#define MIPS_INT_MULT "mult $a0 $a1\nmflo $v0\n"

#define MIPS_INT_DIV "div $a0 $a1\nmflo $v0\n"

#define MIPS_INT_MOD "div $a0 $a1\nmfhi $v0\n"


// TODO: decl, read, write tab
// TODO: decl str
// TODO: I/O

#endif