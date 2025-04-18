#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>



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



void merge_sort(int arr[], int left, int right)
{
    if (left < right)
    {
        int mid = left + (right - left) / 2;

        merge_sort(arr, left, mid);
        merge_sort(arr, mid + 1, right);
        
        merge(arr, left, mid, right);
    }
}



int main(int argc, char *argv[])
{
    int i, j, k;
    int n = 100000;
    int *arr = malloc(n * sizeof(int));
    srand(time(NULL));    
        
    for (i = 0; i < n; ++i)
    {
	    arr[i] = rand() % (32768 * 2) - 32768;
    }
    
    struct timeval tv1, tv2;
    gettimeofday(&tv1, NULL);
    
    merge_sort(arr, 0, n);
        
    gettimeofday(&tv2, NULL);
    long seconds = tv2.tv_sec - tv1.tv_sec;
    long microseconds = tv2.tv_usec - tv1.tv_usec;
    if (microseconds < 0)
    {
        --seconds;
        microseconds += 1000000;
    }
    printf("%f\n", (float) (seconds * 1000000 + microseconds) / 1000000);

    free(arr);
    return 0;
}
