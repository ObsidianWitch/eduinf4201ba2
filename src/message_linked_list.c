#include <stdlib.h>
#include <stdio.h>
#include "message_linked_list.h"

/**
 * Creates a node containing the message given in parameter and pointing to
 * NULL.
 *
 * @param msg node's message
 * @return Newly created node
 */
node* create_node(message *msg) {
    node* new_node = malloc(sizeof(node));

    new_node->msg = msg;
    new_node->next = NULL;

    return new_node;
}

/**
 * Frees the given node and the message contained in it.
 *
 * @param n Node which will be freed
 */
void free_node(node* n) {
    free_message(n->msg);
    free(n);
}

/**
 * Frees all the nodes of a linked list. Once all the nodes are freed, The given
 * linked_list points to NULL.
 *
 * @param linked_list Linked List which will be freed
 */
void free_linked_list(node** linked_list) {
    node* cur_node = *linked_list;

    while (cur_node != NULL) {
        node* tmp = cur_node->next;
        free_node(cur_node);
        cur_node = tmp;
    }

    *linked_list = NULL;
}

/*
 * Compares two nodes n1 and n2. Nodes are compared according to their message's
 * timestamp and host_id.
 *
 * @param n1 first node to compare
 * @param n2 second node to compare
 * @return It returns an integer less than, equal to, or greater than zero if n1
 * is found, respectively, to be less than, to match, or be greater than n2.
 */
int nodecmp(const node* n1, const node* n2) {
    message *m1 = n1->msg;
    message *m2 = n2->msg;

    if (m1->timestamp < m2->timestamp) {
        return -1;
    }
    else if (m1->timestamp > m2->timestamp) {
        return 1;
    }
    else {
        if (m1->host_id < m2->host_id) {
            return -1;
        }
        else if (m1->host_id > m2->host_id) {
            return 1;
        }
        else {
            return 0;
        }
    }
}

/**
 * Inserts a node at the begining of the given linked_list.
 *
 * @param linked_list
 * @param new_node
 */
void insert_node_front(node** linked_list, node* new_node) {
    node* tmp = *linked_list;
    *linked_list = new_node;
    new_node->next = tmp;
}

/**
 * Inserts a node into the given linked_list. The node is created with the
 * message in parameter. If the linked_list was ordered before the execution of
 * this function, it will still be after : the node is inserted at the right
 * place (list ordered by timestamp and host_id).
 *
 * @param linked_list
 * @param new_msg
 */
void insert_message(node** linked_list, message* new_msg) {
    node* new_node = create_node(new_msg);

    if (*linked_list == NULL) {
        *linked_list = new_node;
    }
    else if (nodecmp(new_node, *linked_list) <= 0) {
        insert_node_front(linked_list, new_node);
    }
    else if (nodecmp(new_node, *linked_list) > 0) {
        node* cur_node = *linked_list;
        node* next_node = cur_node->next;

        while(next_node != NULL && nodecmp(new_node, next_node) > 0) {
            cur_node = cur_node->next;
            next_node = cur_node->next;
        }

        new_node->next = next_node;
        cur_node->next = new_node;
    }
}

/**
 * Removes the first element from the given linked_list.
 *
 * @param linked_list
 */
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
void print_messages_linked_list(const node* head) {
    while (head != NULL) {
        printf(
            "%d - %d - %s\n",
            head->msg->host_id,
            head->msg->timestamp,
            head->msg->str
        );

        head = head->next;
    }
}
