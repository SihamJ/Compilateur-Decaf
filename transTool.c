#include <stdio.h>
#include <string.h>
#include "transTool.h"

extern FILE *fout;
static int tmp_reg_count = 0;
static int branch_count = 0;

void mips_dec_global(quadop q){
	if(q.u.global.type == QO_SCAL)
		fprintf(fout, "\t%s: %s %d\n", add_namespace(q.u.global.name),".word", 0);
	else{
		// problèmes avec strcat ...
		int size = (q.u.global.size/4)-1;
		char* str = malloc(size*3 + 2);
		str[0] = '0';
		int i = 1;
		while(i<size*3){
			str[i] =',';
			str[i+1] = ' ';
			str[i+2] = '0';
			i+=3;
		}
		str[i] ='\0';

		fprintf(fout, "\t%s: %s %s\n",add_namespace(q.u.global.name), ".word", str);
		// Save constant value length for Dynamic Check
		fprintf(fout, "\t%s_SIZE: .word %d\n", add_namespace(q.u.global.name), q.u.global.size);
	}
}

void mips_label(char *name, int n) {
	fprintf(fout, "\t%s%d:\n",name, n);
}

void mips_load_immediate(char *reg, int val) {
    fprintf(fout, "\tli %s, %d\n", reg, val);
}

void mips_load_word(char *reg, char *glob) {
    fprintf(fout, "\tlw %s, %s\n", reg, glob);
}

void mips_load_from_addr(char *target, char *src) {
    fprintf(fout, "\tla %s, (%s)\n", target, src);
}


void mips_copy_rtn_val(char *target) {
    fprintf(fout, "\tla %s, ($v0)\n", target);
}

int mips_push_word(char *src) {
	if(strcmp("$t0", src))
    	mips_load_from_addr("$t0", src);
    mips_instruction(MIPS_PUSH);
    return sizeof(int);
}

void mips_store_word(char *reg, char *target){
	fprintf(fout, "\tsw %s, %s\n",reg, target);
}

void mips_pop_word() {
    mips_instruction(MIPS_POP_TO_V0);
}

void mips_read_stack(char* target, int offset) {
    fprintf(fout, "\tlw %s, %d($sp)\n",target, offset);
}

void mips_write_stack(char *target, int offset) {
	fprintf(fout, "\tsw %s, %d($sp)\n", target, offset);
}

void mips_pop_stack(int offset){
	fprintf(fout,"\tadd $sp $sp %d\n",offset);
}

void mips_read_tmp(char *tmp, char *target) {
	fprintf(fout, "\tla %s, (%s)\n", target, tmp);
}

void mips_write_tmp(char *tmp, char *target) {
	fprintf(fout, "\tla %s, (%s)\n", tmp, target);
}

void mips_instruction(const char *cstInstruct) {
    fprintf(fout, "%s",cstInstruct);
}

int mips_save_tmp_int(int val) {
    fprintf(fout, "li t%d %d\n", tmp_reg_count, val);
    tmp_reg_count++;
    return tmp_reg_count-1;
}

void mips_destroy_tmp_vals(int count) {
    tmp_reg_count-=count;
}

void mips_load_1args(quadop op, HashTable* ctx) {
	if (op.type == QO_ID || op.type == QO_TMP){
		item_table* val = lookup(op.u.name, ctx);
		mips_read_stack("$t0", offset(val, ctx));
	}

	else if (op.type == QO_GLOBAL){
		mips_load_word("$t0", add_namespace(op.u.global.name));
	}
	else if (op.type == QO_CST)
		mips_load_immediate("$t0", op.u.cst);
}

void mips_load_2args( quadop op1, quadop op2, HashTable* ctx) {
	if (op1.type == QO_ID || op1.type == QO_TMP){
		item_table* val = lookup(op1.u.name, ctx);
		mips_read_stack("$t0", offset(val, ctx));
	}
	else if (op1.type == QO_GLOBAL)
		mips_load_word("$t0", add_namespace(op1.u.global.name));
	else if (op1.type == QO_CST)
		mips_load_immediate("$t0", op1.u.cst);

	if (op2.type == QO_ID || op2.type == QO_TMP){
		item_table* val = lookup(op2.u.name, ctx);
		mips_read_stack("$t1", offset(val, ctx));
	}
	else if (op2.type == QO_GLOBAL)
		mips_load_word("$t1", add_namespace(op2.u.global.name));
	else if (op2.type == QO_CST)
		mips_load_immediate("$t1", op2.u.cst);
}


