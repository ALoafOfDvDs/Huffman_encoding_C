//clang-format off
#include "huffman.h"
#include "pq.h"
#include "stack.h"
#include "defines.h"
#include "header.h"
#include "node.h"
#include "code.h"
#include "io.h"

#include <unistd.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <inttypes.h>
#include <ctype.h>
// #include <>
//clang-format on

#define OPTIONS "hi:o:v"

// uint64_t bytes_read;
// uint64_t bytes_written;

void spaces(int space) {
    int counter = 0;
    while(counter < space) {
        fprintf(stderr, " ");
        fflush(stderr);
        counter += 1;
    }
}
// this print tree function was given by Prof. Long in discord and was used for debugging
void printTree(Node *t, int depth) {
    // return;
    // printf("%p\n", (void *) t);
    if (t) {
        printTree(t->left, depth + 1);
        spaces(4 * depth);
        if (t->symbol != '$') {
            if (isgraph(t->symbol)) {
                fprintf(stderr, "'%c' (%" PRIu64 ")\n", t->symbol, t->frequency);
            } else {
                fprintf(stderr, "0x%02X (%" PRIu64 ")\n", t->symbol, t->frequency);
            }
        } else {
            fprintf(stderr, "$ (%" PRIu64 ")\n", t->frequency);
        }
        printTree(t->right, depth + 1);
    }
    return;
}

int main(int argc, char **argv) {

    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;
    bool stats = false;

    int opt = 0;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
            case 'h': 
                fprintf(stderr,"%s\n%s\n%s\n\n%s\n%s\n\n%s\n%s\n%s\n%s\n%s\n", "SYNOPSIS", "  A Huffman decoder.", "  Decompresses a file using the Huffman coding algorithm.",
                "USAGE", "  ./decode [-h] [-i infile] [-o outfile]",
                "OPTIONS", "  -h             Program usage and help.", "  -v             Print compression statistics.", "  -i infile      Input file to decompress.", 
                "  -o outfile     Output of decompressed data.");
                return 0;
                break;
            case 'i':
                // change input
                input_fd = open(optarg, O_RDONLY);
                break;
            case 'o':
                output_fd = open(optarg, O_WRONLY | O_CREAT);
                break;
            case 'v':
                stats = true;
                break;
            case '?':
                fprintf(stderr,"%s\n%s\n%s\n\n%s\n%s\n\n%s\n%s\n%s\n%s\n%s\n", "SYNOPSIS", "  A Huffman decoder.", "  Decompresses a file using the Huffman coding algorithm.",
                "USAGE", "  ./decode [-h] [-i infile] [-o outfile]",
                "OPTIONS", "  -h             Program usage and help.", "  -v             Print compression statistics.", "  -i infile      Input file to decompress.", 
                "  -o outfile     Output of decompressed data.");
                return 1;
                break;
        }
    }

    

    // I have to first read in the header somehow
    bytes_read = 0;
    Header header;
    read_bytes(input_fd, ((uint8_t*) &header), sizeof(Header));

    // check the magic number
    // need to do this before we start reading the tree because this is what confirms it is in fact a tree WE dumped
    if(header.magic != MAGIC) {
        fprintf(stderr, "Invalid magic number.\n");
        fprintf(stderr, "%x\n", header.magic);
        return 1;
    }

    // I then have to set the permissions for outfile
    struct stat permissions;
    fstat(input_fd, &permissions);
    fchmod(output_fd, header.permissions);

    

    uint8_t tree_array[header.tree_size];
    read_bytes(input_fd, tree_array, header.tree_size);

    Node * root = rebuild_tree(header.tree_size, tree_array);
    // fprintf(stderr, "looping here\n");


    // printTree(root, 0);

    uint64_t num_decoded = 0;
    Node * current_node = root;
    
    // uint8_t symbol;
    // uint32_t byte_counter = 0;
    uint8_t bit;
    while(num_decoded < header.file_size) {
        
        if(read_bit(input_fd, &bit)) {
            // fprintf(stderr, "bit: %u\nprinting node: \n", bit);
            // node_print(current_node);
            // fprintf(stderr, "%u ", bit);
            if (current_node->left == NULL && current_node->right == NULL) {
                // symbol = current_node->symbol;
                write_bytes(output_fd, &current_node->symbol, 1);
                current_node = root;
                num_decoded += 1;
                // fprintf(stderr, "\n");
            }
            if(bit == 0) {
                // fprintf(stderr, "moving left down tree\n");
                current_node = current_node->left;
            }
            else if (bit == 1){
                // fprintf(stderr, "moving right down tree\n");
                current_node = current_node->right;
            }
        }
        // else {
        //     break;
        // }
        
    }


    float val = ((float)bytes_read)/bytes_written;
    float space = 100 * (1 - val);

    if(stats) {
        fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_read);
        fprintf(stderr, "Decompressed file size: %lu bytes\n", bytes_written);
        fprintf(stderr, "Space saving: %.2f%%\n", space);
    }

    delete_tree(&root);
    close(input_fd);
    close(output_fd);
    return 0;


}
