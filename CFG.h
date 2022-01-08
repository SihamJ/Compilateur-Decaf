#ifndef __CFG_H__
#define __CFG_H__

#define MAX_CFG 5000
#include <stdio.h>
#include <stdlib.h>
#include "intermediaire.h"
#include "text_formating.h"



typedef struct block{
    int index;
    int start;
    int end;
    struct block* suiv;
    struct block* prec;
} *block;


void new_cfg();

void add_prec(int prec, int index);

void add_suiv(int suiv, int index);

void add_block(block b);

block new_block(int start, int end);

void get_basic_blocks();

void set_basic_blocks();

void print_basic_blocks();

int is_jump(quad_type type);

int is_start(quad_type type);

#endif