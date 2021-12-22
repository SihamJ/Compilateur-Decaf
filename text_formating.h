#ifndef __TEXT_FORMATING_H__
#define __TEXT_FORMATING_H__
#include <stdio.h>

#define RED "\x1B[31m"
#define YELLOW "\x1B[33m"
#define NORMAL "\x1B[0m"

void set_color(char *code);

#endif