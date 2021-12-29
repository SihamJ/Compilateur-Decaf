#ifndef OPERATIONS_H_
#define OPERATIONS_H_

#define MIPS_TRUE "\tli $v0 1\n"

#define MIPS_FALSE "\tli $v0 0\n"

/**
 * @param $t0 variable value
 * @param $t1 cariable size
*/
#define MIPS_PUSH "\tsub $sp $sp 4\n\tsw $t0 ($sp)\n"

/**
 * @param $t0 Size of the variable to pop out
 * \return $v0 Value of the variable popped out
 */
#define MIPS_POP_TO_V0 "\tlw $v0 ($sp)\n\tadd $sp $sp 4\n"


/**
 * @param $t0 Size of the stack to pop out
 */
#define MIPS_ELIM_STACK "\taddiu $sp $sp $t0\n"

/**
 * @brief Calculate the sum of $t0 and $t1, return the sum value to $v0
 * 
 */
#define MIPS_INT_SUM "\tadd $v0 $t0 $t1\n"

/**
 * @brief @brief Calculate the diferance between $t0 and $t1 ($t0 - $t1), return the sum value to $v0
 */ 
#define MIPS_INT_SUB "\tsub $v0 $t0 $t1\n"

#define MIPS_INT_MULT "\tmult $t0 $t1\n\tmflo $v0\n"

#define MIPS_INT_DIV "\tdiv $t0 $t1\n\tmflo $v0\n"

#define MIPS_INT_MOD "\tdiv $t0 $t1\n\tmfhi $v0\n"

/**
* Assuming the value to print is in the stack
* Called by jal, jalr $target_reg or jalr $ra $target_reg
*/
#define MIPS_LIB_IO_WRITE_INT "\nWriteInt:\n\tlw $a0, 0($sp)\n\tli $v0, 1\n\tsyscall\n\tjr $ra\n"

#define MIPS_LIB_IO_WRITE_STRING "\nWriteString:\n\tli $v0 4\n\tsyscall\n\tjr $ra\n"

/*
* The value read will be stored in $v0
*/
#define MIPS_LIB_IO_READ_INT "\nReadInt:\n\tli $v0, 5\n\tsyscall\n\tjr $ra\n"



#define MIPS_MACRO "\nSTR_TRUE: .asciiz \"true\"\n\tSTR_FALSE: .asciiz \"false\"\nSTR_DYN_CHECK: .asciiz \"Index Out Of Bound\"\n"

/**
* Assuming the value to print is stored in the stack
* Called by jal, jalr $target_reg or jalr $ra $target_reg
* Assuming having 'True' and 'False' stored as literal string in STR_TRUE and STR_FALSE
*/
#define MIPS_LIB_IO_WRITE_BOOL "\nWriteBool:\n\tlw $a0, 0($sp)\n\tbeqz $a0 Load_False\n\tLoad_True:\n\tla $a0 STR_TRUE\n\tj Print_Bool\n\tLoad_False:\n\tla $a0 STR_FALSE\n\tPrint_Bool:\n\tli $v0 4\n\tsyscall\n\tjr $ra\n"

/**
 * Assuming the index is in $t0
 * Assuming the uppder bound of the array is in $t1
 * Called by jal when accessing tables
 */
#define MIPS_DYN_CHECK "\nDYN_CHECK:\n\tbltz $s0 Quit_Program\n\tbge $s0 $s1 Quit_Program\n\tjr $ra\n"

/* Print Error and quit program */
#define MIPS_OUT_OF_BOUND "\nOut_Of_Bound:\n\tla $a0 STR_DYN_CHECK\n\tli $v0 4\n\tsyscall\n\tj Quit_Program\n"

// #define MIPS_OUT_OF_BOUND "\nOut_Of_Bound:\n\tli $v0 4\n\tsyscall\n\tj Quit_Program\n"

#define MIPS_QUIT_PROGRAM "\nQuit_Program:\n\tli $v0 10 \n\tsyscall\n"

/**
 * @brief bzero() fuction in Mips
 * Assuming $t0 has the target label name
 * $t1 has the size
 * Called by jal
 */

#define MIPS_BZERO "\nBZero:\n\taddi $s1 $s1 -1\n\tsw $zero ($s0)\n\taddu $s0 $s0 4\n\tbgtz $s1 BZero\n\tjr $ra\n"

// TODO: decl str

#endif
