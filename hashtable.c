#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__
#include "hashtable.h"

#define CAPACITY 50000 // Size of the Hash Table

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
    temp->next = node;

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
    free(temp->item->value);
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
        free(temp->item->value);
        free(temp->item);
        free(temp);
    }
}

static LinkedList **create_lists(HashTable *table)
{
    LinkedList **lists = (LinkedList **)calloc(table->size, sizeof(LinkedList *));
    for (int i = 0; i < table->size; i++)
        lists[i] = NULL;
    return lists;
}

static void free_lists(HashTable *table)
{
    LinkedList **lists = table->lists;
    for (int i = 0; i < table->size; i++)
        free_linkedlist(lists[i]);
    free(lists);
}

Ht_item *create_item(char *key, char *value)
{
    Ht_item *item = (Ht_item *)malloc(sizeof(Ht_item));
    item->key = (char *)malloc(strlen(key) + 1);
    item->value = (char *)malloc(strlen(value) + 1);

    strcpy(item->key, key);
    strcpy(item->value, value);

    return item;
}

HashTable *create_table()
{
    HashTable *table = (HashTable *)malloc(sizeof(HashTable));
    table->size = CAPACITY;
    table->count = 0;
    table->items = (Ht_item **)calloc(table->size, sizeof(Ht_item *));
    for (int i = 0; i < table->size; i++)
        table->items[i] = NULL;
    table->lists = create_lists(table);

    return table;
}

void free_item(Ht_item *item)
{
    free(item->key);
    free(item->value);
    free(item);
}

void free_table(HashTable *table)
{
    for (int i = 0; i < table->size; i++)
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
        // We need to crate the list
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

void ht_insert(HashTable *table, char *key, char *value)
{
    // Create the item
    Ht_item *item = create_item(key, value);

    unsigned long index = hash_function(key);

    Ht_item *current_item = table->items[index];

    if (current_item == NULL)
    {
        // no exist.
        if (table->count == table->size)
        {
            // Hash Table Full
            printf("Insert Error: Hash Table is full\n");
            // Remove the create item
            free_item(item);
            return;
        }

        // Insert directly
        table->items[index] = item;
        table->count++;
    }

    else
    {
        //update value
        if (strcmp(current_item->key, key) == 0)
        {
            strcpy(table->items[index]->value, value);
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

char *ht_search(HashTable *table, char *key)
{
    // Searches the key in the hashtable
    // and returns NULL if it doesn't exist
    int index = hash_function(key);
    Ht_item *item = table->items[index];
    LinkedList *head = table->lists[index];

    // Ensure that we move to items which are not NULL
    while (item != NULL)
    {
        if (strcmp(item->key, key) == 0)
            return item->value;
        if (head == NULL)
            return NULL;
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
                LinkedList *node = head;
                head = head->next;
                node->next = NULL;
                table->items[index] = create_item(node->item->key, node->item->value);
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
                        return;
                    }
                    else
                    {
                        // This is somewhere in the chain
                        prev->next = curr->next;
                        curr->next = NULL;
                        free_linkedlist(curr);
                        table->lists[index] = head;
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
    printf("\n-------------------\n");
    for (int i = 0; i < table->size; i++)
    {
        if (table->items[i])
        {
            printf("Index:%d, Key:%s, Value:%s", i, table->items[i]->key, table->items[i]->value);
            if (table->lists[i])
            {
                printf(" => list => ");
                LinkedList *head = table->lists[i];
                while (head)
                {
                    printf("Key:%s, Value:%s ", head->item->key, head->item->value);
                    head = head->next;
                }
            }
            printf("\n");
        }
    }
    printf("-------------------\n");
}