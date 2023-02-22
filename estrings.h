#pragma once
/*
	String and String Buffer Utility Functions
	Copyright (c) 2022, 2023 Eddy L O Jansson. Licensed under The MIT License.

	See https://github.com/eloj/eutils
*/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

enum escape_err {
	NO_ERROR,
	ESC_ERROR,
	ESC_ERROR_CHAR,
	ESC_ERROR_HEX,
	ESC_ERROR_DEC,
};

void fprint_hex(FILE *stream, const uint8_t *data, size_t len, int width, const char *indent, int show_offset);

size_t expand_escapes(const char *input, size_t slen, char *dest, size_t dlen, int *err);

size_t buf_printf(char *buf, size_t bufsize, size_t *wp, int *truncated, const char *format, ...);

char *read_entire_file(const char *filename, size_t *len);

char** generate_cstr_ptrs(char *data, size_t len, const char *delims, size_t *count);
const char** generate_delim_ptrs(const char *data, size_t len, const char delim, size_t *count);

#ifdef EUTILS_IMPLEMENTATION
#include <assert.h>
#include <ctype.h> // for isxdigit()
#include <stdarg.h>

void fprint_hex(FILE *f, const uint8_t *data, size_t len, int width, const char *indent, int show_offset) {
	for (size_t i = 0 ; i < len ; ++i) {
		if (show_offset && (i % width == 0)) fprintf(f, "%08zx: ", i);
		fprintf(f, "%02x", data[i]);
		if (i < len -1) {
			if (indent && *indent && ((i+1) % width == 0)) {
				fprintf(f, "%s", indent);
			} else {
				fprintf(f, " ");
			}
		}
	}
}

static uint8_t nibble(const char c) {
	// return c + '0' + (((unsigned)(9 - c) >> 4) & 0x27);
	if (c >= '0' && c <= '9') {
		return c - '0';
	} else if (c >= 'a' && c <= 'f') {
		return 10 + c - 'a';
	} else if (c >= 'A' && c <= 'F') {
		return 10 + c - 'A';
	}
	return 0;
}

// Expand escape codes. Not compatible with stdlib!
//
// Accepts:
//  - Standard escapes: \r\n\t, etc.
//  - Hexadecimal escapes: \x00 - \xFF
//  - Decimal escapes: \0 - \255
//  No support of octal.
//
// Returns
//   If dest is NULL, then returns number of bytes that WOULD be written.
//   On success:
//   	Returns number of bytes written.
//   On error:
//   	Sets err, and returns position of error in input.
size_t expand_escapes(const char *input, size_t slen, char *dest, size_t dlen, int *err) {
	size_t rp = 0;
	size_t wp = 0;

#define RETURN_ERR(err_enum) { if (err) *err = err_enum; return rp_start; } while (0)
	assert(err != NULL);

	while (rp < slen && (wp < dlen || dest == NULL)) {
		// Use start of scan as return value on error.
		size_t rp_start = rp;
		char c = input[rp++];
		if (c == '\\') {
			// Check if dangling escape
			if (rp >= slen)
				RETURN_ERR(ESC_ERROR);

			if (input[rp] == 'x') {
				// HEX escape
				if (rp + 2 < slen && isxdigit(input[rp+1]) && isxdigit(input[rp+2])) {
					c = (nibble(input[rp+1]) << 4) | nibble(input[rp+2]);
					rp += 3;
				} else {
					RETURN_ERR(ESC_ERROR_HEX);
				}
			} else if (isdigit(input[rp])) {
				// DECimal escape
				int decval = input[rp++] - '0';
				if (rp < slen && isdigit(input[rp])) {
					decval *= 10;
					decval += input[rp++] - '0';
				}
				if (rp < slen && isdigit(input[rp])) {
					decval *= 10;
					decval += input[rp++] - '0';
				}
				if (decval > 255) {
					RETURN_ERR(ESC_ERROR_DEC);
				}
				c = decval;
			} else {
				// Standard escape character
				// We don't support \? because that is a trigraphs legacy.
				switch (input[rp++]) {
					case 'a': c = '\a'; break;
					case 'b': c = '\b'; break;
					case 'f': c = '\f'; break;
					case 'n': c = '\n'; break;
					case 'r': c = '\r'; break;
					case 't': c = '\t'; break;
					case 'v': c = '\v'; break;
					case '"': c = '"';  break;
					case '\\': c = '\\';break;
					default:
						RETURN_ERR(ESC_ERROR_CHAR);
				}
			}
		}

		if (dest && dlen) {
			if (wp < dlen) {
				dest[wp] = c;
			}
		}
		++wp;
	}
	if (err)
		*err = 0;
	return wp;
#undef RETURN_ERR
}

