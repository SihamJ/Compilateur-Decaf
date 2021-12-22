%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>
	#include "intermediaire.h"
	#include "token.h"
	#include "decaf_plus.tab.h"

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
comment 		\/\/.*
%%

{comment}		;

"=" return '=';
"+=" return aff_add;
"-=" return aff_sub;
"+" return '+';
"-" return '-';
"/" return '/';
"*" return '*';
"%" return '%';
"<" return '<';
">=" return geq;
"<=" return leq;
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
"[" return '[';
"]" return ']';
"int" {yylval.intval = INT;
		return integer;}
"boolean" {yylval.intval = BOOL;
		return boolean;}
"void" {yylval.intval = VOIDTYPE;
		return voidtype;}
"," 	return ',';
"class" return class;


"Program" 			{
						yylval.stringval = malloc(strlen(yytext)+1);
						strcpy(yylval.stringval, yytext);
						return Program;
					}

"if" return If;
"else" return Else;
"for" return For;
"return" return Return;
"continue" return Continue;
"break" return Break;



{bool_literal} 		{
						yylval.intval = (strcmp(yytext, "true") == 0);
						return bool_literal;
					}
					
{hex_literal} 		{
						yylval.intval = strtol(yytext+2, NULL, 16);
						return hex_literal;
					}

{char_literal} 		{
						yylval.intval = yytext[1];
						return char_literal;
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

{string_literal}	{
						yylval.stringval = malloc(strlen(yytext)+1);
						strcpy(yylval.stringval, yytext);
						return string_literal;
					}

[[:space:]]			;


.					{fprintf(stderr,"erreur lexical\n");exit(0);}

%%