// if target != null, copy result from $v0 to target
void mips_sum(char *target, char *addL, char *addR) {
    if (strcmp("$t0", addL))
        mips_load_from_addr("$t0", addL);
    if (strcmp("$t1", addR))        
        mips_load_from_addr("$t1", addR);

    mips_instruction(MIPS_INT_SUM);

    if (target != NULL && strcmp(target, "$v0"))
        mips_copy_rtn_val(target);
}

// if target != null, copy result from $v0 to target
void mips_sub(char *target, char *subL, char *subR) {
    if (strcmp("$t0", subL))
        mips_load_from_addr("$t0", subL);
    if (strcmp("$t1", subR))
        mips_load_from_addr("$t1", subR);

    mips_instruction(MIPS_INT_SUB);

    if (target != NULL && strcmp(target, "$v0"))
        mips_copy_rtn_val(target);
}

// if target != null, copy result from $v0 to target
// long int result is not supported
void mips_mult(char *target, char *multL, char *multR) {
    if (strcmp("$t0", multL))
        mips_load_from_addr("$t0", multL);
    if (strcmp("$t1", multR))
        mips_load_from_addr("$t1", multR);

    mips_instruction(MIPS_INT_MULT);

    if (target != NULL && strcmp(target, "$v0"))
        mips_copy_rtn_val(target);
}

// if target != null, copy result from $v0 to target
void mips_div(char *target, char *divL, char *divR) {
    if (strcmp("$t0", divL))
        mips_load_from_addr("$t0", divL);
    if (strcmp("$t1", divR))
        mips_load_from_addr("$t1", divR);

    mips_instruction(MIPS_INT_DIV);

    if (target != NULL && strcmp(target, "$v0"))
        mips_copy_rtn_val(target);
}

// if target != null, copy result from $v0 to target
void mips_mod(char *target, char *modL, char *modR) {
    if (strcmp("$t0", modL))
        mips_load_from_addr("$t0", modL);
    if (strcmp("$t1", modR))
        mips_load_from_addr("$t1", modR);

    mips_instruction(MIPS_INT_MOD);

    if (target != NULL && strcmp(target, "$v0"))
        mips_copy_rtn_val(target);
}

void mips_eq(char *target, char *eqL, char *eqR, char *label) {
	if (strcmp("$t0", eqL))
        mips_load_from_addr("$t0", eqL);
    if (strcmp("$t1", eqR))
        mips_load_from_addr("$t1", eqR);
	fprintf(fout,"\tbeq $t0 $t1 %s\n",label);
}

void mips_seq(char *target, char *eqL, char *eqR) {
	if (strcmp("$t0", eqL))
        mips_load_from_addr("$t0", eqL);
    if (strcmp("$t1", eqR))
        mips_load_from_addr("$t1", eqR);
	fprintf(fout,"\tseq %s $t0 $t1 \n", target);
}

void mips_neq(char *target, char *neqL, char *neqR, char *label) {
	if (strcmp("$t0", neqL))
        mips_load_from_addr("$t0", neqL);
    if (strcmp("$t1", neqR))
        mips_load_from_addr("$t1", neqR);
	fprintf(fout,"\tbne $t0 $t1 %s\n", label);
}

void mips_sne(char *target, char *eqL, char *eqR) {
	if (strcmp("$t0", eqL))
        mips_load_from_addr("$t0", eqL);
    if (strcmp("$t1", eqR))
        mips_load_from_addr("$t1", eqR);
	fprintf(fout,"\tsne %s $t0 $t1 \n",target);
}

void mips_lt(char *target, char *ltL, char *ltR, char *label) {
	if (strcmp("$t0", ltL))
        mips_load_from_addr("$t0", ltL);
    if (strcmp("$t1", ltR))
        mips_load_from_addr("$t1", ltR);
	fprintf(fout,"\tblt $t0 $t1 %s\n",label);
}

void mips_gt(char *target, char *gtL, char *gtR, char *label) {
	if (strcmp("$t0", gtL))
        mips_load_from_addr("$t0", gtL);
    if (strcmp("$t1", gtR))
        mips_load_from_addr("$t1", gtR);
	fprintf(fout,"\tbgt $t0 $t1 %s\n", label);
}

