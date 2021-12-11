#include "intermediaire.h"

quad global_code[5000];
size_t nextquad; 
size_t tmpCount; 

Ht_item* new_temp(int type){

  int a = tmpCount;
  int cpt = 1;

  while(a){
    a=a/10;
    cpt++;
  }
  
  char *name = malloc(cpt+3);
  sprintf(name, "t%ld",tmpCount);

  Ht_item *item = create_item(name, ID_TMP, type);
	newname(item);

  tmpCount++;
  return item;
}

void gencode(quadop op1, quadop op2, quadop op3, quad_type t, char *label, int jump, HashTable* context){
  global_code[nextquad].type = t;
  global_code[nextquad].op1 = op1;
  global_code[nextquad].op2 = op2;
  global_code[nextquad].op3 = op3;
  global_code[nextquad].jump = jump;
  global_code[nextquad].context = context;

  if(label != NULL ){
    global_code[nextquad].label = malloc(strlen(label)+1);
    strcpy(global_code[nextquad].label, label);
  }
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
  printf("\nCode Intermediaire:\n______________________________________________\n    op1           op2           op3          oper   \n_______________________________________________\n\n");
  for (int i=0; i<nextquad; i++){
    printf("%d:   ",i);

    if(global_code[i].op1.type == QO_CST)
      printf("  %d        ", global_code[i].op1.u.cst);
    else 
      printf(" (%d)       ",global_code[i].op1.u.offset);

    if(global_code[i].op2.type == QO_CST)
      printf("  %d        ", global_code[i].op2.u.cst);
    else 
      printf(" (%d)       ",global_code[i].op2.u.offset);

    if(global_code[i].op3.type == QO_CST)
      printf("  %d        ", global_code[i].op3.u.cst);
    else 
      printf(" (%d)       ",global_code[i].op3.u.offset);

    printf("  %s \n",op_type(global_code[i].type));
  }
  printf("\n");
}

char *op_type(int type){
  switch (type)
        {
		case Q_DECL:
    return("DECL");
			break;
		case Q_AFF:
    return("AFF");

			break;
		case Q_AFFADD:
    return("+AFF");

			break;
		case Q_AFFSUB:
    return("-AFF");

			break;
		case Q_NOT:
    return("NOT");

			break;
        case Q_ADD:
    return("ADD");

        	break;
        case Q_SUB:
    return("SUB");

        	break;
        case Q_MUL:
    return("MUL");

        	break;
        case Q_DIV:
    return("DIV");

        	break;
        case Q_MOD:
    return("MOD");

        	break;
		case Q_EQ:
    return("EQ");

			break;
		case Q_NEQ:
    return("NEQ");

			break;
		case Q_LT:
    return("LT");

			break;
		case Q_GT:
    return("GT");

			break;
		case Q_LEQ:
    return("LEQ");

			break;
		case Q_GEQ:
    return("GEQ");

			break;
        default:
        	break;
        }
}