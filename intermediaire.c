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

quad get_quad(type t, quadop op1, quadop op2, quadop op3, int label){
  quad q;
  q.type = t;
  q.op1 = op1;
  q.op2 = op2;
  q.op3 = op3;
  q.label = label;
  return q;
}

void gencode(quad q){
  global_code[nextquad] = q;
  nextquad++;
}
