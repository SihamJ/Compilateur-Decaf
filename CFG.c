#include "CFG.h"


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

    int i = 0;
    int j = 0;

    while(j < nb_block && cfg[j]->index != index )
        j++;
    
    if (j < nb_block){
       
        block b = cfg[j];

        while(i < nb_block){
            if(cfg[i]->index == prec){
                block pt = b->prec;
                while(pt->suiv != NULL){
                    pt = pt->suiv;
                }
                pt->suiv = cfg[i];
                pt->suiv->suiv = NULL;
            }
            i++;
        }
    }
}

void add_suiv(int suiv, int index){

    int i = 0;
    int j = 0;

    while(j < nb_block && cfg[j]->index != index )
        j++;
    
    if (j < nb_block){
       
        block b = cfg[j];

        while(i < nb_block){
            if(cfg[i]->index == suiv){
                block pt = b->suiv;
                while(pt->suiv != NULL){
                    pt = pt->suiv;
                }
                pt->suiv = cfg[i];
                pt->suiv->suiv = NULL;
            }
            i++;
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
    return b;
}

void get_base_blocks(){
    int counter = 0;
    for(int i=0; i<nextquad; i++){
        if(is_jump(global_code[i].type)){
            tab[i] = 2;
            tab[i+1] = 1; 
            tab[global_code[i].jump] = 1;
        }
        else if(is_start(global_code[i].type))
            tab[i] = 1;
        else 
            tab[i] = 0; 
    }
}

int is_jump(quad_type type){
  if(type == Q_EQ || type == Q_NEQ || type == Q_LT || type == Q_GT || type == Q_LEQ || type == Q_GEQ || type == Q_METHODCALL || type == Q_GOTO || type == Q_ENDFUNC || type == Q_SYSCALL )
    return 1;
  return 0;
}

int is_start(quad_type type){
  if(type == Q_FUNC || type == Q_PUSH)
    return 1;
  return 0;
}

void set_base_blocks(){
    int start = -1;
    int end = -1;
    for(int i=0; i<nextquad; i++){
        if(tab[i]==1)
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

void print_base_blocks(){
    for(int i = 2; i < nb_block; i++){
        printf("\nB%d:\n",i);
        printf("start: %d   -  End: %d \n",cfg[i]->start, cfg[i]->end);
    }
}