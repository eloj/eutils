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

static int test_reverse_array(void) {
	TEST_START(reverse_array);

	int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	int n = ARRAY_SIZE(arr);

	reverse_array(arr, n);
	fails += CHECK_ARRAY(8,7,6,5,4,3,2,1);

	reverse_array(arr + 1, n - 3);
	fails += CHECK_ARRAY(8,3,4,5,6,7,2,1);

	TEST_END();
}

struct tile_t {
	int dummy;
};

GEN_ROTATE_ARRAY_CB(rotate_int_array_cb, int);
GEN_ROTATE_ARRAY_CB(rotate_tile_array_cb, struct tile_t);

static const struct re {
	int offset;
	int d;
	int n;
	int *input;
	int *expected;
} rotate_array_tests[] = {
	{ 0,  -1, 8, (int[]){1,2,3,4,5,6,7,8}, (int[]){8,1,2,3,4,5,6,7} },
	{ 0,  -1, 8, (int[]){8,1,2,3,4,5,6,7}, (int[]){7,8,1,2,3,4,5,6} },
	{ 0,  -6, 8, (int[]){7,8,1,2,3,4,5,6}, (int[]){1,2,3,4,5,6,7,8} },
	{ 0,   1, 8, (int[]){1,2,3,4,5,6,7,8}, (int[]){2,3,4,5,6,7,8,1} },
	{ 0,   1, 8, (int[]){2,3,4,5,6,7,8,1}, (int[]){3,4,5,6,7,8,1,2} },
	{ 0,   6, 8, (int[]){3,4,5,6,7,8,1,2}, (int[]){1,2,3,4,5,6,7,8} },
	{ 0, 3*8, 8, (int[]){1,2,3,4,5,6,7,8}, (int[]){1,2,3,4,5,6,7,8} },
	{ 0,-3*8, 8, (int[]){1,2,3,4,5,6,7,8}, (int[]){1,2,3,4,5,6,7,8} },
	{ 0,   9, 8, (int[]){1,2,3,4,5,6,7,8}, (int[]){2,3,4,5,6,7,8,1} },
	{ 0,  -9, 8, (int[]){2,3,4,5,6,7,8,1}, (int[]){1,2,3,4,5,6,7,8} },
	{ 1,   1, 6, (int[]){1,2,3,4,5,6,7,8}, (int[]){1,3,4,5,6,7,2,8} },
};

static int test_rotate_array(void) {
	TEST_START(rotate_array);

	int arr[16];

	for (size_t i = 0 ; i < ARRAY_SIZE(rotate_array_tests) ; ++i) {
		const struct re re = rotate_array_tests[i];
		size_t size = re.n * sizeof(re.input[0]);
		assert(size <= sizeof(arr));
		memcpy(arr, re.input, size);
		rotate_array(arr + re.offset, re.n, re.d);
		if (memcmp(arr, re.expected, size) != 0) {
			fprintf(stderr, "test %zu failed\n", i);
			++fails;
		}
	}

	TEST_END();
}

static int test_rotate_array_cb(void) {
	TEST_START(rotate_array_cb);

	int arr[16];
	int ctx; // don't need complex context, use directly as temporary storage.

	for (size_t i = 0 ; i < ARRAY_SIZE(rotate_array_tests) ; ++i) {
		const struct re re = rotate_array_tests[i];
		size_t size = re.n * sizeof(re.input[0]);
		assert(size <= sizeof(arr));
		memcpy(arr, re.input, size);
		rotate_array_cb(arr + re.offset, re.n, re.d, rotate_int_array_cb, &ctx);
		if (memcmp(arr, re.expected, size) != 0) {
			fprintf(stderr, "test %zu failed\n", i);
			++fails;
		}
	}

	TEST_END();
}

int main(int UNUSED(argc), char UNUSED(*argv[])) {
	size_t failed = 0;

	failed += test_reverse_array();
	failed += test_rotate_array();
	failed += test_rotate_array_cb();

	if (failed != 0) {
		printf("Tests " RED "FAILED" NC "\n");
	} else {
		printf("All tests " GREEN "passed OK" NC ".\n");
	}

	return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
