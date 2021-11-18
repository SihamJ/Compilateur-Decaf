#IFNDEF __QUADOP_H__
#DEFINE __QUADOP_H__
#include "quadop.h"
#ENDIF

quadop* new_temp(){
  quadop* op;
  op.u.name = malloc(10);
  snprintf(op.u.name,9,"t%d",tmpCount);
  // TO DO: mettre op dans la table des symboles
  tmpCount++;
  return op;
}

void gencode(type t, quadop op1, quadop op2, quadop op3, int label){
  global_code[nextquad].type = t;
  global_code[nextquad].op1 = op1;
  global_code[nextquad].op2 = op2;
  global_code[nextquad].op3 = op3;
  global_code[nextquad].label = label;
  nextquad++;
}

next crelist(int addr){
  next n = malloc(sizeof(struct next));
  n->addr = addr;
  n->suiv = NULL;
  return n;
}

void complete(next n, int addr){
  next pt = n;
  while(pt->suiv != NULL){
    global_code[pt->addr].label = addr;
    pt = pt->suiv;
  }
}

next concat(next n1, next n2){
  next pt = n1;
  while(pt->suiv != NULL)
    pt = pt->suiv;
  pt->suiv = n2;
  return n1;
}
