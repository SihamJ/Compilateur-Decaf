#include <stdio.h>
#include "intermediaire.h"
#include "hashtable.h"
#include "translater.h"

extern int yyparse();
extern int yylex();
extern int yydebug;
extern FILE* fout;

int main(int argc, char* argv[]){
    
    //yydebug=1;
    curr_context = NULL;
    int t = yyparse();
    if (t==0)
        add_labels();    
    
    int i = 1;

    while(i<argc){

        if(!strcmp(argv[i],"-version")){
            printf("\nXU Chenglin  -  JANATI Siham  -  HAMMER Tom  -  WEREY Laurent\n");
            printf("SIL          .  SDSC          .  SIL         .  SIL\n\n");
        }
        else if(!strcmp(argv[i],"-tos")){
            print_stack();
        }
        else if(!strcmp(argv[i],"-o")){
            if(i+1>=argc){
                fout = fopen("out.asm","w");
            }
            else if(!strcmp(argv[i+1],"stdout")){
                fout = stdout;
                i++;
            }
            else if((fout = fopen(argv[i+1],"w"))==NULL){
                fout = fopen("out.asm","w");
                i++;
            }
            translate();
            if(fout != stdout)
                fclose(fout);
            
        }
        else{
            fprintf(stderr, "\nUsage: %s [-version] [-tos] [-i] [-o [<name>]]  <  source \n\n",argv[0]);
            exit(0);
        }
        i++;
    }

    free_stack();
    return 0;
}