void mips_leq(char *target, char *leqL, char *leqR, char *label) {
	if (strcmp("$t0", leqL))
        mips_load_from_addr("$t0", leqL);
    if (strcmp("$t1", leqR))
        mips_load_from_addr("$t1", leqR);
	fprintf(fout,"\tble $t0 $t1 %s\n", label);
}

void mips_geq(char *target, char *geqL, char *geqR, char *label) {
	if (strcmp("$t0", geqL))
        mips_load_from_addr("$t0", geqL);
    if (strcmp("$t1", geqR))
        mips_load_from_addr("$t1", geqR);
	fprintf(fout,"\tbge $t0 $t1 %s\n",label);
}

void mips_jump(char *label){
	fprintf(fout,"\tj %s\n",label);
}

void mips_syscall(int num){
	fprintf(fout, "\tli $v0, %d\n",num);
	fprintf(fout,"\tsyscall\n");
}

/**
 * @brief Save the value in the buffer to the target table at a specified offset
 * @param buffer_reg The buffer register holding the value to save
 * @param tab_name The name of the target table
 * @param offset The offset
 */
void mips_tab_put(char *buffer_reg, char *tab_name, int offset) {
	fprintf(fout, "\tlw $s1 %s_SIZE\n", add_namespace(tab_name)); // Load table size to $t1
	fprintf(fout, "\tmove $t2 $ra\n"); // In case $ra is in use
	fprintf(fout, "\tli $s0 %d\n\tjal __glob_DYN_CHECK\n", offset); // Effectuate dynamic check for offset value
	fprintf(fout, "\tmove $ra $t2\n");
	fprintf(fout, "\tsw %s %s+%d\n", buffer_reg, add_namespace(tab_name), offset);
}
/* pass offset by register*/
void mips_tab_put_IdxByReg(char *buffer_reg, char *tab_name, char *offset_reg) {
	if (strcmp("$s0", offset_reg))
		fprintf(fout, "\tmove $s0 %s\n",offset_reg);
	
	fprintf(fout, "\tlw $s1 %s_SIZE\n", add_namespace(tab_name)); // Load table size to $t3
	fprintf(fout, "\tmove $t2 $ra\n"); // In case $ra is in use
	fprintf(fout, "\tjal __glob_DYN_CHECK\n"); // Effectuate dynamic check for offset value
	fprintf(fout, "\tmove $ra $t2\n");

	fprintf(fout, "\tsw %s %s(%s)\n",buffer_reg, add_namespace(tab_name), offset_reg);
}

/**
 * @brief Save the value in the table at a specified offset to the target buffer
 * @param buffer_reg The target buffer to save a value 
 * @param tab_name The name of the table
 * @param offset The offset
 */
void mips_tab_get(char *buffer_reg, char *tab_name, int offset) {
	fprintf(fout, "\tmove $t2 $ra\n"); // In case $ra is in use
	fprintf(fout, "\tlw $s1 %s_SIZE\n", add_namespace(tab_name)); // Load table size to $t3
	fprintf(fout, "\tli $s0 %d\n\tjal __glob_DYN_CHECK\n", offset); // Effectuate dynamic check for offset value
	fprintf(fout, "\tmove $ra $t2\n");

	fprintf(fout, "\tlw %s %s+%d\n", buffer_reg, add_namespace(tab_name), offset);
}
/* pass offset by register*/
void mips_tab_get_IdxByReg(char *buffer_reg, char *tab_name, char *offset_reg) {
	if (strcmp("$s0", offset_reg))
		fprintf(fout, "\tmove $s0 %s\n",offset_reg);

	fprintf(fout, "\tlw $s1 %s_SIZE\n", add_namespace(tab_name)); // Load table size to $t1
	fprintf(fout, "\tmove $t2 $ra\n"); // In case $ra is in use
	fprintf(fout, "\tjal __glob_DYN_CHECK\n"); // Effectuate dynamic check for offset value
	fprintf(fout, "\tmove $ra $t2\n");

	fprintf(fout, "\tlw %s %s(%s)\n", buffer_reg, add_namespace(tab_name), offset_reg);
}

void mips_decl_string(char *varName, char *value) {
	fprintf(fout, "\t%s: .asciiz %s\n",varName, value);
}

