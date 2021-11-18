prefixe=decaf
dep=intermediaire
# exige 3 fichiers:
# - $(prefixe).y (fichier bison)
# - $(prefixe).lex (fichier flex)
# - main.c (programme principal)
# construit un exécutable nommé "main"

# note : le programme principal ne doit surtout pas s'appeler $(prefixe).c
# (make l'écraserait parce qu'il a une règle "%.c: %.y")

all: main

main: $(prefixe).tab.o lex.yy.o main.o $(dep).o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

*.c : $(dep).h
*.y : $(dep).h
*.lex : $(dep).h

$(prefixe).tab.c: $(prefixe).y
	bison -t -d $(prefixe).y

lex.yy.c: $(prefixe).lex $(prefixe).tab.h
	flex $(prefixe).lex

doc:
	bison --report=all --report-file=$(prefixe).output \
		--graph=$(prefixe).dot --output=/dev/null \
		$(prefixe).y
	dot -Tpdf < $(prefixe).dot > $(prefixe).pdf

archive:
	tar -cvzf Siham_Janati_exo3.tar.gz *.c *.h *.lex *.y *.txt Makefile

clean:
	rm -f *.o $(prefixe).tab.c $(prefixe).tab.h lex.yy.c main *.gz \
		$(prefixe).output $(prefixe).dot $(prefixe).pdf
