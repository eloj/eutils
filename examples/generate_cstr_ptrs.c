/*
	Read a file into memory, then process it line-by-line.

	`generate_cstr_ptrs()` zero-terminates the input at every
	delimiter found, and returns an array with pointers to every
	string leading up to those terminators.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emacros.h"
#define EUTILS_IMPLEMENTATION
#include "estrings.h"

int main(int argc, char *argv[]) {
	const char *filename = argc > 1 ? argv[1] : "LICENSE";
	const char *delims = argc > 2 ? argv[2] : "\n";

	size_t str_len;
	char *str = read_entire_file(filename, &str_len);
	if (!str) {
		fprintf(stderr, "ERROR: Couldn't open file '%s'.\n", filename);
		exit(1);
	}

	size_t lines;
	// We can include the zt we know is in the source buffer,
	// just in case there is no newline at the end of the file.
	char** str_ptrs = generate_cstr_ptrs(str, str_len + 1, delims, &lines);

	for (size_t i = 0 ; i < lines ; ++i) {
		if (*str_ptrs[i])
			printf("[%03zu]=%s\n", i, str_ptrs[i]);
	}

	free(str_ptrs);
	free(str);

	return 0;
}
