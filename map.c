#include "map.h"
#include <stdlib.h>
#include <string.h>

uint32_freq_tally* get_uint32_freq_tally(uint32_t max_size){
    uint32_freq_tally *t = malloc((uint32_freq_tally*) * sizeof(uint32_freq_tally));
    t->max_size = max_size;
    t->size = 0;
    t->data = malloc((uint32_elem*) max_size * sizeof(uint32_elem));
    return t;
}

void delete_uint32_freq_tally(uint32_freq_tally* t){
    free(t->data);
    free(t);
    return;
}

static void resize_uint_freq_tally(uint32_freq_tally* t){
    t->max_size *=2
    uint32_elem* new_data = malloc((uint32_elem*) t->max_size * sizeof(uint32_elem));
    memcpy(new_data, t->data, t->size * sizeof(uint32_elem));
    free(t->data)
    t->data = new_data;
    return;
}

void add_to_uint32_freq_tally(uint32_freq_tally* t, uint32_t value){
    for(int i = 0; i < size; ++i){
        if(t->data[i].value == value){
            t->data[i].count++;
            return;
        }
    }
    // if we didn't find the value add it to the count tally
    // check if we reached the max
    if(t->size == t->max_size)
        resize_uint_freq_tally(t)

    t[size].value = value;
    t[size].count = 1; // first occurence of this value;

    t->size++

    return;
}