#include "hashtable.h"

 HashTable* curr_context;
 HashTable* glob_context;
 HashTable* stack;
 int count;

unsigned long hash_function(char *str)
{   
    unsigned long i = 0;
    for (int j = 0; str[j]; j++)
        i += str[j];
    return i % CAPACITY;
}

static LinkedList *allocate_list()
{
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    return list;
}

static LinkedList *linkedlist_insert(LinkedList *list, Ht_item *item)
{
    if (!list)
    {
        LinkedList *head = allocate_list();
        head->item = item;
        head->next = NULL;
        list = head;
        return list;
    }

    else if (list->next == NULL)
    {
        LinkedList *node = allocate_list();
        node->item = item;
        node->next = NULL;
        list->next = node;
        return list;
    }

    LinkedList *temp = list;
    while (temp->next->next)
    {
        temp = temp->next;
    }

    LinkedList *node = allocate_list();
    node->item = item;
    node->next = NULL;
    temp->next->next = node;

    return list;
}

static Ht_item *linkedlist_remove(LinkedList *list)
{
    // return the item of the popped element
    if (!list)
        return NULL;
    if (!list->next)
        return NULL;
    LinkedList *node = list->next;
    LinkedList *temp = list;
    temp->next = NULL;
    list = node;
    Ht_item *it = NULL;
    memcpy(temp->item, it, sizeof(Ht_item));
    free(temp->item->key);
    free(temp->item);
    free(temp);
    return it;
}

static void free_linkedlist(LinkedList *list)
{
    LinkedList *temp = list;
    while (list)
    {
        temp = list;
        list = list->next;
        free(temp->item->key);
        free(temp->item);
        free(temp);
    }
}

static LinkedList **create_lists(HashTable *table)
{
    LinkedList **lists = (LinkedList **)calloc(table->max_size, sizeof(LinkedList *));
    for (int i = 0; i < table->max_size; i++)
        lists[i] = NULL;
    return lists;
}

static void free_lists(HashTable *table)
{
    LinkedList **lists = table->lists;
    for (int i = 0; i < table->max_size; i++)
        free_linkedlist(lists[i]);
    free(lists);
}

Ht_item *create_item(char *key, int id_type, int value)
{
    Ht_item *item = (Ht_item *) malloc(sizeof(Ht_item));
    item->key = (char *) malloc(strlen(key) + 1);
    strcpy(item->key, key);

    item->id_type = id_type;
    item->value = value;
    item->order = -1;

    return item;
}

HashTable *create_table()
{
    HashTable *table = (HashTable *)malloc(sizeof(HashTable));
    table->max_size = CAPACITY;
    table->count = 0;
    table->items = (Ht_item **)calloc(table->max_size, sizeof(Ht_item *));
    for (int i = 0; i < table->max_size; i++)
        table->items[i] = NULL;
    table->lists = create_lists(table);

    return table;
}

void free_item(Ht_item *item)
{
    free(item->key);
    free(item);
}

void free_table(HashTable *table)
{
    for (int i = 0; i < table->max_size; i++)
    {
        Ht_item *item = table->items[i];
        if (item != NULL)
            free_item(item);
    }

    free_lists(table);
    free(table->items);
    free(table);
}

void handle_collision(HashTable *table, unsigned long index, Ht_item *item)
{
    LinkedList *head = table->lists[index];

    if (head == NULL)
    {
        // We need to create the list
        head = allocate_list();
        head->item = item;
        table->lists[index] = head;
        return;
    }
    else
    {
        // Insert to the list
        table->lists[index] = linkedlist_insert(head, item);
        return;
    }
}

void ht_insert(HashTable *table, Ht_item *item)
{

    unsigned long index = hash_function(item->key);

    Ht_item *current_item = table->items[index];

    if (current_item == NULL)
    {
        // no exist.
        if (table->count == table->max_size)
        {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Remove the create item
            free_item(item);
            return;
        }

        // Insert directly
        table->items[index] = item;
     //   printf("\nsegf %s \n",item->key);
    }

    else
    {
        //update value
        if (strcmp(current_item->key, item->key) == 0){
            table->items[index]->value = item->value;
            return;
        }

        else
        {

            // Collision
            handle_collision(table, index, item);
           
            return;    
        }
    }
}

