#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef struct {
	int32_t* ptr;
	size_t size;
	size_t cap;
} arrayint;

void array_realloc(arrayint *arr) {
	arr->cap = (int) (arr->cap * 1.1) + 2;
	int32_t* tmp = realloc(arr->ptr, arr->cap * sizeof(int));
	if (tmp == NULL) {
		printf("Allocation failed! OOM!");
		exit(1);
	}
	arr->ptr = tmp;
}

arrayint array_init(void) {
	arrayint arr = {NULL, 0, 0};
	array_realloc(&arr);
	return arr;
}

void array_insert(arrayint *arr, const int32_t num) {
	if (arr->size == arr->cap) {
		array_realloc(arr);
	}
	arr->ptr[arr->size++] = num;
}

int intcmp(const void* a, const void* b)
{
	int32_t arg1 = *(const int32_t*)a;
	int32_t arg2 = *(const int32_t*)b;

	if (arg1 < arg2) return -1;
	if (arg1 > arg2) return 1;
	return 0;

}

int main(const int argc, const char** argv) {
	if (argc != 3) {
		printf("Numero argomenti sbagliato");
		return 1;
	}

	FILE *input = fopen(argv[1], "r");
	if (input == NULL) {
		printf("Apertura file input fallita");
		return 1;
	}

	FILE *output = fopen(argv[2], "w");
	if (output < 0) {
		printf("Apertura file output fallita");
		return 1;
	}

	int tmp = 0;
	arrayint arr = array_init();

	while (fscanf(input, "%d", &tmp)) {
		array_insert(&arr, tmp);
		if (feof(input)) break;
	}
	fclose(input);

	qsort(arr.ptr, arr.size, sizeof(int), intcmp);

	for (size_t i = 0; i < arr.size; ++i) {
		if (!fprintf(output, "%d\n", arr.ptr[i])) {
			printf("Errore durante l'output");
			return 1;
		}
	}

	fclose(output);
	return 0;

}