#ifndef __TEXT_FORMATING_H__
#define __TEXT_FORMATING_H__
#include <stdio.h>

#define RED "\x1B[31m"
#define GREEN "\e[0;32m"
#define YELLOW "\x1B[33m"
#define NORMAL "\x1B[0m"
#define BLUE "\e[0;34m"
#define PURPLE "\e[0;35m"

void set_color(char *code);

#endif