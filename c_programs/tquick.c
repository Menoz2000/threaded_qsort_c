#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SLICE_THRESH 800
#define N_ELEM_ARRAY 1000000000
#define RANGE_ELEM_ARRAY 10000000000000



typedef struct {
    void *array;
    long int left;
    long int right;
    size_t width;
    int (*compar)(const void *, const void *);
} sort_args_t;

void swap(void *array, long int i, long int j, size_t width) {
    unsigned char temp[width];
    memcpy(temp, (unsigned char *)array + i * width, width);
    memcpy((unsigned char *)array + i * width, (unsigned char *)array + j * width, width);
    memcpy((unsigned char *)array + j * width, temp, width);
}

int partition(void *array, long int left, long int right, size_t width, int (*compar)(const void *, const void *)) {
    /*_____________________________________________________________________________________________*/
    //Ultimo elemento dell'array scelto come pivot
    /*void *pivot = (unsigned char *)array + right * width;
    printf("%d\n", *(int *)pivot);*/
    /*_____________________________________________________________________________________________*/

    /*_____________________________________________________________________________________________*/
    //Calcolo la mediana tra il primo elemento, l'ultimo e quello centrale per poter avere 
    //un valore pivot più efficace per la partizione dell'array
    long int mid = left + (right - left) / 2;
    //Trovo i tre elementi: primo, centrale e ultimo
    void *a=(unsigned char *)array + left *width;
    void *b=(unsigned char *)array + mid *width;
    void *c=(unsigned char *)array + right *width;
    printf("a: %ld; b: %ld; c: %ld\n", *(long int *)a, *(long int *)b, *(long int *)c);
    //ordino i tre elementi per trovare la midiana
    if (compar(a, b) > 0) swap (array, left, mid, width);
    if (compar(a, c) > 0) swap (array, left, right, width);
    if (compar(b, c) > 0) swap (array, mid, right, width);
    //uso il valore centrale come pivot
    swap(array, mid, right, width);
    // Pivot è ora in `right`
    void *pivot = (unsigned char *)array + right * width;
    printf("pivot: %ld\n\n",*(long int *)pivot);
    /*_____________________________________________________________________________________________*/

    /*_____________________________________________________________________________________________*/
    //Scelta casuale
    //srand(time(NULL)); // Inizializza il generatore di numeri casuali
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
    return i + 1;

}

void *quicksort(void *args) {
    sort_args_t *sargs = (sort_args_t *)args;
    void *array = sargs->array;
    long int left = sargs->left;
    long int right = sargs->right;
    size_t width = sargs->width;
    int (*compar)(const void *, const void *) = sargs->compar;

    if (right - left < SLICE_THRESH) {
        printf("Sort sequentially\n");
        // Sort sequentially
        qsort((unsigned char *)array + left * width, right - left + 1, width, compar);
        free(sargs); // Free dynamically allocated args
        return NULL;
    }

    printf("Sort Using threads\n");
    long int pivotIndex = partition(array, left, right, width, compar);
    
    sort_args_t *leftArgs = malloc(sizeof(sort_args_t));
    sort_args_t *rightArgs = malloc(sizeof(sort_args_t));
    if (!leftArgs || !rightArgs) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    *leftArgs = (sort_args_t){array, left, pivotIndex - 1, width, compar};
    *rightArgs = (sort_args_t){array, pivotIndex + 1, right, width, compar};

    pthread_t leftThread, rightThread;
    pthread_create(&leftThread, NULL, quicksort, leftArgs);
    pthread_create(&rightThread, NULL, quicksort, rightArgs);

    pthread_join(leftThread, NULL);
    pthread_join(rightThread, NULL);

    free(sargs); // Free dynamically allocated args
    return NULL;
}

void threaded_quicksort(void *array, long int n, size_t width, int (*compar)(const void *, const void *)) {
    sort_args_t *args = malloc(sizeof(sort_args_t));
    if (!args) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    *args = (sort_args_t){array, 0, n - 1, width, compar};
    quicksort(args);
}

int compare(const void *a, const void *b) {
    return (*(int *)a - *(int *)b);
}

// Example usage
int main() {
    
    long int n = N_ELEM_ARRAY;
    long int *arr = (long int *)malloc(n * sizeof(long int));

    // Inizializza il generatore di numeri casuali
    srand(time(NULL));

    if (arr == NULL) {
        printf("Errore nell'allocazione della memoria.\n");
        return 1;
    }

    // Popola l'array con valori casuali e stampa
    for (int i = 0; i < n; i++) {
        arr[i] = rand() % RANGE_ELEM_ARRAY; // Valori casuali tra 0 e 99
        //printf("- %ld -", arr[i]);
    }
    printf("\n");

    threaded_quicksort(arr, n, sizeof(long int), compare);
    
    for (long int i = 0; i < n; i++) {
        printf("%ld ", arr[i]);
    }

    printf("\n\n");
    
    // Verifica se l'array è ordinato
    int is_sorted = 1;
    for (long int i = 1; i < n; i++) {
        if (arr[i - 1] > arr[i]) {
            is_sorted = 0;
            break;
        }
    }

    if (is_sorted) {
        printf("L'array è ordinato in ordine crescente.\n");
    } else {
        printf("L'array NON è ordinato in ordine crescente.\n");
    }

    free(arr);
    return 0;

}
