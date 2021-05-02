#include <iostream>
#include <vector>
#include <ipp.h>
#include <pthread.h>
using namespace std;

#define SIZE 1048576
#define MAX 200

vector<float> vec1(SIZE);

typedef struct{
    int threadNum;
    int low;
    int High;
} threadArguments;


int partition (int low, int high, vector<float>& vec){
  int i, j, temp, key;
    key = vec[low];
    i= low + 1;
    j= high;
    while(1){
      while(i < high && key >= vec[i])
          i++;
      while(key < vec[j])
            j--;
      if(i < j){
          temp = vec[i];
          vec[i] = vec[j];
          vec[j] = temp;
      } else {
          temp= vec[low];
          vec[low] = vec[j];
          vec[j]= temp;
          return(j);
      }
    }
}

void serial_quick_sort (int p, int r, vector<float>& vec){
    if (p < r) {
        int q = partition (p , r, vec);
        serial_quick_sort (p, q - 1, vec);
        serial_quick_sort (q + 1, r, vec);
    }
}

int threadPartition(int low, int high){
    float pivot = vec1[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++){
        if (vec1[j] <= pivot){
            i++;
            float temp = vec1[i];
            vec1[i] = vec1[j];
            vec1[j] = temp;
        }
    }
    float temp = vec1[i + 1];
    vec1[i + 1] = vec1[high];
    vec1[high] = temp;
    return (i + 1);
}

void smallQuickSort(int low, int high){
    if (low < high){
        int pivot = threadPartition(low, high);
        smallQuickSort(low, pivot - 1);
        smallQuickSort(pivot + 1, high);
    }
}

void* parallelQuicksort(void* args) {
    threadArguments* arg = (threadArguments*) args;
    int low = arg->low;
    int High = arg->High;
    int threads = arg->threadNum;
    if (threads >= MAX)
        smallQuickSort(low, High);
    else if (low <  High)
    {
        pthread_t my_threads[2];
        int pivot = threadPartition(low, High);
        threadArguments arg[2];
        arg[0].threadNum = threads * 2;
        arg[0].low = low;
        arg[0].High = pivot - 1;
        arg[1].threadNum = threads * 2; 
        arg[1].low = pivot + 1;
        arg[1].High = High;
        for (int i = 0; i < 2; ++i)
            pthread_create(&my_threads[i], NULL, parallelQuicksort, (void*)&arg[i]);
        for (int i = 0; i < 2; ++i)
            pthread_join(my_threads[i], NULL);
    }
    pthread_exit(0);
}

void sort_check (vector<float>& vec){
    int i;
    for (i = 0; i < SIZE-1; i++) {
        if (vec[i] > vec[i+1]) {
            printf ("Chek failed!\n");
        }
    }
}

int main()
{
    printf("SIDs : 810195548 , 810195346\n");
    printf("----------------------------\n");
    vector<float> vec(SIZE);
    for (int i = 0; i < SIZE; i++){
        vec[i] = rand() % 1048576;
        vec1[i] = vec[i];
    }
    Ipp64u start, end;
    /*-----------Serial-----------*/
    start = ippGetCpuClocks();
    serial_quick_sort(0, SIZE - 1,vec);
    end = ippGetCpuClocks();
    int serial_time = (Ipp32u)(end - start);
     sort_check(vec);
    /*----------Parallel----------*/
    pthread_t main_thread;
    threadArguments main_arg;
    main_arg.threadNum = 1;
    main_arg.low = 0;
    main_arg.High = SIZE - 1;
    start = ippGetCpuClocks();
    pthread_create(&main_thread, NULL, parallelQuicksort, (void*)&main_arg);
    pthread_join(main_thread, NULL);
    end = ippGetCpuClocks();
    int parallel_time_quicksort = (Ipp32u)(end - start);
    sort_check(vec1);

    cout << "Serial Runtime: " << serial_time << endl;
    cout << "Parallel Runtime: " << parallel_time_quicksort << endl;
    cout << "Speed-up: " << float(serial_time) / parallel_time_quicksort << endl;

    pthread_exit(0);
}