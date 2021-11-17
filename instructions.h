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
 * @brief Calculate the sum of $a0 and $a1, return the sum value to $v0
 * 
 */
#define MIPS_INT_SUM "la $v0 ($zero)\nadd $v0 $a0 $a1\n"

/**
 * @brief @brief Calculate the diferance between $a0 and $a1 ($a0 - $a1), return the sum value to $v0
 * 
 */
#define MIPS_INT_SUB "la $t0 ($zero)\nsub $t0 $a0 $a1\nla $v0 ($t0)\n"

#define MIPS_INT_MULT "mult $a0 $a1\nmvlo $v0\n"

#define MIPS_INT_DIV "div $a0 $a1\nmvlo $v0\n"

#define MIPS_INT_MOD "div $a0 $a1\nmvhi $v0\n"

// TODO: decl, read, write tab
// TODO: decl str
// TODO: I/O

#endif