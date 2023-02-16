#include "huffman.h"
#include "pq.h"
#include "stack.h"
#include "io.h"

#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>


Node *build_tree(uint64_t hist[static ALPHABET]) {
    // I would think that the capacity of the priority queue would be some range in terms of ALPHABET
    // nvm there's literally a variable for it
    PriorityQueue * q = pq_create(MAX_TREE_SIZE);
    for (uint16_t i = 0; i < ALPHABET; i += 1) {
        if(hist[i] > 0) {    
            Node * n = node_create(i, hist[i]);
            bool check = enqueue(q, n);
            if(!check) {
                fprintf(stderr, "queue ran out of space\n");
            }
        }

    }
    while(pq_size(q) > 1) {
        Node * l;
        Node * r;
        bool check_l = dequeue(q, &l);
        bool check_r = dequeue(q, &r);
        if (!check_l || !check_r) {
            fprintf(stderr, "dequeue failed calling from build tree\n");
        }
        Node * p = node_join(l, r);

        enqueue(q, p);

    }
    if (pq_size(q) != 1) {
        fprintf(stderr, "should be leaving queue with one element, but %u elements present in queue\n", pq_size(q));
    }
    else {
        Node * root;
        bool check_root = dequeue(q, &root);
        pq_delete(&q);
        if(!check_root) {
            fprintf(stderr, "priority queue failed to dequeue root");
        }
        else {
            return root;
        }
    }
    return NULL;
    

}


bool code_initalized = false;
Code letter_code = {0};

void build_codes(Node *root, Code table[static ALPHABET]) {
    // I know I need this for creating the codes for each letter
    if(!code_initalized) {
        letter_code = code_init();
        code_initalized = true;
    }
    if (root == NULL) {
        return;
    }
    else {
        if(root->left == NULL && root->right == NULL) {
            uint8_t c = root->symbol;
            table[c] = letter_code; //I think this should copy by value and not by reference, and so if I change code later it should not change what I put into the table at any one index
            return;
        }
        else {
            uint8_t bit_popped;

            code_push_bit(&letter_code, 0);
            build_codes(root->left, table);
            code_pop_bit(&letter_code, &bit_popped);
            code_push_bit(&letter_code, 1);
            build_codes(root->right, table);
            code_pop_bit(&letter_code, &bit_popped);
        }
    }

}

void dump_tree(int outfile, Node *root) {
    if(root) {
        dump_tree(outfile, root->left);
        dump_tree(outfile, root->right);
        if(root->left || root->right) {
            // we would be an interior node
            uint8_t i = 'I';
            write_bytes(outfile, &(i), 1);
        }
        else {
            // we are not an interior node
            uint8_t l = 'L';
            uint8_t sym = root->symbol;
            write_bytes(outfile, &(l), 1);
            write_bytes(outfile, &(sym), 1);
        }
        
    }
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    uint64_t counter = 0;
    Stack * s = stack_create(nbytes);
    while (counter < nbytes) {
        if(tree[counter] == 'L') {
            Node * n = node_create(tree[counter + 1], 1);
            stack_push(s, n);
            // moves 
            counter += 1;
        }
        else if(tree[counter]=='I') {
            Node * r;
            bool right = stack_pop(s, &r);
            Node *l;
            bool left = stack_pop(s, &l);
            if (!left || !right) {
                fprintf(stderr, "left or right stack pop is failing\n");
            }
            Node * p = node_join(l, r);
            stack_push(s, p);
        }
        counter += 1;

    }
    // At this point we should in theory have exactly one element in the stack

    Node * root;
    stack_pop(s, &root);

    stack_delete(&s);
    return root;

}

void delete_tree(Node **root) {
    if(*root) {
        delete_tree(&(*root)->left);
        delete_tree(&(*root)->right);
        node_delete(root);
    }
}
