//clang-format off
#include "stack.h"

#include <stdio.h>
#include <stdlib.h>
//clang-format on

struct Stack {
    uint32_t top; // remember that top always points at the space above the last filled index
    uint32_t capacity;
    Node ** nodes;

};

Stack *stack_create(uint32_t capacity) {
    Stack *s = (Stack *)malloc(sizeof(Stack));
    if(!s) {
        return NULL;
    }
    s->capacity = capacity;
    s->top = 0;
    s->nodes = (Node **)calloc(capacity, sizeof(Node *));
    if(!s->nodes) {
        free(s);
        return NULL;
    }
    return s;
}

void stack_delete(Stack **s) {
    free((*s)->nodes);
    free(*s);
    *s = NULL;
}

bool stack_empty(Stack *s) {
    return s->top == 0;
}

bool stack_full(Stack *s) {
    return s->top == s->capacity;
}

uint32_t stack_size(Stack *s) {
    return s->top;
}

bool stack_push(Stack *s, Node *n) {
    if(!stack_full(s)) {
        s->nodes[s->top] = n;
        s->top += 1;
        return true;
    }
    else {
        return false;
    }
}

bool stack_pop(Stack *s, Node **n) {
    if(!stack_empty(s)) {
        s->top -= 1;
        *n = s->nodes[s->top];
        return true;
    }
    else {
        return false;
    }
}

void stack_print(Stack *s) {
    for (uint32_t i = 0; i < s->top; i +=1) {
        node_print(s->nodes[i]);
    }
}
