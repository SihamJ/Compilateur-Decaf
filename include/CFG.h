#ifndef __CFG_H__
#define __CFG_H__

#define MAX_CFG 5000
#include <stdio.h>
#include <stdlib.h>
#include "intermediaire.h"
#include "text_formating.h"

// liste chaînée pour représenter l'index des successeurs/predecesseurs dans le CFG
typedef struct pos{
    int index;          
    struct pos* next;
} *pos;

typedef struct block{
    int index;      // index dans le tableau
    int start;      // index de début du bloc de base dans global_code
    int end;        // index de fin du bloc de base adns global_code
    pos suiv;      // index des successeurs dans le tableau
    pos prec;      // index des predecesseurs dans le tableau
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

int islabel(int index);


#endif