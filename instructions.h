#ifndef OPERATIONS_H_
#define OPERATIONS_H_

#define MIPS_TRUE "  li $v0 1\n"

#define MIPS_FALSE "  li $v0 0\n"

/**
 * @param $a0 variable value
 * @param $a1 cariable size
*/
#define MIPS_PUSH "  sub $sp $sp 4\n   sw $a0 ($sp)\n"

/**
 * @param $a0 Size of the variable to pop out
 * \return $v0 Value of the variable popped out
 */
#define MIPS_POP_TO_V0 "   lw $v0 ($sp)\n   add $sp $sp 4\n"


/**
 * @param $a0 Size of the stack to pop out
 */
#define MIPS_ELIM_STACK "   addiu $sp $sp $a0\n"

/**
 * @brief Calculate the sum of $a0 and $a1, return the sum value to $v0
 * 
 */
#define MIPS_INT_SUM "   add $v0 $a0 $a1\n"

/**
 * @brief @brief Calculate the diferance between $a0 and $a1 ($a0 - $a1), return the sum value to $v0
 */ 
#define MIPS_INT_SUB "  sub $v0 $a0 $a1\n"

#define MIPS_INT_MULT "  mult $a0 $a1\n   mflo $v0\n"

#define MIPS_INT_DIV "  div $a0 $a1\n   mflo $v0\n"

#define MIPS_INT_MOD "  div $a0 $a1\n   mfhi $v0\n"

/**
* Assuming the value to print is stored in $a0
* Called by jal, jalr $target_reg or jalr $ra $target_reg
*/
#define MIPS_LIB_IO_WRITE_INT "Write_INT:\n  li $v0, 1\n  syscall\n  jr $ra\n"

#define MIPS_LIB_IO_WRITE_STRING "Print_String:\n  li $v0 4\n  syscall\n  jr $ra\n"

/*
* The value read will be stored in $v0
*/
#define MIPS_LIB_IO_READ_INT "Read_INT:\n  li $v0, 5\n  syscall\n  jr $ra\n"



#define MIPS_MACRO ".data\n  STR_TRUE: .asciiz \"True\"\n  STR_FALSE: .asciiz \"False\"\n  STR_DYN_CHECK: .asciiz \"Index Out Of Bound\"\n"

/**
* Assuming the value to print is stored in $a0
* Called by jal, jalr $target_reg or jalr $ra $target_reg
* Assuming having 'True' and 'False' stored as literal string in STR_TRUE and STR_FALSE
*/
#define MIPS_LIB_IO_WRITE_BOOL "Write_BOOL:\n  beqz $a0 Load_False\n  Load_True:\n  la $a0 STR_TRUE\n  j Print_Bool\n  Load_False:\n    la $a0 STR_FALSE\n  Print_Bool:\n    li $v0 4\n    syscall\n  jr $ra\n"

/**
 * Assuming the index is in $a0
 * Assuming the uppder bound of the array is in $a1
 * Called by jal when accessing tables
 */
#define MIPS_DYN_CHECK "DYN_CHECK:\n  bltz $a0 Out_Of_Bound\n  bge $a0 $a1 Out_Of_Bound\n  jr $ra\n"

/* Print Error and quit program */
#define MIPS_OUT_OF_BOUND "Out_Of_Bound:\n  la $a0 STR_DYN_CHECK\n  li $v0 4\n  syscall\n  j Quit_Program\n"

#define MIPS_QUIT_PROGRAM "Quit_Program:\n  li $v0 10 \n  syscall\n"

// TODO: decl, read, write tab
// TODO: decl str

#endif