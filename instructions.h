#ifndef OPERATIONS_H_
#define OPERATIONS_H_

#define MIPS_TRUE "\tli $v0 1\n"

#define MIPS_FALSE "\tli $v0 0\n"

/**
 * @param $a0 variable value
 * @param $a1 cariable size
*/
#define MIPS_PUSH "\tsub $sp $sp 4\n\tsw $a0 ($sp)\n"

/**
 * @param $a0 Size of the variable to pop out
 * \return $v0 Value of the variable popped out
 */
#define MIPS_POP_TO_V0 "\tlw $v0 ($sp)\n\tadd $sp $sp 4\n"


/**
 * @param $a0 Size of the stack to pop out
 */
#define MIPS_ELIM_STACK "\taddiu $sp $sp $a0\n"

/**
 * @brief Calculate the sum of $a0 and $a1, return the sum value to $v0
 * 
 */
#define MIPS_INT_SUM "\tadd $v0 $a0 $a1\n"

/**
 * @brief @brief Calculate the diferance between $a0 and $a1 ($a0 - $a1), return the sum value to $v0
 */ 
#define MIPS_INT_SUB "\tsub $v0 $a0 $a1\n"

#define MIPS_INT_MULT "\tmult $a0 $a1\n\tmflo $v0\n"

#define MIPS_INT_DIV "\tdiv $a0 $a1\n\tmflo $v0\n"

#define MIPS_INT_MOD "\tdiv $a0 $a1\n\tmfhi $v0\n"

/**
* Assuming the value to print is stored in $a0
* Called by jal, jalr $target_reg or jalr $ra $target_reg
*/
#define MIPS_LIB_IO_WRITE_INT "\nWrite_INT:\n\tli $v0, 1\n\tsyscall\n\tjr $ra\n"

#define MIPS_LIB_IO_WRITE_STRING "\nPrint_String:\n\tli $v0 4\n\tsyscall\n\tjr $ra\n"

/*
* The value read will be stored in $v0
*/
#define MIPS_LIB_IO_READ_INT "\nRead_INT:\n\tli $v0, 5\n\tsyscall\n\tjr $ra\n"



#define MIPS_MACRO "\tSTR_TRUE: .asciiz \"True\"\n\tSTR_FALSE: .asciiz \"False\"\nSTR_DYN_CHECK: .asciiz \"Index Out Of Bound\"\n"

/**
* Assuming the value to print is stored in $a0
* Called by jal, jalr $target_reg or jalr $ra $target_reg
* Assuming having 'True' and 'False' stored as literal string in STR_TRUE and STR_FALSE
*/
#define MIPS_LIB_IO_WRITE_BOOL "\nWrite_BOOL:\n\tbeqz $a0 Load_False\n\tLoad_True:\n\tla $a0 STR_TRUE\n\tj Print_Bool\n\tLoad_False:\n\tla $a0 STR_FALSE\n\tPrint_Bool:\n\tli $v0 4\n\tsyscall\n\tjr $ra\n"

/**
 * Assuming the index is in $a0
 * Assuming the uppder bound of the array is in $a1
 * Called by jal when accessing tables
 */
#define MIPS_DYN_CHECK "DYN_CHECK:\n  bltz $a0 Out_Of_Bound\n  bge $a0 $a1 Out_Of_Bound\n  jr $ra\n"

/* Print Error and quit program */
#define MIPS_OUT_OF_BOUND "Out_Of_Bound:\n  la $a0 STR_DYN_CHECK\n  li $v0 4\n  syscall\n  j Quit_Program\n"

#define MIPS_QUIT_PROGRAM "\nQuit_Program:\n\tli $v0 10 \n\tsyscall\n"

/**
 * @brief bzero() fuction in Mips
 * Assuming $a0 has the target label name
 * $a1 has the size
 * Called by jal
 */

#define MIPS_BZERO "BZero:\n  sub $a1 $a2 1\n  sw $zero ($a0)\n  addu $a0 $a0 4\n  bgtz $a1 BZero\n  jr $ra\n"

// TODO: decl str

#endif
