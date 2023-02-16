/*
	Array Utility Functions
	Copyright (c) 2023 Eddy L O Jansson. Licensed under The MIT License.

	See https://github.com/eloj/eutils
*/
#ifdef __cplusplus
extern "C" {
#endif

enum rotate_array_action {
	ROT_ACTION_SAVE,
	ROT_ACTION_RESTORE,
	ROT_ACTION_COPY
};

typedef void (*rot_cb)(void *arr, int src, int dst, enum rotate_array_action, void *ctx);

/*
	Can be used to rotate [part of] an array, and also to implement rotation/shifting of more complex
	structures, such as a memory block.
*/
void rotate_array(void *arr, int n, int d, rot_cb cb, void *ctx);

#ifdef EUTILS_IMPLEMENTATION
#include <assert.h>

static int gcd(int a, int b) {
	assert(a >= 0);
	assert(b >= 0);
	if (a == 0)
		return b;
	if (b == 0)
		return a;
	while (b != 0) {
		int t = b;
		b = a % b;
		a = t;
	}
	return a;
}

void rotate_array(void *arr, int n, int d, rot_cb cb, void *ctx) {
	if (d == 0 || n == 0)
		return;
	if (d < 0) {
		d = n - (-d % n);
	} else {
		d = d % n;
	}

	int steps = gcd(d, n);
	for (int i=0 ; i < steps ; ++i) {
		int j = i;
		cb(arr, i, -1, ROT_ACTION_SAVE, ctx);
		while (1) {
			// int k = (j + d) % n; // ... or instead of mod: if (k >= n) k -= n;
			int k = j + d;
			if (k >= n)
				k -= n;
			if (k == i)
				break;
			cb(arr, k, j, ROT_ACTION_COPY, ctx);
			j = k;
		}
		cb(arr, -1, j, ROT_ACTION_RESTORE, ctx);
	}
}

#endif

#ifdef __cplusplus
}
#endif
