%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>
	#include "y.tab.h"
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
newline			[\n]+
arith_op		[+-/*%]
rel_op			[<>]|"<="|">="
eq_op			"=="|"!="
cond_op			"&&"|"||"

%%

{bool_literal} 		{yylval.boolval = (strcmp(yytext, "true") == 0); return bool_literal;}
{char_literal} 		{yylval.intval = (int) yytext[1]; return char_literal;}
{string_literal} 	{yylval.stringval = strcpy(yylval.stringval, yytext);return string_literal;}
{hex_literal} 		{yylval.intval = strtol(yytext+2, NULL, 16); return hex_literal;}
{decimal_literal} 	{yylval.intval = strtol(yytext, NULL, 10); return decimal_literal;}
{id}				{yylval.stringval = strcpy(yylval.stringval, yytext); return id;}
{arith_op}			{yylval.stringval = strcpy(yylval.stringval, yytext;return arith_op;}
{rel_op}			{yylval.stringval = strcpy(yylval.stringval, yytext;return rel_op;}
{eq_op}				{yylval.stringval = strcpy(yylval.stringval, yytext;return eq_op;}
{cond_op}			{yylval.stringval = strcpy(yylval.stringval, yytext;return cond_op;}
{newline}			{return newline;}

%%

int main(){
    yylex();
    return 0;
}