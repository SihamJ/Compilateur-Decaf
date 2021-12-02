#include <stdio.h>
#include "intermediaire.h"
#include "hashtable.h"
#include "translater.h"

extern int yyparse();
extern int yylex();
extern int yydebug;
extern FILE* fout;
//extern int sommet;
extern size_t tmpCount, nextquad;

int main(){
//yydebug=1;
curr_context = NULL;
int t = yyparse();

if (t==0)
    print_globalcode();
print_ctx();
fout = stdout;
translate();
return 0;
}
