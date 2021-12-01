%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>
	#include "intermediaire.h"
	#include "hashtable.h"
	#include "decaf.tab.h"
	#include "token.h"
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

"=" return '=';
"+=" return aff_add;
"-=" return aff_sub;
"+" return '+';
"-" return '-';
"/" return '/';
"*" return '*';
"%" return '%';
"<" return '<';
">=" geq;
"<=" leq;
">" return '>';
"==" return eq;
"!=" return neq;
"&&" return and;
"||" return or;
"(" return '(';
")" return ')';
";" return ';';
"{" return '{';
"}" return '}';
"!" return '!';
"int" {yylval.intval = INT;
		return integer;}
"bool" {yylval.intval = BOOL;
		return boolean;}
"," 	return ',';

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
						yylval.stringval = malloc(strlen(yytext)+1);
						strcpy(yylval.stringval, yytext);
						return id;
					}

[[:space:]] 		;

.					;

%%
