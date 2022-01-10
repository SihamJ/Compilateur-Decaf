#include "../include/library.h"

void add_libs_to_tos(){

        Ht_item *item1 = create_item("WriteInt", ID_METHOD, VOIDTYPE);
        newname(item1);
        param p_write_int = (struct param*) malloc(sizeof(struct param));
        p_write_int->type = INT;
        p_write_int->byAddress = 0;
        p_write_int->next = NULL;
        item_table* var1 = lookup("WriteInt", curr_context);
        if(var1 != NULL && var1->item != NULL)
        var1->item->p = p_write_int;

        Ht_item *item2 = create_item("WriteString", ID_METHOD, VOIDTYPE);
        newname(item2);
        param p_write_string = (struct param*) malloc(sizeof(struct param));
        p_write_string->type = STRING;
        p_write_string->byAddress = 0;
        p_write_string->next = NULL;
        item_table* var2 = lookup("WriteString", curr_context);
        var2->item->p = p_write_string;

        Ht_item *item3 = create_item("WriteBool", ID_METHOD, VOIDTYPE);
        newname(item3);
        param p_write_bool = (struct param*) malloc(sizeof(struct param));
        p_write_bool->type = BOOL;
        p_write_bool->byAddress = 0;
        p_write_bool->next = NULL;
        item_table* var3 = lookup("WriteBool", curr_context);
        var3->item->p = p_write_bool;

        Ht_item *item4 = create_item("ReadInt", ID_METHOD, VOIDTYPE);
        newname(item4);
        param p_read_int = (struct param*) malloc(sizeof(struct param));
        p_read_int->type = INT;
        p_read_int->byAddress = 1;
        p_read_int->next = NULL;
        item_table* var4 = lookup("ReadInt", curr_context);
        var4->item->p = p_read_int;

        Ht_item *item5 = create_item("Random", ID_METHOD, INT);
        newname(item5);
        param p_random = (struct param*) malloc(sizeof(struct param));
        p_random->type = INT;
        p_random->byAddress = 0;
        p_random->next = NULL;
        item_table* var5 = lookup("Random", curr_context);
        var5->item->p = p_random;

        Ht_item *item6 = create_item("Exit", ID_METHOD, VOIDTYPE);
        newname(item6);
        param p_exit = (struct param*) malloc(sizeof(struct param));
        p_exit->type = INT;
        p_exit->byAddress = 0;
        p_exit->next = NULL;
        item_table* var6 = lookup("Exit", curr_context);
        var6->item->p = p_exit;

}
