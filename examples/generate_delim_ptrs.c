/*
	Read a file into memory, then process it line-by-line.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "emacros.h"
#define EUTILS_IMPLEMENTATION
#include "estrings.h"

int main(int argc, char *argv[]) {
	const char *filename = argc > 1 ? argv[1] : "LICENSE";
	const char *delim = argc > 2 ? argv[2] : "\n";

	size_t str_len;
	char *str = read_entire_file(filename, &str_len);
	if (!str) {
		fprintf(stderr, "ERROR: Couldn't open file '%s'.\n", filename);
		exit(1);
	}

	size_t lines;
	const char** str_ptrs = generate_delim_ptrs(str, str_len, *delim, &lines);

	for (size_t i = 0 ; i < lines ; ++i) {
		size_t len = str_ptrs[i+1] - str_ptrs[i];
		printf("[%03zu]=%.*s\n", i, (int)len-1, str_ptrs[i]);
	}

	free(str_ptrs);
	free(str);

	return 0;
}
