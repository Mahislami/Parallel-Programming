#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>

int partition(int low, int high,float *array);
void serial_quick_sort (int p, int r, float *array);
void quick_sort (int p, int r, float *array, int threshold);
void parallel_quick_sort (int n, float *array, int threshold);
void sort_check (int n, float *array);

int main (int argc, char *argv[]){
    printf("SIDs : 810195548 , 810195346\n");
    printf("----------------------------\n");

    int n = 1048576;
    float *array, *array2;
    double start, end;
    omp_set_num_threads(4);

    array = (float *)malloc (sizeof (float) * n);
    array2 = (float *)malloc (sizeof (float) * n);
    for (int i=0; i<n; i++ ) {
        array[i] = rand();
        array2[i] = array[i];
    }
    /*----------Parallel----------*/
    start = omp_get_wtime();
    parallel_quick_sort (n - 1, &array[0], 8);
    end = omp_get_wtime();
    double parallel_time = end - start;

    /*-----------Serial-----------*/
    start = omp_get_wtime();
    serial_quick_sort (0, n - 1, &array2[0]);
    end = omp_get_wtime();
    double serial_time = end - start;

    sort_check(n, &array[0]);
    sort_check(n, &array2[0]);

    printf("Parallel: %.4f\n", parallel_time);
    printf("Serial: %.4f\n", serial_time);

    /*----------Speed-up----------*/
    double speed_up = (serial_time) / (float) parallel_time;
    printf("Speed-up %4f\n", speed_up);

    free (array);
    return 0;
}



int partition(int low, int high, float *array){
  int i, j, temp, key;
    key = array[low];
    i= low + 1;
    j= high;
    while(1){
      while(i < high && key >= array[i])
          i++;
      while(key < array[j])
            j--;
      if(i < j){
          temp = array[i];
          array[i] = array[j];
          array[j] = temp;
      } else {
          temp= array[low];
          array[low] = array[j];
          array[j]= temp;
          return(j);
      }
    }
}

void serial_quick_sort (int p, int r, float *array){
    if (p < r) {
        int q = partition (p, r, array);
        serial_quick_sort (p, q - 1, array);
        serial_quick_sort (q + 1, r, array);
    }
}

void sort_check (int n, float *array){
    int i;
    for (i = 0; i < n - 1; i++) {
        if (array[i] > array[i+1]) {
            printf ("Chek failed!\n");
        }
    }
}

void quick_sort (int p, int r, float *array, int threshold) {
    if (p < r) {
        if ((r - p) < threshold) {
            serial_quick_sort(p, r, array);
        }
        else {
            int q = partition (p, r, array);
            #pragma omp task firstprivate(array, threshold, r, q)
                quick_sort (p, q - 1, array, threshold);
            #pragma omp task firstprivate(array, threshold, r, q)
                quick_sort (q + 1, r, array, threshold);
        }
    }
}

void parallel_quick_sort (int n, float *array, int threshold) {
    #pragma omp parallel
    {
        #pragma omp single nowait
            quick_sort (0, n, array, threshold);
    }
}