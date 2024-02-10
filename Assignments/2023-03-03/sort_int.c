#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


typedef struct {
    int32_t* arr;
    uint64_t n;
    uint64_t cap;
} arrayint;

void a_init(arrayint* a) {
    a->cap = 10;
    a->n = 0;
    a->arr = malloc(a->cap * sizeof(int32_t));
}

int int32_t_cmp(const void *aa, const void *bb) {
    int32_t a = *(int32_t *)aa;
    int32_t b = *(int32_t *)bb;
    return a == b ? 0 : a < b ? -1 : 1;
}

void a_realloc(arrayint* a) {
    a->cap = a->cap * 1.1 + 2;
    a->arr = realloc(a->arr, a->cap * sizeof(int32_t));
}

void a_insert(arrayint* a, int32_t n) {
    if (a->cap - a->n < 1) a_realloc(a);
    a->arr[a->n++] = n;
}

int main(int argc, char**argv)
{

    arrayint a;

    a_init(&a);



    if (argc != 3) {
        printf("Usage: %s <filein.txt> <fileout.txt>", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* input = fopen(argv[1],"r");
    if (input == NULL) {
        printf("input fallito");
        return EXIT_FAILURE;
    }

    FILE* output = fopen(argv[2], "w");
    if (input == NULL) {
        printf("output fallito");
        return EXIT_FAILURE;
    }

    int32_t numero;
    while (fscanf(input, "%d", &numero) == 1) {
        //printf("%d\n", numero);
        a_insert(&a, numero);
        if (feof(input)) break;
    }

    qsort(a.arr, a.n, sizeof(int32_t), int32_t_cmp);

    for (int i = 0; i < a.n; i++) {
        fprintf(output, "%d\n", a.arr[i]);
    }

    fclose(output);
    fclose(input);

    return EXIT_SUCCESS;
}
