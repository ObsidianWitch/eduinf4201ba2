#ifndef MESSAGE_LINKED_LIST_H
#define MESSAGE_LINKED_LIST_H

#include "message_tools.h"

typedef struct node {
    message* msg;
    struct node* next;
} node;

node* create_node(message *msg);
void free_linked_list(node* head);
void insert_message(node** linked_list, message* msg);
void print_messages_linked_list(node* head);

#endif // LINKED_LIST_H
