CC = gcc

SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include
BIN_DIR := .

EXE := $(BIN_DIR)/decaf
SRC := $(wildcard $(SRC_DIR)/*.c) $(SRC_DIR)/lex.yy.c $(SRC_DIR)/compiler.tab.c
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS := -Iinclude
CFLAGS :=
LDFLAGS :=
LDLIBS :=

all: $(EXE)

.PHONY: all clean

$(EXE): $(OBJ) | $(BIN_DIR)
	@echo $(SRC)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(SRC_DIR)/compiler.tab.c + $(SRC_DIR)/compiler.tab.h: $(SRC_DIR)/compiler.y
	bison -d $(SRC_DIR)/compiler.y
	mv compiler.tab.h $(INC_DIR)
	mv compiler.tab.c $(SRC_DIR)

$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/compiler.lex $(SRC_DIR)/compiler.tab.c
	flex $(SRC_DIR)/compiler.lex
	mv lex.yy.c $(SRC_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(OBJ_DIR) $(EXE)
	@$(RM) $(SRC_DIR)/compiler.tab.c $(SRC_DIR)/lex.yy.c $(INC_DIR)/compiler.tab.h

-include $(OBJ:.o=.d)