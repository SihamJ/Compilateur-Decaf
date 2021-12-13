#include "intermediaire.h"

quad global_code[5000];
size_t nextquad; 
size_t tmpCount; 



void gencode(quadop op1, quadop op2, quadop op3, quad_type t, char *label, int jump){
  global_code[nextquad].type = t;
  global_code[nextquad].op1 = op1;
  global_code[nextquad].op2 = op2;
  global_code[nextquad].op3 = op3;
  global_code[nextquad].jump = jump;

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

void complete(list n, int addr){
  list pt = n;
  while(pt){
    global_code[pt->addr].jump = addr;
    pt = pt->suiv;
  }
}

void print_globalcode(){
  printf("\nCode Intermediaire:\n______________________________________________\n     type      op1           op2           op3          oper         jump   \n_______________________________________________\n\n");
  for (int i=0; i<nextquad; i++){
    printf("%d:   ",i);
      printf("  %s        ", get_type_oper(global_code[i].op1.type));
      
    if(global_code[i].op1.type == QO_CST)
      printf("  %d        ", global_code[i].op1.u.cst);
    else if(global_code[i].op1.type == QO_GLOBAL)
      printf("  %s        ", global_code[i].op1.u.global.name);
    else 
      printf(" (%d)       ",global_code[i].op1.u.offset);

    if(global_code[i].op2.type == QO_CST)
      printf("  %d        ", global_code[i].op2.u.cst);
    else if(global_code[i].op2.type == QO_GLOBAL)
      printf("  %s        ", global_code[i].op2.u.global.name);
    else 
      printf(" (%d)       ",global_code[i].op2.u.offset);

    if(global_code[i].op3.type == QO_CST)
      printf("  %d        ", global_code[i].op3.u.cst);
    else if(global_code[i].op3.type == QO_GLOBAL)
      printf("  %s        ", global_code[i].op3.u.global.name);
    else 
      printf(" (%d)       ",global_code[i].op3.u.offset);

    printf("  %s        ",op_type(global_code[i].type));
    printf("  %d \n",global_code[i].jump);
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

    case Q_GOTO:
    return ("GOTO");
			break;
        default:
        	break;
        }
}

char *get_type_oper(int type){
  if (type == QO_CST)
    return "CST";
  else if (type == QO_ID)
    return "ID";
  else if (type == QO_TMP)
    return "TMP";
    else if (type == QO_GLOBAL)
    return "GLOB";
  else if (type == QO_EMPTY)
    return "NONE";
}