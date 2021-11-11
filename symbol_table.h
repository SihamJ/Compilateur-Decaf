#ifndef SYMBOL_TABLE_H_
#define SYMBOL_TABLE_H_

typedef struct type {
    const char* name;
    int size;
} type;

const type INTEGER = {"INT", 4};
const type BOOLEAN = {"BOOLEAN", 4};
// Funtions are declared globally, we won't save them on stack. Hence, its size on stack is zero.(Its size should be useless.)
const type FUNCTION = {"FUNCTION", 0};

typedef struct entry {
    struct entry *parent;
    type type;
    // As the prof did during TD, we use INT to represent BOOL
    const char* name;
    int value;
} entry;


typedef struct context {
    struct context *parent;
    entry *lastEntry;
} context;

typedef struct symbol_table {
    context *global;
    context *lastContext;
} symbol_table;

#endif
