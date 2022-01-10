#ifndef __OPTIMIZER_H__
#define __OPTIMIZER_H__

#include <stdio.h>
#include <stdlib.h>
#include "intermediaire.h"
#include "CFG.h"

void useless_gotos();

int islabel(int index);

int delete_quad(int index);

int delete_block(int start, int end);

int replace_quad(int index, quadop op1, quadop op2, quadop op3, quad_type t, char *label, int jump, param p);

void delete_quad_list(list n);

void insert_quad(int index, quadop op1, quadop op2, quadop op3, quad_type t, char *label, int jump, param p);

void optimize(int p);

#endif