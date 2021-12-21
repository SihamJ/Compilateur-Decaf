#include <stdio.h>
#include "intermediaire.h"
#include "hashtable.h"
#include "translater.h"

extern int yyparse();
extern int yylex();
extern int yydebug;
extern FILE* fout;
extern size_t tmpCount, nextquad;

int main(int argc, char* argv[]){
    

    //yydebug=1;
    curr_context = NULL;
    int t = yyparse();
    //fclose(source);
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
        else if(!strcmp(argv[i],"-i")){
            print_globalcode();   
        }
        else if(!strcmp(argv[i],"-o")){
            if(i+1>=argc){
                fprintf(stderr, "\nUsage: %s source [-version] [-tos] [-i] [-o <name>]\n\n",argv[0]);
                exit(0);
            }
            fout = fopen(argv[i+1],"w");
            if(fout == NULL){
                printf("\nImpossible de créer le fichier %s\n",argv[i+1]);
                fout = stdout;
            }
            translate();
            fclose(fout);
        }
        i++;
    }

    free_stack();
    return 0;
}
