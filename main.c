#include <stdio.h>
#include "intermediaire.h"

extern int yyparse();
extern int yylex();
extern int yydebug;

int main(){
// yydebug=1;
int t = yyparse();
if (t==0)
print_globalcode();
return 0;
}
