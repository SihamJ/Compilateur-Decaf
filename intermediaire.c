#include "intermediaire.h"

quadop new_temp(){
  quadop op;
  int a = tmpCount;
  int cpt = 1;
  while(a){
    a=a/10;
    cpt++;
  }
  op.u.temp = malloc(cpt+2);
  op.type = QO_TMP;
  sprintf(op.u.temp, "$t%ld",tmpCount);
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

list concat(list n1, list n2){
  list pt = n1;
  while(pt->suiv != NULL)
    pt = pt->suiv;
  pt->suiv = n2;
  return n1;
}


void print_globalcode(){
  printf("\nCode Intermediaire:\n__________________________\n    op1   op2   op3   oper\n__________________________\n");
  for (int i=0; i<nextquad; i++){
    printf("%d: ",i);
    if(global_code[i].op1.type == QO_ID )
      printf(" %d   ", global_code[i].op1.u.offset);
    else if(global_code[i].op1.type == QO_CST)
      printf(" %d   ",global_code[i].op1.u.cst);
	else
	  printf(" %s   ",global_code[i].op1.u.temp);
    if(global_code[i].op2.type == QO_ID)
      printf(" %d   ", global_code[i].op2.u.offset);
    else if(global_code[i].op2.type == QO_CST)
      printf(" %d   ",global_code[i].op2.u.cst);
	else
	  printf(" %s   ",global_code[i].op2.u.temp);
    if(global_code[i].op3.type == QO_ID)
      printf(" %d   ", global_code[i].op3.u.offset);
    else if(global_code[i].op3.type == QO_CST)
      printf(" %d   ",global_code[i].op3.u.cst);
	else
	  printf(" %s   ",global_code[i].op3.u.temp);

    printf("op[%d] \n",global_code[i].type);
  }
  printf("\n");
}
