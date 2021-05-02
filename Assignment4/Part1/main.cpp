#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h> 
#include <sys/time.h>
#include <iomanip>
#include "time.h"
#include "x86intrin.h"
#include "omp.h"


using namespace std;

int main(void) {
    printf("SIDs : 810195548 , 810195346\n\n");
    double start, end;
	double time1, time2;

    const  int res = pow(2,20);
    float *fArray;
	fArray = new float [res];
    for (int i=0;i<res;i++){
		fArray[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100.0));
    }

    // Serial implementation
	start = omp_get_wtime();
    int index;
    float max = fArray[0];   
    for (int j = 1; j < res; j++) 
        if (fArray[j] > max) {
            max = fArray[j];  
            index = j;
        }
	end  = omp_get_wtime();
	time1 = end - start;

	// openmp implementation
    start = omp_get_wtime();
    int index2;
    float max2 = fArray[0];   
    #pragma omp parallel for simd num_threads(2) 
        for (int j = 1; j < res; j++)
            if (fArray[j] > max2) {
                max2 = fArray[j];  
                index2 = j;
            }
	end  = omp_get_wtime();
	time2 = end - start;

    cout << "Serial Result: " << max <<endl; 
    cout << "index: "<< index << endl;
    cout << "Parallel Result: "<< max << endl;
    cout << "index: " << index  << endl;
    printf ("\nSerial Run time = %f seconds\n", (float) time1);
	printf ("OpenMP Run time = %f seconds\n", (float) time2);
	printf ("\nSpeedup(OpenMP vs Serial)  = %4.2f\n", (double) (time1)/(double) time2);

}