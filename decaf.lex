%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>
	#include "intermediaire.h"
	#include "decaf.tab.h"
%}

%option noyywrap

alpha			[a-zA-Z_]
char			[^'"]
digit 			[0-9]
hex_digit 		({digit}|[a-fA-F])
alpha_num 		({alpha}|{digit})
bool_literal 	"true"|"false"
char_literal 	\'{char}\'
string_literal 	\"{char}*\"
hex_literal		"0x"{hex_digit}{hex_digit}*
decimal_literal	{digit}{digit}*
id 				{alpha}{alpha_num}*
newline			\\n
%%

"=" return aff;
"+=" return aff_add;
"-=" return aff_sub;
"+" return add;
"-" return sub;
"/" return divide;
"*" return mul;
"%" return mod;
"<" return lt;
">" return gt;
">=" geq;
"<=" leq;
"==" return eq;
"!=" return neq;
"&&" return and;
"||" return or;
"(" return opar;
")" return cpar;
";" return instr;
"{" return oac;
"}" return cac;
"!" return not;
"int" return integer;
"bool" return boolean;
"," 	return sep;

{bool_literal} 		{
						yylval.boolval = (strcmp(yytext, "true") == 0);
						return bool_literal;
					}

{char_literal} 		{
						yylval.boolval = (int) yytext[1];
						return char_literal;
					}

{hex_literal} 		{
						yylval.intval = strtol(yytext+2, NULL, 16);
						return hex_literal;
					}

{decimal_literal} 	{
						yylval.intval = strtol(yytext, NULL, 10);
						return decimal_literal;
					}

{id}				{
						yylval.stringval = malloc(strlen(yytext));
						strcpy(yylval.stringval, yytext);
						return id;
					}

{newline}			{
						return newline;
					}

[[:space:]] ;

.					;

%%
