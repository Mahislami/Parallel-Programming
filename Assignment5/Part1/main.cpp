#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <math.h> 
#include <iostream>
#include <sys/time.h>
#include <ipp.h>



#define THREAD_COUNT 4
const  int ARRAY_SIZE = pow(2,20);
using namespace std;

struct ThreadParameters
{
    float* array;
    int start;
    int end;
    float largest;
	int index1;
};

void* find_max(void* args)
{
    struct ThreadParameters* params = (struct ThreadParameters*)args;
    float *array = params->array;
    int start = params->start;
    int end = params->end;
	int index = params -> index1;
    float largest = array[start];

    for (int i = start; i < end; i++)
    {
        if (array[i] > largest)
        {
            largest = array[i];
			index = i ;
        }
    }
    params->largest = largest;
	params -> index1 = index;
    return NULL;
}

int main()
{
    printf("SIDs : 810195548 , 810195346\n");
    printf("----------------------------\n");

    float arr[ARRAY_SIZE];
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
		arr[i] = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/100.0));
    }
    Ipp64u start, end;

    // Serial implementation
    start = ippGetCpuClocks();

	int index;
    float max = arr[0];   
    for (int j = 1; j < ARRAY_SIZE; j++) 
	if (arr[j] > max) {
		max = arr[j];  
		index = j;
	}

  	end  = ippGetCpuClocks();
	int serial_time = (Ipp32u)(end - start);

	// POSIX implementation
    start = ippGetCpuClocks();

    float largest;
	int indexposix;
    pthread_t threads[THREAD_COUNT];
    struct ThreadParameters thread_parameters[THREAD_COUNT];
    largest = arr[0];
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        thread_parameters[i].array = arr;
        thread_parameters[i].start = i * (ARRAY_SIZE / THREAD_COUNT);
        thread_parameters[i].end = (i+1) * (ARRAY_SIZE / THREAD_COUNT);
        thread_parameters[i].largest = 0;
        pthread_create(&threads[i], NULL, find_max, &thread_parameters[i]);
    }
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        pthread_join(threads[i], NULL);
    }
    for (int i = 0; i < THREAD_COUNT; i++)
    {
        if (thread_parameters[i].largest > largest)
        {
            largest = thread_parameters[i].largest;
			indexposix = thread_parameters[i].index1;
        }
    }

	end  = ippGetCpuClocks();
	int parallel_time_quicksort = (Ipp32u)(end - start);

    cout << "Serial Result: " << max <<endl; 
    cout << "index: "<< index << endl;
    cout << "POSIX Result: "<< largest << endl;
    cout << "index: " << indexposix  << endl;
    cout << "Serial Runtime: " << serial_time << endl;
    cout << "Parallel Runtime: " << parallel_time_quicksort << endl;
    cout << "Speed-up: " << float(serial_time) / parallel_time_quicksort << endl;


}