void mips_method_call(quad q, HashTable *ctx){

	// we save $ra and $fp in the stack
	fprintf(fout, "\tmove $t0 $ra\n");
	mips_push_word("$t0");
	fprintf(fout, "\tmove $t0 $fp\n");
	mips_push_word("$t0");

	int size = 0;
	// push args in stack and return the number of parameters in size so we can pop them after jal
	if(q.p != NULL && strcmp(q.op1.u.string_literal.label,"WriteString") && strcmp(q.op1.u.string_literal.label,"ReadInt"))
		size = mips_push_args(q.p, ctx);

	// if we are calling WriteString, the argument is in the declared labels
	else if(!strcmp(q.op1.u.string_literal.label,"WriteString")){
		fprintf(fout,"\n\tla $a0 %s\n",q.p->arg.u.string_literal.label);
	}

	// we initialize to 0 the register indicating if there is a return value
	mips_load_immediate("$v1",0);
	// jump and link
	fprintf(fout, "\tjal %s\n",q.op1.u.string_literal.label); //jump and link 

	// popping method call parameters from the stack
	if(size>0)
		mips_pop_stack(size*4);
	
	// we retrieve $fp and $ra from the stack
	mips_read_stack("$t0",0);
	fprintf(fout, "\tmove $fp $t0\n");
	mips_pop_stack(4); // we pop $fp from the stack
	mips_read_stack("$t0",0);
	fprintf(fout, "\tmove $ra $t0\n");
	mips_pop_stack(4); // we pop $ra from the stack

	// if there is a return value, we save it to the corresponding offset
	if(q.op2.type == QO_CST){
		fprintf(fout,"\tbeqz $v1 __glob_No_Return\n"); // verif dynamique for return value
		item_table *val = lookup(q.op3.u.name, ctx);
		mips_write_stack("$v0", offset(val,ctx));
	}
	
	// if we called ReadInt, we store the value in the corresponding location
	if(!strcmp(q.op1.u.string_literal.label,"ReadInt")){
		if(q.p->arg.type == QO_ID){
			item_table *val = lookup(q.p->arg.u.name, ctx);
			mips_write_stack("$v0", offset(val,ctx));
		}
		else if(q.p->arg.type == QO_GLOBAL && q.p->arg.u.global.type == QO_SCAL)
			fprintf(fout,"\tsw $v0 %s\n",add_namespace(q.p->arg.u.global.name));
	}

}

int mips_push_args(param p, HashTable *ctx){
	int size = 0;

	while(p!=NULL){
		if(p->arg.type == QO_ID || p->arg.type == QO_TMP){
		// reading arguments from stack, we add the offset of ra + fp + the args that were pushed before
		item_table* val = lookup(p->arg.u.name, ctx);
			mips_read_stack("$t0",offset(val, ctx)+(size+2)*4); 
		}
		else if(p->arg.type == QO_GLOBAL){
			mips_load_word("$t0", add_namespace(p->arg.u.global.name));
			// can't be an array!
		}
		else if(p->arg.type == QO_CST)
			mips_load_immediate("$t0", p->arg.u.cst);
			
		mips_push_word("$t0");
		p = p->next;
		size++;
	}
	return size;
}

void mips_return(quad q, HashTable *ctx){

	/* if there is a return value, we store it in $vo and indicate in $v1 that there is one*/
	if(q.op2.type != QO_EMPTY){
		if(q.op1.type == QO_CST){
			mips_load_immediate("$v0", q.op1.u.cst);
			mips_load_immediate("$v1",1);
		}
		else if(q.op1.type == QO_ID || q.op1.type == QO_TMP){
			item_table* val = lookup(q.op1.u.name, ctx);
			mips_read_stack("$v0", offset(val, ctx));
			mips_load_immediate("$v1",1);
		}
		else if(q.op1.type == QO_GLOBAL){
			if(q.op1.u.global.type == QO_SCAL){
				mips_load_word("$v0", add_namespace(q.op1.u.global.name));
				mips_load_immediate("$v1",1);
			}
		}
	}
	/* we jump to the end of the function label*/
	fprintf(fout,"\tj %s\n",global_code[q.jump].label);
}

void mips_declare_strings(){
	int i = 0;
	while(i<str_count){
		mips_decl_string(str_labels[i].label, str_labels[i].value);
		i++;
	}
}

void mips_push_stack(int size){
	fprintf(fout, "\tsub $sp $sp %d\n",size);
}

void mips_initialise_stack(int size){
	size -= 4;
	for (int i=0; i <= size; i+=4){
		mips_load_immediate("$t0", 0);
		mips_write_stack("$t0", i);
	}
}

char* add_namespace(char *name){
	char* res = malloc(strlen(name)+8);
	strcpy(res, "__glob_");
	strcpy(res+7,name);
	return res;
} 