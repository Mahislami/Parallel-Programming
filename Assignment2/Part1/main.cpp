#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h> 
#include <sys/time.h>
#include <iomanip>
#include "time.h"
#include "x86intrin.h"


using namespace std;

int main(void) {
    printf("SIDs : 810195548 , 810195346\n\n");
    struct timeval start1, end1;
    struct timeval start2, end2;
    const  int res = pow(2,20);
    float *fArray;
	fArray = new float [res];
    for (int i=0;i<res;i++){
        fArray[i] = (float)rand();
    }

    // Serial implementation
    gettimeofday(&start1, NULL);
    int index;
    float max = fArray[0];   
    for (int j = 1; j < res; j++) 
        if (fArray[j] > max) {
            max = fArray[j];  
            index = j;
        }
    gettimeofday(&end1, NULL);
	long seconds1 = (end1.tv_sec - start1.tv_sec);
	long micros1 = ((seconds1 * 1000000) + end1.tv_usec) - (start1.tv_usec);

	// Parallel implementation
    gettimeofday(&start2, NULL);
    const __m128 increment = _mm_set1_ps(4);
    __m128 indexes         = _mm_setr_ps(0, 1, 2, 3);
    __m128 maxindexes      = indexes;
    __m128 maxvalues     = _mm_loadu_ps(fArray);
    for (int i=4; i <= res; i += 4) {

        indexes = _mm_add_ps(indexes, increment);
        const __m128 values        = _mm_loadu_ps(fArray + i);
        const __m128 gt            = _mm_cmpgt_ps(values, maxvalues);
        maxindexes = _mm_blendv_ps(maxindexes, indexes, gt);
        maxvalues  = _mm_max_ps(values, maxvalues);
    }
    __m128 values_fArray;
    __m128 indices_fArray;
    values_fArray = maxvalues;
    indices_fArray =  maxindexes;
    float  maxindex = indices_fArray[0];
    float maxvalue = values_fArray[0];
    for (int i=0; i < 4; i++) {
        if (values_fArray[i] > maxvalue) {
            maxvalue = values_fArray[i];
            maxindex = indices_fArray[i];
        } 
    }
    gettimeofday(&end2, NULL);
	long seconds2 = (end2.tv_sec - start2.tv_sec);
	long micros2 = ((seconds2 * 1000000) + end2.tv_usec) - (start2.tv_usec);

    cout << "Serial Result: " << max <<endl; 
    cout << "index: "<< index << endl;
    cout << "Serial Run time is " << seconds1  << " seconds and " << micros1 << " micro seconds\n\n";
    cout << "Parallel Result: "<< maxvalue << endl;
    cout << "index: " << fixed << setprecision(0) << maxindex << endl;
    cout << "Parallel Run time is " << seconds2  << " seconds and " << micros2 << " micro seconds\n\n";
    printf ("Speedup = %f\n\n", (float) (micros1)/(float) micros2);

}