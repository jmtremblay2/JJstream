#include "utils.h"

unsigned int charptr_to_unsigned_int_big_endian(char* c){
    unsigned int x;
    char* x_char = (char*) &x;
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