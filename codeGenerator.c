#include <stdio.h>
#include "instructions.h"

extern FILE *fout;


void gen_load_immediate(const char *reg, int val)
{
    fprintf(fout, "li %s %d\n", reg, val);
}

int gen_save_int(int val)
{   
    gen_load_immediate("$a1", sizeof(int));
    gen_load_immediate("$a0", val);
    genCstInstruction(MIPS_PUSH);
    return sizeof(int);
}

void gen_readFromStack(const char* target, int offset)
{
    fprintf(fout, MIPS_READ_STACK, offset);
    fprintf(fout, MIPS_RETURN_TO_REG, target);
}

void genCstInstruction(const char *cstInstruct)
{
    fprintf(fout, cstInstruct);
}
