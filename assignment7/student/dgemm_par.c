#include "dgemm.h"
#include <immintrin.h>
#include <inttypes.h>

void dgemm(float *a, float *b, float *c, int n) {
    int rem = n % 8;
    int n_8 = n-rem;
    float result [8];

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n_8; k+=8) {
                /* c[i * n + j] += a[i * n  + k] * b[j * n  + k]; */
                __m256 a_i = _mm256_loadu_ps(&a[i * n + k]);
                __m256 b_i = _mm256_loadu_ps(&b[j * n + k]);

                __m256 mul =  _mm256_mul_ps(a_i, b_i);
                __m256 mul_p = _mm256_permute2f128_ps(mul, mul, 1);

                __m256 sum = _mm256_add_ps(mul, mul_p);
                sum = _mm256_hadd_ps (sum, sum);
                sum = _mm256_hadd_ps (sum, sum);

                _mm256_store_ps(result, sum);
                c[i * n + j] += result[0];
            }

            for (int k = n_8; k < n; k++) {
                c[i * n + j] += a[i * n + k] * b[j * n + k];
            }
        }
    }
}
