#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct
{
    double real;
    double imag;
} complex;

void complex_set(double real, double imag, complex *a);
void complex_set_exp(double x, complex *a);
complex add(const complex *a, const complex *b);
complex minus(const complex *a, const complex *b);
complex mult(const complex *a, const complex *b);
complex conjugate(const complex *a);
char *complex_toString(const complex *a);

#endif