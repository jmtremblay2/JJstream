#include "utils.h"

uint32_t uint8_ptr_to_uint32_big_endian(uint8_t* c){
    uint32_t x;
    uint8_t* x_char = (uint8_t*) &x;
    x_char[3] = c[0];
    x_char[2] = c[1];
    x_char[1] = c[2];
    x_char[0] = c[3];
    return x;
}

int check_equal(double x1, double x2){
    double tol = 0.000000001;
    if ((x1 - x2) < -tol || (x1 - x2) > tol)
        return 0;
    return 1;
}