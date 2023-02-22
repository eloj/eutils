/*
	String and String Buffer Utility Functions Tests
	Copyright (c) 2022, 2023 Eddy L O Jansson. Licensed under The MIT License.

	See https://github.com/eloj/eutils
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

#include "emacros.h"
#include "internal/tests.h"

#define EUTILS_IMPLEMENTATION
#include "estrings.h"

static int debug = 0;
static int debug_hex = 1;

struct escape_test {
	const char *input;
	const char *expected_output;
	size_t expected_len;
	int expected_err;
};

static int test_expand_escapes(void) {
	TEST_START(expand_escapes);
	char buf[1024];

	struct escape_test tests[] = {
		// Expected pass tests:
		{ "", "", 0, 0 },
		{ "A", "A", 1, 0 },
		{ "\\xFF", "\xFF", 1, 0 },
		{ "A\\x40A", "A@A", 3, 0 },
		{ "\\0", "\0", 1, 0 },
		{ "\\1\\32\\128", "\1\40\200", 3, 0 },
		{ "\\\"", "\"", 1, 0 },
		{ "\\a\\b\\f\\n\\r\\t\\v", "\a\b\f\n\r\t\v", 7, 0 },
		// Expected error tests:
		{ "\\", "", 0, ESC_ERROR },
		{ "\\x", "", 0, ESC_ERROR_HEX },
		{ "\\x8", "", 0, ESC_ERROR_HEX }, // NOTE: Should perhaps accept as extension?
		{ "\\xfz", "", 0, ESC_ERROR_HEX },
		{ "\\256", "", 0, ESC_ERROR_DEC },
		{ "\\?", "", 0, ESC_ERROR_CHAR },
	};

	for (size_t i = 0 ; i < sizeof(tests)/sizeof(tests[0]) ; ++i) {
		struct escape_test *test = &tests[i];
		int err;

		size_t res_len = expand_escapes(test->input, strlen(test->input), NULL, 0, &err);
		if (err != test->expected_err) {
			TEST_ERRMSG("unexpected error, expected '%d', got '%d' (position %zu).", test->expected_err, err, res_len);
			++fails;
			continue;
		}
		if (test->expected_err != 0) {
			// Expected error -- we're done here.
			continue;
		}

		if (res_len != test->expected_len) {
			TEST_ERRMSG("length-determination result mismatch, expected '%zu', got '%zu'.", test->expected_len, res_len);
			++fails;
			continue;
		}

		size_t res = expand_escapes(test->input, strlen(test->input), buf, sizeof(buf), &err);
		if (res != res_len) {
			TEST_ERRMSG("output length differs, expected '%zu', got '%zu'.", res_len, res);
			++fails;
			continue;
		}

		if (memcmp(buf, test->expected_output, res) != 0) {
			TEST_ERRMSG("output buffer contents mismatch.");
			if (debug_hex) {
				printf("expected:\n");
				fprint_hex(stdout, (const unsigned char*)test->expected_output, strlen(test->expected_output), 32, "\n", 1);
				printf("\ngot:\n");
				fprint_hex(stdout, (const unsigned char*)buf, res, 32, "\n", 1);
				printf("\n");
			}
		}
	}

	TEST_END();
}

static int test_buf_printf(void) {
	TEST_START(buf_printf);
	size_t i = 0;

	char buf[128];
	size_t wp = 0;
	size_t res;
	int tr = 0;

	buf[7] = 1;
	res = buf_printf(buf, 8, &wp, &tr, "This buffer is %zd bytes", sizeof(buf));
	if (res != 8) {
		TEST_ERRMSG("Undersized buffer, expected res '%zu', got '%zu'.", (size_t)8, res);
		++fails;
	}
	if (wp != 8) {
		TEST_ERRMSG("Undersized buffer, expected wp '%zu', got '%zu'.", (size_t)8, wp);
		++fails;
	}
	if (!tr) {
		TEST_ERRMSG("Undersized buffer, expected truncation flag set, got '%d'.", tr);
		++fails;
	}
	if (buf[7] != 0) {
		TEST_ERRMSG("Undersized buffer, expected zero termination not detected!");
		++fails;
	}

	wp = 0;
	res = buf_printf(buf, sizeof(buf), &wp, NULL, "This buffer is %zd bytes", sizeof(buf));
	if (res != 24) {
		TEST_ERRMSG("Correctly sized buffer, expected '24', got '%zu'.", res);
		++fails;
	}
	if (wp != 24) {
		TEST_ERRMSG("Correctly sized buffer, expected wp '24', got '%zu'.", wp);
		++fails;
	}

	if (memcmp(buf, "This buffer is 128 bytes", 24) != 0) {
		TEST_ERRMSG("Buffer contents failed, got '%s'.", buf);
		++fails;
	}

	wp = -sizeof(buf); // Test write-point under/overflow
	res = buf_printf(buf, sizeof(buf), &wp, NULL, "This buffer is %zd bytes", sizeof(buf));
	if (res != 0) {
		TEST_ERRMSG("Write offset beyond input buffer NOT detected!");
		++fails;
	}

	TEST_END();
}

static int test_read_entire_file(void) {
	TEST_START(read_entire_file);

	size_t len = 0;
	char *str = read_entire_file("LICENSE", &len);
	if (str) {
		if (len != strlen(str)) {
			TEST_ERRMSG("File size returned doesn't match strlen");
			++fails;
		}
		size_t expected_len = 1079;
		if (len != expected_len) {
			TEST_ERRMSG("LICENSE file size mismatch, got '%zu', expected %zu", len, expected_len);
			++fails;
		}
		if (strstr(str, "SOFTWARE.") == NULL) {
			TEST_ERRMSG("LICENSE file contents mismatch.");
			++fails;
		}
		free(str);
	} else {
		TEST_ERRMSG("Reading LICENSE failed");
		++fails;
	}

	TEST_END();
}

static int test_generate_cstr_ptrs(void) {
	TEST_START(generate_cstr_ptrs);

	char text1[] = "The";
	char text2[] = "The quick ";
	char text3[] = "The\nquick\nbrown\nfox\njumps\nover\nthe\nlazy\ndog\n";

	size_t entries = 0;
	char** str_ptrs = generate_cstr_ptrs(text1, strlen(text1), " ", &entries);
	free(str_ptrs);

	if (entries != 0) {
		TEST_ERRMSG("Expected zero entries, got %zu", entries);
		++fails;
	}

	str_ptrs = generate_cstr_ptrs(text1, strlen(text1)+1, " ", &entries);
	free(str_ptrs);

	if (entries != 1) {
		TEST_ERRMSG("Expected 1 entry, got %zu", entries);
		++fails;
	}

	str_ptrs = generate_cstr_ptrs(text2, strlen(text2), " ", &entries);
	if (entries != 2) {
		TEST_ERRMSG("Expected 2 entries, got %zu", entries);
		++fails;
	}
	free(str_ptrs);

	str_ptrs = generate_cstr_ptrs(text3, strlen(text3), "\n", &entries);
	if (entries != 9) {
		TEST_ERRMSG("Expected 9 entries, got %zu", entries);
		++fails;
	}

	if (strcmp(str_ptrs[8], "dog") != 0) {
		TEST_ERRMSG("Expected [8]='dog', got %s", str_ptrs[8]);
		++fails;
	}

	free(str_ptrs);

	TEST_END();
}

static int test_generate_delim_ptrs(void) {
	TEST_START(generate_delim_ptrs);

	const char* text = "The";
	const char* text2= "The quick ";
	const char* text3= "The\nquick\nbrown\nfox\njumps\nover\nthe\nlazy\ndog\n";

	size_t entries = 0;
	const char** str_ptrs = generate_delim_ptrs(text, strlen(text), ' ', &entries);

	if (entries != 1) {
		TEST_ERRMSG("Expected 1 entry, got %zu", entries);
		++fails;
	}

	size_t wlen = str_ptrs[1] - str_ptrs[0];
	if (strncmp(str_ptrs[0], "The", wlen - 1) != 0) {
		TEST_ERRMSG("Expected 'The', got '%.*s'", (int)wlen-1, str_ptrs[0]);
		++fails;
	}
	free(str_ptrs);

	str_ptrs = generate_delim_ptrs(text2, strlen(text2), ' ', &entries);
	if (entries != 3) {
		TEST_ERRMSG("Expected 3 entries, got %zu", entries);
		++fails;
	}
	free(str_ptrs);

	str_ptrs = generate_delim_ptrs(text3, strlen(text3), '\n', &entries);
	if (entries != 10) {
		TEST_ERRMSG("Expected 10 entries, got %zu", entries);
		++fails;
	}
	free(str_ptrs);

#if 0
	for (size_t i = 0 ; i < words ; ++i) {
		size_t len = str_ptrs[i+1] - str_ptrs[i];
		printf("[%03zu]=%.*s\n", i, (int)len-1, str_ptrs[i]);
	}
#endif

	TEST_END();
}

int main(int UNUSED(argc), char UNUSED(*argv[])) {
	size_t failed = 0;

	failed += test_expand_escapes();
	failed += test_buf_printf();
	failed += test_read_entire_file(); // Requires 'LICENSE' file to be available in current directory.
	failed += test_generate_cstr_ptrs();
	failed += test_generate_delim_ptrs();

	if (failed != 0) {
		printf("Tests " RED "FAILED" NC "\n");
	} else {
		printf("All tests " GREEN "passed OK" NC ".\n");
	}

	return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
