#include <transTool.h>
#include <intermediaire.h>

 /* Translater from intermediate code to ASM */

 /**
  * @brief Basically, we translate the intemediate code to mips based on the mechanism of stack machine.
  * https://www.d.umn.edu/~rmaclin/cs5641/Notes/L19_CodeGenerationI.pdf
  * 
  */
extern quad global_code[]; // code généré
extern size_t nextquad;
extern FILE *fout;

 void translate() {
     for (int i=0; i < nextquad; i++) {
         switch (global_code[i].type)
         {
         case Q_ADD:
           // These two offsets should be deduced/implied in a proper way
           int offSet_A = 0;
           int offSet_B = 1;
           mips_read_stack("$a0", offSet_A);
           mips_read_stack("$a1", offSet_B);
           mips_sum("$v0", "$a0", "$a1");
           // I push the result in $vo directly, but we might need to store the result to q0's memory
           mips_push_word("v0");
           break;

          case Q_SUB:
           // These two offsets should be deduced/implied in a proper way
           int offSet_A = 0;
           int offSet_B = 1;
           mips_read_stack("$a0", offSet_A);
           mips_read_stack("$a1", offSet_B);
           mips_sub("$v0", "$a0", "$a1");
           // I push the result in $vo directly, but we might need to store the result to q0's memory
           mips_push_word("v0");
           break;
          
          case Q_MUL:
           // These two offsets should be deduced/implied in a proper way
           int offSet_A = 0;
           int offSet_B = 1;
           mips_read_stack("$a0", offSet_A);
           mips_read_stack("$a1", offSet_B);
           mips_mult("$v0", "$a0", "$a1");
           // I push the result in $vo directly, but we might need to store the result to q0's memory
           mips_push_word("v0");
           break;
          
          case Q_DIV:
           // These two offsets should be deduced/implied in a proper way
           int offSet_A = 0;
           int offSet_B = 1;
           mips_read_stack("$a0", offSet_A);
           mips_read_stack("$a1", offSet_B);
           mips_sub("$v0", "$a0", "$a1");
           // I push the result in $vo directly, but we might need to store the result to q0's memory
           mips_push_word("v0");
           break;

          case Q_MOD:
           // These two offsets should be deduced/implied in a proper way
           int offSet_A = 0;
           int offSet_B = 1;
           mips_read_stack("$a0", offSet_A);
           mips_read_stack("$a1", offSet_B);
           mips_sub("$v0", "$a0", "$a1");
           // I push the result in $vo directly, but we might need to store the result to q0's memory
           mips_push_word("v0");
           break;
         default:
           break;
         }
     }
 }