#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "intermediaire.h"
#include "hashtable.h"
#include "translater.h"
#include "text_formating.h"
#include "optimizer.h"
#include "CFG.h"

extern int yyparse();
extern int yylex();
extern int yydebug;
extern int warning;
extern int tos;
extern int count;
extern block cfg[MAX_CFG];
extern int tab[5000];
extern int nb_block;
extern FILE* fout;
extern FILE* src;
void usage(char *name);
void version(char *m1, char* f1, char* m2, char *f2, char* m3, char *f3, char *m4, char*f4, char* v);

int main(int argc, char* argv[]){

    warning = 0; count = 0; tos = 0;
    if(argc == 1){
        usage(argv[0]);
    }
    else if(argc < 4 && strcmp(argv[1],"-version")){
        usage(argv[0]);
    }

    else if(!strcmp(argv[1],"-version")){
        version("XU Chenglin","SIL","JANATI Siham","SDSC","HAMMER Tom","SIL","WEREY Laurent","SIL",NULL);
    }

    else {
    
        if((stdin=freopen(argv[1], "r", stdin))==NULL){
            fprintf(stderr,"%s\tCan't open source file\n%s",YELLOW,NORMAL);
            usage(argv[0]);
        }

        if(strcmp(argv[2],"-o")){
            usage(argv[0]);
        }

        if(!strcmp(argv[3],"stdout"))
            fout = stdout;
        else 
            fout = fopen(argv[3],"w");
        
        if(fout == NULL){
             fprintf(stderr,"%s\tCan't create destination file\n%s",YELLOW,NORMAL);
            usage(argv[0]);
        }

        int i = 4; int opti = 0;
        bool inter = false;

        while(i < argc){
            if(!strcmp(argv[i],"-tos"))
                tos = true;

            else if(!strcmp(argv[i],"-i"))
                inter = true;
           
            else if(!strcmp(argv[i],"-W"))
                warning = 1;
            else if(!strcmp(argv[i],"-Cafeine"))
                opti = 1;
            else
                usage(argv[0]);
            i++;
        }
        
        curr_context = NULL;
        str_count = 0;
        
        // yydebug=1;
        int t = yyparse();

        if(t!=0)
            exit(EXIT_FAILURE);

        add_labels();    
        
        if(opti){
            optimize();
        }

        if(inter)
            print_globalcode();
        

        translate();

        if(fout != stdout)
            fclose(fout);

        
        
        free_tables();
        free_global_code();

        exit(EXIT_SUCCESS);
    }
}


void usage(char *name){
    fprintf(stderr, "\n%s%8sUsage: %s <source> -o <dest> [-tos] [-i] [-W] [-Cafeine]\n",YELLOW,"",name);
    fprintf(stderr, "%8sUsage: %s -version \n\n%s","",name,NORMAL);
    fprintf(stderr, "\n%8s<source>\tFichier source à compiler\n%8s-o <dest>\tSpécifie le nom du fichier produit en assembleur MIPS (stdout = sortie standard)\n%8s-tos\t\tAffiche la table des symboles\n%8s-i\t\tAffiche le code intermédiaire\n%8s-version\tAffiche les membres du groupe\n%8s-W\t\tGénère les messages de Warning\n%8s-Cafeine\tOptimisation du code intermédiaire\n\n","","","","","","","");
    exit(0);
}

void version(char *m1, char* f1, char* m2, char *f2, char* m3, char *f3, char *m4, char*f4, char* v){
    printf("\n%8sMembres:\n","");
    printf("\n%8s\033[1m%s  -  %s  -  %s  -  %s\033[0m \n","",m1,m2,m3,m4);
    printf("%8s%s\033[1m%s\033[0m          .  %s\033[1m%s\033[0m          .  %s\033[1m%s\033[0m         .  %s\033[1m%s\033[0m%s\n\n","",CYAN,f1,YELLOW,f2,CYAN,f3, CYAN,f4, NORMAL);
    if(v!=NULL){
        printf("\n%8s%s version: \033[1m%s\033[0m\n","", BLUE, v);
    }
    exit(0);

}