#include <stdio.h>
#include "intermediaire.h"

extern int yyparse();
extern int yylex();
extern int yydebug;

int main(){
yydebug=1;
int t = yyparse();
printf("size: %ld, nb_var: %ld\n",nextquad,tmpCount);
return 0;
}
