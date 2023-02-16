//clang-format off
#include "code.h"
#include "defines.h"

#include <stdlib.h>
#include <stdio.h>
//clang-format on


Code code_init(void) { // Note that due to my implementation of this I will need to call free(&CodeVar) on this later whenever i declare it
    Code code; 
    code.top = 0;
    for (uint32_t i = 0; i < MAX_CODE_SIZE; i += 1) {
        code.bits[i] = 0;
    }
    return code;
}

uint32_t code_size(Code *c) {
    return c->top;
}

bool code_empty(Code *c) {
    return c->top == 0;
}

bool code_full(Code *c) {
    return c->top == MAX_CODE_SIZE;
}

bool code_set_bit(Code *c, uint32_t i) {
    if (i >= 0 && i < c->top) {
        uint8_t val = (1 << (i % 8));
        c->bits[i / 8] |= val;
        return true;
    }
    else {
        return false;
    }
}

bool code_clr_bit(Code *c, uint32_t i) {
    if (i >= 0 && i < c->top) {

        // new code courtesy of Lev

        c->bits[i] &= 0x0;

        return true;
    }
    else {
        return false;
    }
}

bool code_get_bit(Code *c, uint32_t i) {
    if (i >= 0 && i < c->top) {
        return c->bits[i] & 0x1; 
    }
    else {
        return false;
    }
}

// The implementation for push bit and pop bit were advised by tutor Lev

bool code_push_bit(Code *c, uint8_t bit) {
    if(code_full(c)) {
        return false;
    }
    else {
        c->top += 1;
        c->bits[c->top - 1] = bit;
        return true;
    }
    

}

bool code_pop_bit(Code *c, uint8_t *bit) {
    // assuming this is in fact supposed to be implemented as a stack as it is said in the pdf "a stack of bits", we pop from the top
    if(code_empty(c)) {
        return false;
    }
    else {
        uint8_t index_to_pop = c->top - 1;
        bool bit_val = c->bits[index_to_pop];
        if(bit_val) {
            *bit = 1;
        }
        else {
            *bit = 0;
        }
        bool clear_status = code_clr_bit(c, index_to_pop);
        if(!clear_status) {
            fprintf(stderr, "something is wrong with clear bit or pop bit in code.c\n"); // debugging statement that should never get hit. 
        }
        c->top -= 1;
        return true;
    }
}

void code_print(Code *c) {
    for (uint32_t i = 0; i < MAX_CODE_SIZE && c->bits[i]; i += 1) {
        fprintf(stderr, "bit %u:\n", i);
        for(uint16_t j = 0; j < c->top; j += 1) {
            fprintf(stderr, "%u", (((c->bits[i / 8]) & (1 << (c->top - j))) >> (c->top - j)));
        }

        fprintf(stderr, "\n\n");
    }
}
