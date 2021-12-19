#include <stdio.h>
#include <string.h>
#include "transTool.h"

extern FILE *fout;
static int tmp_reg_count = 0;
static int branch_count = 0;

void mips_dec_global(quadop q){
	if(q.u.global.size == 4)
		fprintf(fout, "		%s: %s %d\n", q.u.global.name, ".word", 0);
	else
		fprintf(fout, "		%s: %s %d\n",q.u.global.name, ".space", q.u.global.size);
}

void mips_label(char *name, int n) {
	fprintf(fout, "	%s%d:\n",name, n);
}

void mips_load_immediate(char *reg, int val) {
    fprintf(fout, "	li %s, %d\n", reg, val);
}

void mips_load_word(char *reg, char *glob) {
    fprintf(fout, "	lw %s, %s\n", reg, glob);
}

void mips_load_from_addr(char *target, char *src) {
    fprintf(fout, "	la %s, (%s)\n", target, src);
}


void mips_copy_rtn_val(char *target) {
    fprintf(fout, "	la %s, ($v0)\n", target);
}

int mips_push_word(char *src) {
	if(strcmp("$a0", src))
    	mips_load_from_addr("$a0", src);
    mips_instruction(MIPS_PUSH);
    return sizeof(int);
}

void mips_store_word(char *reg, char *target){
	fprintf(fout, "	sw %s, %s\n",reg, target);
}

void mips_pop_word() {
    mips_instruction(MIPS_POP_TO_V0);
}

void mips_read_stack(char* target, int offset) {
    fprintf(fout, "	lw %s, %d($sp)\n",target, offset);
}

void mips_write_stack(char *target, int offset) {
	fprintf(fout, "	sw %s, %d($sp)\n", target, offset);
}

void mips_read_tmp(char *tmp, char *target) {
	fprintf(fout, "	la %s, (%s)\n", target, tmp);
}

void mips_write_tmp(char *tmp, char *target) {
	fprintf(fout, "	la %s, (%s)\n", tmp, target);
}

void mips_instruction(const char *cstInstruct) {
    fprintf(fout, "	%s", cstInstruct);
}

int mips_save_tmp_int(int val) {
    fprintf(fout, "	li t%d %d\n", tmp_reg_count, val);
    tmp_reg_count++;
    return tmp_reg_count-1;
}

void mips_destroy_tmp_vals(int count) {
    tmp_reg_count-=count;
}

void mips_load_1args(quad q) {
	if (q.op2.type == QO_ID || q.op2.type == QO_TMP)
		mips_read_stack("$a0", q.op2.u.offset);
	else if (q.op2.type == QO_GLOBAL)
		mips_load_word("$a0", q.op2.u.global.name);
	else if (q.op2.type == QO_CST)
		mips_load_immediate("$a0", q.op2.u.cst);
}

void mips_load_2args(quad q) {
	if (q.op2.type == QO_ID || q.op2.type == QO_TMP)
    	mips_read_stack("$a0", q.op2.u.offset);
	else if (q.op2.type == QO_GLOBAL)
		mips_load_word("$a0", q.op2.u.global.name);
	else if (q.op2.type == QO_CST)
		mips_load_immediate("$a0", q.op2.u.cst);

	if (q.op3.type == QO_ID || q.op3.type == QO_TMP)
    	mips_read_stack("$a1", q.op3.u.offset);
	else if (q.op3.type == QO_GLOBAL)
		mips_load_word("$a1", q.op3.u.global.name);
	else if (q.op3.type == QO_CST)
		mips_load_immediate("$a1", q.op3.u.cst);
}

/*void mips_beqz(char *target) {
	fprintf(fout,"beqz $a0 true%d\n", branch_count);
	mips_instruction(MIPS_FALSE);
	mips_jump("false",branch_count);
	mips_label("true",branch_count);
	mips_instruction(MIPS_TRUE);
	mips_label("false",branch_count);
	branch_count++;
}*/

// if target != null, copy result from $v0 to target
void mips_sum(char *target, char *addL, char *addR) {
    if (strcmp("$a0", addL))
        mips_load_from_addr("$a0", addL);
    if (strcmp("$a1", addR))        
        mips_load_from_addr("$a1", addR);

    mips_instruction(MIPS_INT_SUM);

    if (target != NULL && strcmp(target, "$v0"))
        mips_copy_rtn_val(target);
}

// if target != null, copy result from $v0 to target
void mips_sub(char *target, char *subL, char *subR) {
    if (strcmp("$a0", subL))
        mips_load_from_addr("$a0", subL);
    if (strcmp("$a1", subR))
        mips_load_from_addr("$a1", subR);

    mips_instruction(MIPS_INT_SUB);

    if (target != NULL && strcmp(target, "$v0"))
        mips_copy_rtn_val(target);
}

// if target != null, copy result from $v0 to target
// long int result is not supported
void mips_mult(char *target, char *multL, char *multR) {
    if (strcmp("$a0", multL))
        mips_load_from_addr("$a0", multL);
    if (strcmp("$a1", multR))
        mips_load_from_addr("$a1", multR);

    mips_instruction(MIPS_INT_MULT);

    if (target != NULL && strcmp(target, "$v0"))
        mips_copy_rtn_val(target);
}

