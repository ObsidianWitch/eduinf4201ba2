#ifndef MESSAGE_LINKED_LIST_H
#define MESSAGE_LINKED_LIST_H

#include "message.h"

typedef struct node {
    message* msg;
    struct node* next;
} node;

node* create_node(message *msg);
void free_node(node* n);
void free_linked_list(node** linked_list);
int nodecmp(const node* n1, const node* n2);
void insert_message(node** linked_list, message* msg);
void pop(node** linked_list);
void print_messages_linked_list(const node* head);

#endif // LINKED_LIST_H
