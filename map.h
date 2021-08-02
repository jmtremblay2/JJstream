#include <ctype.h>

typedef struct uint32_elem{
    uint32_t value;
    uint32_t count;
} uint32_elem;

typedef struct uint32_freq_tally{
    uint32_t max_size;
    uint32_t size;
    uint32_elem* data;
} uint32_freq_tally;

uint32_freq_tally* get_uint32_freq_tally(uint32_t max_size);
void delete_uint32_freq_tally(uint32_freq_tally* t);
void add_to_uint32_freq_tally(uint32_freq_tally* t, uint32_t value);
