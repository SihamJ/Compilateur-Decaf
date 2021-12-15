#include "intermediaire.h"

quad global_code[5000];
size_t nextquad; 
size_t tmpCount;
size_t labelCount; 
size_t glob_dec_count;

char* new_label(){
  char* label;
  int a = labelCount;
  int cpt = 1;
  while(a){
    a=a/10;
    cpt++;
  }
  label = malloc(cpt+6);
  sprintf(label, "label%ld",labelCount);
  labelCount++;
  return label;
}

char* next_label_name(){
  char* label;
  int a = labelCount;
  int cpt = 1;
  while(a){
    a=a/10;
    cpt++;
  }
  label = malloc(cpt+6);
  sprintf(label, "label%ld",labelCount);
  return label;
}

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
  if(t != Q_LABEL)
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
  char snum[10];
  char s[10];
  printf("\nCode Intermediaire:\n______________________________________________________________________________________________________\n          type         op1          op2         op3       oper       label        jump   \n______________________________________________________________________________________________________\n\n");
  for (int i=0; i<nextquad; i++){
    printf("%2d",i);
    printf("%12s", get_type_oper(global_code[i].op1.type));
      
    if(global_code[i].op1.type == QO_CST)
      printf("%12d", global_code[i].op1.u.cst);
    else if(global_code[i].op1.type == QO_GLOBAL)
      printf("%12s", global_code[i].op1.u.global.name);
    else {
      sprintf(snum,"(%d)",global_code[i].op1.u.offset);
      printf("%12s",snum);
    }

    if(global_code[i].op2.type == QO_CST)
      printf("%12d", global_code[i].op2.u.cst);
    else if(global_code[i].op2.type == QO_GLOBAL)
      printf("%12s", global_code[i].op2.u.global.name);
    else {
      sprintf(snum,"(%d)",global_code[i].op2.u.offset);
      printf("%12s",snum);
    }

    if(global_code[i].op3.type == QO_CST)
      printf("%12d", global_code[i].op3.u.cst);
    else if(global_code[i].op3.type == QO_GLOBAL)
      printf("%12s", global_code[i].op3.u.global.name);
    else { 
      sprintf(snum,"(%d)",global_code[i].op3.u.offset);
      printf("%12s",snum);
    }
    
    printf("%12s",op_type(global_code[i].type));
    if(global_code[i].label != NULL)
      printf("%12s",global_code[i].label);
    else
      printf("%12s","______");

    printf("%12d\n",global_code[i].jump);
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

    case Q_LABEL:
    return ("LABEL");
    break;

    case Q_SYSCALL:
    return ("SYSCALL");
    break;

    case Q_ENDFUNC:
    return ("END FUNC");
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

void add_labels(){
  char *label;
  for(int i=0; i<nextquad; i++){
    if(global_code[i].jump != -1 && global_code[global_code[i].jump].type != Q_ENDFUNC && global_code[global_code[i].jump].label == NULL){
      label = new_label();
      global_code[global_code[i].jump].label = label;
    }
  }
}