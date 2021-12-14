#include "translater.h"

 /* Translater from intermediate code to ASM */

 /**
  * @brief Basically, we translate the intemediate code to mips based on the mechanism of stack machine.
  * https://www.d.umn.edu/~rmaclin/cs5641/Notes/L19_CodeGenerationI.pdf
  * 
  */

FILE *fout;
void translate() {
	printf("\n# _________________________ MIPS START ________________________# \n\n\n");
	
	/* Déclarations des variables globales dans le segment .data  ; "global_dec_count" est un compteur du nb de declarations globales*/
	fprintf(fout, ".data\n");
	for(int i=0; i<glob_dec_count;i++){
		mips_dec_global(global_code[i].op1);
	}
	/* Fin Déclarations globales */

	fprintf(fout, "\n.text\n");
	fprintf(fout, "\n.globl main\n\n");

	fprintf(fout, "j main\n\n");


	for (int i = glob_dec_count; i < nextquad; i++) {
		if(global_code[i].label != NULL)
			fprintf(fout, "\n%s:\n", global_code[i].label);

		switch (global_code[i].type)
        {
		case Q_DECL:
			mips_load_immediate("$a0", 0);			
			mips_push_word("$a0");
			break;
		case Q_AFF:
			mips_load_1args(global_code[i]);
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP)			
				mips_write_stack("$a0", global_code[i].op1.u.offset);
			else if(global_code[i].op1.type == QO_GLOBAL)
				mips_store_word("$a0", global_code[i].op1.u.global.name);
			break;
        case Q_ADD:
        	// These two offsets should be deduced/implied in a proper way
			mips_load_2args(global_code[i]);
			mips_sum("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else if(global_code[i].op1.type == QO_GLOBAL)
				mips_store_word("$v0",global_code[i].op1.u.global.name);
        	break;
        case Q_SUB:
        	// These two offsets should be deduced/implied in a proper way
        	mips_load_2args(global_code[i]);
			mips_sub("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else if(global_code[i].op1.type == QO_GLOBAL)
				mips_store_word("$v0",global_code[i].op1.u.global.name);
        	break;
        case Q_MUL:
        	// These two offsets should be deduced/implied in a proper way
        	mips_load_2args(global_code[i]);
        	mips_mult("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else if(global_code[i].op1.type == QO_GLOBAL)
				mips_store_word("$v0",global_code[i].op1.u.global.name);
        	break;
        case Q_DIV:
        	// These two offsets should be deduced/implied in a proper way
        	mips_load_2args(global_code[i]);
			mips_div("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else if(global_code[i].op1.type == QO_GLOBAL)
				mips_store_word("$v0",global_code[i].op1.u.global.name);
			break;
        case Q_MOD:
        	// These two offsets should be deduced/implied in a proper way
        	mips_load_2args(global_code[i]);
        	mips_mod("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else if(global_code[i].op1.type == QO_GLOBAL)
				mips_store_word("$v0",global_code[i].op1.u.global.name);
        	break;
		case Q_EQ:
			mips_load_2args(global_code[i]);
			mips_eq("$v0", "$a0", "$a1", global_code[global_code[i].jump].label);
			break;
		case Q_NEQ:
			mips_load_2args(global_code[i]);
			mips_neq("$v0", "$a0", "$a1", global_code[global_code[i].jump].label);
			break;
		case Q_LT:
			mips_load_2args(global_code[i]);
			mips_lt("$v0", "$a0", "$a1", global_code[global_code[i].jump].label);
			break;
		case Q_GT:
			mips_load_2args(global_code[i]);
			mips_gt("$v0", "$a0", "$a1", global_code[global_code[i].jump].label);
			break;
		case Q_LEQ:
			mips_load_2args(global_code[i]);
			mips_leq("$v0", "$a0", "$a1", global_code[global_code[i].jump].label);
			break;
		case Q_GEQ:
			mips_load_2args(global_code[i]);
			mips_geq("$v0", "$a0", "$a1", global_code[global_code[i].jump].label);
			break;
		case Q_GOTO:
			mips_jump(global_code[global_code[i].jump].label);
			break;
		case Q_SYSCALL:
			mips_syscall(global_code[i].op1.u.cst);
			break;
        default:
        	break;
        }
    }
	printf("\n");
}