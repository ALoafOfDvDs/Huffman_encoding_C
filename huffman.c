#include "huffman.h"
#include "pq.h"
#include "stack.h"
#include "io.h"

#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>

// static void spaces(int space) {
//     int counter = 0;
//     while(counter < space) {
//         printf(" ");
//         // fflush(stdout);
//         counter += 1;
//     }
// }

// static void printTree(Node *t, int depth) {
//     // return;
//     // printf("%p\n", (void *) t);
//     if (t) {
//         printTree(t->left, depth + 1);
//         spaces(4 * depth);
//         if (t->symbol != '$') {
//             if (isgraph(t->symbol)) {
//                 fprintf(stderr, "'%c' (%" PRIu64 ")\n", t->symbol, t->frequency);
//             } else {
//                 fprintf(stderr, "0x%02X (%" PRIu64 ")\n", t->symbol, t->frequency);
//             }
//         } else {
//             fprintf(stderr, "$ (%" PRIu64 ")\n", t->frequency);
//         }
//         printTree(t->right, depth + 1);
//     }
//     return;
// }

Node *build_tree(uint64_t hist[static ALPHABET]) {
    // I would think that the capacity of the priority queue would be some range in terms of ALPHABET
    // nvm there's literally a variable for it
    PriorityQueue * q = pq_create(MAX_TREE_SIZE);
    for (uint16_t i = 0; i < ALPHABET; i += 1) {
        if(hist[i] > 0) {    
            Node * n = node_create(i, hist[i]);
            // node_print(n);
            bool check = enqueue(q, n);
            if(!check) {
                fprintf(stderr, "queue ran out of space\n");
            }
        }

    }
    // printf("Print queue: \n");
    // pq_print(q);
    while(pq_size(q) > 1) {
        // printf("looping inside of size\n");
        // printf("Print queue: \n");
        // pq_print(q);
        Node * l;
        Node * r;
        bool check_l = dequeue(q, &l);
        bool check_r = dequeue(q, &r);
        if (!check_l || !check_r) {
            fprintf(stderr, "dequeue failed calling from build tree\n");
        }
        Node * p = node_join(l, r);

        // printf("children: \n");
        // node_print(l);
        // node_print(r);
        // printf("parent \n");
        // node_print(p);
        // printf("tree:\n");
        // printTree(p, 0);

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
            // pq_delete(&q);
            return root;
        }
    }
    // pq_delete(&q);
    return NULL;
    

}

// I think I need some sort of depth first searching helper function
// I think this might at this point build the codes properly?
// static void dfs_helper(Node * root, Code table[static ALPHABET], Code* code) {
//     if (root == NULL) {
//         return;
//     }
//     if(root->left == NULL && root->right == NULL) {
//         uint8_t c = root->symbol;
//         // fprintf(stderr, "symbol: %c\n", root->symbol);
//         // code_print(code);
//         // fprintf(stderr, "\n\n");
//         table[c] = *code; //I think this should copy by value and not by reference, and so if I change code later it should not change what I put into the table at any one index
//         return;
//     }
//     else {
//         uint8_t bit_popped;
//         if(root->left) { //I was having issues with the bit stack not being handled properly, and either the issue is here or in code.c
//             code_push_bit(code, 0);
//             dfs_helper(root->left, table, code);
//             code_pop_bit(code, &bit_popped);
//         }
//         if(root->right) {
//             code_push_bit(code, 1);
//             dfs_helper(root->right, table, code);
//             code_pop_bit(code, &bit_popped);
//         }
//     }
// }


bool code_initalized = false;
Code letter_code = {0};

void build_codes(Node *root, Code table[static ALPHABET]) {
    // I know I need this for creating the codes for each letter
    if(!code_initalized) {
        letter_code = code_init();
        code_initalized = true;
    }
    // this recursive method has not been working
    // dfs_helper(root, table, &letter_code);
    if (root == NULL) {
        return;
    }
    else {
        if(root->left == NULL && root->right == NULL) {
            uint8_t c = root->symbol;
            // fprintf(stderr, "symbol: %c\n", root->symbol);
            // code_print(code);
            // fprintf(stderr, "\n\n");
            table[c] = letter_code; //I think this should copy by value and not by reference, and so if I change code later it should not change what I put into the table at any one index
            return;
        }
        else {
            uint8_t bit_popped;
            // if(root->left) { //I was having issues with the bit stack not being handled properly, and either the issue is here or in code.c
            code_push_bit(&letter_code, 0);
            build_codes(root->left, table);
            code_pop_bit(&letter_code, &bit_popped);
            // }
            // if(root->right) {
            code_push_bit(&letter_code, 1);
            build_codes(root->right, table);
            code_pop_bit(&letter_code, &bit_popped);
            // }
        }
    }

}

void dump_tree(int outfile, Node *root) {
    if(root) {
        dump_tree(outfile, root->left);
        dump_tree(outfile, root->right);
        if(root->left || root->right) {
            // we would be an interior node
            // dprintf(outfile, "I");
            uint8_t i = 'I';
            write_bytes(outfile, &(i), 1);
        }
        else {
            // we are not an interior node
            // dprintf(outfile, "L%c", root->symbol);
            uint8_t l = 'L';
            uint8_t sym = root->symbol;
            write_bytes(outfile, &(l), 1);
            write_bytes(outfile, &(sym), 1);
        }
        
    }
}

Node *rebuild_tree(uint16_t nbytes, uint8_t tree[static nbytes]) {
    // debugging statement written by tutor Albert
    // for (uint16_t i = 0; i < nbytes; i += 1) {
    //     if (isgraph(tree[i])) {
    //         fprintf(stderr, "%c", tree[i]);
    //     } 
    //     else {
    //         fprintf(stderr, "0x%02X", tree[i]);
    //     }

    // }
    // fprintf(stderr, "\n");
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
    // fprintf(stderr, "elements in stack for rebuilding tree: %u\n", stack_size(s));
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
