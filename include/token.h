#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#ifndef __TOKEN_H__
#define __TOKEN_H__



#define BOOL 0
#define INT 1
#define VOIDTYPE 2
#define STRING 3        // used for WriteString function
#define BOOLGOTO 4
#define BOOLVAL 5
#define CAPACITY 8192 // Size of the Hash Table
#define LEXERROR -99;
#endif