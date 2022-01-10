#include "../include/CFG.h"


block cfg[MAX_CFG];
int tab[5000];
int nb_block;

void new_cfg(){

    block entry, exit;
    entry = (block) malloc(sizeof(struct block));
    exit = (block) malloc(sizeof(struct block));

    entry->index = 0;
    exit->index = 1;

    entry->start = -1;
    entry->end = -1;
    entry->prec = NULL;
    entry->suiv = NULL;

    exit->start = -1;
    exit->end = -1;
    exit->prec = NULL;
    exit->suiv = NULL;

    cfg[0] = entry;
    cfg[1] = exit;
    nb_block = 2;
}

void add_prec(int prec, int index){

    if(index < nb_block){
        pos p = (pos) malloc(sizeof(struct pos));
        p->index = prec;
        p->next = NULL;

        if(cfg[index]->prec == NULL){
            cfg[index]->prec = p;
        }
        else {
            p->next = cfg[index]->prec;
            cfg[index]->prec = p;
        }
    }
}

void add_suiv(int suiv, int index){

    if(index < nb_block){
        pos p = (pos) malloc(sizeof(struct pos));
        p->index = suiv;
        p->next = NULL;

        if(cfg[index]->suiv == NULL){
            cfg[index]->suiv = p;
        }
        else {
            p->next = cfg[index]->prec;
            cfg[index]->suiv = p;
        }
    }
}

void add_block(block b){
    cfg[nb_block] = b;
    nb_block++;
}

block new_block(int start, int end){
    block b = (block) malloc(sizeof(struct block));
    b->start = start;
    b->end = end;
    b->prec = NULL;
    b->suiv = NULL;
    b->index = nb_block;
    return b;
}

void init_tab(){

    for(int i = 0; i < nextquad; i++)
        tab[i] = 0;
}

void get_basic_blocks(){
    int counter = 0;
    for(int i=0; i<nextquad; i++){
        if(is_jump(global_code[i].type)){
            tab[i] = 2;
            if(i < nextquad)
                tab[i+1] = 1; 
        }
        else if(is_start(global_code[i].type)){
            tab[i] = 1;
            if(i>0)
                tab[i-1] = 2;
        }
        if(islabel(i)){
            if(tab[i] == 0)
                tab[i] = 1;
            if(i > 0)
                tab[i-1] = 2;
        }
    }
}

void set_basic_blocks(){
    int start = -1;
    int end = -1;
    for(int i = 0; i < nextquad; i++){
        if(tab[i] == 1)
            start = i;
        else if(tab[i] == 2){
            if(start == -1)
                start = i;
            end = i;
            }
        
        if(end == -1) continue;
        block b = new_block(start, end);
        cfg[nb_block] = b;
        nb_block++;
        start = -1;
        end = -1;
    }
}


int is_jump(quad_type type){
  if(type == Q_EQ || type == Q_NEQ || type == Q_LT || type == Q_GT || type == Q_LEQ || type == Q_GEQ || type == Q_GOTO || type == Q_SYSCALL || type == Q_ENDFUNC || type == Q_RETURN || type == Q_METHODCALL)
    return 1;
  return 0;
}

int is_start(quad_type type){
  if(type == Q_FUNC || type == Q_PUSH)
    return 1;
  return 0;
}

void print_basic_blocks(){
    printf("\n%s%sBlocs de Base:%s\n",CYAN,BOLD,NORMAL);
    for(int i = 2; i < nb_block; i++){
        printf("\n\t%sB%d%s \t%d  ->  %d\n",YELLOW, i-1, NORMAL, cfg[i]->start, cfg[i]->end);
    }
}

int islabel(int index){
  if(global_code[index].label != NULL)
    return 1;
  return 0;
}