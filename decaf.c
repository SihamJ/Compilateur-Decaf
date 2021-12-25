#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "intermediaire.h"
#include "hashtable.h"
#include "translater.h"
#include "text_formating.h"

extern int yyparse();
extern int yylex();
extern int yydebug;
extern FILE* fout;
extern FILE* src;
void usage(char *name);


int main(int argc, char* argv[]){

  //  if(argc!=4)
    //    usage(argv[0]);
    /* Options d'exécution */
   /* int output=0;
    int output_file=0;
    int source=0;
    int source_file=0;
    int version=0;
    int tos=0;
    int inter=0;
    int no_parse=1;*/

    int i = 1;

    if((stdin=freopen(argv[1], "r", stdin))==NULL){
        fprintf(stderr,"can't open source file\n");
        exit(1);
    }

    if(!strcmp(argv[2],"-o")){
        fout = fopen(argv[3],"w");
    }

    /*while(i<argc){

        if(!strcmp(argv[i],"-version")){
            if(version == 1)
                usage(argv[0]);
            version = 1;
        }
        else if(!strcmp(argv[i],"-s")){
            if(source == 1)
                usage(argv[0]);
            source=1;
            no_parse=0;
            if(i+1 >= argc || argv[i+1][0]=='-'){
                usage(argv[0]);
            }
            else {
                source_file = i+1;
                i++;
            }
        }
        else if(!strcmp(argv[i],"-tos")){
            no_parse=0;
            tos=1;
        }
        else if(!strcmp(argv[i],"-i")){
            if(inter == 1)
                usage(argv[0]);
            no_parse=0;
            inter=1;
        }
        else if(!strcmp(argv[i],"-o")){
            if(output == 1)
                usage(argv[0]);
            no_parse=0;
            output = 1;
            if(i+1 >= argc || argv[i+1][0]=='-'){
                output_file = 0;
                i++;
                continue;
            }
            else if(!strcmp(argv[i+1],"stdout")){
                fout = stdout;
                i++;
                output_file = -1;
            }
            else {
                output_file = i+1;
                i++;
            }
        }
        else{
            usage(argv[0]);
        }
        i++;
    }

    // -s option used
    if(source){
        // -s option and redirection of stdin both used
        if(!isatty(fileno(stdin))){
            usage(argv[0]);
        }
        else if((stdin=freopen(argv[source_file], "r", stdin))==NULL){
            printf("\nCan't open source file\n");
            exit(1);
        }
    }
    // no -s option and no redirection of stdin. Reading from command line.
    else if((isatty(fileno(stdin)) && !version)){
        int response;
        printf("\nDo you want to write the source code in Command Line ?\n\n%s0-NO and QUIT %s1-YES%s\n\n",RED,GREEN,NORMAL);
        scanf("%d",&response);
        if(response == 0)
            usage(argv[0]);
        else{
            printf("\nType source text to parse:\n");
        }
    }
    // uniquement -version comme option
    else if(isatty(fileno(stdin)) && version && no_parse){
        printf("\n%8sMembres du groupes:\n","");
        printf("\n%8s\033[1mXU Chenglin  -  JANATI Siham  -  HAMMER Tom  -  WEREY Laurent\033[0m \n","");
        printf("%8s%s\033[1mSIL\033[0m          .  %s\033[1mSDSC\033[0m          .  %s\033[1mSIL\033[0m         .  %s\033[1mSIL\033[0m%s\n\n","",BLUE,PURPLE,BLUE,BLUE,NORMAL);
        exit(0);
    }


    /* un fichier source à parser existe*/

    curr_context = NULL;
 //   yydebug=1;
    int t = yyparse();

    if(t!=0)
        exit(EXIT_FAILURE);
    

    add_labels();    
    
   /* if(version){
        printf("\n%8sMembres du groupes:\n","");
        printf("\n%8s\033[1mXU Chenglin  -  JANATI Siham  -  HAMMER Tom  -  WEREY Laurent\033[0m \n","");
        printf("%8s%s\033[1mSIL\033[0m          .  %s\033[1mSDSC\033[0m          .  %s\033[1mSIL\033[0m         .  %s\033[1mSIL\033[0m%s\n\n","",BLUE,PURPLE,BLUE,BLUE,NORMAL);
    }

    if(inter){
        print_globalcode();
    }

    if(tos){
        print_stack();
    }
    
    if(output){
        if(output_file==0){
            fout = fopen("out.asm","w");
        }
        else if(output_file==-1){
            fout = stdout;
        }
        else {
            fout = fopen(argv[output_file],"w");
        }
        translate();
        if(fout != stdout)
            fclose(fout);
    }*/
    print_globalcode();	
    translate();
    if(fout != stdout)
            fclose(fout);
    printf("\n%8s%s\033[1mPARSING SUCCESSFULL!\033[0m \n%8s%s--------------------%s\n\n","",GREEN,"",GREEN,NORMAL);
    free_stack();
    exit(EXIT_SUCCESS);
}


void usage(char *name){
    fprintf(stderr, "\n%s%8sUsage: %s [-version] [-tos] [-i] [-o [<name>]] [-s <source> ]\n",YELLOW,"",name);
    fprintf(stderr, "%8sUsage: %s [-version] [-tos] [-i] [-o [<name>]] < <source>\n\n","",name);
    fprintf(stderr, "%s%8s-version:\n%14sMembres du groupe\n",NORMAL,"","");
    fprintf(stderr, "%8s-tos:\n%14sAffiche la table des symboles\n","","");
    fprintf(stderr,"%8s-i:\n%14sAffiche le code intermédiaire\n","","");
    fprintf(stderr,"%8s-o [<name>]:\n%14sEcris le code assembleur dans le fichier <name>. \n%14sSi <name> n'est pas mentionné, crée le fichier out.asm \n%14sSi <name> = stdout, affiche le résultat dans la ligne de commande\n","","","","");
    fprintf(stderr,"%8s-s <source>: \n%14sLis le code source à partir du fichier <source>\n","","");
    fprintf(stderr,"%8s< <source>: \n%14sLis le code source à partir du fichier <source>\n\n","","");

    exit(0);
}