#ifndef ABL_CORE_H
#define ABL_CORE_H

// TODO: rename abl_config ?
// you modify here the typedefs and macro to configure the lang/impl as you need

// float and int types carried by ABL values
typedef float abl_float;
typedef int abl_int;

// c functions
#include <assert.h>
#define ABL_ASSERT(x) assert(x)

#include <stdlib.h>
#define ABL_MALLOC(x) malloc(x)
#define ABL_FREE(x) free(x)

#endif
