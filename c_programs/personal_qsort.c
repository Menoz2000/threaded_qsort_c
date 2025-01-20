#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>          // Global variables and macro to manage errors in std libraries
#include <stdbool.h>

typedef struct {
    void *array;
    long int left;
    long int right;
    size_t width;
    int (*compar)(const void *, const void *);
    long int slice_thresh;
} sort_args_t;

void threaded_quicksort(void *array, long int n, size_t width, int (*compar)(const void *, const void *), long int slice_tresh) {
    sort_args_t *args = malloc(sizeof(sort_args_t));
    if (!args) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    *args = (sort_args_t){array, 0, n - 1, width, compar, slice_tresh};
    //quicksort(args);
}

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

    // Reset the global variable errno and convert buffer to log int
    errno = 0;
    value = strtol(buffer, &endptr, 10);

    // Error verify
    if (errno == ERANGE || endptr == buffer || *endptr != '\0') {
        return -1;          // Non valid value or out of range
    }

    return value;
}

int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

bool is_sorted (long int *arr, int n){
    bool is_sorted = true;
    for (long int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            is_sorted = false;
            break;
        }
    }
    return is_sorted;
}

bool contain_same_number (long int *non_sorted, long int *sorted, long int n){

    long int *sorted_correctly = malloc(n * sizeof(long int));
    if (sorted_correctly == NULL) {
        printf("Errore nell'allocazione della memoria.\n");
        return 0;
    }

    for (int i = 0; i < n ; i++){
        sorted_correctly[i] = non_sorted[i];
        //printf("%ld", sorted_correctly[i]);
        //printf("%ld\n", non_sorted[i]);
    }

    qsort(sorted_correctly, n, sizeof(long int), compare);

    for (int i = 0; i < n ; i++){
        //printf("%ld = %ld\n", sorted_correctly[i], sorted[i]);
        if (sorted_correctly[i] != sorted[i]){
            free(sorted_correctly);
            return 0;
        }
    }

    free(sorted_correctly);
    return 1;

    
}

int main(){
    long int n;                     //Number of array's elements
    long int range_elem_array;      // Max number value in the array
    long int *arr;                  //Array to order
    long int *arr_cpy;              //Array copy for checks
    long int slice_tresh;           //dimensione minima del sottoarray TODO
    

    srand(time(NULL));              //Initialize the random number generator

    printf("Insert the number of elements to have inside the array: ");
    n = get_safe_long();
    while (n < 1){
        printf("Error: Non valid input\nRetry: ");
        n = get_safe_long();
    }

    printf("Insert the maximum number value to insert in the array: ");
    range_elem_array = get_safe_long();
    while (range_elem_array < 1){
        printf("Error: Non valid input\nRetry: ");
        range_elem_array = get_safe_long();
    }

    printf("Insert the minimum sotto-array size: ");
    slice_tresh = get_safe_long();
    while (slice_tresh < 1){
        printf("Error: Non valid input\nRetry: ");
        slice_tresh = get_safe_long();
    }

    arr = (long int *)malloc(n * sizeof(long int));
    if (arr == NULL) {
        printf("Errore nell'allocazione della memoria.\n");
        return 1;
    }

    arr_cpy = (long int *)malloc(n * sizeof(long int));
    if (arr_cpy == NULL) {
        printf("Errore nell'allocazione della memoria.\n");
        return 1;
    }

    // Populate the array with random values
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % range_elem_array; 
        arr_cpy[i] = arr[i];
        //printf("- %ld -", arr[i]);
    }

    printf("\n");

    threaded_quicksort(arr, n, sizeof(long int), compare, slice_tresh);

    // Print the sorted array
    for (long int i = 0; i < n; i++) {
        printf("%ld ", arr[i]);
    }
    printf("\n");

    // Verify if the array is sorted
    if (is_sorted(arr, n)){
        printf("The array is sorted\n");
    } else {
        printf("The array is NOT sorted\n");
    }

    // Verify if the two array contain same numbers
    if (contain_same_number(arr_cpy, arr, n)){
        printf("The array are sorted and contain same numbers\n");
    } else {
        printf("The array is NOT sorted or not contain same numbers\n");
    }

    free(arr);
    free(arr_cpy);
    return 0;
}

//TODO comincia funzione qsort