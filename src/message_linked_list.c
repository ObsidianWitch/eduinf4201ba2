#include "message_linked_list.h"

node* create_node(message *msg) {
    node new_node* = malloc(sizeof(node));

    new_node-> msg = msg;
    new_node-> next = NULL;

    return new_node;
}

void free_node(node* n) {
    free(n->msg);
    free(n);
}

void free_linked_list(node* head) {
    while (*linked_list != NULL) {
        node* tmp = head->next;

        free_node(head);

        head = tmp;
    }
}

void insert_message(node** linked_list, message* msg) {
    node* new_node = create_node(msg);

    if (*linked_list == NULL) {
        *linked_list = new_node;
    }
    else if (*linked_list->msg->el > msg->el) {
        new_node->next = *linked_list;
        *linked_list = new_node;
    }
    else {
        node* next_node;
        node* cur_node = *linked_list;

        while(cur_node != NULL && cur_node->msg->el < msg->el) {
            cur_node = cur_node->next;
        }

        /* TODO ?
        next_node = cur_node->next;
        if (next_node->msg->el == msg->el &&
            next_node->msg->host_id < msg->host_id)
        {
            cur_node = next_node;
        }
        */

        new_node->next = cur_node->next;
        cur_node->next = new_node;
    }
}
