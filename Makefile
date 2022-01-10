CC = gcc

SRC_DIR := src
OBJ_DIR := obj
INC_DIR := include
BIN_DIR := .

EXE := $(BIN_DIR)/decaf
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CPPFLAGS := -Iinclude
CFLAGS :=
LDFLAGS :=
LDLIBS :=

all: parser $(EXE)

.PHONY: all clean

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

parser:
	bison -d $(SRC_DIR)/compiler.y
	flex $(SRC_DIR)/compiler.lex
	mv compiler.tab.c $(SRC_DIR)
	mv compiler.tab.h $(INC_DIR)
	mv lex.yy.c $(SRC_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR) $(SCOP2_FILES) $(SCOP_FILES) $(EXE)
	@$(RM) $(SRC_DIR)/compiler.tab.c $(SRC_DIR)/lex.yy.c $(INC_DIR)/compiler.tab.h

-include $(OBJ:.o=.d)