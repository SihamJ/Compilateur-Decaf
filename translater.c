#include "translater.h"

 /* Translater from intermediate code to ASM */

 /**
  * @brief Basically, we translate the intemediate code to mips based on the mechanism of stack machine.
  * https://www.d.umn.edu/~rmaclin/cs5641/Notes/L19_CodeGenerationI.pdf
  * 
  */

void translate() {
	for (int i=0; i < nextquad; i++) {
    	switch (global_code[i].type)
        {
		case Q_AFF:
			if (global_code[i].op2.type == QO_ID)
				mips_read_stack("$a0", global_code[i].op2.u.offset);
			else if (global_code[i].op2.type == QO_TMP)
				mips_read_tmp(global_code[i].op2.u.temp, "$a0");
			else
				mips_load_immediate("$a0", global_code[i].op2.u.cst);
			
			if(global_code[i].op1.u.offset == -4)
				mips_push_word("$a0");
			else
				mips_write_stack("$a0", global_code[i].op1.u.offset);
			break;
        case Q_ADD:
        	// These two offsets should be deduced/implied in a proper way
			if (global_code[i].op2.type == QO_ID)
        		mips_read_stack("$a0", global_code[i].op2.u.offset);
			else if (global_code[i].op2.type == QO_TMP)
        		mips_read_tmp(global_code[i].op2.u.temp, "$a0");
			else
				mips_load_immediate("$a0", global_code[i].op2.u.cst);
			
			if (global_code[i].op3.type == QO_ID)
        		mips_read_stack("$a1", global_code[i].op3.u.offset);
			else if (global_code[i].op3.type == QO_TMP)
        		mips_read_tmp(global_code[i].op3.u.temp, "$a1");
			else
				mips_load_immediate("$a1", global_code[i].op3.u.cst);
        	
			mips_sum("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
        	break;

        case Q_SUB:
        	// These two offsets should be deduced/implied in a proper way
        	if (global_code[i].op2.type == QO_ID)
        		mips_read_stack("$a0", global_code[i].op2.u.offset);
			else if (global_code[i].op2.type == QO_TMP)
        		mips_read_tmp(global_code[i].op2.u.temp, "$a0");
			else
				mips_load_immediate("$a0", global_code[i].op2.u.cst);
			
			if (global_code[i].op3.type == QO_ID)
        		mips_read_stack("$a1", global_code[i].op3.u.offset);
			else if (global_code[i].op3.type == QO_TMP)
        		mips_read_tmp(global_code[i].op3.u.temp, "$a1");
			else
				mips_load_immediate("$a1", global_code[i].op3.u.cst);
        	mips_sub("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
        	break;
          
        case Q_MUL:
        	// These two offsets should be deduced/implied in a proper way
        	if (global_code[i].op2.type == QO_ID)
        		mips_read_stack("$a0", global_code[i].op2.u.offset);
			else if (global_code[i].op2.type == QO_TMP)
        		mips_read_tmp(global_code[i].op2.u.temp, "$a0");
			else
				mips_load_immediate("$a0", global_code[i].op2.u.cst);
			
			if (global_code[i].op3.type == QO_ID)
        		mips_read_stack("$a1", global_code[i].op3.u.offset);
			else if (global_code[i].op3.type == QO_TMP)
        		mips_read_tmp(global_code[i].op3.u.temp, "$a1");
			else
				mips_load_immediate("$a1", global_code[i].op3.u.cst);
        	mips_mult("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
        	break;
          
        case Q_DIV:
        	// These two offsets should be deduced/implied in a proper way
        	if (global_code[i].op2.type == QO_ID)
        		mips_read_stack("$a0", global_code[i].op2.u.offset);
			else if (global_code[i].op2.type == QO_TMP)
        		mips_read_tmp(global_code[i].op2.u.temp, "$a0");
			else
				mips_load_immediate("$a0", global_code[i].op2.u.cst);
			
			if (global_code[i].op3.type == QO_ID)
        		mips_read_stack("$a1", global_code[i].op3.u.offset);
			else if (global_code[i].op3.type == QO_TMP)
        		mips_read_tmp(global_code[i].op3.u.temp, "$a1");
			else
				mips_load_immediate("$a1", global_code[i].op3.u.cst);
        	mips_sub("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
        	break;

        case Q_MOD:
        	// These two offsets should be deduced/implied in a proper way
        	if (global_code[i].op2.type == QO_ID)
        		mips_read_stack("$a0", global_code[i].op2.u.offset);
			else if (global_code[i].op2.type == QO_TMP)
        		mips_read_tmp(global_code[i].op2.u.temp, "$a0");
			else
				mips_load_immediate("$a0", global_code[i].op2.u.cst);
			
			if (global_code[i].op3.type == QO_ID)
        		mips_read_stack("$a1", global_code[i].op3.u.offset);
			else if (global_code[i].op3.type == QO_TMP)
        		mips_read_tmp(global_code[i].op3.u.temp, "$a1");
			else
				mips_load_immediate("$a1", global_code[i].op3.u.cst);
        	mips_mod("$v0", "$a0", "$a1");
        	// I push the result in $vo directly, but we might need to store the result to q0's memory
			if(global_code[i].op1.type == QO_ID)
        		mips_write_stack("$v0",global_code[i].op1.u.offset);
			else
				mips_write_tmp(global_code[i].op1.u.temp, "$v0");
        	break;
		case Q_EQ:
			break;
		case Q_NEQ:
			break;
		case Q_LT:
			break;
		case Q_GT:
			break;
		case Q_LEQ:
			break;
		case Q_GEQ:
			break;
        default:
        	break;
        }
    }
}