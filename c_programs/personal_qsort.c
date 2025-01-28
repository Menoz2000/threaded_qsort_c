#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

// Structure to hold the arguments for the quicksort function
typedef struct {
    void *array;                       // Pointer to the array to sort
    long int left;                     // Left boundary of the current partition
    long int right;                    // Right boundary of the current partition
    size_t width;                      // Size of each element in the array
    int (*compar)(const void *, const void *); // Comparison function
    long int slice_thresh;             // Threshold for switching to sequential sort
} sort_args_t;

// Swaps two elements in the array
void swap(void *array, long int i, long int j, size_t width) {
    unsigned char temp[width];
    memcpy(temp, (unsigned char *)array + i * width, width);
    memcpy((unsigned char *)array + i * width, (unsigned char *)array + j * width, width);
    memcpy((unsigned char *)array + j * width, temp, width);
}

// Partitions the array and selects a pivot using the median-of-three strategy
int partition(void *array, long int left, long int right, size_t width, int (*compar)(const void *, const void *)) {
    /*_____________________________________________________________________________________________*/
    //last array element aas pivot
    /*void *pivot = (unsigned char *)array + right * width;
    printf("%d\n", *(int *)pivot);*/
    /*_____________________________________________________________________________________________*/

    /*_____________________________________________________________________________________________*/
    //Calc the med between the firt, last and middle-one values of array to make a better partition
    long int mid = left + (right - left) / 2;
    //find the three elements
    void *a=(unsigned char *)array + left *width;
    void *b=(unsigned char *)array + mid *width;
    void *c=(unsigned char *)array + right *width;
    //printf("a: %ld; b: %ld; c: %ld\n", *(long int *)a, *(long int *)b, *(long int *)c);
    //ordering the three elements to find the median
    if (compar(a, b) > 0) swap (array, left, mid, width);
    if (compar(a, c) > 0) swap (array, left, right, width);
    if (compar(b, c) > 0) swap (array, mid, right, width);
    //use the central value as pivot
    swap(array, mid, right, width);
    // Pivot now is in 'right'
    void *pivot = (unsigned char *)array + right * width;
    printf("pivot: %ld\n\n",*(long int *)pivot);
    /*_____________________________________________________________________________________________*/

    /*_____________________________________________________________________________________________*/
    //random choose
    //srand(time(NULL)); // initialize the random number generator
    //long int randIndex = left + rand() % (right - left + 1);
    //swap(array, randIndex, right, width);
    //void *pivot = (unsigned char *)array + right * width;
    /*_____________________________________________________________________________________________*/

    long int i = left - 1;
 
    for (long int j = left; j < right; j++) {
        if (compar((unsigned char *)array + j * width, pivot) <= 0) {
            i++;
            swap(array, i, j, width);
        }
    }

    swap(array, i + 1, right, width);
    return i + 1; // Return the final pivot position
}

// Recursive quicksort function that uses threads
void *quicksort(void *args) {
    sort_args_t *sargs = (sort_args_t *)args;
    void *array = sargs->array;
    long int left = sargs->left;
    long int right = sargs->right;
    size_t width = sargs->width;
    int (*compar)(const void *, const void *) = sargs->compar;

    /*for (int i=0; i<20; i++){
        printf("ciao\n");
    }*/

    // Base case: Switch to sequential sorting if the partition is below the threshold
    if (right - left < sargs->slice_thresh) {
        printf("Sort sequentially\n");
        // Sort sequentially
        qsort((unsigned char *)array + left * width, right - left + 1, width, compar);
        free(sargs); // Free dynamically allocated args
        return NULL;
    }

    printf("Sort Using threads\n");
    // Partition the array and get the pivot index
    long int pivotIndex = partition(array, left, right, width, compar);
    
    // Prepare arguments for the left and right partitions
    sort_args_t *leftArgs = malloc(sizeof(sort_args_t));
    sort_args_t *rightArgs = malloc(sizeof(sort_args_t));
    if (!leftArgs || !rightArgs) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    *leftArgs = (sort_args_t){array, left, pivotIndex - 1, width, compar, sargs->slice_thresh};
    *rightArgs = (sort_args_t){array, pivotIndex + 1, right, width, compar, sargs->slice_thresh};

    // Create threads for the left and right partitions
    pthread_t leftThread, rightThread;
    pthread_create(&leftThread, NULL, quicksort, leftArgs);
    pthread_create(&rightThread, NULL, quicksort, rightArgs);

    // Wait for the threads to complete
    pthread_join(leftThread, NULL);
    pthread_join(rightThread, NULL);

    free(sargs); // Free the dynamically allocated args
    return NULL;
}