// if target != null, copy result from $v0 to target
void mips_div(char *target, char *divL, char *divR) {
    if (strcmp("$a0", divL))
        mips_load_from_addr("$a0", divL);
    if (strcmp("$a1", divR))
        mips_load_from_addr("$a1", divR);

    mips_instruction(MIPS_INT_DIV);

    if (target != NULL && strcmp(target, "$v0"))
        mips_copy_rtn_val(target);
}

// if target != null, copy result from $v0 to target
void mips_mod(char *target, char *modL, char *modR) {
    if (strcmp("$a0", modL))
        mips_load_from_addr("$a0", modL);
    if (strcmp("$a1", modR))
        mips_load_from_addr("$a1", modR);

    mips_instruction(MIPS_INT_MOD);

    if (target != NULL && strcmp(target, "$v0"))
        mips_copy_rtn_val(target);
}

void mips_eq(char *target, char *eqL, char *eqR, char *label) {
	if (strcmp("$a0", eqL))
        mips_load_from_addr("$a0", eqL);
    if (strcmp("$a1", eqR))
        mips_load_from_addr("$a1", eqR);
	fprintf(fout,"	beq $a0 $a1 %s\n", label);
}

void mips_neq(char *target, char *neqL, char *neqR, char *label) {
	if (strcmp("$a0", neqL))
        mips_load_from_addr("$a0", neqL);
    if (strcmp("$a1", neqR))
        mips_load_from_addr("$a1", neqR);
	fprintf(fout,"	bne $a0 $a1 %s\n", label);
}

void mips_lt(char *target, char *ltL, char *ltR, char *label) {
	if (strcmp("$a0", ltL))
        mips_load_from_addr("$a0", ltL);
    if (strcmp("$a1", ltR))
        mips_load_from_addr("$a1", ltR);
	fprintf(fout,"	blt $a0 $a1 %s\n", label);
}

void mips_gt(char *target, char *gtL, char *gtR, char *label) {
	if (strcmp("$a0", gtL))
        mips_load_from_addr("$a0", gtL);
    if (strcmp("$a1", gtR))
        mips_load_from_addr("$a1", gtR);
	fprintf(fout,"	bgt $a0 $a1 %s\n", label);
}

void mips_leq(char *target, char *leqL, char *leqR, char *label) {
	if (strcmp("$a0", leqL))
        mips_load_from_addr("$a0", leqL);
    if (strcmp("$a1", leqR))
        mips_load_from_addr("$	a1", leqR);
	fprintf(fout,"ble $a0 $a1 %s\n", label);
}

void mips_geq(char *target, char *geqL, char *geqR, char *label) {
	if (strcmp("$a0", geqL))
        mips_load_from_addr("$a0", geqL);
    if (strcmp("$a1", geqR))
        mips_load_from_addr("$a1", geqR);
	fprintf(fout,"	bge $a0 $a1 %s\n", label);
}

void mips_jump(char *label){
	fprintf(fout,"	j %s\n", label);
}

void mips_syscall(int num){
	fprintf(fout, "	li $v0, %d\n",num);
	fprintf(fout,"	syscall\n");
}
/** The abstraction of "For" and "While" should be the same.
* (Looping the content in its block based on the value of a boolean. 
* @param block_content The content to loop
* @param condition_reg The register holding the boolean value
*/
/*void mips_loop(char *condition_reg, const char *block_content) {
	mips_label("loop", branch_count);
	fprintf(fout, "bnez %s, endloop%d\n", condition_reg, branch_count);
	mips_instruction(block_content);
	mips_jump("loop", branch_count);
	mips_label("loop", branch_count);
}

/**
 * Control structure if-else
 * 
 * @param condition_reg The condition register
 * @param blk_if_content The content in "if's block"
 * @param blk_el_content The content in "else's block", if this value is NULL, 
 * 							none "else" block will be generated
 */
/*void mips_if_else(char *condition_reg, const char* blk_if_content, const char * blk_el_content) {
	if (blk_el_content != NULL) {
		fprintf(fout, "beqz %s, else%d\n", condition_reg, branch_count);
		mips_instruction(blk_if_content);
		mips_jump("endif", branch_count);
		mips_label("else", branch_count);
		mips_instruction(blk_el_content);
		mips_label("endif", branch_count);
	} else {
		fprintf(fout, "beqz %s, endif%d\n", condition_reg, branch_count);
		mips_instruction(blk_if_content);
		mips_label("endif", branch_count);
	}
}*/

// /**
//  * @brief Declare a table
//  */
// void declare_tab(char *tab_name, int size) {
// 	fprintf(fout, ".data\n%s: .word %d\n", tab_name, size);
// }

/**
 * @brief Save the value in the buffer to the target table at a specified offset
 * @param buffer_reg The buffer register holding the value to save
 * @param tab_name The name of the target table
 * @param offset The offset
 */
void tab_put(char *buffer_reg, char *tab_name, int offset) {
	fprintf(fout, "	sw %s %s+%d\n", buffer_reg, tab_name, offset);
}

/**
 * @brief Save the value in the table at a specified offset to the target buffer
 * @param buffer_reg The target buffer to save a value 
 * @param tab_name The name of the table
 * @param offset The offset
 */
void tab_get(char *buffer_reg, char *tab_name, int offset) {
	fprintf(fout, "	lw %s %s+%d\n", buffer_reg, tab_name, offset);
}

void mips_decl_string(char *varName, char *value) {
	fprintf(fout, "	 %s: .asciiz \"%s\"\n", varName, value);
}
