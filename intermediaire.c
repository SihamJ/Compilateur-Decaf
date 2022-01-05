#include "intermediaire.h"

quad global_code[5000];
size_t nextquad; 
size_t tmpCount;
size_t labelCount; 
size_t glob_dec_count;
size_t str_count;
string_labels str_labels[100];



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

char* new_str(){
  char* label;
  int a = str_count;
  int cpt = 1;
  while(a){
    a=a/10;
    cpt++;
  }
  label = malloc(cpt+6);
  sprintf(label, "str_%ld",str_count);
  str_count++;
  return label;
}

char* new_endfunc_label(char *name){
  char* label = malloc(strlen(name)+6);
  sprintf(label, "fin_%s",name);
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

void gencode(quadop op1, quadop op2, quadop op3, quad_type t, char *label, int jump, param p){
  global_code[nextquad].type = t;
  global_code[nextquad].op1 = op1;
  global_code[nextquad].op2 = op2;
  global_code[nextquad].op3 = op3;
  global_code[nextquad].jump = jump;
  global_code[nextquad].p = p;
  global_code[nextquad].ctx = curr_context;

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
  if(n1 == NULL && n2 == NULL)
    return NULL;
  else if(n1 == NULL)
    return n2;
  else if(n2 == NULL)
    return n1;
  
  list pt = n1;
  while(pt->suiv != NULL)
    pt = pt->suiv;
  pt->suiv = n2;
  return n1;
}

void complete(list n, int addr){
  while(n != NULL){
    if(n->addr < nextquad && global_code[n->addr].jump == -1)

        global_code[n->addr].jump = addr;
    n = n->suiv;
  }
}

void print_globalcode(){

  printf("\n%s%sCode Intermediaire:%s\n\n%sYELLOW = Offset  %s|  %sCYAN = Constant%s\n\n%6stype%12sop1%8sop2%8sop3%8soper%10slabel%4sjump%s\n___________________________________________________________________________________\n\n",CYAN,BOLD,NORMAL,YELLOW,NORMAL,CYAN,NORMAL,"","","","","","","",NORMAL);
  
  for (int i=0; i<nextquad; i++){
    printf("%2d",i);
    printf("%8s", get_type_oper(global_code[i].op1.type));
      
    if(global_code[i].op1.type == QO_CST)
      printf("%s%s%14d%s", CYAN,BOLD, global_code[i].op1.u.cst, NORMAL);
      else if(global_code[i].op1.type == QO_CSTSTR)
      printf("%14s", global_code[i].op1.u.string_literal.label);
    else if(global_code[i].op1.type == QO_GLOBAL)
      printf("%14s", global_code[i].op1.u.global.name);
    else if(global_code[i].op1.type == QO_EMPTY)
      printf("%14s","-");
    else 
      printf("%s%s%14s%s",YELLOW,BOLD,global_code[i].op1.u.name,NORMAL);
    

    if(global_code[i].op2.type == QO_CST)
      printf("%s%s%10d%s", CYAN,BOLD, global_code[i].op2.u.cst, NORMAL);
      else if(global_code[i].op2.type == QO_CSTSTR)
      printf("%10s", global_code[i].op2.u.string_literal.label);
    else if(global_code[i].op2.type == QO_GLOBAL)
      printf("%10s", global_code[i].op2.u.global.name);
    else if(global_code[i].op2.type == QO_EMPTY)
      printf("%10s","-");
    else 
      printf("%s%s%10s%s",YELLOW,BOLD,global_code[i].op2.u.name,NORMAL);
    

    if(global_code[i].op3.type == QO_CST)
      printf("%s%s%10d%s", CYAN,BOLD, global_code[i].op3.u.cst, NORMAL);
    else if(global_code[i].op3.type == QO_CSTSTR)
      printf("%10s", global_code[i].op3.u.string_literal.label);
    else if(global_code[i].op3.type == QO_GLOBAL)
      printf("%10s", global_code[i].op3.u.global.name);
    else if(global_code[i].op3.type == QO_EMPTY)
      printf("%10s","-");
    else 
      printf("%s%s%10s%s",YELLOW,BOLD,global_code[i].op3.u.name,NORMAL);
    
    
    printf("%14s",op_type(global_code[i].type));
    
    if(global_code[i].label != NULL)
      printf("%14s",global_code[i].label);
    else
      printf("%14s","-");
    
    if(global_code[i].jump!=-1)
      printf("%8d\n",global_code[i].jump);
    else
      printf("%8s\n","-");
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

    case Q_SEQ:
    return ("SEQ");
    break;

    case Q_SNE:
    return ("SNE");
    break;

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

    case Q_FUNC:
    return ("START FUNC");
    break;

    case Q_SYSCALL:
    return ("SYSCALL");
    break;

    case Q_ENDFUNC:
    return ("END FUNC");
    break;

    case Q_METHODCALL:
    return ("METHOD CALL");
    break;

    case Q_ACCESTAB:
    return ("ACCES TAB");
    break;

    case Q_RETURN:
    return ("RETURN");
    break;

    case Q_POP:
    return ("POP STACK");
    break;

    case Q_PUSH:
    return ("PUSH CTX");
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
    else if (type == QO_CSTSTR)
    return "STRING";
  else if(type == QO_GOTO)
    return "GOTO";
  else
    return "NONE";

}

void add_labels(){
  char *label;
  for(int i=0; i<nextquad; i++){
    if(global_code[i].jump != -1 && global_code[global_code[i].jump].label == NULL){
      label = new_label();
      global_code[global_code[i].jump].label = label;
    }
  }
}


int delete_quad(int index){
  
  if(index >= nextquad){
    fprintf(stderr,"\n l'index à supprimer est supérieur à la taille du tableau\n");
    return 0;
  }
  while(index < nextquad){
    global_code[index] = global_code[index+1];
    index++;
  }
  return 1;
}

void delete_quad_list(list n){
  while(n){
    delete_quad(n->addr);
    n=n->suiv;
  }
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

// sometimes I'm good at making jokes, here is an example (or the one above):
void insert_quad(int index, quadop op1, quadop op2, quadop op3, quad_type t, char *label, int jump, param p){
  
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
}

