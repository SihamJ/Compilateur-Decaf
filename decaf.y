%{
#include <stdio.h>
#include "intermediaire.h"
extern int yylex();
void yyerror(const char * msg);
%}


%union {
  char* strval;
  int intval;
  typedef struct expr{
    quadop result;
    //quad true, false; // pour plus tard, structures de contrôles
  }expr;
}


%%


%%

void yyerror(const char * msg){
  fprintf(stderr, "%s\n", msg);
}
