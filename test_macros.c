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

#include "emacros.h"
#include "internal/tests.h"

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
	fails += MAX(0, MAX(1, 2)) == 2 ? 0 : 1;
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

	res = LERP(0, 255, 0.0f);
	if (res != 0) {
		TEST_ERRMSG("Unexpected error: LERP(0, 255, 0.0f) returned %d, expected 0", res);
		++fails;
	}

	res = LERP(0, 255, 1.0f);
	if (res != 255) {
		TEST_ERRMSG("Unexpected error: LERP(0, 255, 1.0f) returned %d, expected 255", res);
		++fails;
	}

	res = LERP(0, 255, 0.5f);
	if (res != 127) {
		TEST_ERRMSG("Unexpected error: LERP(0, 255, 0.5f) returned %d, expected 127", res);
		++fails;
	}

	float fres = LERP(0.0f, 1.0f, 0.5f);
	if (CMP_FLOAT(fres, 0.5f)) {
		TEST_ERRMSG("Unexpected error: LERP(0.0f, 1.0f, 0.5f) returned %a, expected %a", (double)fres, 0.5);
		++fails;
	}

	TEST_END();
}

int main(int UNUSED(argc), char UNUSED(*argv[])) {
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
