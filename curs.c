#include <mpi.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <stdlib.h>

#include <g2.h>
#include <g2_X11.h>



void merge(int arr[], int left, int mid, int right)
{
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
    while (i < n1 && j < n2)
    {
        if (left_arr[i] <= right_arr[j])
        {
            arr[k] = left_arr[i];
            ++i;
        }
        else
        {
            arr[k] = right_arr[j];
            ++j;
        }
        ++k;
    }

    while (i < n1)
    {
        arr[k] = left_arr[i];
        ++i;
        ++k;
    }

    while (j < n2)
    {
        arr[k] = right_arr[j];
        ++j;
        ++k;
    }
}



void merge_sort(int arr[], int left, int right, int left_send, int right_send, int elements_per_proc)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;

        merge_sort(arr, left, mid, left_send, right_send, elements_per_proc);
        merge_sort(arr, mid + 1, right, left_send, right_send, elements_per_proc);
        
        merge(arr, left, mid, right);
        
        int local_arr[elements_per_proc];
        int i, k = 0;
        for (i = left_send; i <= right_send; ++i)
        {
            local_arr[k] = arr[i];
            ++k;
        }
        MPI_Gather(&local_arr, elements_per_proc, MPI_INT, &arr, elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
    }
}


void print_array(int arr[], int n)
{
      printf("\n");
      int i;
      for (i = 0; i < n; ++i)
      {
          printf("%d ", arr[i]);            
      }
      printf("\n");
}



int main(int argc, char *argv[])
{
    int i, j, k;
    int rank, size;
    int n = 20;
    int arr[n];
    
    srand(time(NULL));    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int elements_per_proc = n / (size - 1);
    int local_arr[elements_per_proc];
    int sorted_arr[n + elements_per_proc];
    
    int offset_left = 20;
    int offset_right = 20;
    int offset_top = 30;
    int offset_bottom = 30;
    int max_height = 200;
    int el_width = 2;
    int gap = 2;
    int width = offset_left + offset_right + n * (el_width + gap);
    int height = offset_top + offset_bottom + max_height;
    int pixels[width][height];
    
    int d = g2_open_X11(width, height);
        
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
        for (i = 0; i < elements_per_proc - 1; ++i)
        {
            MPI_Gather(&local_arr, elements_per_proc, MPI_INT, &sorted_arr, elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
        
            for (j = elements_per_proc; j < n + elements_per_proc; ++j)
            {
                arr[j - elements_per_proc] = sorted_arr[j];
            }  

            print_array(arr, n);
        }
        
        MPI_Gather(&local_arr, elements_per_proc, MPI_INT, &sorted_arr, elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
        
        for (i = elements_per_proc; i < n + elements_per_proc; ++i)
        {
            arr[i - elements_per_proc] = sorted_arr[i];
        }
           
        for (i = 1; i <= size - 2; ++i)
        {
            print_array(arr, n);
            merge(arr, 0, elements_per_proc * i - 1, elements_per_proc * (i + 1) - 1);
        }
        
        print_array(arr, n);
    }
    
    else
    {
        MPI_Bcast(&arr, n, MPI_INT, 0, MPI_COMM_WORLD);
        
        int left = (rank - 1) * elements_per_proc;
        int right = rank * elements_per_proc - 1;
        
        merge_sort(arr, left, right, left, right, elements_per_proc);        
        
        for (i = left; i <= right; ++i)
        {
            local_arr[k] = arr[i];
            ++k;
        }
        
        MPI_Gather(&local_arr, elements_per_proc, MPI_INT, &arr, elements_per_proc, MPI_INT, 0, MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    return 0;
}
