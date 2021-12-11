#include <stdio.h>
#include "intermediaire.h"
#include "hashtable.h"
#include "translater.h"

extern int yyparse();
extern int yylex();
extern int yydebug;
extern FILE* fout;
extern size_t tmpCount, nextquad;

int main(){
//yydebug=1;
curr_context = NULL;
int t = yyparse();

if (t==0)
    print_globalcode();
printf("%ld\n",tmpCount);
//print_ctx();
fout = stdout;
//translate();
/*
int t=yylex();
while(t)
t=yylex();*/
return 0;
}
