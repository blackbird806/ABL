#ifndef ABL_CORE_H
#define ABL_CORE_H

#include <stdbool.h>
#include <stdint.h>

// TODO: rename abl_config ?
// you modify here the typedefs and macro to configure the lang/impl as you need

// float and int types carried by ABL values
typedef float abl_float;
typedef char abl_char;
typedef int abl_int;
typedef bool abl_bool;

#define ABL_ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0])) 

// c functions
#include <assert.h>
#define ABL_ASSERT(x) assert(x)

#include <stdlib.h>
#define ABL_MALLOC(x) malloc(x)
#define ABL_REALLOC(x, s) realloc(x, s)
#define ABL_FREE(x) free(x)

	#include <stdio.h>
	#define ABL_DEBUG_DIAGNOSTIC(fmt, ...) fprintf(stderr, fmt, __VA_ARGS__)
	#define ABL_DEBUG_VDIAGNOSTIC(fmt, args) vfprintf(stderr, fmt, args)

#endif
