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

// Generate identifier that includes the line number to aid debugging.
#define EUTILS_MK_ID(a,b) CONCATENATE(CONCATENATE(_ ## a, __LINE__), CONCATENATE(_, b))
// Generate unique identifier using argument as the base name
#define GENID(a) EUTILS_MK_ID(a, __COUNTER__)

// Require Statement Expressions GNU extension (GCC and clang has it)
// GENID is used to generate name for local identifier, avoiding shadowing.
#define MIN(a, b) MIN_IMPL(a, b, GENID(x), GENID(y))
#define MIN_IMPL(a, b, aID, bID) ({ \
	__auto_type aID = (a); \
	__auto_type bID = (b); \
	MACRO_TYPE_ASSERT(aID, bID); \
	(aID < bID ? aID : bID); \
})

#define MAX(a, b) MAX_IMPL(a, b, GENID(x), GENID(y))
#define MAX_IMPL(a, b, aID, bID) ({ \
	__auto_type aID = (a); \
	__auto_type bID = (b); \
	MACRO_TYPE_ASSERT(aID, bID); \
	(aID > bID ? aID : bID); \
})

#define CLAMP(value, min, max) CLAMP_IMPL(value, min, max, GENID(val), GENID(x), GENID(y))
#define CLAMP_IMPL(value, min, max, valID, xID, yID) ({ \
	__auto_type valID = (value); \
	__auto_type xID = (min); \
	__auto_type yID = (max); \
	MACRO_TYPE_ASSERT(xID, valID); \
	MACRO_TYPE_ASSERT(yID, valID); \
	(valID > yID ? yID : (valID < xID ? xID : valID)); \
})

// Linearly interpolate v0->v1, where t is in [0, 1].
//
// Compiler Explorer Verified that both GCC and clang compile this into
// vsubss+vfmadd231ss at -O2 when AVX available.
//
#define LERP(v0, v1, t) LERP_IMPL(v0, v1, t, GENID(x), GENID(y))
#define LERP_IMPL(v0, v1, t, xID, yID) ({ \
	__auto_type xID = (v0); \
	__auto_type yID = (v1); \
	MACRO_TYPE_ASSERT(xID, yID); \
	(xID + (t) * (yID - xID)); \
})
// ALT: return (1 - t) * v0 + t * v1; // non-monotonic but precise at t=1=v1
