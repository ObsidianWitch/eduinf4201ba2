#include <stdlib.h>
#include <stdio.h>
#include "message_linked_list.h"

node* create_node(message *msg) {
    node* new_node = malloc(sizeof(node));

    new_node->msg = msg;
    new_node->next = NULL;

    return new_node;
}

void free_node(node* n) {
    free(n->msg);
    free(n);
}

void free_linked_list(node* head) {
    node* cur_node = head;

    while (cur_node != NULL) {
        node* tmp = cur_node->next;
        free(cur_node);
        cur_node = tmp;
    }
}

void insert_message(node** linked_list, message* new_msg) {
    node* new_node = create_node(new_msg);

    if (*linked_list == NULL) {
        *linked_list = new_node;
    }
    else {
        node* cur_node = *linked_list;
        node* next_node = cur_node->next;

        while(next_node != NULL && next_node->msg->timestamp < new_msg->timestamp) {
            cur_node = cur_node->next;
            next_node = cur_node->next;
        }

        /* TODO ?
        next_node = cur_node->next;
        if (next_node->msg->timestamp == new_msg->timestamp &&
            next_node->msg->host_id < new_msg->host_id)
        {
            cur_node = next_node;
        }
        */

        new_node->next = next_node;
        cur_node->next = new_node;
    }
}

void pop(node** linked_list) {
    if (*linked_list != NULL) {
        node* tmp = (*linked_list)->next;
        free_node(*linked_list);
        *linked_list = tmp;
    }
}

/**
 * Prints all the messages contained in the linked list given in parameter.
 *
 * @param head Linked list's head
 */
void print_messages_linked_list(node* head) {
    node* cur_node = head;

    while (cur_node != NULL) {
        printf("%d - %s\n", cur_node->msg->timestamp, cur_node->msg->str);
        cur_node = cur_node->next;
    }
}
