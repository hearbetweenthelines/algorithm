#include <string.h>
#include "complex.h"

void complex_set(double real, double imag, complex *a)
{
    a->real = real;
    a->imag = imag;
}
void complex_set_exp(double x, complex *a)
{
    a->real = cos(x);
    a->imag = sin(x);
}
complex add(const complex *a, const complex *b)
{
    complex c;
    c.real = a->real + b->real;
    c.imag = a->imag + b->imag;
    return c;
}
complex minus(const complex *a, const complex *b)
{
    complex c;
    c.real = a->real - b->real;
    c.imag = a->imag - b->imag;
    return c;
}
complex mult(const complex *a, const complex *b)
{
    complex c;
    c.real = a->real * b->real - a->imag * b->imag;
    c.imag = a->imag * b->real + a->real * b->imag;
    return c;
}
complex conjugate(const complex *a)
{
    complex c;
    c.real = a->real;
    c.imag = -a->imag;
    return c;
}
char *complex_toString(const complex *a)
{
    char *s1 = (char *)malloc(50 * sizeof(char));
    char *s2 = (char *)malloc(50 * sizeof(char));
    char *s3 = (char *)malloc(100 * sizeof(char));
    sprintf(s1, "%.16f", a->real);
    sprintf(s2, "%.16fi", a->imag);
    if (a->real == 0)
        return s2;
    else if (a->imag == 0)
        return s1;

    if (a->imag < 0)
        sprintf(s3, "%s%s", s1, s2);
    else
        sprintf(s3, "%s+%s", s1, s2);
    return s3;
}
