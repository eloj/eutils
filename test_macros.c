/*
	Tests for Utility Macros
	Copyright (c) 2023, Eddy L O Jansson. Licensed under The MIT License.

	See https://github.com/eloj/eutils
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "macros-eddy.h"

#define RED "\e[1;31m"
#define GREEN "\e[0;32m"
#define YELLOW "\e[1;33m"
#define NC "\e[0m"

#define TEST_ERRMSG(fmt, ...) \
	fprintf(stderr,"%s:%d:" RED " error: " NC fmt "\n", testname, __LINE__ __VA_OPT__(,) __VA_ARGS__)

#define TEST_START(name) \
	const char *testname = STRINGIFY(name); \
	int fails = 0;

#define TEST_END() \
	if (fails == 0) { \
		printf("Suite '%s' ... " GREEN "PASS" NC "\n", testname); \
	} else { \
		printf("Suite '%s' ... " RED "FAIL" NC "\n", testname); \
	} \
	return fails;

// Returns 0 if bit-pattern equal.
#define CMP_FLOAT(a,b) ({ \
	float fa = (a); \
	float fb = (b); \
	uint32_t f1, f2; \
	memcpy(&f1, &fa, sizeof(f1)); \
	memcpy(&f2, &fb, sizeof(f2)); \
	(f1 == f2 ? 0 : 1); \
})

static int test_MIN(void) {
	TEST_START(MIN);

	fails += MIN(-1, 0) == -1 ? 0 : 1;
	fails += MIN(2, MIN(1, 0)) == 0 ? 0 : 1;
	fails += CMP_FLOAT(MIN(0.0f, 1.0f), 0.0f);

	TEST_END();
}

static int test_MAX(void) {
	TEST_START(MAX);

	fails += MAX(-1, 0) == 0 ? 0 : 1;
	fails += MAX(0, MAX(1, 2)) ? 0 : 1;
	fails += CMP_FLOAT(MAX(0.0f, 1.0f), 1.0f);

	TEST_END();
}

static int test_CLAMP(void) {
	TEST_START(CLAMP);

	fails += CLAMP(2, -1, 1) == 1 ? 0 : 1;
	fails += CLAMP(-2, -1, 1) == -1 ? 0 : 1;
	fails += CMP_FLOAT(CLAMP(0.5f, 0.0f, 1.0f), 0.5f);
	fails += CMP_FLOAT(CLAMP(1.5f, 0.0f, 1.0f), 1.0f);
	fails += CMP_FLOAT(CLAMP(-0.5f, 0.0f, 1.0f), 0.0f);

	TEST_END();
}

static int test_LERP(void) {
	TEST_START(LERP);
	int res;

	res = LERP(0, 256, 0.0f);
	if ((res = LERP(0, 256, 0.0f)) != 0) {
		TEST_ERRMSG("Unexpected error: LERP(0, 256, 0.0f) returned %d, expected %d", res, 0);
		++fails;
	}

	res = LERP(0, 256, 1.0f);
	if (res != 256) {
		TEST_ERRMSG("Unexpected error: LERP(0, 256, 1.0f) returned %d, expected %d", res, 256);
		++fails;
	}

	res = LERP(0, 256, 0.5f);
	if (res != 128) {
		TEST_ERRMSG("Unexpected error: LERP(0, 256, 0.5f) returned %d, expected %d", res, 128);
		++fails;
	}

	float fres = LERP(0.0f, 1.0f, 0.5f);
	if (CMP_FLOAT(fres, 0.5f)) {
		TEST_ERRMSG("Unexpected error: LERP(0.0f, 1.0f, 0.5f) returned %f, expected %f", (double)fres, (double)0.5f);
		++fails;
	}

	TEST_END();
}

int main(int argc, char *argv[]) {

	size_t failed = 0;

	failed += test_MIN();
	failed += test_MAX();
	failed += test_CLAMP();
	failed += test_LERP();

	if (failed != 0) {
		printf("Tests " RED "FAILED" NC "\n");
	} else {
		printf("All tests " GREEN "passed OK" NC ".\n");
	}

	return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
