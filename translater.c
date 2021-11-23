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

 void translate() {
     for (int i=0; i < nextquad; i++) {
         
     }
 }