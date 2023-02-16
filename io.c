//clang-format off
#include "io.h"
#include "defines.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>

//clang-format on
uint64_t bytes_read;
uint64_t bytes_written;
uint8_t buffer[BLOCK];

int read_bytes(int infile, uint8_t *buf, int nbytes) {

    ssize_t actual_bytes_read = read(infile, buf, nbytes);
    ssize_t total_read = actual_bytes_read;
    while (actual_bytes_read > 0 && total_read < nbytes) {
        actual_bytes_read = read(infile, buf + total_read, nbytes - total_read);
        total_read += actual_bytes_read;
    }
    bytes_read += total_read;

    return total_read;




}

int write_bytes(int outfile, uint8_t *buf, int nbytes) {

    ssize_t actual_bytes_written = write(outfile, buf, nbytes);
    ssize_t total_written = actual_bytes_written;
    while (actual_bytes_written > 0 && total_written < nbytes) {
        actual_bytes_written = write(outfile, buf + total_written, nbytes - total_written);
        total_written += actual_bytes_written;
    }

    bytes_written += total_written;
    return total_written;
}

static uint64_t bit_offset = 0;
static uint8_t buffer_bit[BLOCK];
static uint64_t current_buffer_size;

bool read_bit(int infile, uint8_t *bit) {
    
    if (bit_offset == 0) {
        current_buffer_size = read_bytes(infile, buffer_bit, BLOCK);
    }
    if(current_buffer_size == 0) {
        return false;
    }

    if(bit_offset != BLOCK * 8) { // This might skip the last byte, I am not sure
        *bit = (buffer_bit[bit_offset / 8] >> bit_offset % 8) & 0x1;
        bit_offset += 1;
        if(bit_offset == BLOCK * 8) {
            bit_offset = 0;
        }
        
    }
    return true;

}

uint8_t write_buf[BLOCK];
static uint64_t buffer_index = 0;



void write_code(int outfile, Code *c) {


    // currently, what I believe this does:
    // we set the starting index
    uint32_t code_index = 0;
    // while the index is not out of bounds of the code
    while (code_index < c->top) {
        // first obtain the byte that stores the bit at code index
        uint8_t byte = code_get_bit(c, code_index);
        // then get the specific bit at code index
        if (byte == 1) {
            // if the bit is 1, then we want to left shift a 1 by the index to insert at modulo 8, 
            // and then bitwise OR it with the byte that has the bit that we are supposed to write to next in our buffer
            write_buf[buffer_index / 8] |= (1 << (buffer_index % 8));
        }
        else if (byte == 0){
            // if the bit is not 1, then we want to 0 out the bit at the index to write to 
            // we can do that by first left shifting a 1 by the index of the bit in the byte
            // then if we subtract 1, all bits lower than that will have a 1 in them, and every bit equal to or higher than the current bit will be 0
            // then if we bitwise AND it with the appropriate byte, it will ensure all lower bits are kept as the same value, and the bit we write to, as well as 
            // any "uninitialized" bits higher than the index we are writing to will all be set to 0
            write_buf[buffer_index / 8] &= ~(1 << (buffer_index % 8));
        }
        else {
            fprintf(stderr, "something is wrong with the bit: %u\n", byte);
        }
        
        // for each bit we write, we need to increment both my code index and the buffer index
        code_index += 1;
        buffer_index += 1;
        
        // if our buffer index is now equal to BLOCK * 8, then it is out of bounds and then buffer needs to be written and our buffer index should be reset
        if(buffer_index == BLOCK * 8) {
            write_bytes(outfile, write_buf, BLOCK);
            buffer_index = 0;
        }
        
    }



}


void flush_codes(int outfile) {

    uint64_t write_index = buffer_index;
    // this is implementation given by TA Nishant
    if (write_index > 0) {
        if (write_index % 8 == 0) {
            write_bytes(outfile, write_buf, write_index / 8);
        }
        else {
            write_bytes(outfile, write_buf, (write_index / 8) + 1);
        }
    }
}
