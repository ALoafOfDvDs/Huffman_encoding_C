//clang-format off
#include "node.h"
 
#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
//clang-format on

Node *node_create(uint8_t symbol, uint64_t frequency) {
    Node * new_node = (Node *)malloc(sizeof(Node));
    if(new_node) {
        new_node->symbol = symbol;
        new_node->frequency = frequency;
        new_node->left = NULL;
        new_node->right = NULL;
    }
    return new_node;
}

void node_delete(Node **n) {
    free(*n);
    *n = NULL;
    return;
}

Node *node_join(Node *left, Node *right) {
    uint64_t sum = left->frequency + right->frequency;
    Node * parent = node_create('$', sum);
    parent->left = left;
    parent->right = right;
    return parent;
}

void node_print(Node *n) {
    if(n == NULL) {
        fprintf(stderr, "NULL\n");
        return;
    }
    if(isprint(n->symbol)) {
        fprintf(stderr, "symbol: %c\nfrequency: %lu\n",n->symbol, n->frequency);
    }
    else {
        // is a control char
        fprintf(stderr, "symbol:-0x%02"PRIx8"-\nfrequency: %lu\n",n->symbol, n->frequency);
    }
}

bool node_cmp(Node *n, Node *m) {
    return n->frequency > m->frequency;
}

void node_print_sym(Node *n) {
    // we can assume that the symbol of a node is a char, so we can use char format specifier
    if(isprint(n->symbol)) {
        fprintf(stderr, "symbol: %c\n",n->symbol);
    }
    else {
        // is a control char
        fprintf(stderr, "symbol:-0x%02"PRIx8"-\n",n->symbol);
    }
}
