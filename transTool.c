#include <stdio.h>
#include <string.h>
#include "instructions.h"
#include "transTool.h"

extern FILE *fout;
static int tmp_reg_count = 0;


void mips_load_immediate(char *reg, int val) {
    fprintf(fout, "li %s %d\n", reg, val);
}

void mips_load_from_addr(char *target, char *src) {
    fprintf(fout, "la %s (%s)\n", target, src);
}


void mips_copy_rtn_val(char *target) {
    fprintf(fout, "la %s ($v0)\n", target);
}

int mips_push_word(char *src) {
    mips_load_from_addr("$a0", src);
    mips_instruction(MIPS_PUSH);
    return sizeof(int);
}

void mips_pop_word() {
    mips_instruction(MIPS_POP_TO_V0);
}

void mips_read_stack(char* target, int offset) {
    fprintf(fout, "lw $v0 %d($sp)\n", offset);

    if (target!=NULL)
        mips_copy_rtn_val(target);
}

void mips_write_stack(char *target, int offset) {
	fprintf(fout, "sw %s %d($sp)\n", target, offset);
}

void mips_read_tmp(char *tmp, char *target) {
	fprintf(fout, "la %s (%s)\n", target, tmp);
}

void mips_write_tmp(char *tmp, char *target) {
	fprintf(fout, "la %s (%s)\n", tmp, target);
}

void mips_instruction(const char *cstInstruct) {
    fprintf(fout, "%s", cstInstruct);
}

int mips_save_tmp_int(int val) {
    fprintf(fout, "li $t%d %d\n", tmp_reg_count, val);
    tmp_reg_count++;
    return tmp_reg_count-1;
}

void mips_destroy_tmp_vals(int count) {
    tmp_reg_count-=count;
}

// if target != null, copy result from $v0 to target
void mips_sum(char *target, char *addL, char *addR) {
    if (strcmp("$a0", addL))
        mips_load_from_addr("$a0", addL);
    if (strcmp("$a1", addR))        
        mips_load_from_addr("$a1", addR);

    mips_instruction(MIPS_INT_SUM);

    if (target != NULL && cmp(target, "$v0"))
        mips_copy_rtn_val(target);
}

// if target != null, copy result from $v0 to target
void mips_sub(char *target, char *subL, char *subR) {
    if (strcmp("$a0", subL))
        mips_load_from_addr("$a0", subL);
    if (strcmp("$a1", subR))
        mips_load_from_addr("$a1", subR);

    mips_instruction(MIPS_INT_SUB);

    if (target != NULL && cmp(target, "$v0"))
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

    if (target != NULL && cmp(target, "$v0"))
        mips_copy_rtn_val(target);
}

// if target != null, copy result from $v0 to target
void mips_div(char *target, char *divL, char *divR) {
    if (strcmp("$a0", divL))
        mips_load_from_addr("$a0", divL);
    if (strcmp("$a1", divR))
        mips_load_from_addr("$a1", divR);

    mips_instruction(MIPS_INT_DIV);

    if (target != NULL && cmp(target, "$v0"))
        mips_copy_rtn_val(target);
}

// if target != null, copy result from $v0 to target
void mips_mod(char *target, char *modL, char *modR) {
    if (strcmp("$a0", modL))
        mips_load_from_addr("$a0", modL);
    if (strcmp("$a1", modR))
        mips_load_from_addr("$a1", modR);

    mips_instruction(MIPS_INT_MOD);

    if (target != NULL && cmp(target, "$v0"))
        mips_copy_rtn_val(target);
}
