#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

typedef struct type {
    const char* name;
    int size;
} type;

#define INTEGER type{"INT", 4};
#define DOUBLE type{"DOUBLE", 8};
#define CHAR type{"CHAR", 1};


typedef struct entry {
    struct entry *parent;
    type type;
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
