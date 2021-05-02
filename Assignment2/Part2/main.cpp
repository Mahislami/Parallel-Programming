#include "stdio.h"
#include "stdlib.h"
#include "time.h"
#include "x86intrin.h"
#include "ipp.h"
#include "math.h"
#define NUM 1048576

Ipp64u p_start_1, p_end_1, p_start_2, p_end_2;
Ipp64u s_start_1, s_end_1, s_start_2, s_end_2;

void parallel(float* a)
{
    float result;
    __m128 vec;
    __m128 temp1;
    __m128 temp3;
    __m128 standard_deviation;
    p_start_1 = ippGetCpuClocks();
    __m128 sum = _mm_set1_ps(0.0f);
    __m128 count = _mm_set1_ps(float(NUM));
    __m128 mean = _mm_set1_ps(0.0f);
    for (long i = 0; i < NUM; i+=4) {
        vec = _mm_loadu_ps (&a[i]);
        sum = _mm_add_ps (sum, vec);
    }
    sum = _mm_hadd_ps (sum, sum);
    sum = _mm_hadd_ps (sum, sum);
    mean = _mm_div_ps(sum, count);
    result = _mm_cvtss_f32 (mean);
    p_end_1 = ippGetCpuClocks();
    printf("Parallel mean: %f\n", result);
    p_start_2 = ippGetCpuClocks();
    sum = _mm_set1_ps(0.0f);
    for (long i = 0; i < NUM; i+=4) {
        vec = _mm_loadu_ps (&a[i]);
        temp1 = _mm_sub_ps(vec, mean);
        temp1 = _mm_mul_ps(temp1, temp1);
        sum = _mm_add_ps(sum, temp1);
    }
    sum = _mm_hadd_ps (sum, sum);
    sum = _mm_hadd_ps (sum, sum);
    temp3 = _mm_div_ps(sum, count);
    standard_deviation = _mm_sqrt_ps(temp3);
    result = _mm_cvtss_f32(standard_deviation);
    p_end_2 = ippGetCpuClocks();
    printf("Parallel standard deviation: %f\n", result);
    return;
}

void serial(float* a)
{
    
    float sum = 0.00;
    float mean = 0.00;
    float standard_deviation = 0.00;
    float temp = 0.00;
    s_start_1 = ippGetCpuClocks();
    for(int i = 0; i < NUM; i++)
        sum += a[i];
    mean = sum / NUM;
    s_end_1 = ippGetCpuClocks();
    printf("Serial mean: %f\n", mean);

    s_start_2 = ippGetCpuClocks();
    for (int i = 0; i < NUM; ++i)
        temp += (a[i] - mean)*(a[i] - mean);
    standard_deviation =  sqrt(temp / NUM);
    s_end_2 = ippGetCpuClocks();
    printf("Serial standard deviation: %f\n", standard_deviation);
    return;
    
}

int main()
{
    srand(static_cast<unsigned int>(time(0)));
    __attribute__ ((aligned(16))) float a[NUM];
    for (int i = 0; i < NUM; i++)
        a[i] = (rand() % 100)/1000.0;
    printf("SIDs : 810195548 , 810195346\n");
    printf("----------------------------\n");
    //serial
    serial(a);
    //parallel
    parallel(a);
    //speed-up
    printf("Serial mean runtime: %d\n", (Ipp32s)(s_end_1 - s_start_1));
    printf("Serial standard deviation runtime: %d\n", (Ipp32s)(s_end_2 - s_start_2));
    printf("Parallel mean runtime: %d\n", (Ipp32u)(p_end_1 - p_start_1));
    printf("Parallel standard deviation runtime: %d\n", (Ipp32s)(p_end_2 - p_start_2));
    printf ("Mean speed-up = %f\n", (float) (s_end_1 - s_start_1)/(float) (p_end_1 - p_start_1));
    printf ("DS speed-up = %f\n", (float) (s_end_2 - s_start_2)/(float) (p_end_2 - p_start_2));
    return 0;
}
