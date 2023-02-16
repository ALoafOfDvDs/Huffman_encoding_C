NOTES:
    There were several functions in which code or pseudocode was given by TAs or tutors. Some functions include: flush_codes(), code_push_bit(), code_pop_bit(), read_bytes(), write_bytes()
    All new function calls such as open() or lseek() were learned from the documentation on man pages
    
    scan-build reports no errors. 
    
encode.c
    This is the file contianing the main function for encoding a file to compress it, and is the file that is compiled into the encode binary
    Its options include:
        -h prints out the help message, displaying usage, synopsis, and lists commandline options and how to use them
        -i <infile> sets the input file to infile
        -o <outfile> sets the output file to outfile, creating it if it does not exist already
        -v enables printing of compression statistics to stderr
    This file takes an input, either from standard in, or an input specified by -i <infile>, and will first build a histogram to track the number of occurences of each ASCII character. It uses this data to build a huffman tree. It also creates a header to contain file size, permissions, and other identifying information. The header is written to either standard out, or the file specified by -o <outfile>. The tree is then dumped to this same output. Finally using the tree, a code is built for each character that appears in the input, and then that code is used to encode character by character the input file, and is sent to the output. 
    It should be noted that statistics are always printed to standard error, and do not impede the functionality of this program
    
decode.c
    This is the file containing the main function for decoding a compressed file, and is the file compiled into decode binary
    Its options include:
        -h prints out the help message, displaying usage, synopsis, and lists commandline options and how to use them
        -i <infile> sets the input file to infile
        -o <outfile> sets the output file to outfile, creating it if it does not exist already
        -v enables printing of compression statistics to stderr
    This file takes an input, either from standard in, or an input specified by -i <infile>. This program first checks the header to know what permissions outfile should have, the size the uncompressed file should be, and ensures it is a valid file to decrypt. Once that has been checked, it will then read through the dumped tree and reconstruct the huffman tree originally built by the encoder. The file is then read through bit by bit, and for each bit, the decoder will travel down the reconstructed tree. Each time it hits a leaf node on the tree, the character associated with that node is written to the outfile. This continues until the number of characters written to the output matches the size of the original file (specified in the header), or until there are no more bits that can be read from the infile. 

code.c
    Contains the implementation for the code ADT. This functions similarly to a stack, where each index is a bit. A code is associated with each character when encoding.

code.h
    The header for code.c. Required for compilation of the binaries for its include statements.

huffman.c
    This file contains implementations of functions that handle everything to do with the huffman trees. Building them, dumping them, rebuilding them, deleting them, and creating the codes based on the tree.

huffman.h
    This is the header for huffman.c. This is required for compilation of the code into the respective binaries. 

io.c
    This file contains implementations for writing to and reading from files. It also handles tracking the stats for file size, compressed and uncompressed. 

io.h
    This is the header file for io.c. It is required for compilation of the code, for handling extern variables, and certain include statements. 

pq.c
    This file contains the implementation for the priority queue ADT. It is sorted as a min-heap using heapsort. 
pq.h
    This is the header file for pq.c. It is required for compilation. 

stack.c
    This contains the implementation of the stack ADT. 

stack.h
    This is the header file for stack.c. It is required for compilation. 

node.c
    This is the implementation for the nodes. This is needed to help build the trees. 

node.h
    This is the header file for node.c. It is required for compilation. 

defines.h
    This file is needed for compilation. It defines specific values that are used throughout other files. 

header.h
    This file defines the header struct and is needed for compilation. 

Makefile
    The Makefile handles the compilation of .c files into .o files, and compiles the .o files into the various binaries
    Its options include
        make : makes both the encode and decode binaries
        make all : makes both the encode and decode binaries
        make encode : compiles only the encode binary
        make decode : compiles only the decode binary
        make clean : removes the .o files if they exist
        make spotless : removes all .o files and the decode and encode binaries if they exist
        make format : formats all .c files for readability
