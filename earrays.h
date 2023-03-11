#pragma once
/*
	Array Utility Functions
	Copyright (c) 2023 Eddy L O Jansson. Licensed under The MIT License.

	See https://github.com/eloj/eutils
*/
#ifdef __cplusplus
extern "C" {
#endif

#include "emacros.h"

#define reverse_array(arr, n) reverse_array_impl(arr, n, GENID(i), GENID(j))
#define reverse_array_impl(arr, n, i, j) do { \
	for (int i=0, j=((n)-1) ; i < j ; ++i, --j) \
		SWAP((arr)[i], (arr)[j]); \
} while(0)

// Insertion sort any simple-valued array in ascending order
#define SORT_ARRAY_CMP_GT(a,b,cmp_data) ((a) > (b))
#define SORT_ARRAY_CMP_LT(a,b,cmp_data) ((a) < (b))
#define SORT_ARRAY_CMP_CSTR_ASC(s1,s2,cmp_data) (strcmp((s1), (s2)) > 0)
#define SORT_ARRAY_CMP_CSTR_DESC(s1,s2,cmp_data) (strcmp((s1), (s2)) < 0)
#define SORT_ARRAY_CMP_PERM_GT(a,b,cmp_data) ((cmp_data)[a] > (cmp_data)[b])
#define SORT_ARRAY_CMP_PERM_LT(a,b,cmp_data) ((cmp_data)[a] < (cmp_data)[b])

#define sort_array(arr, n) sort_array_simple_impl(arr, n, SORT_ARRAY_CMP_GT, NULL, GENID(j), GENID(x))
#define sort_array_cmp(arr, n, cmp) sort_array_simple_impl(arr, n, cmp, NULL, GENID(j), GENID(x))
#define sort_array_cmp_data(arr, n, cmp, cmp_data) sort_array_simple_impl(arr, n, cmp, cmp_data, GENID(j), GENID(x))
#define sort_array_simple_impl(arr, n, cmp, cmp_data, j, x) do { \
	_Pragma("GCC diagnostic push") \
	_Pragma("GCC diagnostic ignored \"-Wtype-limits\"") \
	size_t j; \
	for (size_t i = 1 ; i < (n) ; ++i) { \
		__auto_type x = (arr)[i]; \
		for (j = i ; (j > 0 && cmp(((arr)[j-1]), x, cmp_data)) ; --j) { \
			(arr)[j] = (arr)[j-1]; \
		} \
		(arr)[j] = x; \
	} \
	_Pragma("GCC diagnostic pop") \
} while(0)

#define rotate_array(arr, n, dir) rotate_array_impl(arr, n, dir, GENID(d))
#define rotate_array_impl(arr, n, dir, dID) do { \
	if ((n) > 1) { \
		int dID; \
		if ((dir) < 0) { \
			dID = (n) - (-(dir) % (n)); \
		} else { \
			dID = ((dir) % (n)); \
		} \
		if ((dID > 0) && (dID < (n))) { \
			--dID; \
			reverse_array((arr)		 , (dID)	); \
			reverse_array((arr)+(dID), (n)-(dID)); \
			reverse_array((arr)      , ((n)-1)  ); \
		} \
	} \
} while(0)

enum rotate_array_action {
	ROT_ACTION_SAVE,
	ROT_ACTION_RESTORE,
	ROT_ACTION_COPY
};

typedef void (*rot_cb)(void *arr, int src, int dst, enum rotate_array_action, void *ctx);

/*
	Macro to generate rotate_array_cb() callbacks for trivial types.
*/
#define GEN_ROTATE_ARRAY_CB(name, type) \
static void name(void *arr, int src, int dst, enum rotate_array_action action, void *ctx) { \
	type *tmp = ctx; \
	type *typed_arr = arr; \
	switch (action) { \
		case ROT_ACTION_COPY: \
			typed_arr[dst] = typed_arr[src]; \
			break; \
		case ROT_ACTION_SAVE: \
			*tmp = typed_arr[src]; \
			break; \
		case ROT_ACTION_RESTORE: \
			typed_arr[dst] = *tmp; \
			break; \
	} \
}

/*
	Juggling/Dolphin Algorithm (Bentley, Programming Pearls)
	See Also: "Swapping Sections" (TR 81-452), David Gries & Harlan Mills.

	Can be used to rotate [part of] an array, and due to the callback-design,
	also more complex structures, such as a whole memory block.

	For something simpler, consider the '3-reverse' algorithm instead.
*/
void rotate_array_cb(void *arr, int n, int d, rot_cb cb, void *ctx);

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

void rotate_array_cb(void *arr, int n, int d, rot_cb cb, void *ctx) {
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
