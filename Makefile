prefixe=decaf_plus
dep=intermediaire
dep2=hashtable
dep3=translater
dep4=transTool
dep5=IOfunctions
dep6=text_formating
# exige 3 fichiers:
# - $(prefixe).y (fichier bison)
# - $(prefixe).lex (fichier flex)
# - main.c (programme principal)
# construit un exécutable nommé "main"

# note : le programme principal ne doit surtout pas s'appeler $(prefixe).c
# (make l'écraserait parce qu'il a une règle "%.c: %.y")

all: decaf

decaf: $(prefixe).tab.o lex.yy.o decaf.o $(dep).o $(dep2).o $(dep3).o $(dep4).o $(dep5).o $(dep6).o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(dep).c : $(dep).h $(dep2).h token.h
$(dep2).c : $(dep2).h $(dep).h token.h
$(dep3).c : $(dep3).h $(dep).h $(dep4).h 
$(dep4).c : $(dep4).h $(dep).h
$(dep5).c : $(dep5).h
$(dep6).c : $(dep6).h

*.y : $(dep).h $(dep2).h $(dep5).h $(dep6).h token.h
*.lex : $(dep).h token.h 

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
	tar -cvzf $(prefixe).tar.gz *.c *.h *.lex *.y *.txt Makefile

clean:
	rm -f *.o $(prefixe).tab.c $(prefixe).tab.h lex.yy.c decaf *.gz \
		$(prefixe).output $(prefixe).dot $(prefixe).pdf

