prefixe=compiler
dep=intermediaire
dep2=hashtable
dep3=translater
dep4=transTool
dep5=IOfunctions
dep6=text_formating
dep7=compiler_functions
# exige 3 fichiers:
# - $(prefixe).y (fichier bison)
# - $(prefixe).lex (fichier flex)
# - main.c (programme principal)
# construit un exécutable nommé "main"

# note : le programme principal ne doit surtout pas s'appeler $(prefixe).c
# (make l'écraserait parce qu'il a une règle "%.c: %.y")

all: decaf

decaf: $(prefixe).tab.o lex.yy.o decaf.o $(dep).o $(dep2).o $(dep3).o $(dep4).o $(dep5).o $(dep6).o $(dep7).o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(dep).o : $(dep).c $(dep).h $(dep2).h token.h
	$(CC) -c -g $(dep).c

$(dep2).o : $(dep2).c $(dep2).h $(dep).h token.h 
	$(CC) -c -g $(dep2).c

$(dep3).o : $(dep3).c $(dep3).h $(dep).h $(dep4).h $(dep2).h instructions.h
	$(CC) -c -g $(dep3).c

$(dep4).o : $(dep4).c $(dep4).h $(dep).h $(dep6).h $(dep2).h instructions.h
	$(CC) -c -g $(dep4).c

$(dep5).o : $(dep5).c $(dep5).h 
	$(CC) -c -g $(dep5).c

$(dep6).o : $(dep6).c $(dep6).h 
	$(CC) -c -g $(dep6).c

$(dep7).o : $(dep7).c $(dep7).h $(dep).h $(dep2).h token.h
	$(CC) -c -g $(dep7).c

*.y : $(dep).h $(dep2).h $(dep5).h $(dep6).h token.h $(dep7).h
*.lex : $(dep).h token.h 

$(prefixe).tab.c: $(prefixe).y
	bison -t -d $(prefixe).y

lex.yy.c: $(prefixe).lex $(prefixe).tab.h
	flex $(prefixe).lex

$(dep).c : $(dep).h $(dep2).h token.h
$(dep2).c : $(dep2).h $(dep).h token.h
$(dep3).c : $(dep3).h $(dep).h $(dep4).h $(dep2).h
$(dep4).c : $(dep4).h $(dep).h $(dep6).h $(dep2).h
$(dep5).c : $(dep5).h 
$(dep6).c : $(dep6).h 
$(dep7).c : $(dep7).h $(dep).h $(dep2).h token.h

doc:
	bison --report=all --report-file=$(prefixe).output \
		--graph=$(prefixe).dot --output=/dev/null \
		$(prefixe).y
	dot -Tpdf < $(prefixe).dot > $(prefixe).pdf

archive:
	tar -cvzf $(prefixe).tar.gz *.c *.h *.lex *.y *.txt Makefile

clean:
	rm -f *.o $(prefixe).tab.c $(prefixe).tab.h lex.yy.c decaf *.gz \
		$(prefixe).output $(prefixe).dot $(prefixe).pdf

