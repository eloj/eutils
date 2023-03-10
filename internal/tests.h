#pragma once

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
		printf("Suite '%s' ... " RED "%d FAIL" NC "\n", testname, fails); \
	} \
	return fails;

// NOTE: Unsafe to use on arrays of structs that may contain padding. Beware.
#define CHECK_ARRAY(arr, ...) \
	((sizeof(arr) == sizeof(__typeof__(arr[0])[]){__VA_ARGS__}) && memcmp((arr), (__typeof__((arr)[0])[]){__VA_ARGS__}, sizeof(arr)) == 0) ? 0 : 1

// This is very special and only for these tests.
// Returns 0 if bit-pattern equal, else 1.
#define CMP_FLOAT(a,b) ({ \
	float fa = (a); \
	float fb = (b); \
	uint32_t f1, f2; \
	memcpy(&f1, &fa, sizeof(f1)); \
	memcpy(&f2, &fb, sizeof(f2)); \
	(f1 == f2 ? 0 : 1); \
})