Ht_item *ht_search(HashTable *table, char *key)
{
    // Searches the key in the hashtable
    // and returns NULL if it doesn't exist
    int index = hash_function(key);
    Ht_item *item = table->items[index];
    LinkedList *head = table->lists[index];

    // Ensure that we move to items which are not NULL
    while (item != NULL)
    {
        if (strcmp(item->key, key) == 0){
            return item;
        }
        if (head == NULL){
            return NULL;
        }
        item = head->item;
        head = head->next;
    }
    return NULL;
}

void ht_delete(HashTable *table, char *key)
{
    // Deletes an item from the table
    int index = hash_function(key);
    Ht_item *item = table->items[index];
    LinkedList *head = table->lists[index];

    if (item == NULL)
    {
        return;
    }
    else
    {
        if (head == NULL && strcmp(item->key, key) == 0)
        {
            // No collision chain. Remove the item
            // and set table index to NULL
            table->items[index] = NULL;
            free_item(item);
            table->count--;
            return;
        }
        else if (head != NULL)
        {
            // Collision Chain exists
            if (strcmp(item->key, key) == 0)
            {
                // Remove this item and set the head of the list
                // as the new item
                free_item(item);
                table->count--;
                LinkedList *node = head;
                head = head->next;
                node->next = NULL;

                table->items[index] = create_item(node->item->key, node->item->id_type, node->item->value);
                table->items[index]->order = node->item->order;
                free_linkedlist(node);
                table->lists[index] = head;
                return;
            }

            LinkedList *curr = head;
            LinkedList *prev = NULL;

            while (curr)
            {
                if (strcmp(curr->item->key, key) == 0)
                {
                    if (prev == NULL)
                    {                       
                        // First element of the chain. Remove the chain
                        free_linkedlist(head);
                        table->lists[index] = NULL;
                        table->count --;
                        return;
                    }
                    else
                    {
                        // This is somewhere in the chain
                        prev->next = curr->next;
                        curr->next = NULL;
                        free_linkedlist(curr);
                        table->lists[index] = head;
                        table->count --;
                        return;
                    }
                
                }
                curr = curr->next;
                prev = curr;
            }
        }
    }
}

void print_table(HashTable *table)
{
    printf("\n\t\033[1m_________________________________________________________________\033[0m\n\n");
    int j = 0;
    for (int i = 0; i < table->max_size; i++)
    {
        if (table->items[i] && table->items[i]->id_type != ID_TMP)
        {
            printf("%s\033[1m%12s\033[0m |%s %4sIndex: %d |  %4sType: %s | ",YELLOW, table->items[i]->key,NORMAL,"", table->items[i]->order,"", get_type_id(table->items[i]->value));
            if(table->items[i]->id_type == ID_TAB)
                printf("%10sArray | %4ssize: %d ","", "", table->items[i]->size/4);
            else if(table->items[i]->id_type == ID_METHOD)
                printf("%10sMethod ","");
            else if(table->items[i]->id_type == ID_VAR)
                printf("%10sScalar ","");
            else if(table->items[i]->id_type == ID_PARAM)
                printf("%10sParam ","");
            if (table->lists[i])
            {
                printf("\n");
                LinkedList *head = table->lists[i];
                while (head)
                {
                    printf("%s\033[1m%12s\033[0m |%s %4sIndex: %d | %4sType:%s | ",YELLOW, head->item->key,NORMAL,"", head->item->order,"", get_type_id(head->item->value));
                    if(head->item->id_type == ID_TAB)
                        printf("%10sArray | %4ssize: %d ","","", head->item->size/4);
                    else if(head->item->id_type == ID_METHOD)
                        printf("%10sMethod ","");
                    else if(table->items[i]->id_type == ID_VAR)
                        printf("%10sScalar ","");
                    else if(table->items[i]->id_type == ID_PARAM)
                        printf("%10sParam ","");
                    printf("\n");
                    head = head->next;
                }
            }
            printf("\n"); j++;
        }
    }
    if(j==0)
        printf("%20sNO DECLARATIONS IN THIS CONTEXT","");
    printf("\n\n");
}

