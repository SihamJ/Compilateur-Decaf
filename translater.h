
#ifndef __TRANSLATER_H__
#define __TRANSLATER_H__

#include "transTool.h"
#include "intermediaire.h"

extern quad global_code[]; // code généré
extern size_t nextquad;
extern FILE *fout;

void translate();

#endif