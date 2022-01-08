#include "translater.h"
#include "text_formating.h"
 /* Translater from intermediate code to ASM */

 /**
  * @brief Basically, we translate the intemediate code to mips based on the mechanism of stack machine.
  * https://www.d.umn.edu/~rmaclin/cs5641/Notes/L19_CodeGenerationI.pdf
  * 
  */

FILE *fout;
FILE *source;

void translate() {
	
	HashTable *ctx;		// pointer to the current context for calculating offsets. 
	
	if(fout == stdout)
		printf("\n# _________________________ MIPS START ________________________# \n\n\n");
	
	// Déclarations des variables globales dans le segment .data  ; "global_dec_count" est un compteur du nb de declarations globales
	// on commence à 1 car le premier quad est celui de push context, et les suivants sont tous des déclarations de variables globales.
	fprintf(fout, ".data\n");
	fprintf(fout, MIPS_MACRO); // Constant String used for 'write_bool'
	for(int i=1; i<glob_dec_count+1;i++){
		mips_dec_global(global_code[i].op1);
	}
	mips_declare_strings();
	/* Fin Déclarations globales */

	fprintf(fout, "\n.text\n");
	fprintf(fout, "\n.globl main\n");
	fprintf(fout, "\nj main\n");

	/* Static Libraries */
	fprintf(fout, "%s", MIPS_LIB_IO_WRITE_BOOL);
    fprintf(fout, "%s", MIPS_LIB_IO_READ_INT);
    fprintf(fout, "%s", MIPS_LIB_IO_WRITE_STRING);
	fprintf(fout, "%s", MIPS_LIB_IO_WRITE_INT);
    fprintf(fout, "%s", MIPS_QUIT_PROGRAM);
	fprintf(fout, "%s", MIPS_OUT_OF_BOUND);
	fprintf(fout, "%s", MIPS_DYN_CHECK);
	//fprintf(fout, "%s", MIPS_BZERO);
	fprintf(fout, "%s", MIPS_LIB_GET_TIME);
	fprintf(fout, "%s", MIPS_NO_RETURN);
	
	for (int i = glob_dec_count+1; i < nextquad; i++) {
		if(global_code[i].label != NULL)
			fprintf(fout, "\n%s:\n", global_code[i].label);
		
		switch (global_code[i].type)
        {
		case Q_FUNC:
			ctx = global_code[i].ctx;
			fprintf(fout,"\tmove $fp $sp\n");
			break;
		case Q_DECL:
			mips_load_immediate("$t0", 0);			
			mips_push_word("$t0");
			break;
		case Q_AFF:
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP){
				mips_load_1args(global_code[i].op2, ctx);
				item_table* val = lookup(global_code[i].op1.u.name, ctx);
				mips_write_stack("$t0", offset(val, ctx));
			}	
			else if(global_code[i].op1.type == QO_GLOBAL){
				if(global_code[i].op1.u.global.type == QO_SCAL){
					mips_load_1args(global_code[i].op2, ctx);
					mips_store_word("$t0", add_diez(global_code[i].op1.u.global.name));
				}
				else if(global_code[i].op1.u.global.type == QO_TAB){
					mips_load_2args(global_code[i].op3, global_code[i].op2, ctx);
					fprintf(fout, "\tmove $v0 $t1\n"); // t1 is the value to store, we save it in v0 before DYNAMIC CHECK.
					mips_tab_put_IdxByReg("$v0", global_code[i].op1.u.global.name,"$t0"); //t0 is the offset
				}
			}
			break;
		case Q_AFFADD:
			mips_load_2args(global_code[i].op1, global_code[i].op2, ctx);
			mips_sum("$v0", "$t0", "$t1");
			// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP){
				item_table *val = lookup(global_code[i].op1.u.name, ctx);
        		mips_write_stack("$v0",offset(val, ctx));
			}
			else if(global_code[i].op1.type == QO_GLOBAL)
				mips_store_word("$v0",add_diez(global_code[i].op1.u.global.name));
			break;
		case Q_AFFSUB:
			mips_load_2args(global_code[i].op1, global_code[i].op2, ctx);
			mips_sub("$v0", "$t0", "$t1");
			// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP){
				item_table* val = lookup(global_code[i].op1.u.name, ctx);
        		mips_write_stack("$v0",offset(val, ctx));
			}
			else if(global_code[i].op1.type == QO_GLOBAL)
				mips_store_word("$v0",add_diez(global_code[i].op1.u.global.name));
			break;
        case Q_ADD:
        	// These two offsets should be deduced/implied in a proper way
			mips_load_2args(global_code[i].op2, global_code[i].op3, ctx);
			mips_sum("$v0", "$t0", "$t1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP){
				item_table* val = lookup(global_code[i].op1.u.name, ctx);
        		mips_write_stack("$v0",offset(val, ctx));
			}
			else if(global_code[i].op1.type == QO_GLOBAL)
				mips_store_word("$v0",add_diez(global_code[i].op1.u.global.name));
        	break;
        case Q_SUB:
        	// These two offsets should be deduced/implied in a proper way
        	mips_load_2args(global_code[i].op2, global_code[i].op3, ctx);
			mips_sub("$v0", "$t0", "$t1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP){
				item_table* val = lookup(global_code[i].op1.u.name, ctx);
        		mips_write_stack("$v0",offset(val, ctx));
			}
			else if(global_code[i].op1.type == QO_GLOBAL)
				mips_store_word("$v0", add_diez(global_code[i].op1.u.global.name));
        	break;
        case Q_MUL:
        	// These two offsets should be deduced/implied in a proper way
        	mips_load_2args(global_code[i].op2, global_code[i].op3, ctx);
        	mips_mult("$v0", "$t0", "$t1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP){
				item_table* val = lookup(global_code[i].op1.u.name, ctx);
        		mips_write_stack("$v0",offset(val, ctx));
			}
			else if(global_code[i].op1.type == QO_GLOBAL)
				mips_store_word("$v0", add_diez(global_code[i].op1.u.global.name));
        	break;
        case Q_DIV:
        	// These two offsets should be deduced/implied in a proper way
        	mips_load_2args(global_code[i].op2, global_code[i].op3, ctx);
			mips_div("$v0", "$t0", "$t1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP){
				item_table* val = lookup(global_code[i].op1.u.name, ctx);
        		mips_write_stack("$v0",offset(val, ctx));
			}
			else if(global_code[i].op1.type == QO_GLOBAL)
				mips_store_word("$v0", add_diez(global_code[i].op1.u.global.name));
			break;
        case Q_MOD:
        	// These two offsets should be deduced/implied in a proper way
        	mips_load_2args(global_code[i].op2, global_code[i].op3, ctx);
        	mips_mod("$v0", "$t0", "$t1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID || global_code[i].op1.type == QO_TMP){
				item_table* val = lookup(global_code[i].op1.u.name, ctx);
        		mips_write_stack("$v0",offset(val, ctx));
			}
			else if(global_code[i].op1.type == QO_GLOBAL)
				mips_store_word("$v0", add_diez(global_code[i].op1.u.global.name));
        	break;
		case Q_EQ:
			if(global_code[i].jump == -1){
					// pour debugger
					fprintf(stderr,"\n\n%sParsing successful but can't translate to MIPS, incomplete EQ\n\n%s",RED,NORMAL);
					exit(0);
				}
			mips_load_2args(global_code[i].op2, global_code[i].op3, ctx);
			mips_eq("$v0", "$t0", "$t1", global_code[global_code[i].jump].label);
			break;
		case Q_NEQ:
			if(global_code[i].jump == -1){
					// pour debugger
					fprintf(stderr,"\n\n%sParsing successful but can't translate to MIPS, incomplete NEQ\n\n%s",RED,NORMAL);
					exit(0);
				}
			mips_load_2args(global_code[i].op2, global_code[i].op3, ctx);
			mips_neq("$v0", "$t0", "$t1", global_code[global_code[i].jump].label);
			break;

		case Q_LT:
			if(global_code[i].jump == -1){
					// pour debugger
					fprintf(stderr,"\n\n%sParsing successful but can't translate to MIPS, incomplete LT\n\n%s",RED,NORMAL);
					exit(0);
				}
			mips_load_2args(global_code[i].op2, global_code[i].op3, ctx);
			mips_lt("$v0", "$t0", "$t1", global_code[global_code[i].jump].label);
			break;
		case Q_GT:
			if(global_code[i].jump == -1){
					// pour debugger
					fprintf(stderr,"\n\n%sParsing successful but can't translate to MIPS, incomplete GT\n\n%s",RED,NORMAL);
					exit(0);
				}
			mips_load_2args(global_code[i].op2, global_code[i].op3, ctx);
			mips_gt("$v0", "$t0", "$t1", global_code[global_code[i].jump].label);
			break;
		case Q_LEQ:
			if(global_code[i].jump == -1){
				// pour debugger
				fprintf(stderr,"\n\n%sParsing successful but can't translate to MIPS, incomplete LEQ\n\n%s",RED,NORMAL);
				exit(0);
			}
			mips_load_2args(global_code[i].op2, global_code[i].op3, ctx);
			mips_leq("$v0", "$t0", "$t1", global_code[global_code[i].jump].label);
			break;
		case Q_GEQ:
		if(global_code[i].jump == -1){
				// pour debugger
				fprintf(stderr,"\n\n%sParsing successful but can't translate to MIPS, incomplete GEQ\n\n%s",RED,NORMAL);
				exit(0);
			}
			mips_load_2args(global_code[i].op2, global_code[i].op3, ctx);
			mips_geq("$v0", "$t0", "$t1", global_code[global_code[i].jump].label);
			break;
		case Q_GOTO:
			if(global_code[i].jump == -1){
					// pour debugger
					fprintf(stderr,"\n\n%sParsing successful but can't translate to MIPS, incomplete GOTO\n\n%s",RED,NORMAL);
					exit(0);
				}
			mips_jump(global_code[global_code[i].jump].label);
			break;
		case Q_ACCESTAB:
			mips_load_1args(global_code[i].op3, ctx);
			mips_tab_get_IdxByReg("$v0", global_code[i].op2.u.global.name, "$t0");
			item_table* val = lookup(global_code[i].op1.u.name, ctx);
			mips_write_stack("$v0", offset(val, ctx));
			break;

		case Q_METHODCALL:
			mips_method_call(global_code[i], ctx);
			break;
		case Q_SYSCALL:
			mips_syscall(global_code[i].op1.u.cst);
			break;
		case Q_RETURN:
			if(global_code[i].jump == -1){
				// pour debugger
				fprintf(stderr,"\n\n%sParsing successful but can't translate to MIPS, incomplete RETURN\n\n%s",RED,NORMAL);
				exit(0);
			}
			mips_return(global_code[i], ctx);
			break;
		case Q_POP:
			ctx = global_code[i].ctx->next;
			mips_pop_stack(global_code[i].op1.u.cst);
			break;
		case Q_ENDFUNC:
			ctx = global_code[i].ctx->next;
			fprintf(fout,"\tsub $t0 $fp $sp\n");
			fprintf(fout,"\tadd $sp $sp $t0\n");
			fprintf(fout,"\tjr $ra\n");
			break;
		case Q_PUSH:
			ctx = global_code[i].ctx;
			mips_push_stack(ctx->size);
			mips_initialise_stack(ctx->size);
			break;

		case Q_DELETED:
			break;
        default:
        	break;
        }
    }
	printf("\n");
	
}