void free_tables(){
    HashTable* t = curr_context;
    HashTable* pt;
    while(t){
        pt = t->next;
        for(int i=0; i<t->max_size; i++){
            if(t->items[i]){
                free(t->items[i]->key);
                free(t->items[i]->p);
                free(t->items[i]);
            }
        }
        free(t->items);
        for(int i=0; i<t->max_size; i++){
            if(t->lists[i]) {
                free(t->lists[i]->item->key);
                free(t->lists[i]->item->p);
                free(t->lists[i]->item);
                free(t->lists[i]);
            }
            free(t->lists);
        }
        free(t);
        t = pt;
    }
}

void pushctx(ctx_type type){
    HashTable *temp = create_table();
	temp->next = curr_context;
	curr_context = temp;
    curr_context->type = type;
}

// pop current context and put it into the stack
void popctx(){
    HashTable *temp = curr_context;
	curr_context = curr_context->next;
    tmpCount = 0;

    HashTable *ht = create_table();
    *ht = *temp;
    if(stack == NULL){
        stack = ht;
        stack->next = NULL;
    }
    else{
        ht->next = stack;
        stack = ht;
    }
}

// called at the end of main to free the TOS stack
void free_stack(){
    HashTable *pt = stack;
    while(pt!=NULL){
        stack = stack->next;
        free(pt);
        pt = stack;
    }
}

void newname(Ht_item *item){
    item->order = curr_context->count;
    ht_insert(curr_context, item);
    curr_context->count ++;
    curr_context->size += item->size;
}

item_table *lookup(char *key, HashTable* ctx){

    item_table *res = (item_table*) malloc(sizeof(item_table));

    for(HashTable *i=ctx; i; i = i->next){
        if((res->item = ht_search(i, key)) != NULL){
            res->table = i;
            return res;
        }
    }
    return NULL;
}


void print_ctx(){
    //printf("\n\tTABLES DES SYMBOLES:\n\n");
	if(count == 0)
        printf("\n\t%s\e[4m\033[1mTABLES DES SYMBOLES\033[0m\e[0m\n\n%s",CYAN,NORMAL);

    if(curr_context == glob_context)
        printf("\t\033[1mGLOBAL CONTEXT:\033[0m");
    else
        printf("\t\033[1mCONTEXT %d:\033[0m",count);

    print_table(curr_context);
    count++;
    if(curr_context->next == NULL)
        printf("\n\t%s\033[1m___________________________  FIN  TOS  __________________________\033[0m%s\n\n",CYAN,NORMAL);
}

Ht_item* new_temp(int type){

    char* label;
    int a = tmpCount;
    int cpt = 1;
    while(a){
        a=a/10;
        cpt++;
    }
    label = malloc(cpt+4);
    sprintf(label, "%ldtmp",tmpCount);

    Ht_item *item = create_item(label, ID_TMP, type);
    item->size = 4;
    newname(item);
    tmpCount++;
    return item;
}

char* get_type_id(int type){
    if(type == INT)
        return "INT";
    else if(type == BOOL)
        return "BOOL";
    else if(type == VOIDTYPE)
        return "VOID";
    else    
        return "NA";
}

char* get_type_global(int type){
    if(type == ID_VAR)
        return "Scalar";
    else if(type == ID_TAB)
        return "Array";
    else return "NA";
}

int offset(item_table *val, HashTable* ctx){

	int out = 0;

    out = 4*(val->table->count - (val->item->order+1));

    if(val->table == ctx)
        return out;

    HashTable *temp = ctx;

    while(temp != val->table && temp != glob_context){
        out += 4*temp->count;
        temp = temp->next;
    }

	return out;
}



