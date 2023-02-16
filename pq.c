//clang-format off
#include "pq.h"

#include <stdlib.h>
#include <stdio.h>
//clang-format on


struct PriorityQueue {
    uint32_t size_used; // In theory size used should be the index of the tail I think

    uint32_t total_capacity;
    Node ** array;
};

/*
######################################################################
######################################################################
######################################################################
*/
// This is space for whatever sorting algorithm I need to implement
// All heap function adapted from heapsort in assignment 4

static uint32_t parent_index(uint32_t i) {
    return (i - 1) / 2;
}
static uint32_t get_left_child(uint32_t i) {
    return (2*i) + 1;
}
static uint32_t get_right_child(uint32_t i) {
    return (2*i) + 2;
}

static void swap(Node ** left, Node ** right) {
    Node * temp = *left;
    *left = *right;
    *right = temp;
    return;
}
void down_heap(Node ** heap, uint32_t heap_size) {
    uint32_t n = 0;
    uint32_t smaller; // this is the index of the smallest current working child
    while(get_left_child(n) < heap_size) {
        //we check to make sure we're inbounds
        if(get_right_child(n) == heap_size) {
            //we have no right child
            smaller = get_left_child(n);
        }
        else {
            if(!node_cmp(heap[get_left_child(n)], heap[get_right_child(n)])) {
                smaller = get_left_child(n);
            }
            else {
                //if they are equal it will fall to this case, but it doesnt matter either way
                smaller = get_right_child(n);
            }
        }
        //smaller has to have A value at this point provided the left child is within the bounds
        if(!node_cmp(heap[n], heap[smaller])) {
            break;
        }
        swap(&(heap[n]), &(heap[smaller]));
        n = smaller;
    }

}

static void up_heap(Node ** arr, uint32_t i) {
    while (i > 0 && (!node_cmp(arr[i], arr[parent_index(i)]))) {
        //while our index is not the top index (0)
        //while whichever value we are on is greater than its parent

        swap(arr + i, arr + parent_index(i));
        i = parent_index(i);
    }
}




/*
######################################################################
######################################################################
######################################################################
*/

PriorityQueue *pq_create(uint32_t capacity) {
    PriorityQueue *q = (PriorityQueue *)malloc(sizeof(PriorityQueue));
    if (q) {
        q->total_capacity = capacity;
        q->size_used = 0; 
        q->array = (Node **)calloc(capacity, sizeof(Node *));
        if(!(q->array)) {
            free(q);
            return NULL;
        }
        return q;
    }
    return NULL;
    
}

void pq_delete(PriorityQueue **q) {
    // fprintf(stderr, "deleting PQ\n");
    if((*q)->size_used != 0) {
        // run through the rest of the queue and delete every element
        while((*q)->size_used != 0) {
            // delete nodes in here
        }
        free((*q)->array);
        free(*q);
        *q = NULL;
        // fprintf(stderr, "deleting PQ\n");
    }
    else {
        // we should really only ever get here since we want to run until the queue is equal to 1 cuz thats when the tree is built
        // fprintf(stderr, "deleting PQ\n");
        free((*q)->array);
        free(*q);
        *q = NULL;

    }
}

bool pq_empty(PriorityQueue *q) {
    return q->size_used == 0;
}

bool pq_full(PriorityQueue *q) {
    return q->size_used == q->total_capacity;
}

uint32_t pq_size(PriorityQueue *q) {
    return q->size_used;
}




bool enqueue(PriorityQueue *q, Node *n) {
    if (q->size_used == q->total_capacity) {
        return false;
    }
    else {
        if(q->size_used == 0) {
            q->array[0] = n;
        }
        else {
            q->array[q->size_used] = n;
        }
        up_heap(q->array, q->size_used);
        q->size_used += 1;

        // uint32_t size = pq_size(q);
        // q->array[size - 1] = n;
        // insertion(q);
        return true;
    }

}

bool dequeue(PriorityQueue *q, Node **n) {
    if(q == NULL || q->size_used == 0) {
        return false;
    }
    else {
        *n = (q->array[0]);
        // Node * left = q->array[0]->left;
        // Node * right = q->array[0]->right;
        q->array[0] = q->array[q->size_used - 1];
        // q->array[0]->left = left;
        // q->array[0]->right = right;
        q->array[q->size_used-1] = NULL;
        q->size_used -= 1;
        // insertion(q);
        down_heap(q->array, q->size_used);
        // *n = q->array[0];
        // q->array[0] = q->array[q->size_used - 1];
        // q->size_used -= 1;
        return true;
    }

}

void pq_print(PriorityQueue *q) {
    for (uint32_t i = 0; i < q->size_used; i += 1) {
        node_print(q->array[i]);
    }
}
