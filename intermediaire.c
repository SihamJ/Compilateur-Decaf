#ifndef __INTERMEDIAIRE_H__
#define __INTERMEDIAIRE_H__
#include "intermediaire.h"

quadop new_temp(){
  quadop op;
  op.u.name = malloc(10);
  snprintf(op.u.name,9,"t%ld",tmpCount);
  // TO DO: mettre op dans la table des symboles
  tmpCount++;
  return op;
}

void gencode(quadop op1, quadop op2, quadop op3, quad_type t, int label){
  global_code[nextquad].type = t;
  global_code[nextquad].op1 = op1;
  global_code[nextquad].op2 = op2;
  global_code[nextquad].op3 = op3;
  global_code[nextquad].label = label;
  nextquad++;
}

list crelist(int addr){
  list n = (list) malloc(sizeof(struct list));
  n->addr = addr;
  n->suiv = NULL;
  return n;
}

void complete(list n, int addr){
  list pt = n;
  while(pt->suiv != NULL){
    global_code[pt->addr].label = addr;
    pt = pt->suiv;
  }
}

list concat(list n1, list n2){
  list pt = n1;
  while(pt->suiv != NULL)
    pt = pt->suiv;
  pt->suiv = n2;
  return n1;
}

quadop quadop_name(char *name){
  quadop q;
  q.u.name = malloc(strlen(name));
  strcpy(q.u.name, name);
  return q;
}


#endif
