#include <stdio.h>
#include "instructions.h"
#include "symbol_table.h"
#include "codeGenerator.h"
extern FILE *fout;

/**
 * @attention By convevntion Mips fonction's return value is always in register $v0
 * 
 * @attention First four parameters of a function should be passed by registers $a0-$a3
 *              any extra parameters should be passed using stack
 * 
 */


/** \brief This function pops the current context and save the deepest variable (aka Return value, if the ctx is for a function call) 
 *  in ctx to register $v0 (Register for return value by convention)
 * 
 *  */
void* pop_context(context *ctx) {
    int countSize = 0;
    entry *cursor = ctx->lastEntry;

    while (cursor->parent) {
        countSize+=cursor->type.size;
        cursor=cursor->parent;
    }
    
    gen_load_immediate("$a0", countSize);
    genCstInstruction(MIPS_ELIM_STACK);

    gen_load_immediate("$a0", cursor->type.size);
    genCstInstruction(MIPS_POP_TO_V0);
}