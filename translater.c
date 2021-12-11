#include "translater.h"

 /* Translater from intermediate code to ASM */

 /**
  * @brief Basically, we translate the intemediate code to mips based on the mechanism of stack machine.
  * https://www.d.umn.edu/~rmaclin/cs5641/Notes/L19_CodeGenerationI.pdf
  * 
  */

FILE *fout;
void translate() {
	for (int i=0; i < nextquad; i++) {
		switch (global_code[i].type)
        {
		case Q_DECL:
			mips_load_1args(global_code[i]);			
			mips_push_word("$a0");
			break;
		case Q_AFF:
			mips_load_1args(global_code[i]);			
			mips_write_stack("$a0", global_code[i].op1.u.offset);
			break;
		case Q_AFFADD:
			mips_load_2args(global_code[i]);
			mips_sum("$v0", "$a0", "$a1");
			mips_write_stack("$v0", global_code[i].op1.u.offset);
			break;
		case Q_AFFSUB:
			mips_load_2args(global_code[i]);
			mips_sub("$v0", "$a0", "$a1");
			mips_write_stack("$v0", global_code[i].op1.u.offset);
			break;
		case Q_NOT:
			mips_load_1args(global_code[i]);
			mips_beqz("$a0");
			mips_write_tmp(global_code[i].op1.u.temp, "$v0");
			break;
        case Q_ADD:
        	// These two offsets should be deduced/implied in a proper way
			mips_load_2args(global_code[i]);
			mips_sum("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
        	break;
        case Q_SUB:
        	// These two offsets should be deduced/implied in a proper way
        	mips_load_2args(global_code[i]);
			mips_sub("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
        	break;
        case Q_MUL:
        	// These two offsets should be deduced/implied in a proper way
        	mips_load_2args(global_code[i]);
        	mips_mult("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
        	break;
        case Q_DIV:
        	// These two offsets should be deduced/implied in a proper way
        	mips_load_2args(global_code[i]);
			mips_div("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
        	break;
        case Q_MOD:
        	// These two offsets should be deduced/implied in a proper way
        	mips_load_2args(global_code[i]);
        	mips_mod("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
        	break;
		case Q_EQ:
			mips_load_2args(global_code[i]);
			mips_eq("$v0", "$a0", "$a1");
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
			break;
		case Q_NEQ:
			mips_load_2args(global_code[i]);
			mips_neq("$v0", "$a0", "$a1");
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
			break;
		case Q_LT:
			mips_load_2args(global_code[i]);
			mips_lt("$v0", "$a0", "$a1");
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
			break;
		case Q_GT:
			mips_load_2args(global_code[i]);
			mips_gt("$v0", "$a0", "$a1");
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
			break;
		case Q_LEQ:
			mips_load_2args(global_code[i]);
			mips_leq("$v0", "$a0", "$a1");
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
			break;
		case Q_GEQ:
			mips_load_2args(global_code[i]);
			mips_geq("$v0", "$a0", "$a1");
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
			break;
        default:
        	break;
        }
    }
}