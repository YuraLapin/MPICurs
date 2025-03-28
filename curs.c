#include <mpi.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <stdlib.h>



void merge(int arr[], int left, int mid, int right) {
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    int left_arr[n1], right_arr[n2];

    
    for (i = 0; i < n1; ++i)
    {
        left_arr[i] = arr[left + i];
    }
           
    for (j = 0; j < n2; ++j)
    {
        right_arr[j] = arr[mid + 1 + j];
    }
    
    i = 0;
    j = 0;
    k = left;
    while (i < n1 && j < n2) {
        if (left_arr[i] <= right_arr[j]) {
            arr[k] = left_arr[i];
            ++i;
        }
        else {
            arr[k] = right_arr[j];
            ++j;
        }
        ++k;
    }

    while (i < n1) {
        arr[k] = left_arr[i];
        ++i;
        ++k;
    }

    while (j < n2) {
        arr[k] = right_arr[j];
        ++j;
        ++k;
    }
}



void merge_sort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}



int main(int argc, char *argv[])
{
    int i, j, k;
    int rank, size;
    int n = 100;
    int arr[n];
    
    srand(time(NULL));    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int elements_per_proc = n / (size - 1);
    int local_arr[elements_per_proc];
    int sorted_arr[n + elements_per_proc]; 
        
    if (rank == 0)
    {
        for (i = 0; i < n; ++i)
        {
            int r = rand() % 99 + 1;
            arr[i] = r;
            printf("%d ", r);
        }
        printf("\n");
        
        MPI_Bcast(&arr, n, MPI_INT, 0, MPI_COMM_WORLD);              
        MPI_Gather(&local_arr, elements_per_proc, MPI_INT, &sorted_arr, elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
        
        for (i = elements_per_proc; i < n + elements_per_proc; ++i)
        {
            arr[i - elements_per_proc] = sorted_arr[i];
        }
           
        for (i = 1; i <= size - 2; ++i)
        {
            merge(arr, 0, elements_per_proc * i - 1, elements_per_proc * (i + 1) - 1);
        }
        
        printf("\n");
        for (i = 0; i < n; ++i)
        {
            printf("%d ", arr[i]);            
        }
        printf("\n");
    }
    
    else
    {
        MPI_Bcast(&arr, n, MPI_INT, 0, MPI_COMM_WORLD);
        
        int left = (rank - 1) * elements_per_proc;
        int right = rank * elements_per_proc - 1;
        
        merge_sort(arr, left, right);        
        
        for (i = left; i <= right; ++i)
        {
            local_arr[k] = arr[i];
            ++k;
        }
        
        MPI_Gather(local_arr, elements_per_proc, MPI_INT, &arr, elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    return 0;
}
