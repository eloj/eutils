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

struct tile_t {
	int dummy;
	char x;
};

static int test_reverse_array(void) {
	TEST_START(reverse_array);

	int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	int n = ARRAY_SIZE(arr);

	reverse_array(arr, 0);
	reverse_array(arr, 1);

	reverse_array(arr, n);
	fails += CHECK_ARRAY(arr, 8,7,6,5,4,3,2,1);

	reverse_array(arr + 1, n - 3);
	fails += CHECK_ARRAY(arr, 8,3,4,5,6,7,2,1);

	TEST_END();
}

static int test_sort_array(void) {
	TEST_START(sort_array);

	int arri[] = { 5,6,7,8,1,2,3,4,-123 };

	sort_array(arri, 0); // Potentially raises -Wtype-limits warning because size_t can't be less than zero.
	sort_array(arri, 1);

	sort_array(arri, ARRAY_SIZE(arri));
	fails += CHECK_ARRAY(arri, -123, 1, 2, 3, 4, 5, 6, 7, 8);

	sort_array_cmp(arri, ARRAY_SIZE(arri), SORT_ARRAY_CMP_LT);
	fails += CHECK_ARRAY(arri, 8, 7, 6, 5, 4, 3, 2, 1, -123);

	char arrc[] = { 'h', 'e', 'x', 'a' };
	sort_array(arrc, ARRAY_SIZE(arrc));
	fails += CHECK_ARRAY(arrc, 'a', 'e', 'h', 'x');

	struct tile_t tile_arr[] = { {65,'B'}, {4,'A'}, {40,'X'}, {128,'@'}};
#define tile_cmp(a,b) (a.x > b.x)
	sort_array_cmp(tile_arr, ARRAY_SIZE(tile_arr), tile_cmp);
#undef tile_cmp

	fails += tile_arr[0].x != '@';
	fails += tile_arr[1].x != 'A';
	fails += tile_arr[2].x != 'B';
	fails += tile_arr[3].x != 'X';

	const char *names[] = { "emma", "amanda", "julie", "ellie", "sarah", "emma" };
	const char *names_expected[] = { "amanda", "ellie", "emma", "emma", "julie", "sarah" };

	sort_array_cmp(names, ARRAY_SIZE(names), SORT_ARRAY_CMP_CSTR_ASC);

	for (size_t i = 0 ; i < ARRAY_SIZE(names) ; ++i) {
		fails += strcmp(names[i], names_expected[i]) == 0 ? 0 : 1;
	}

	TEST_END();
}

GEN_ROTATE_ARRAY_CB(rotate_int_array_cb, int);
GEN_ROTATE_ARRAY_CB(rotate_tile_array_cb, struct tile_t);

static const struct re {
	int offset;
	int d;
	int n;
	int *input;
	int *expected;
} rotate_array_tests[] = {
	{ 0,   0, 0, (int[]){}, (int[]){} },
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
	failed += test_sort_array();
	failed += test_rotate_array();
	failed += test_rotate_array_cb();

	if (failed != 0) {
		printf("Tests " RED "FAILED" NC "\n");
	} else {
		printf("All tests " GREEN "passed OK" NC ".\n");
	}

	return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
