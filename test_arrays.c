/*
	Tests for Array Utilities
	Copyright (c) 2023, Eddy L O Jansson. Licensed under The MIT License.

	See https://github.com/eloj/eutils
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "emacros.h"
#include "internal/tests.h"

#define EUTILS_IMPLEMENTATION
#include "earrays.h"

#define GEN_ROTATE_ARRAY_CB(name, type) \
static void name(void *arr, int src, int dst, enum rotate_array_action action, void *ctx) { \
	type *tmp = ctx; \
	type *typed_arr = arr; \
	switch (action) { \
		case ROT_ACTION_COPY: \
			typed_arr[dst] = typed_arr[src]; \
			break; \
		case ROT_ACTION_SAVE: \
			*tmp = typed_arr[src]; \
			break; \
		case ROT_ACTION_RESTORE: \
			typed_arr[dst] = *tmp; \
			break; \
	} \
}

GEN_ROTATE_ARRAY_CB(rotate_int_array_cb, int)

#define CHECK_ROT(...) memcmp(arr, (int[]){__VA_ARGS__}, sizeof(arr)) == 0 ? 0 : 1

static int test_simple_rotate(void) {
	TEST_START(simple_rotate);

	int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	int n = sizeof(arr)/sizeof(arr[0]);
	int ctx; // don't need complex context, use directly as temporary storage.

	rotate_array(arr, n, -1, rotate_int_array_cb, &ctx);
	fails += CHECK_ROT(8,1,2,3,4,5,6,7);
	rotate_array(arr, n, -1, rotate_int_array_cb, &ctx);
	fails += CHECK_ROT(7,8,1,2,3,4,5,6);
	rotate_array(arr, n, -6, rotate_int_array_cb, &ctx);
	fails += CHECK_ROT(1,2,3,4,5,6,7,8);
	rotate_array(arr, n, 1, rotate_int_array_cb, &ctx);
	fails += CHECK_ROT(2,3,4,5,6,7,8,1);
	rotate_array(arr, n, 1, rotate_int_array_cb, &ctx);
	fails += CHECK_ROT(3,4,5,6,7,8,1,2);
	rotate_array(arr, n, 6, rotate_int_array_cb, &ctx);
	fails += CHECK_ROT(1,2,3,4,5,6,7,8);
	rotate_array(arr, n, n*3, rotate_int_array_cb, &ctx);
	fails += CHECK_ROT(1,2,3,4,5,6,7,8);
	rotate_array(arr, n, -n*3, rotate_int_array_cb, &ctx);
	fails += CHECK_ROT(1,2,3,4,5,6,7,8);
	rotate_array(arr, n, n+1, rotate_int_array_cb, &ctx);
	fails += CHECK_ROT(2,3,4,5,6,7,8,1);
	rotate_array(arr, n, -(n+1), rotate_int_array_cb, &ctx);
	fails += CHECK_ROT(1,2,3,4,5,6,7,8);

	// Rotate inner portion of array, leaving first and last element intact.
	rotate_array(arr + 1, n - 2, 1, rotate_int_array_cb, &ctx);
	fails += CHECK_ROT(1,3,4,5,6,7,2,8);

	TEST_END();
}
#undef CHECK_ROT

int main(int UNUSED(argc), char UNUSED(*argv[])) {
	size_t failed = 0;

	failed += test_simple_rotate();

	if (failed != 0) {
		printf("Tests " RED "FAILED" NC "\n");
	} else {
		printf("All tests " GREEN "passed OK" NC ".\n");
	}

	return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
