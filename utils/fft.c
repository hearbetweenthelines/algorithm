#include <string.h>
#include "fft.h"

void bit_reverse_copy(int n, double *a, complex *A)
{
    int bits = log(n) / log(2);
    for (int i = 0; i < n; i++)
    {
        int t = i, w = bits;
        int ans = 0;
        while (w > 0)
        {
            int k = t & 1;
            t     = t >> 1;
            w--;
            k = k << w;
            ans |= k;
        }
        complex_set(a[ans], 0, &A[i]);
    }
}

complex *fft(double *a, int n)
{
    complex *A = (complex *)malloc(sizeof(complex) * n);
    bit_reverse_copy(n, a, A);
    int lgn = (int)(log(n) / log(2));
    for (int i = 1; i <= lgn; i++)
    {
        int m        = 1 << i;
        double index = -2 * PI / m;
        complex omiga;
        complex_set(cos(index), sin(index), &omiga);
        for (int k = 0; k < n; k += m)
        {
            complex w;
            complex_set(1, 0, &w);
            for (int j = 0; j < m / 2; j++)
            {
                complex t        = mult(&w, &A[k + j + m / 2]);
                complex u        = A[k + j];
                A[k + j]         = add(&u, &t);
                A[k + j + m / 2] = minus(&u, &t);
                w                = mult(&w, &omiga);
            }
        }
    }
    return A;
}

void ifft_bit_reverse_copy(int n, complex *a, complex *A)
{
    int bits = log(n) / log(2);
    for (int i = 0; i < n; i++)
    {
        int t = i, w = bits;
        int ans = 0;
        while (w > 0)
        {
            int k = t & 1;
            t     = t >> 1;
            w--;
            k = k << w;
            ans |= k;
        }
        A[i] = a[ans];
    }
}


double *ifft(complex *a, int n)
{
    complex *A = (complex *)malloc(sizeof(complex) * n);
    ifft_bit_reverse_copy(n, a, A);
    int lgn = (int)(log(n) / log(2));
    for (int i = 1; i <= lgn; i++)
    {
        int m        = 1 << i;
        double index = 2 * PI / m;
        complex omiga;
        complex_set(cos(index), sin(index), &omiga);
        for (int k = 0; k < n; k += m)
        {
            complex w;
            complex_set(1, 0, &w);
            for (int j = 0; j < m / 2; j++)
            {
                complex t        = mult(&w, &A[k + j + m / 2]);
                complex u        = A[k + j];
                A[k + j]         = add(&u, &t);
                A[k + j + m / 2] = minus(&u, &t);
                w                = mult(&w, &omiga);
            }
        }
    }
    double *ans = (double *)malloc(sizeof(double) * n);
    for (int i = 0; i < n; i++)
    {
        ans[i] = A[i].real / n;
    }
    return ans;
}