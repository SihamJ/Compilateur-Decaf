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


#define MIPS_MACRO ".data\nSTR_TRUE: .asciiz \"True\"\n\tSTR_FALSE: .asciiz \"False\"\n"

/**
* Assuming the value to print is stored in $a0
* Called by jal, jalr $target_reg or jalr $ra $target_reg
* Assuming having 'True' and 'False' stored as literal string in STR_TRUE and STR_FALSE
*/
#define MIPS_LIB_IO_WRITE_BOOL "\nWrite_BOOL:\n\tbeqz $a0 Load_False\n\tLoad_True:\n\tla $a0 STR_TRUE\n\tj Print_Bool\n\tLoad_False:\n\tla $a0 STR_FALSE\n\tPrint_Bool:\n\tli $v0 4\n\tsyscall\n\tjr $ra\n"

#define MIPS_QUIT_PROGRAM "\nQuit_Program:\n\tli $v0 10 \n\tsyscall\n"

// TODO: decl, read, write tab
// TODO: decl str

#endif