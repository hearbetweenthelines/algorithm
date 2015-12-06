#ifndef __FFT_H__
#define __FFT_H__

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "../math/complex.h"

#ifndef PI
#define PI 3.1415926
#endif

complex *fft(double *a, int n);
double *ifft(complex *a, int n);

#endif