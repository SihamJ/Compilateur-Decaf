%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>
	int yylex();
	void yyerror(char*);
%}


%token <intval> decimal_literal hex_literal char_literal
%token <boolval> bool_literal
%token <stringval> string_literal id arith_op rel_op eq_op cond_op
%token newline
%left arith_op rel_op eq_op cond_op
%left NEG

%union {
	int intval;
	bool boolval;
	char *stringval;
}

%%

S 			:	expr
			|	expr newline S
			;
expr		:	operand
			|	expr bin_op operand
			;
operand		:	literal
			|	'-' expr %prec NEG
			|	'!' expr %prec NEG
			|	'(' expr ')'
			;
bin_op 		: 	arith_op
			|	rel_op
			|	eq_op
			|	cond_op
			;
literal		:	int_literal
			|	char_literal
			|	bool_literal
			;
int_literal	: 	decimal_literal
			| 	hex_literal
			;
			
%%

void yyerror(char *s) {
	fprintf(stderr, "%s\n", s);
}

int main() {
	printf("Entrez une chaine :\n");
	yyparse();
	printf("Au revoir\n");
	return 0;
}