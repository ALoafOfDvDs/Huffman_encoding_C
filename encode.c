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
#include <sys/types.h>
#include <ctype.h>
#include <inttypes.h>
#include <fcntl.h>
// #include <>

//clang-format on
#define OPTIONS "hi:o:v"


extern uint64_t buffer_index;
// The print tree function was written by Prof. Long
void spaces(int space) {
    int counter = 0;
    while(counter < space) {
        fprintf(stderr, " ");
        fflush(stderr);
        counter += 1;
    }
}

void printTree(Node *t, int depth) {
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
                fprintf(stderr, "%s\n%s\n%s\n\n%s\n%s\n\n%s\n%s\n%s\n%s\n%s\n", "SYNOPSIS", "  A Huffman encoder.", "  Compresses a file using the Huffman coding algorithm.",
                "USAGE", "  ./encode [-h] [-i infile] [-o outfile]", "OPTIONS",
                "  -h             Program usage and help.", "  -v             Print compression statistics.", "  -i infile      Input file to compress.",
                "  -o outfile     Output of compressed data.");
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
                fprintf(stderr, "%s\n%s\n%s\n\n%s\n%s\n\n%s\n%s\n%s\n%s\n%s\n", "SYNOPSIS", "  A Huffman encoder.", "  Compresses a file using the Huffman coding algorithm.",
                "USAGE", "  ./encode [-h] [-i infile] [-o outfile]", "OPTIONS",
                "  -h             Program usage and help.", "  -v             Print compression statistics.", "  -i infile      Input file to compress.",
                "  -o outfile     Output of compressed data.");
                return 1;
                break;
        }
    }


    FILE * temp = tmpfile();

    int temp_fd = fileno(temp);
    static uint8_t letter;
    while (read_bytes(input_fd, &letter, 1) ) {
        write_bytes(temp_fd, &letter, 1);
    }
    lseek(temp_fd, 0, SEEK_SET);

    // I have to handle files like this since I start with stdin and stdout and those are already FILE * datatypes


    // construct the histogram
    
    uint64_t hist[ALPHABET];
    for (uint16_t i = 0; i < ALPHABET; i += 1) {
        hist[i] = 0;
    }
    uint8_t character;


    while(read_bytes(temp_fd, &character, 1)) {
        hist[character] += 1;
    }
    if(hist[0] == 0) {
        hist[0] = 1;
    }
    if(hist[1] == 0) {
        hist[1] = 1;
    }
    lseek(temp_fd, 0, SEEK_SET);
    Node * root = build_tree(hist);


    Code c[ALPHABET] = {0};
    build_codes(root, c); 

    // handling file permissions and other things
    Header header;
    header.magic = 0xBEEFBBAD;
    struct stat file_stats;
    fstat(temp_fd, &file_stats);
    mode_t permissions = file_stats.st_mode; // This is the set of file permissions
    header.permissions = permissions;
    fchmod(output_fd, permissions);

    // calculate number of unique symbols:
    uint64_t uniq_sym = 0;
    for (uint16_t i = 0; i < ALPHABET; i += 1) {
        if(hist[i]) {
            uniq_sym += 1;
        }
    }
    header.tree_size = (3 * uniq_sym) - 1;
    header.file_size = file_stats.st_size;
    bytes_written = 0;
    write_bytes(output_fd, (uint8_t *)&header, sizeof(Header));

    dump_tree(output_fd, root);

    int amount_read;
    uint8_t buffer[BLOCK];
    bytes_read = 0;
    while((amount_read = read_bytes(temp_fd, buffer, BLOCK)) > 0) {
        for(int i = 0; i < amount_read; i += 1) {
            write_code(output_fd, &(c[buffer[i]]));
        }
    }
    flush_codes(output_fd);

    float val = ((float)bytes_written)/bytes_read;
    float space = 100 * (1 - val);

    if(stats) {
        fprintf(stderr, "Uncompressed file size: %lu bytes\n", bytes_read);
        fprintf(stderr, "Compressed file size: %lu bytes\n", bytes_written);
        fprintf(stderr, "Space saving: %.2f%%\n", space);
    }
    fclose(temp);
    close(input_fd);
    close(output_fd);
    delete_tree(&root);

    return 0;
}
