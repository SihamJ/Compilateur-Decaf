%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <stdbool.h>
	#include <string.h>
	#include "intermediaire.h"
	#include "token.h"
	#include "compiler.tab.h"
	#include "text_formating.h"
	void lexerror(const char *msg);
%}

%option noyywrap
%option yylineno

alpha			[a-zA-Z_]
char			[!#-&(-\[\]-_a-~]|[[:space:]]|{newline}|\\t|\\\'|\\\"|\\\\
digit 			[0-9]
hex_digit 		({digit}|[a-fA-F])
alpha_num 		({alpha}|{digit})
bool_literal 	"true"|"false"
char_literal 	\'{char}\'
string_literal 	\"{char}*\"
hex_literal		"0x"{hex_digit}{hex_digit}*
decimal_literal	{digit}{digit}*
id 				{alpha}{alpha_num}*
address			&{id}
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
						yylval.intval = strtoll(yytext+2, NULL, 16);
						if(yylval.intval < -2147483648 || yylval.intval > 2147483647 ){
							lexerror("Dépassement Héxadécimal");
							return 1;
						}
						return hex_literal;
					}

{char_literal} 		{
						yylval.intval = yytext[1];
						return char_literal;
					}

{decimal_literal} 	{
						yylval.intval = strtoll(yytext, NULL, 10);

						if(yylval.intval < -2147483648 || yylval.intval > 2147483647){
							lexerror("Dépassement Décimal");
							return 1;
						}
						
						return decimal_literal;
					}

{id}				{
						yylval.stringval = malloc(strlen(yytext)+1);
						strcpy(yylval.stringval, yytext);
						return id;
					}

{address}			{
						yylval.stringval = malloc(strlen(yytext)+1);
						strcpy(yylval.stringval, yytext);
						return address;
					}

{string_literal}	{
						yylval.stringval = malloc(strlen(yytext)+1);
						strcpy(yylval.stringval, yytext);
						return string_literal;					
					}

[[:space:]]				;


.					{	lexerror("Erreur lexicale: caractère non reconnu"); return 1;	}

%%

void lexerror(const char *msg){
	fprintf(stderr,"\n%s %s, at line %d%s\n\n", RED, msg, yylineno, NORMAL);
}