// Helper for safe but slow string concatenation. Result is always zero-terminated.
// Returns bytes actually written, updates wp to next write position.
size_t buf_printf(char *buf, size_t bufsize, size_t *wp, int *truncated, const char *format, ...) {
	size_t written = 0;
	int left = 0;

	if (!__builtin_sub_overflow(bufsize, *wp, &left) && left > 0) {
		va_list args;
		va_start(args, format);
		int res = vsnprintf(buf + *wp, left, format, args);
		va_end(args);
		if (res >= 0) {
			if (res >= left) {
				// Output was truncated, return actual number of bytes written.
				res = left;
				if (truncated)
					*truncated = 1;
			}
			written = res;
		}
		// No overflow check here.
		*wp += written;
	}

	return written;
}

#endif

static_assert(sizeof(size_t) >= sizeof(long), "size_t < long"); // Never truncate ftell. PS. You have a weird platform.

char *read_entire_file(const char *filename, size_t *len) {
	FILE *f = fopen(filename, "rb");

	if (!f)
		return NULL;

	fseek(f, 0, SEEK_END);
	long bytes = ftell(f);
	if (bytes < 0) {
		fclose(f);
		return NULL;
	}
	rewind(f);

	char *buf = malloc(bytes + 1);
	if (!buf) {
		fclose(f);
		return NULL;
	}

	size_t rnum = fread(buf, bytes, 1, f);
	fclose(f);

	if (bytes && rnum != 1) {
		free(buf);
		return NULL;
	}

	buf[bytes] = 0; // always zero-terminate.

	if (len)
		*len = bytes;

	return buf;
}

char** generate_cstr_ptrs(char *data, size_t len, const char *delims, size_t *count) {
	size_t j = 0;
	size_t entries = 0;
	char *p = data;
	char *b = data; // points to beginning of entry
	char *end = data + len;
	char **strs = NULL;

	if (!data || len == 0)
		goto leave;

	while (p < end) {
		if (*p == '\0'|| strchr(delims, *p)) {
			++entries;
		}
		++p;
	}

	// Basic overflow check, but callers should make sure to not call with unreasonably-sized buffers anyway.
	if (!__builtin_mul_overflow(entries, sizeof(char*), &j)) {
		strs = malloc(j);
	}
	j = 0;
	if (!strs)
		goto leave;

	p = data;
	while (p < end) {
		if (*p == '\0'|| strchr(delims, *p)) {
			assert(j < entries);
			strs[j++] = b;
			b = p + 1;
			*p = '\0';
		}
		++p;
	}
	assert(j == entries);

leave:
	if (count)
		*count = j;

	return strs;
}

const char** generate_delim_ptrs(const char *data, size_t len, const char delim, size_t *count) {
	size_t j = 0;
	size_t entries = 0;
	const char *p = data;
	const char *b = data; // points to beginning of entry
	const char *end = data + len;
	const char **strs = NULL;

	if (!data || len == 0)
		goto leave;

	while (p < end) {
		if (*p == delim) {
			++entries;
		}
		++p;
	}

	// +2 for end-of-buffer sentinels
	if (!__builtin_mul_overflow(entries, sizeof(char*), &j) && !__builtin_add_overflow(j, 2, &j)) {
		strs = malloc(j);
	}
	j = 0;
	if (!strs)
		goto leave;

	p = data;
	while (p < end) {
		if (*p == delim) {
			assert(j < entries);
			strs[j++] = b;
			b = p + 1;
		}
		++p;
	}
	strs[j++] = b; // handle input with no delimiter at end
	strs[j] = b; // true length sentinel, not counted.
	assert(j < entries + 2);

leave:
	if (count)
		*count = j;

	return strs;
}

#ifdef __cplusplus
}
#endif