// Wrapper function for the threaded quicksort
void threaded_quicksort(void *array, long int n, size_t width, int (*compar)(const void *, const void *), long int slice_tresh) {
    sort_args_t *args = malloc(sizeof(sort_args_t));
    if (!args) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    *args = (sort_args_t){array, 0, n - 1, width, compar, slice_tresh};
    quicksort(args);
}

// Reads a safe long integer from user input
long int get_safe_long() {
    char buffer[100];       // Input Buffer
    char *endptr;           // Pointer to find residues inside the string
    long int value;

    // Read terminal input
    if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
        return -1;          // Error in input reading
    }

    // Remove the character newline, if it is present
    char *newline = strchr(buffer, '\n');
    if (newline) *newline = '\0';

    // Reset the global variable errno and convert buffer to long int
    errno = 0;
    value = strtol(buffer, &endptr, 10);

    // Error verify
    if (errno == ERANGE || endptr == buffer || *endptr != '\0') {
        return -1;          // Non valid value or out of range
    }

    return value;
}

// Comparison function for integers
int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// Checks if the array is sorted
bool is_sorted(long int *arr, int n) {
    bool is_sorted = true;
    for (long int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            is_sorted = false;
            break;
        }
    }
    return is_sorted;
}

// Verifies that two arrays contain the same elements
bool contain_same_number(long int *non_sorted, long int *sorted, long int n) {
    long int *sorted_correctly = malloc(n * sizeof(long int));
    if (sorted_correctly == NULL) {
        printf("Memory allocation error.\n");
        return false;
    }

    memcpy(sorted_correctly, non_sorted, n * sizeof(long int));
    qsort(sorted_correctly, n, sizeof(long int), compare);

    for (long int i = 0; i < n; i++) {
        if (sorted_correctly[i] != sorted[i]) {
            free(sorted_correctly);
            return false;
        }
    }

    free(sorted_correctly);
    return true;
}

int main() {
    long int n, range_elem_array, slice_tresh;
    long int *arr, *arr_cpy;

    srand(time(NULL));

    // Get input from the user
    printf("Insert the number of elements to have inside the array: ");
    n = get_safe_long();
    while (n < 1) {
        printf("Error: Non valid input\nRetry: ");
        n = get_safe_long();
    }

    printf("Insert the maximum number value to insert in the array: ");
    range_elem_array = get_safe_long();
    while (range_elem_array < 1) {
        printf("Error: Non valid input\nRetry: ");
        range_elem_array = get_safe_long();
    }

    printf("Insert the maximum sotto-array size: ");
    slice_tresh = get_safe_long();
    while (slice_tresh < 1) {
        printf("Error: Non valid input\nRetry: ");
        slice_tresh = get_safe_long();
    }

    arr = malloc(n * sizeof(long int));
    arr_cpy = malloc(n * sizeof(long int));
    if (!arr || !arr_cpy) {
        printf("Memory allocation error.\n");
        return 1;
    }

    // Populate the array with random values
    printf("Array values:\n[");
    for (long int i = 0; i < n; i++) {
        arr[i] = rand() % range_elem_array;
        arr_cpy[i] = arr[i];
        printf("%ld ", arr[i]);
    }
    printf("]\n\n");

    // Sort the array using threaded quicksort
    threaded_quicksort(arr, n, sizeof(long int), compare, slice_tresh);

    // Print the sorted array
    printf("Sorted array:\n");
    for (long int i = 0; i < n; i++) {
        printf("%ld ", arr[i]);
    }
    printf("\n");

    // Verify correctness
    if (is_sorted(arr, n)) {
        printf("The array is sorted.\n");
    } else {
        printf("The array is NOT sorted.\n");
    }

    if (contain_same_number(arr_cpy, arr, n)) {
        printf("The array is sorted and contains the same elements.\n");
    } else {
        printf("The array is NOT sorted or contains different elements.\n");
    }

    free(arr);
    free(arr_cpy);
    return 0;
}
