#pragma once
/*
	Utility Macros (C11+)
	Copyright (c) 2022, 2023, Eddy L O Jansson. Licensed under The MIT License.

	See https://github.com/eloj/eutils
*/

// TODO: For c23, replace '__auto_type' with 'auto', '__typeof__ with 'typeof'(?)

#ifndef STRINGIFY
#define STRINGIFY_INTERNAL(x) #x
#define STRINGIFY(x) STRINGIFY_INTERNAL(x)
#endif

#ifndef CONCATENATE
#define CONCATENATE_INTERNAL(a,b) a ## b
#define CONCATENATE(a,b) CONCATENATE_INTERNAL(a,b)
#endif

#ifndef SWAP
#define SWAP(a, b) do { __auto_type T = (a); a = b; b = T; } while (0)
#endif

#ifndef UNUSED
#define UNUSED(x) (x) __attribute__((unused))
#endif

// Requires C11 + __typeof__ extension
#define MACRO_TYPE_ASSERT_IMPL(a, b, line) _Static_assert(_Generic((a), __typeof__(b):1, default:0), "Type mismatch in macro at line " STRINGIFY(line))
#define MACRO_TYPE_ASSERT(a, b) MACRO_TYPE_ASSERT_IMPL(a, b, __LINE__)

// Generate a unique identifier that includes the line number to aid debugging.
#define LVAR(a,b) CONCATENATE(CONCATENATE(_ ## a, __LINE__), CONCATENATE(_, b))

// Require Statement Expressions GNU extension (GCC and clang has it)
// LVAR is used to generate local identifier to avoid shadowing.
#define MIN(a, b) MIN_IMPL(a, b, __COUNTER__)
#define MIN_IMPL(a, b, ID) ({ \
	__auto_type LVAR(x,ID) = (a); \
	__auto_type LVAR(y,ID) = (b); \
	MACRO_TYPE_ASSERT(LVAR(x,ID), LVAR(y,ID)); \
	LVAR(x,ID) < LVAR(y,ID) ? LVAR(x,ID) : LVAR(y,ID); \
})

#define MAX(a, b) MAX_IMPL(a, b, __COUNTER__)
#define MAX_IMPL(a, b, ID) ({ \
	__auto_type LVAR(x,ID) = (a); \
	__auto_type LVAR(y,ID) = (b); \
	MACRO_TYPE_ASSERT(LVAR(x,ID), LVAR(y,ID)); \
	LVAR(x,ID) > LVAR(y,ID) ? LVAR(x,ID) : LVAR(y,ID); \
})

#define CLAMP(value, min, max) CLAMP_IMPL(value, min, max, __COUNTER__)
#define CLAMP_IMPL(value, min, max, ID) ({ \
	__auto_type LVAR(val,ID) = (value); \
	__auto_type LVAR(x,ID) = (min); \
	__auto_type LVAR(y,ID) = (max); \
	MACRO_TYPE_ASSERT(LVAR(x,ID), LVAR(val,ID)); \
	MACRO_TYPE_ASSERT(LVAR(y,ID), LVAR(val,ID)); \
	(LVAR(val,ID) > LVAR(y,ID) ? LVAR(y,ID) : (LVAR(val,ID) < LVAR(x,ID) ? LVAR(x,ID) : LVAR(val,ID))); \
})

// Linearly interpolate v0->v1, where t is in [0, 1].
#define LERP(v0, v1, t) LERP_IMPL(v0, v1, t, __COUNTER__)
#define LERP_IMPL(v0, v1, t, ID) ({ \
	__auto_type LVAR(x,ID) = (v0); \
	__auto_type LVAR(y,ID) = (v1); \
	MACRO_TYPE_ASSERT(LVAR(x,ID), LVAR(y,ID)); \
	(LVAR(x,ID) + (t) * (LVAR(y,ID) - LVAR(x,ID))); \
})
// ALT: return (1 - t) * v0 + t * v1; // non-monotonic but precise at t=1=v1
