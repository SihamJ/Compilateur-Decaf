#include "optimizer.h"


void useless_gotos(){
    for (int i=0; i<nextquad; i++){
        if(global_code[i].type == Q_GOTO && !islabel(i)){
            if( (global_code[i].jump == i+1) || (global_code[i+1].type == Q_GOTO && global_code[i].jump == global_code[i+1].jump))
                delete_quad(i);
        }
    }
}



int delete_quad(int index){
  
  if(index >= nextquad){
    fprintf(stderr,"\n l'index à supprimer est supérieur à la taille du tableau\n");
    return 0;
  }
  global_code[index].type = Q_DELETED;
  return 1;
}

void delete_quad_list(list n){
  while(n){
    delete_quad(n->addr);
    n=n->suiv;
  }
}

int delete_block(int start, int end){
  for(int i=start; i<=end; i++)
    delete_quad(i);
}

int replace_quad(int index, quadop op1, quadop op2, quadop op3, quad_type t, char *label, int jump, param p){
  if(index >= nextquad){
    fprintf(stderr,"\nIndex de quad à remplacer vide\n");
    return 0;
  }
  global_code[index].type = t;
  global_code[index].op1 = op1;
  global_code[index].op2 = op2;
  global_code[index].op3 = op3;
  global_code[index].jump = jump;
  global_code[index].p = p;

  if(label != NULL ){
    global_code[index].label = malloc(strlen(label)+1);
    strcpy(global_code[index].label, label);
  }
  return 1;
  }

void optimize(){
  useless_gotos();
  new_cfg();
  get_basic_blocks();
  set_basic_blocks();
  print_basic_blocks();
}

/* void insert_quad(int index, quadop op1, quadop op2, quadop op3, quad_type t, char *label, int jump, param p){
  
  int i = nextquad;
  while(i != index){
    global_code[i] = global_code[i-1];
    i--;
  }

  global_code[index].type = t;
  global_code[index].op1 = op1;
  global_code[index].op2 = op2;
  global_code[index].op3 = op3;
  global_code[index].jump = jump;
  global_code[index].p = p;

  if(label != NULL ){
    global_code[index].label = malloc(strlen(label)+1);
    strcpy(global_code[index].label, label);
  }
  nextquad++;
}*/

