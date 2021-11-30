#ifndef TRANSLATER_H_
#define TRANSLATER_H_

#include "transTool.h"
#include "intermediaire.h"

extern quad global_code[]; // code généré
extern size_t nextquad;
FILE *fout;

void translate();

#endif