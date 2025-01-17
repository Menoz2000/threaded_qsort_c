#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Funzione per verificare se un array è ordinato in ordine crescente
bool is_sorted(int *array, int size) {
    for (int i = 0; i < size - 1; i++) {
        if (array[i] > array[i + 1]) {
            return false; // Non è ordinato
        }
    }
    return true; // È ordinato
}

int main() {
    int *array = NULL;
    int capacity = 10; // Capacità iniziale dell'array
    int size = 0;      // Dimensione attuale dell'array

    array = (int *)malloc(capacity * sizeof(int));
    if (array == NULL) {
        fprintf(stderr, "Errore nell'allocazione della memoria.\n");
        return 1;
    }

    printf("Inserisci gli elementi dell'array (termina con CTRL+D o un input non numerico):\n");

    int value;
    while (scanf("%d", &value) == 1) {
        if (size == capacity) {
            capacity *= 2;
            int *new_array = realloc(array, capacity * sizeof(int));
            if (new_array == NULL) {
                fprintf(stderr, "Errore nella riallocazione della memoria.\n");
                free(array);
                return 1;
            }
            array = new_array;
        }
        array[size++] = value;
    }

    if (size == 0) {
        printf("Array vuoto o nessun input valido fornito.\n");
        free(array);
        return 0;
    }

    if (is_sorted(array, size)) {
        printf("L'array è in ordine crescente.\n");
    } else {
        printf("L'array NON è in ordine crescente.\n");
    }

    free(array);
    return 0;
}
